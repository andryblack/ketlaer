#ifndef SHUTDOWN_HPP
#define SHUTDOWN_HPP

#include "module.hpp"
#include "singleton.hpp"

#include <string>

class Shutdown : public Module
{
private:
  int check_status();
  void print_status();

  Overlay overlay;

  bool general_exit;
  bool disabled;
  int count;

public:

  static pthread_mutex_t singleton_mutex;

  // Constructor adds itself to the updater loop
  Shutdown();

  Options* get_opts()
  {
    return 0;
  }

  void startup_updater()
  {}

  std::string mainloop();

  bool get_status();
  bool is_enabled();

  void cancel();

  void disable();
  void enable();
};

typedef Singleton<Shutdown> S_Shutdown;

#endif
