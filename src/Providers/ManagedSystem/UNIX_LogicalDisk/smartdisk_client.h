
class smartdisk_client
{
public:

  smartdisk_client();
  ~smartdisk_client();

  long getTotalPowerOnHours(char* target) const;
  long getPowerOnHours(char* target) const;

};