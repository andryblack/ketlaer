#include "rtdinp.hpp"

#include "config.h"

#include "gettext.hpp"
#include "config.hpp"
#include "common.hpp"
#include "print.hpp"

#include "global.hpp"

#include <cstdio>

#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <libketlaer.h>
#include <qtkeys.h>

RtdInp::RtdInp()
{
  printf("[RTDINP]init\n");
  init_libketlaer();
  suspended = false;
  stop = false;
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-rtd", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-rtd", nl_langinfo(CODESET));
#endif
}

RtdInp::~RtdInp()
{
  printf("[RTDINP]uninit\n");
  stop = true;
  if (suspended)
    wake_up();
  uninit_libketlaer();
}

bool RtdInp::init()
{
  return true;
}

void RtdInp::suspend()
{
  printf("[INPURTD]suspend\n");
  suspended = true;
  ev_suspended.wait();
  ev_suspended.reset();
}

void RtdInp::wake_up()
{
  printf("[RTDINP]wakeup\n");
  if (suspended) {
    suspended = false;
    ev_wakeup.signal();
  }
}

static struct {
  int   key;
  char *str;
} rtd_keymap[] = {
  /*
  {Key_0,             "0"        },
  {Key_1,             "1"        },
  {Key_2,             "2"        },
  {Key_3,             "3"        },
  {Key_4,             "4"        },
  {Key_5,             "5"        },
  {Key_6,             "6"        },
  {Key_7,             "7"        },
  {Key_7,             "8"        },
  {Key_9,             "9"        },
  */
  {Key_Up,            "UP"       },
  {Key_Down,          "DOWN"     },
  {Key_Left,          "LEFT"     },
  {Key_Right,         "RIGHT"    },
  {Key_PageUp,        "PGUP"     },
  {Key_PageDown,      "PGDOWN"   },
  {Key_Enter,         "ENTER"    },
  {Key_Home,          "SPACE"    },
  {Key_Backspace,     "END"      },
  {Key_MediaNext,     "9"        },
  {Key_MediaPrevious, "8"        },
  {Key_MediaPlay,     "3"        },
  {Key_MediaStop,     "1"        },
  {Key_VolumeUp,      "f"        },
  {Key_VolumeDown,    "g"        },
  {Key_VolumeMute,    "m"        },
  {Key_PowerOff,      "POWER"    },
  {Key_F9,            "RED"      },
  {Key_F10,           "GREEN"    },
  {Key_F11,           "YELLOW"   },
  {Key_F12,           "BLUE"     },
  {0, 0},
};

void RtdInp::run()
{
  fd_set       set;
  int          fd = ir_getfd();
  timeval      tv;
  int          ret;
  InputMaster *input_master = S_InputMaster::get_instance();
  Input        input;

  while (!stop) {
    FD_ZERO(&set);
    FD_SET(fd, &set);
    tv.tv_sec  = 0;
    tv.tv_usec = 100 * 1000;
    ret = select(fd+1, &set, NULL, NULL, &tv);
    if (suspended) {
      ev_suspended.signal();
      ev_wakeup.wait();
      ev_wakeup.reset();
    }
    else if (ret > 0) {
      int idx = 0, key;
      char *str = NULL;

      key = ir_getkey();
      while(rtd_keymap[idx].str) {
	if (rtd_keymap[idx].key == key) {
	  str = rtd_keymap[idx].str;
	  break;
	}
	idx++;
      }
      if (str) {
        printf("[RTDINP]str = %s\n", str);
	if (find_input(str, input)) {
	  input_master->add_input(input, name());
        }
      }
    }
  }
}
