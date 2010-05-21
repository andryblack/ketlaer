#include "config.h"

#include "touch.hpp"

#include "updater.hpp"

#include <iostream>

pthread_mutex_t Touch::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

TouchArea::TouchArea(const rect& area, int l, const callback_func& func)
  : r(area), layer(l), callback(func)
{}

bool sort_areas(const TouchArea& area1, const TouchArea& area2)
{
  return area1.layer > area2.layer;
}

Touch::Touch()
  : callback(0), enabled(true)
{}

void Touch::register_area(const TouchArea& area)
{
#ifdef use_mouse
  areas.push_back(area);
  //  std::cout << "registering an area" << areas.size() << std::endl;
#endif
}

void Touch::got_input(int x, int y)
{
  // make sure the areas are sorted by layer
  std::sort(areas.begin(), areas.end(), sort_areas);

  foreach (TouchArea& area, areas) {
    if (area.r.intersects(x, y)) {
      //      std::cout << "registered a callback" << std::endl;
      callback = area.callback;
      break;
    }
  }
}

bool Touch::run_callback()
{
  if (callback != 0 && enabled) {
    //    std::cout << "running callback" << std::endl;
    callback_func the_callback = callback;
    callback = 0;
    the_callback();
    return true;
  } else
    return false;
}

void Touch::clear()
{
#ifdef use_mouse
//   std::cout << "clearing touch areas" << std::endl;
  areas.clear();
#endif
}
