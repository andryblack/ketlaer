#include "cd.hpp"

#include "config.hpp"
#include "global_options.hpp"
#include "print.hpp"

// mypair
#include "common.hpp"
#include "common-output.hpp"
#include "global.hpp"

#include "gettext.hpp"

#include "boost.hpp"

// cd
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "kernelheaders.hpp"

// close
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <list>
#include <string>
#include <iostream>

#include <cassert>

using std::string;
using std::pair;
using std::list;

pthread_mutex_t Cd::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Cd::Cd()
  : cdrom(-1), cd_speed_set(false)
{}

Cd::~Cd()
{
  close();
}

bool Cd::open()
{
  string device = get_device();

  cdrom = ::open(device.c_str(), O_RDONLY | O_NONBLOCK);

  if(cdrom == -1) {
    Print pdialog(gettext("Could not open the dvd/cd drive"),
		  Print::SCREEN);
    return false;
  } else {
    return true;
  }
}

void Cd::close()
{
  if (cdrom != -1) {
    ::close(cdrom);
    cdrom = -1;
  }
}

void Cd::cd_set_speed()
{
  int speed = 4;
  int ret;

  assert(cdrom != -1);

  ret = ioctl(cdrom, CDROM_SELECT_SPEED, speed);

  if(ret == -1 && cd_speed_set == false) {
    Print pdialog(gettext("Could not set drive speed"),
		  Print::SCREEN);
    cd_speed_set = true;
  }

  if(ret == -ENOSYS)
    print_critical(gettext("The drive does not support speed selection"));
}

Cd::cdstatus Cd::check_cddrive()
{
  assert(cdrom != -1);

  struct cdrom_tochdr toch;

  switch(ioctl(cdrom, CDROM_DISC_STATUS)) {

  case CDS_NO_DISC:
    {
      if (!S_Global::get_instance()->autostarting) {
	Print pdialog(Print::SCREEN);
	pdialog.add_line(gettext("There is no disc in the drive"));
	pdialog.print();
      }
    }
    break;
  case CDS_TRAY_OPEN:
    {
      if (!S_Global::get_instance()->autostarting) {
	Print pdialog(Print::SCREEN);
	pdialog.add_line(gettext("The tray is open"));
	pdialog.add_line("");
	pdialog.add_line(gettext("Please close it"));
	pdialog.print();
      }
    }
    break;
  case CDS_DRIVE_NOT_READY:
    {
      if (!S_Global::get_instance()->autostarting) {
	Print pdialog(Print::SCREEN);
	pdialog.add_line(gettext("The drive is not ready"));
	pdialog.add_line("");
	pdialog.add_line(gettext("Please try again later"));
	pdialog.print();
      }
    }
    break;
  default:
    {
      if (ioctl(cdrom, CDROMREADTOCHDR, &toch) < 0) {
	return NODISC;
      } else{
	/* set the speed so it's doesn't sound like it's a turbo jet starting! */
	    cd_set_speed();
	    return OK;
      }
    }
  }

  return NODISC;
}

bool Cd::data_disc()
{
  switch(ioctl(cdrom, CDROM_DISC_STATUS)) {

  case CDS_NO_DISC:
  case CDS_TRAY_OPEN:
  case CDS_DRIVE_NOT_READY:
  case CDS_AUDIO:
    return false;
    break;
  case CDS_NO_INFO: // FUBARED DRIVES
  case CDS_DISC_OK:
  case CDS_DATA_1:
  case CDS_DATA_2:
  case CDS_XA_2_1:
  case CDS_XA_2_2:
    return true;
    break;
  default:
    return false;
    break;
  }
}

void Cd::openclose_cddrive()
{
  Config *conf = S_Config::get_instance();

  if (conf->p_tray_open()) {
    system((conf->p_eject_path() + " " + conf->p_eject_closeopts() + " " + get_device()).c_str());
    conf->set_tray_open(false);
  } else {
    system((conf->p_eject_path() + " " + conf->p_eject_openopts() + " " + get_device()).c_str());
    conf->set_tray_open(true);
  }
}

int Cd::get_nr_tracks_cd()
{
  struct cdrom_tochdr tochdr;

  assert(cdrom != -1);

  /* fetch the table of contents */
  if (ioctl(cdrom, CDROMREADTOCHDR, &tochdr) == -1)
    return -1;

  return tochdr.cdth_trk1;
}

typedef pair<string, string> path_and_name;

string Cd::get_device()
{
  GlobalOptions *go = S_GlobalOptions::get_instance();
  Config *conf = S_Config::get_instance();

  string device_name = vector_lookup(go->cd_device->values, go->cd_device->pos);

  foreach (path_and_name pair, conf->p_cdname())
    if (pair.second == device_name)
      return pair.first;

  assert(false);
}

string Cd::get_mount_point()
{
  Config *conf = S_Config::get_instance();

  string device = get_device();

  foreach (path_and_name pair, conf->p_cdpath())
    if (pair.first == device)
      return pair.second;

  assert(false);
}

