#ifndef EVDEV_HPP
#define EVDEV_HPP

#include "remote.hpp"

#include "evdev_config.hpp"

class Evdev : public Remote
{
private:
  int evsd, number_of_keypresses;
  int pid_evdev;

  EvdevConfig *evdev_conf;

  volatile bool exit;

public:
  void run();
  bool init();

  void suspend();
  void wake_up();

  std::string name()
  {
    return "evdev";
  }

  Evdev();
  ~Evdev();
};

#endif
