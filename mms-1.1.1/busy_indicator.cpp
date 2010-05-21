#include "busy_indicator.hpp"

#include "updater.hpp"
#include "graphics.hpp"

pthread_mutex_t BusyIndicator::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

BusyIndicator::BusyIndicator()
  : status_overlay("status led"), status(IDLE), disabled(false)
{
  conf = S_Config::get_instance();
  render = S_Render::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  if (conf->p_busy_indicator())
    screen_updater->timer.add(TimeElement("status indicator", boost::bind(&BusyIndicator::check_status, this),
					  boost::bind(&BusyIndicator::print_status, this)));
}

int BusyIndicator::check_status()
{
  return 200; // ms
}

void BusyIndicator::print_status()
{
  if (!disabled) {

    if (status == old_status) // nothing to draw
      return;

    render->wait_and_aquire();

    if (status_overlay.elements.size() > 0)
      status_overlay.partial_cleanup();

    if (status == IDLE)
      status_overlay.add(new PObj("idle.png", conf->p_h_res()-30, 5, 1, NOSCALING));
    else if (status == BUSY)
      status_overlay.add(new PObj("busy.png", conf->p_h_res()-30, 5, 1, NOSCALING));
    else
      assert(false); // we should never end up here

    old_status = status;

    render->draw_and_release("status led", true);
  }
}

void BusyIndicator::idle()
{
  if (conf->p_busy_indicator())
    status = IDLE;
}

void BusyIndicator::busy()
{
  if (conf->p_busy_indicator()) {
    status = BUSY;
    print_status();
  }
}

void BusyIndicator::busy_no_print()
{
  if (conf->p_busy_indicator())
    status = BUSY;
}

void BusyIndicator::disable()
{
  disabled = true;
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.deactivate("status indicator");
  render->wait_and_aquire();
  status_overlay.cleanup();
  render->image_mut.leaveMutex();
}

void BusyIndicator::enable()
{
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.activate("status indicator");
  disabled = false;
}
