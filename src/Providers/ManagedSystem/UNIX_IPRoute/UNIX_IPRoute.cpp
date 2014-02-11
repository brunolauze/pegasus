//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%////////////////////////////////////////////////////////////////////////////

#include "UNIX_IPRoute.h"

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/radix.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netipx/ipx.h>
#include <netatalk/at.h>
#include <netgraph/ng_socket.h>

#include <sys/sysctl.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <kvm.h>
#include <libutil.h>
#include <netdb.h>
#include <nlist.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <err.h>

#define _KERNEL 1
#include <netinet/ip_mroute.h>
#undef _KERNEL

#define INET6 1

struct	timespec uptime;

PEGASUS_USING_STD;

#define	kget(p, d) (kread((u_long)(p), (char *)&(d), sizeof (d)))

/*
 * Definitions for showing gateway flags.
 */
struct bits {
	u_long	b_mask;
	char	b_val;
} bits[] = {
	{ RTF_UP,	'U' },
	{ RTF_GATEWAY,	'G' },
	{ RTF_HOST,	'H' },
	{ RTF_REJECT,	'R' },
	{ RTF_DYNAMIC,	'D' },
	{ RTF_MODIFIED,	'M' },
	{ RTF_DONE,	'd' }, /* Completed -- for routing messages only */
	{ RTF_XRESOLVE,	'X' },
	{ RTF_STATIC,	'S' },
	{ RTF_PROTO1,	'1' },
	{ RTF_PROTO2,	'2' },
	{ RTF_PRCLONING,'c' },
	{ RTF_PROTO3,	'3' },
	{ RTF_BLACKHOLE,'B' },
	{ RTF_BROADCAST,'b' },
#ifdef RTF_LLINFO
	{ RTF_LLINFO,	'L' },
#endif
#ifdef RTF_WASCLONED
	{ RTF_WASCLONED,'W' },
#endif
#ifdef RTF_CLONING
	{ RTF_CLONING,	'C' },
#endif
	{ 0 , 0 }
};

static void p_rtable_sysctl(int, int, p_iproute_table_t*);
static void p_rtentry_sysctl(p_iproute_t* pRoute, struct rt_msghdr *rtm);
static const char *fmt_sockaddr(struct sockaddr *sa, struct sockaddr *mask, int flags);
static bool p_rt_iterate_sysctl(p_iproute_t* pRoute, p_iproute_table_t* pRouteTable);
static void p_flags(int, const char *);
static const char *fmt_flags(int f);

typedef union {
	long	dummy;		/* Helps align structure. */
	struct	sockaddr u_sa;
	u_short	u_data[128];
} sa_u;

static sa_u pt_u;

struct ifmap_entry {
	char ifname[IFNAMSIZ];
};


static struct ifmap_entry *ifmap;
static int ifmap_size;
static kvm_t *kvmd;
static char *nlistf = NULL, *memf = NULL;
/*
static char*
pr_family(int af1)
{
        char *afname;

        switch (af1) {
        case AF_INET:
                afname = "Internet";
                break;
#ifdef INET6
        case AF_INET6:
                afname = "Internet6";
                break;
#endif //INET6
        case AF_IPX:
                afname = "IPX";
                break;
        case AF_ISO:
                afname = "ISO";
                break;
        case AF_APPLETALK:
                afname = "AppleTalk";
                break;
        case AF_CCITT:
                afname = "X.25";
                break;
        case AF_NETGRAPH:
                afname = "Netgraph";
                break;
        default:
                afname = NULL;
                break;
        }
        if (afname)
                return afname;
		char* val;
	    sprintf(val, "%d", af1);
        return val;
}
*/


char *
routename(in_addr_t in)
{
        char *cp;
        static char line[MAXHOSTNAMELEN];
        struct hostent *hp;
      
        cp = 0;
        /*if (!numeric_addr) { */
                hp = gethostbyaddr(&in, sizeof (struct in_addr), AF_INET);
                if (hp) {
                        cp = hp->h_name;
                        trimdomain(cp, strlen(cp));
                }
        /*} */
        if (cp) {
                strlcpy(line, cp, sizeof(line));
        } else {
#define C(x)    ((x) & 0xff)
                in = ntohl(in);
                sprintf(line, "%u.%u.%u.%u",
                    C(in >> 24), C(in >> 16), C(in >> 8), C(in));
        }
        return (line);
}

#define	NSHIFT(m) (							\
	(m) == IN_CLASSA_NET ? IN_CLASSA_NSHIFT :			\
	(m) == IN_CLASSB_NET ? IN_CLASSB_NSHIFT :			\
	(m) == IN_CLASSC_NET ? IN_CLASSC_NSHIFT :			\
	0)

static void
domask(char *dst, in_addr_t addr __unused, u_long mask)
{
	int b, i;

	if (mask == 0 || (NSHIFT(mask) != 0)) { /* !numeric_addr && */
		*dst = '\0';
		return;
	}
	i = 0;
	for (b = 0; b < 32; b++)
		if (mask & (1 << b)) {
			int bb;

			i = b;
			for (bb = b+1; bb < 32; bb++)
				if (!(mask & (1 << bb))) {
					i = -1;	/* noncontig */
					break;
				}
			break;
		}
	if (i == -1)
		sprintf(dst, "&0x%lx", mask);
	else
		sprintf(dst, "/%d", 32-i);
}

/*
 * Return the name of the network whose address is given.
 */
char *
netname(in_addr_t in, in_addr_t mask)
{
	char *cp = 0;
	static char line[MAXHOSTNAMELEN];
	struct netent *np = 0;
	in_addr_t i;

	/* It is ok to supply host address. */
	in &= mask;

	i = ntohl(in);
	if (i) { /* !numeric_addr &&  */
		np = getnetbyaddr(i >> NSHIFT(ntohl(mask)), AF_INET);
		if (np != NULL) {
			cp = np->n_name;
			trimdomain(cp, strlen(cp));
		}
	}
	if (cp != NULL) {
		strlcpy(line, cp, sizeof(line));
	} else {
		inet_ntop(AF_INET, &in, line, sizeof(line) - 1);
	}
	domask(line + strlen(line), i, ntohl(mask));
	return (line);
}

#undef NSHIFT

#ifdef INET6
void
in6_fillscopeid(struct sockaddr_in6 *sa6)
{
#if defined(__KAME__)
	/*
	 * XXX: This is a special workaround for KAME kernels.
	 * sin6_scope_id field of SA should be set in the future.
	 */
	if (IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) ||
	    IN6_IS_ADDR_MC_NODELOCAL(&sa6->sin6_addr) ||
	    IN6_IS_ADDR_MC_LINKLOCAL(&sa6->sin6_addr)) {
		/* XXX: override is ok? */
		sa6->sin6_scope_id =
		    ntohs(*(u_int16_t *)&sa6->sin6_addr.s6_addr[2]);
		sa6->sin6_addr.s6_addr[2] = sa6->sin6_addr.s6_addr[3] = 0;
	}
#endif
}

const char *
netname6(struct sockaddr_in6 *sa6, struct in6_addr *mask)
{
	static char line[MAXHOSTNAMELEN];
	u_char *p = (u_char *)mask;
	u_char *lim;
	int masklen, illegal = 0, flag = 0;

	if (mask) {
		for (masklen = 0, lim = p + 16; p < lim; p++) {
			switch (*p) {
			 case 0xff:
				 masklen += 8;
				 break;
			 case 0xfe:
				 masklen += 7;
				 break;
			 case 0xfc:
				 masklen += 6;
				 break;
			 case 0xf8:
				 masklen += 5;
				 break;
			 case 0xf0:
				 masklen += 4;
				 break;
			 case 0xe0:
				 masklen += 3;
				 break;
			 case 0xc0:
				 masklen += 2;
				 break;
			 case 0x80:
				 masklen += 1;
				 break;
			 case 0x00:
				 break;
			 default:
				 illegal ++;
				 break;
			}
		}
		if (illegal)
			fprintf(stderr, "illegal prefixlen\n");
	}
	else
		masklen = 128;

	if (masklen == 0 && IN6_IS_ADDR_UNSPECIFIED(&sa6->sin6_addr))
		return("default");

	/*if (numeric_addr)
		flag |= NI_NUMERICHOST; 
	*/
	getnameinfo((struct sockaddr *)sa6, sa6->sin6_len, line, sizeof(line),
		    NULL, 0, flag);

	/*if (numeric_addr)
		sprintf(&line[strlen(line)], "/%d", masklen);
	*/
	return line;
}

char *
routename6(struct sockaddr_in6 *sa6)
{
	static char line[MAXHOSTNAMELEN];
	int flag = 0;
	/* use local variable for safety */
	struct sockaddr_in6 sa6_local;

	sa6_local.sin6_family = AF_INET6;
	sa6_local.sin6_len = sizeof(sa6_local);
	sa6_local.sin6_addr = sa6->sin6_addr;
	sa6_local.sin6_scope_id = sa6->sin6_scope_id;
	/*
	if (numeric_addr)
		flag |= NI_NUMERICHOST;
	*/
	getnameinfo((struct sockaddr *)&sa6_local, sa6_local.sin6_len,
		    line, sizeof(line), NULL, 0, flag);

	return line;
}
#endif /*INET6*/

static int
kvmd_init(void)
{
	char errbuf[_POSIX2_LINE_MAX];

	if (kvmd != NULL)
		return (0);

	kvmd = kvm_openfiles(nlistf, memf, NULL, O_RDONLY, errbuf);
	setgid(getgid());

	if (kvmd == NULL) {
		warnx("kvm not available: %s", errbuf);
		return (-1);
	}

	return (0);
}

/*
 * Read kernel memory, return 0 on success.
 */
int
kread(u_long addr, void *buf, size_t size)
{

	if (kvmd_init() < 0)
		return (-1);

	if (!buf)
		return (0);
	if (kvm_read(kvmd, addr, buf, size) != (ssize_t)size) {
		warnx("%s", kvm_geterr(kvmd));
		return (-1);
	}
	return (0);
}


static struct sockaddr *
kgetsa(struct sockaddr *dst)
{

	if (kget(dst, pt_u.u_sa) != 0)
		return (NULL);
	if (pt_u.u_sa.sa_len > sizeof (pt_u.u_sa))
		kread((u_long)dst, (char *)pt_u.u_data, pt_u.u_sa.sa_len);
	return (&pt_u.u_sa);
}


/*
 * Print routing tables.
 */
void
routepr(int fibnum, int af, p_iproute_table_t* pRouteTable)
{
	size_t intsize;
	int numfibs;

	intsize = sizeof(int);
	if (fibnum == -1 &&
	    sysctlbyname("net.my_fibnum", &fibnum, &intsize, NULL, 0) == -1)
		fibnum = 0;
	if (sysctlbyname("net.fibs", &numfibs, &intsize, NULL, 0) == -1)
		numfibs = 1;
	if (fibnum < 0 || fibnum > numfibs - 1)
		errx(EX_USAGE, "%d: invalid fib", fibnum);
	/*
	 * Since kernel & userland use different timebase
	 * (time_uptime vs time_second) and we are reading kernel memory
	 * directly we should do rt_rmx.rmx_expire --> expire_time conversion.
	 */
	if (clock_gettime(CLOCK_UPTIME, &uptime) < 0)
		err(EX_OSERR, "clock_gettime() failed");
	
	/*
	if (fibnum)
		printf(" (fib: %d)", fibnum);
	printf("\n");
	*/	
	p_rtable_sysctl(fibnum, af, pRouteTable);
}

void test() 
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_netmask;
            addr = inet_ntoa(sa->sin_addr);
            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
        }
    }

    freeifaddrs(ifap);	
}

static void
p_rtable_sysctl(int fibnum, int af, p_iproute_table_t* pRouteTable)
{
	int mib[7];
	char *buf;
	size_t needed;
	int ifindex = 0, size;
	struct sockaddr_dl *sdl;
	/*
	 * Retrieve interface list at first
	 * since we need #ifindex -> if_xname match
	 */
	struct ifaddrs *ifap, *ifa;
	if (getifaddrs(&ifap) != 0)
		err(EX_OSERR, "getifaddrs");
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		
		if (ifa->ifa_addr->sa_family != AF_LINK)
			continue;

		sdl = (struct sockaddr_dl *)ifa->ifa_addr;
		ifindex = sdl->sdl_index;

		if (ifindex >= ifmap_size) {
			size = roundup(ifindex + 1, 32) *
			    sizeof(struct ifmap_entry);
			if ((ifmap = (ifmap_entry*)realloc(ifmap, size)) == NULL)
				errx(2, "realloc(%d) failed", size);
			memset(&ifmap[ifmap_size], 0,
			    size - ifmap_size *
			     sizeof(struct ifmap_entry));

			ifmap_size = roundup(ifindex + 1, 32);
		}

		if (*ifmap[ifindex].ifname != '\0')
			continue;

		strlcpy(ifmap[ifindex].ifname, ifa->ifa_name, IFNAMSIZ);
	}

	freeifaddrs(ifap);

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = af;
	mib[4] = NET_RT_DUMP;
	mib[5] = 0;
	mib[6] = fibnum;
	if (sysctl(mib, 7, NULL, &needed, NULL, 0) < 0) {
		err(1, "sysctl: net.route.0.%d.dump.%d estimate", af, fibnum);
	}

	if ((buf = (char*)malloc(needed)) == 0) {
		errx(2, "malloc(%lu)", (unsigned long)needed);
	}
	if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
		err(1, "sysctl: net.route.0.%d.dump.%d", af, fibnum);
	}

	pRouteTable->buffer = buf;
	pRouteTable->length = needed;
	pRouteTable->lim  = buf + needed;
}

static bool p_rt_iterate_sysctl(p_iproute_t* pRoute, p_iproute_table_t* pRouteTable)
{
	struct rt_msghdr *rtm;
	struct sockaddr *sa;
	int fam = 0;
	if (pRouteTable->buffer < pRouteTable->lim) {
		rtm = (struct rt_msghdr *)pRouteTable->buffer;
		/*
		 * Peek inside header to determine AF
		 */
		sa = (struct sockaddr *)(rtm + 1);
		if (fam != sa->sa_family) {
			fam = sa->sa_family;
			//cout << "Family: " << fam << endl;
		}
		p_rtentry_sysctl(pRoute, rtm);
		pRouteTable->buffer += rtm->rtm_msglen;
		return true;
	}
	return false;
}

static void
p_rtentry_sysctl(p_iproute_t* pRoute, struct rt_msghdr *rtm)
{
	struct sockaddr *sa = (struct sockaddr *)(rtm + 1);
	char prettyname[128];
	sa_u addr, mask, gw, gmask, brd;
	unsigned int l;

#define	GETSA(_s, _f)	{ \
	bzero(&(_s), sizeof(_s)); \
	if (rtm->rtm_addrs & _f) { \
		l = roundup(sa->sa_len, sizeof(long)); \
		memcpy(&(_s), sa, (l > sizeof(_s)) ? sizeof(_s) : l); \
		sa = (struct sockaddr *)((char *)sa + l); \
	} \
}

	GETSA(addr, RTA_DST);
	GETSA(gw, RTA_GATEWAY);
	GETSA(mask, RTA_NETMASK);
	GETSA(gmask, RTA_GENMASK);
	GETSA(brd, RTA_BRD);
	pRoute->addressType = addr.u_sa.sa_family;
	pRoute->destination = fmt_sockaddr(&addr.u_sa, &mask.u_sa, rtm->rtm_flags);
	pRoute->gateway = fmt_sockaddr(&gw.u_sa, NULL, RTF_HOST);
	pRoute->index = rtm->rtm_index;
	/*
	if (addr.u_sa.sa_family == AF_INET)
	{
		struct sockaddr_in *sockin = (struct sockaddr_in *)(&(mask.u_sa));
		pRoute->netmask = netname(sockin->sin_addr.s_addr, INADDR_ANY);
	}
	else if (addr.u_sa.sa_family == AF_INET6)
	{
		struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)(&(mask.u_sa));
		in6_fillscopeid(sa6);
		pRoute->netmask = netname6(sa6, NULL);
	}
	*/
	/*
	if (addr.u_sa.sa_family == AF_INET)
	{
		struct sockaddr_in *sockin = (struct sockaddr_in *)(&(gmask.u_sa));
		cout << "GNetMask: " << netname(sockin->sin_addr.s_addr, INADDR_ANY) << endl;
	}
	else if (addr.u_sa.sa_family == AF_INET6)
	{
		struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)(&(gmask.u_sa));
		in6_fillscopeid(sa6);
		cout << "GNetMask: " << netname6(sa6, NULL) << endl;
	}
	*/
	//cout << "Destination: " << pRoute->destination << endl;
	//cout << "Gateway: " << pRoute->gateway << endl;
	/*
	cout << "Broadcast: " << fmt_sockaddr(&brd.u_sa, &mask.u_sa, RTF_HOST) << endl;
	*/
	/* p_flags(rtm->rtm_flags, buffer); */

	cout << "Flags: " << rtm->rtm_flags << endl;


	/* if (Wflag) { */
		if (rtm->rtm_rmx.rmx_mtu != 0)
			cout << "MTU: " << rtm->rtm_rmx.rmx_mtu << endl;
			/* printf("%*lu ", wid_mtu, rtm->rtm_rmx.rmx_mtu); */
		else
			cout << "MTU: " << endl; /* NO MTU */
			/* printf("%*s ", wid_mtu, ""); */
	/* } */

	memset(prettyname, 0, sizeof(prettyname));

	if (rtm->rtm_index < ifmap_size) {
		struct ifaddrs *ifap, *ifa;
		if (getifaddrs(&ifap) != 0)
			err(EX_OSERR, "getifaddrs");
		for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
			if (addr.u_sa.sa_family == AF_INET)
			{
				if (ifa->ifa_addr->sa_family==AF_INET && strcmp(ifa->ifa_name,ifmap[rtm->rtm_index].ifname) == 0)
				{
		            struct sockaddr_in* sa2 = (struct sockaddr_in *) ifa->ifa_netmask;
		            pRoute->netmask = inet_ntoa(sa2->sin_addr);
				}
			}
			else if (addr.u_sa.sa_family == AF_INET6)
			{
				if (ifa->ifa_addr->sa_family==AF_INET && strcmp(ifa->ifa_name,ifmap[rtm->rtm_index].ifname) == 0)
				{
		            struct sockaddr_in6* sa6 = (struct sockaddr_in6 *) ifa->ifa_netmask;
		            in6_fillscopeid(sa6);
		            pRoute->netmask = netname6(sa6, NULL);
				}
			}
		}
		strlcpy(prettyname, ifmap[rtm->rtm_index].ifname,
		    sizeof(prettyname));
		if (*prettyname == '\0')
			strlcpy(prettyname, "---", sizeof(prettyname));
	}

	if (rtm->rtm_rmx.rmx_hopcount)
		cout << "HopCount: " << rtm->rtm_rmx.rmx_hopcount << endl;
	if (rtm->rtm_rmx.rmx_rtt)
		cout << "Roundtrip Time: " << rtm->rtm_rmx.rmx_rtt << endl;
	if (rtm->rtm_rmx.rmx_weight)
		cout << "Weight: " << rtm->rtm_rmx.rmx_weight << endl;
	
	cout << "PrettyName: " << prettyname << endl;


	/* printf("%*.*s", wid_if, wid_if, prettyname); */

	if (rtm->rtm_rmx.rmx_expire) {
		time_t expire_time;

		if ((expire_time =
		    rtm->rtm_rmx.rmx_expire - uptime.tv_sec) > 0)
		    cout << "Expire: " << expire_time << endl;
			/* printf(" %*d", wid_expire, (int)expire_time); */
	}

	/* putchar('\n'); */
}


char *
ipx_print(struct sockaddr *sa)
{
	u_short port;
	struct servent *sp = 0;
	const char *net = "", *host = "";
	char *p;
	u_char *q;
	struct ipx_addr work = ((struct sockaddr_ipx *)sa)->sipx_addr;
	static char mybuf[50];
	char cport[10], chost[15], cnet[15];

	port = ntohs(work.x_port);

	if (ipx_nullnet(work) && ipx_nullhost(work)) {

		if (port) {
			if (sp)
				sprintf(mybuf, "*.%s", sp->s_name);
			else
				sprintf(mybuf, "*.%x", port);
		} else
			sprintf(mybuf, "*.*");

		return (mybuf);
	}

	if (ipx_wildnet(work))
		net = "any";
	else if (ipx_nullnet(work))
		net = "*";
	else {
		q = work.x_net.c_net;
		sprintf(cnet, "%02x%02x%02x%02x",
			q[0], q[1], q[2], q[3]);
		for (p = cnet; *p == '0' && p < cnet + 8; p++)
			continue;
		net = p;
	}

	if (ipx_wildhost(work))
		host = "any";
	else if (ipx_nullhost(work))
		host = "*";
	else {
		q = work.x_host.c_host;
		sprintf(chost, "%02x%02x%02x%02x%02x%02x",
			q[0], q[1], q[2], q[3], q[4], q[5]);
		for (p = chost; *p == '0' && p < chost + 12; p++)
			continue;
		host = p;
	}

	if (port) {
		if (strcmp(host, "*") == 0)
			host = "";
		if (sp)
			snprintf(cport, sizeof(cport),
				"%s%s", *host ? "." : "", sp->s_name);
		else
			snprintf(cport, sizeof(cport),
				"%s%x", *host ? "." : "", port);
	} else
		*cport = 0;

	snprintf(mybuf, sizeof(mybuf), "%s.%s%s", net, host, cport);
	return(mybuf);
}

char *
ipx_phost(struct sockaddr *sa)
{
	struct sockaddr_ipx *sipx = (struct sockaddr_ipx *)sa;
	struct sockaddr_ipx work;
	static union ipx_net ipx_zeronet;
	char *p;

	work = *sipx;

	work.sipx_addr.x_port = 0;
	work.sipx_addr.x_net = ipx_zeronet;
	p = ipx_print((struct sockaddr *)&work);
	if (strncmp("*.", p, 2) == 0) p += 2;

	return(p);
}

void
upHex(char *p0)
{
	char *p = p0;

	for (; *p; p++)
		switch (*p) {

		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			*p += ('A' - 'a');
			break;
		}
}
	

static const char *
fmt_sockaddr(struct sockaddr *sa, struct sockaddr *mask, int flags)
{
	static char workbuf[128];
	const char *cp;

	if (sa == NULL)
		return ("null");

	switch(sa->sa_family) {
	case AF_INET:
	    {
		struct sockaddr_in *sockin = (struct sockaddr_in *)sa;

		if ((sockin->sin_addr.s_addr == INADDR_ANY) &&
			mask &&
			ntohl(((struct sockaddr_in *)mask)->sin_addr.s_addr)
				==0L)
				cp = "default" ;
		else if (flags & RTF_HOST)
			cp = routename(sockin->sin_addr.s_addr);
		else if (mask)
			cp = netname(sockin->sin_addr.s_addr,
			    ((struct sockaddr_in *)mask)->sin_addr.s_addr);
		else
			cp = netname(sockin->sin_addr.s_addr, INADDR_ANY);
		break;
	    }

#ifdef INET6
	case AF_INET6:
	    {
		struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)sa;

		/*
		 * The sa6->sin6_scope_id must be filled here because
		 * this sockaddr is extracted from kmem(4) directly
		 * and has KAME-specific embedded scope id in
		 * sa6->sin6_addr.s6_addr[2].
		 */
		in6_fillscopeid(sa6);

		if (flags & RTF_HOST)
		    cp = routename6(sa6);
		else if (mask)
		    cp = netname6(sa6,
				  &((struct sockaddr_in6 *)mask)->sin6_addr);
		else {
		    cp = netname6(sa6, NULL);
		}
		break;
	    }
#endif /*INET6*/

	case AF_IPX:
	    {
		struct ipx_addr work = ((struct sockaddr_ipx *)sa)->sipx_addr;
		if (ipx_nullnet(satoipx_addr(work)))
			cp = "default";
		else
			cp = ipx_print(sa);
		break;
	    }
	case AF_APPLETALK:
	    {
	    /* TODO: */
	    /*
		if (!(flags & RTF_HOST) && mask)
			cp = atalk_print2(sa,mask,9);
		else
			cp = atalk_print(sa,11);
		*/
		break;
	    }
	case AF_NETGRAPH:
	    {
		strlcpy(workbuf, ((struct sockaddr_ng *)sa)->sg_data,
		        sizeof(workbuf));
		cp = workbuf;
		break;
	    }

	case AF_LINK:
	    {
		struct sockaddr_dl *sdl = (struct sockaddr_dl *)sa;

		if (sdl->sdl_nlen == 0 && sdl->sdl_alen == 0 &&
		    sdl->sdl_slen == 0) {
			(void) sprintf(workbuf, "link#%d", sdl->sdl_index);
			cp = workbuf;
		} else
			switch (sdl->sdl_type) {

			case IFT_ETHER:
			case IFT_L2VLAN:
			case IFT_BRIDGE:
				if (sdl->sdl_alen == ETHER_ADDR_LEN) {
					cp = ether_ntoa((struct ether_addr *)
					    (sdl->sdl_data + sdl->sdl_nlen));
					break;
				}
				/* FALLTHROUGH */
			default:
				cp = link_ntoa(sdl);
				break;
			}
		break;
	    }

	default:
	    {
		u_char *s = (u_char *)sa->sa_data, *slim;
		char *cq, *cqlim;

		cq = workbuf;
		slim =  sa->sa_len + (u_char *) sa;
		cqlim = cq + sizeof(workbuf) - 6;
		cq += sprintf(cq, "(%d)", sa->sa_family);
		while (s < slim && cq < cqlim) {
			cq += sprintf(cq, " %02x", *s++);
			if (s < slim)
			    cq += sprintf(cq, "%02x", *s++);
		}
		cp = workbuf;
	    }
	}

	return (cp);
}

static void
p_flags(int f, const char *format)
{
	/* printf(format, fmt_flags(f)); */
}

static const char *
fmt_flags(int f)
{
	static char name[33];
	char *flags;
	struct bits *p = bits;

	for (flags = name; p->b_mask; p++)
		if (p->b_mask & f)
			*flags++ = p->b_val;
	*flags = '\0';
	return (name);
}


UNIX_IPRoute::UNIX_IPRoute(void)
{
}

UNIX_IPRoute::~UNIX_IPRoute(void)
{
}


Boolean UNIX_IPRoute::getName(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_NAME, String(pRoute.destination));
	return true; 
}

Boolean UNIX_IPRoute::getCaption(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_CAPTION, String(pRoute.destination));
    return true;
}

Boolean UNIX_IPRoute::getDescription(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_DESCRIPTION, getHandle());
    return true;
}

Boolean UNIX_IPRoute::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_INSTANCE_ID, getHandle());
    return true;
}

Boolean UNIX_IPRoute::getInstallDate(CIMProperty &p) const
{
	struct tm* clock;			// create a time structure
	struct stat attrib;			// create a file attribute structure
	stat("/mnt", &attrib);		// get the attributes mnt
	clock = gmtime(&(attrib.st_birthtime));	// Get the last modified time and put it into the time structure

	p = CIMProperty(PROPERTY_INSTALL_DATE, CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			));
    return true;
}

Boolean UNIX_IPRoute::getServiceName(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_SERVICE_NAME, String(""));
   return true;
}

Boolean UNIX_IPRoute::getServiceCreationClassName(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_SERVICE_CREATION_CLASS_NAME, String("CIM_ForwardingService"));
   return true;
}

Boolean UNIX_IPRoute::getAddressType(CIMProperty &p) const
{
   Uint16 addressType;
   if (pRoute.addressType == AF_INET) addressType = 1;
   else if (pRoute.addressType == AF_INET6) addressType = 2;
   else addressType = 0;
   p = CIMProperty(PROPERTY_ADDRESS_TYPE, Uint16(pRoute.addressType));
   return true;
}

String UNIX_IPRoute::getAddressType() const
{
   if (pRoute.addressType == AF_INET) return String("1");
   else if (pRoute.addressType == AF_INET6) return String("2");
   else return String("0");
}

String UNIX_IPRoute::getIPDestinationAddress() const
{
   return String(pRoute.destination);
}

String UNIX_IPRoute::getIPDestinationMask() const
{
   return String(pRoute.netmask);
}

String UNIX_IPRoute::getServiceName() const
{
   return String("");
}

String UNIX_IPRoute::getServiceCreationClassName() const
{
   return String("CIM_ServiceCreationClassName");
}

Boolean UNIX_IPRoute::getDestinationAddress(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_DESTINATION_ADDRESS, getIPDestinationAddress());
   return true;
}

Boolean UNIX_IPRoute::getDestinationMask(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_DESTINATION_MASK, String(pRoute.netmask));
   return true;
}

Boolean UNIX_IPRoute::getIPDestinationAddress(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_IP_DESTINATION_ADDRESS, String(pRoute.destination));
   return true;
}

Boolean UNIX_IPRoute::getIPDestinationMask(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_IP_DESTINATION_MASK, String(pRoute.netmask));
   return true;
}

Boolean UNIX_IPRoute::getNextHop(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_NEXT_HOP, String(pRoute.gateway));
   return true;
}

Boolean UNIX_IPRoute::getIsStatic(CIMProperty &p) const
{
   p = CIMProperty(PROPERTY_IS_STATIC, Boolean(true));
   return true;
}

String UNIX_IPRoute::getHandle() const
{
	String handle;
	handle.append(pRoute.destination);
	handle.append("-");
	handle.append(pRoute.netmask);
	handle.append(" ");
	handle.append(pRoute.gateway);
	return handle;
}

#define __initialize_H
Boolean UNIX_IPRoute::initialize()
{
   int fibs = -1;
   int af = 0;
   routepr(fibs, af, &pRouteTable);
   return true;
}



Boolean UNIX_IPRoute::load(int& pIndex)
{
   if (pRouteTable.buffer == NULL) return false;
   if (pIndex < pRouteTable.length)
   {
   		return p_rt_iterate_sysctl(&pRoute, &pRouteTable);
   }
   return false;
}


Boolean UNIX_IPRoute::find(String& handle)
{
   initialize();
   bool found = false;
   for(int i = 0; load(i); i++)
   {
   		if (String::equal(pRoute.destination,handle))
   		{
   			found = true;
   			break;
   		}
   }
   finalize();
   return found;
}

Boolean UNIX_IPRoute::validateKey(CIMKeyBinding &kb) const
{
	/* Keys  */
	//CreationClassName
	//SystemCreationClassName
	//Name
	//SystemName

	CIMName name = kb.getName();
	if (name.equal(PROPERTY_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SYSTEM_NAME) ||
		name.equal(PROPERTY_SYSTEM_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SERVICE_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SERVICE_NAME) ||
		name.equal(PROPERTY_ADDRESS_TYPE) ||
		name.equal(PROPERTY_IP_DESTINATION_ADDRESS) ||
		name.equal(PROPERTY_IP_DESTINATION_MASK))
	{
		return true;
	}
	return false;
}

void UNIX_IPRoute::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//IPAddress
	if (kb.getName().equal(PROPERTY_IP_DESTINATION_ADDRESS))
	{
		s.append(kb.getValue());
	}
}

#include <CIM_ManagedSystemElement.cpp>