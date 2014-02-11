#include "zfs_client.h"
#define B_TRUE	true
#define B_FALSE false
#include <strings.h>
#include <unistd.h>
#include <vm/vm.h>
#include <libnvpair.h>
#include <zfs_context.h>
#include <libzfs.h>
#include <iostream>

zfs_client::zfs_client()
{
	
}

zfs_client::~zfs_client()
{

}

uint64_t getProperty(char* target, zfs_prop_t prop)
{
	libzfs_handle_t *zhandle = libzfs_init();
	zfs_handle_t *zvhandle = zfs_open(zhandle, target, ZFS_TYPE_FILESYSTEM);
	uint64_t value = zfs_prop_get_int(zvhandle, ZFS_PROP_COMPRESSION);
	zfs_close(zvhandle);
	libzfs_fini(zhandle);
	return value;
}

time_t zfs_client::getCreationDate(char* target) const
{
	libzfs_handle_t *zhandle = libzfs_init();
	zfs_handle_t *zvhandle = zfs_open(zhandle, target, ZFS_TYPE_FILESYSTEM);
	time_t value = zfs_prop_get_int(zvhandle, ZFS_PROP_CREATION);
	zfs_close(zvhandle);
	libzfs_fini(zhandle);
	return value;
}

bool zfs_client::getCompressed(char* target) const
{
    return getProperty(target, ZFS_PROP_COMPRESSION) != ZIO_COMPRESS_OFF;
}


bool zfs_client::getIsolated(char* target) const
{
    return getProperty(target, ZFS_PROP_ZONED) == B_TRUE;
}


bool zfs_client::getExecutable(char* target) const
{
    return getProperty(target, ZFS_PROP_EXEC) == B_TRUE;
}


bool zfs_client::getChecksum(char* target) const
{
    return getProperty(target, ZFS_PROP_EXEC) == B_TRUE;
}



bool zfs_client::getReadOnly(char* target) const
{
	return getProperty(target, ZFS_PROP_READONLY) == B_TRUE;
}

bool zfs_client::getDeduplicationEnabled(char* target) const
{
    return getProperty(target, ZFS_PROP_DEDUP) == B_TRUE;
}    