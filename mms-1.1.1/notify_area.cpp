#include "notify_area.hpp"

#include "updater.hpp"
#include "graphics.hpp"

using std::string;

pthread_mutex_t NotifyArea::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

NotifyArea::NotifyArea()
  : status_overlay("notify area"), disabled(false), active_elem(false)
{
  last_index = elements.end();
  index = elements.begin();

  render = S_Render::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  screen_updater->timer.add(TimeElement("update area", 
					boost::bind(&NotifyArea::idle_item, this),
					boost::bind(&NotifyArea::update_item, this)));

  screen_updater->timer.add(TimeElement("status area", 
					boost::bind(&NotifyArea::check_status, this),
					boost::bind(&NotifyArea::print_status, this)));

  curlayer = 0;
}

int NotifyArea::idle_item()
{
  return 5000; // ms
}

void NotifyArea::update_item()
{
  elements_mut.enterMutex();

  if (elements.size() == 0) {
    elements_mut.leaveMutex();
    return;
  }

  std::list<NotifyElement>::iterator iter = index;
  int count = 0;

  do {
	++iter;
	if (iter == elements.end()) {
		count++;
		iter = elements.begin();
	}

	if (iter->active) {
		last_index = index;
		index = iter;
		active_elem = true;
		elements_mut.leaveMutex();
		return;
       }
  } while (iter != last_index && count <= 2);
  active_elem = false;

  elements_mut.leaveMutex();
}

int NotifyArea::check_status()
{
  return 1000; // ms
}

void NotifyArea::print_status()
{
  if (!active_elem)
      return;

/*  render->device->animation_section_begin();
  curlayer = render->device->get_current_layer();
  render->device->animation_section_end(); */

  render->wait_and_aquire();
    
  if (status_overlay.elements.size() > 0)
    status_overlay.partial_cleanup();

  if (index->active)
	index->callback();

 /* render->device->animation_section_begin();
  render->device->animation_fade(0,1,50,curlayer);
  render->device->animation_fade(1,0,50,curlayer);
  render->device->animation_section_end();  */

  render->draw_and_release("notify area", true);

}

void NotifyArea::disable()
{
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.deactivate("status area");
  screen_updater->timer.deactivate("update area");
  render->wait_and_aquire();
  status_overlay.cleanup();
  disabled = true;
  render->image_mut.leaveMutex();
}

void NotifyArea::enable()
{
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.activate("status area");
  screen_updater->timer.activate("update area");
  last_index = elements.end();
  index = elements.begin();
  if (index != elements.end() && !index->active)
    update_item();
  disabled = false;
}

void NotifyArea::add(NotifyElement ne, bool active)
{
  std::cout << "NotifyArea::add " << ne.name << std::endl; 
  ne.active = active;
  elements_mut.enterMutex();
  elements.push_back(ne);
  elements_mut.leaveMutex();
}

void NotifyArea::del(std::string name)
{
  elements_mut.enterMutex();
  for (std::list<NotifyElement>::iterator iter = elements.begin(), end = elements.end();
       iter != end; ++iter)
    if (iter->name == name) {
      elements.erase(iter);
      break;
    }
  elements_mut.leaveMutex();
}

void NotifyArea::activate(const std::string& name)
{
  elements_mut.enterMutex();
  for (std::list<NotifyElement>::iterator iter = elements.begin(), end = elements.end();
       iter != end; ++iter)
    if (iter->name == name) {
      iter->active = true;
      break;
    }
  elements_mut.leaveMutex();
}

void NotifyArea::deactivate(const std::string& name)
{
  elements_mut.enterMutex();
  for (std::list<NotifyElement>::iterator iter = elements.begin(), end = elements.end();
       iter != end; ++iter)
    if (iter->name == name) {
      iter->active = false;
     if (iter == index) {
	update_item();
      }
      break;
    }
  elements_mut.leaveMutex();
}

bool NotifyArea::status(const std::string& name)
{
  elements_mut.enterMutex();
  bool active_element = false;
  for (std::list<NotifyElement>::iterator iter = elements.begin(), end = elements.end();
       iter != end; ++iter)
    if (iter->name == name) {
      active_element = iter->active;
      break;
    }
  elements_mut.leaveMutex();

  return active_element;
}

int NotifyArea::getRealWidth()
{
  Config *conf = S_Config::get_instance();
  int min = conf->p_h_res();
  foreach(GObj* elem, status_overlay.elements) {
    if (min > elem->x) min = elem->x;
  }
  int ret = conf->p_h_res() - min;
  return (ret > width ? ret : width);
}
