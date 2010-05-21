#ifndef TOUCH_HPP
#define TOUCH_HPP

#include "singleton.hpp"
#include "common.hpp"

#include "boost.hpp"

#include <vector>

class TouchArea
{
public:
  rect r;
  int layer;

  typedef boost::function<void (void)> callback_func;
  callback_func callback;

  TouchArea(const rect& area, int layer, const callback_func& func);
};

class Touch
{
private:
  std::vector<TouchArea> areas;

  typedef boost::function<void (void)> callback_func;
  callback_func callback;

public:
  bool enabled;
  static pthread_mutex_t singleton_mutex;

  Touch();

  void register_area(const TouchArea& area);
  void got_input(int x, int y);

  // will run callback if one is queued
  bool run_callback();

  void clear();
};

typedef Singleton<Touch> S_Touch;

#endif
