#ifndef CD_HPP
#define CD_HPP

#include "config.h"

#include "singleton.hpp"

#include <string>

class Cd
{
public:

  static pthread_mutex_t singleton_mutex;

  enum cdstatus { NODISC, TRAYOPEN, NOTREADY, OK };

  // returns true is open was succesful
  bool open();
  void close();
  cdstatus check_cddrive();
  bool data_disc();

  int get_nr_tracks_cd();

  std::string get_device();
  std::string get_mount_point();

  void openclose_cddrive();

  ~Cd();
  Cd();

  int cdrom;

private:

  void cd_set_speed();

  bool cd_speed_set;
};

typedef Singleton<Cd> S_Cd;

#endif
