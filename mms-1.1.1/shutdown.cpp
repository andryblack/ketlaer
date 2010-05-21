#include "config.h"

#include "shutdown.hpp"

#include "common.hpp"
#include "gettext.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "input.hpp"

#include "startmenu.hpp"

using std::string;

pthread_mutex_t Shutdown::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Shutdown::Shutdown()
  : overlay("dialog"), general_exit(false), disabled(true), count(10)
{
  render = S_Render::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  screen_updater->timer.add(TimeElement("regressive count", boost::bind(&Shutdown::check_status, this),
					boost::bind(&Shutdown::print_status, this)));
}

int Shutdown::check_status()
{
  return 1000; // ms
}

void Shutdown::print_status()
{
  if (!disabled) {
    print_dialog_step2(overlay);
    if (count > 0) {
      string msg = gettext("Shutdown in");
      msg  += " " + conv::itos(count);
      std::list<string> messages;
      messages.push_back(msg);
      print_dialog_step1(messages, overlay);
    } else if (count <= 0) {
      DialogWaitPrint pdialog(gettext("Shutdown Now"), 3000);
      general_exit = true;
      disable();
      clean_up(true);
    }
    --count;
  }
}

string Shutdown::mainloop()
{
  return "";
}

bool Shutdown::get_status()
{
  return general_exit;
}

bool Shutdown::is_enabled()
{
  return !disabled;
}

void Shutdown::disable()
{
  disabled = true;
  ScreenUpdater *updater = S_ScreenUpdater::get_instance();
  updater->timer.deactivate("regresive count");
  print_dialog_step2(overlay);
}

void Shutdown::enable()
{
  disabled = false;
  ScreenUpdater *updater = S_ScreenUpdater::get_instance();
  updater->timer.activate("regresive count");
  count = 10;
}

void Shutdown::cancel()
{
  if (!disabled) {
    disable();
    print_dialog_step2(overlay);
    DialogWaitPrint pdialog(gettext("Canceled Shutdown"), 1000);
  }
}
