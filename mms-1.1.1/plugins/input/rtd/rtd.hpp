#ifndef LIRC_HPP
#define LIRC_HPP

#include "remote.hpp"

class Rtd : public Remote
{
private:

public:
  void run();
  bool init();

  Rtd();
  ~Rtd();

  std::string name()
  {
    return "rtd";
  }

  void suspend();
  void wake_up();
};


#endif
