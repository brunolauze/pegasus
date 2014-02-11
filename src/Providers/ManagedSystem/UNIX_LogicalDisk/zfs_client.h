#include <time.h>

class zfs_client
{
public:

  zfs_client();
  ~zfs_client();

  time_t getCreationDate(char* target) const;
  bool getCompressed(char* target) const;
  bool getIsolated(char* target) const;
  bool getExecutable(char* target) const;
  bool getChecksum(char* target) const;
  bool getReadOnly(char* target) const;
  bool getDeduplicationEnabled(char* target) const;
};