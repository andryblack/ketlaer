#ifndef LIRC_HPP
#define LIRC_HPP

#include "remote.hpp"

class RtdInp : public Remote
{
private:

  ost::Event ev_suspended;
  ost::Event ev_wakeup;

  bool suspended;
  bool stop;

public:
  void run();
  bool init();

  RtdInp();
  ~RtdInp();

  std::string name()
  {
    return "rtdinp";
  }

  void suspend();
  void wake_up();
};


#endif
