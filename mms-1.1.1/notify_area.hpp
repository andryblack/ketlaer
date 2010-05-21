#ifndef NOTIFY_AREA_HPP
#define NOTIFY_AREA_HPP

#include "image.hpp"
#include "renderer.hpp"
#include "updater.hpp"

#include "singleton.hpp"

class NotifyElement
{
public:
  typedef boost::function<void (void)> callback_func;

  NotifyElement(const std::string& n, const callback_func& c)
    : name(n), active(true), callback(c)
  {}
  
  std::string name;
  bool active;
  callback_func callback;
  struct timeval next_time;
};


class NotifyArea
{
private:
  // Constructor adds itself to the updater loop

  int check_status();
  void print_status();

  int idle_item();
  void update_item();

  std::list<NotifyElement> elements;
  std::list<NotifyElement>::iterator index;
  std::list<NotifyElement>::iterator last_index;

  Render *render;

  bool disabled;
  bool active_elem;

  int curlayer;

  ost::Mutex elements_mut;

public:

  void add(NotifyElement ne, bool active = true);
  void del(std::string name);

  // for individual elements
  void activate(const std::string& name);
  void deactivate(const std::string& name);
  bool status(const std::string& name);

  static pthread_mutex_t singleton_mutex;

  NotifyArea();

  static const int width = 144;
  static const int height = 76;

  void disable();
  void enable();

  int getRealWidth();

  Overlay status_overlay;
};

typedef Singleton<NotifyArea> S_NotifyArea;

#endif
