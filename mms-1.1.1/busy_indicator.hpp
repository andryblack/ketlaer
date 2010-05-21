#ifndef BUSY_INDICATOR_HPP
#define BUSY_INDICATOR_HPP

#include "image.hpp"
#include "config.hpp"
#include "renderer.hpp"

#include "singleton.hpp"

enum IdleStatus { IDLE, BUSY, ERROR };

class BusyIndicator
{
private:
  int check_status();
  void print_status();

  Overlay status_overlay;

  IdleStatus status;
  IdleStatus old_status;
  Config *conf;
  Render *render;

  bool disabled;

public:

  static pthread_mutex_t singleton_mutex;

  // Constructor adds itself to the updater loop
  BusyIndicator();

  void idle();
  void busy();
  void busy_no_print();

  void disable();
  void enable();
};

typedef Singleton<BusyIndicator> S_BusyIndicator;

#endif
