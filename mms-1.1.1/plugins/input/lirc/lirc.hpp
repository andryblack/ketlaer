#ifndef LIRC_HPP
#define LIRC_HPP

#include "remote.hpp"

class Lirc : public Remote
{
private:
  int irsd, number_of_keypresses;
  int pid_lircd;
  volatile bool running;
  bool flush; //used after returning from external programs
              //if set, all lirc events in the first 200ms time interval
             //are discarded; it's a 1 shot flag, it gets reset after the said time
  void connect();

  // hack to fix lirc giving the same command twice

  unsigned long long  next_time;
  unsigned long long  this_time;

  int timeinterv;
  int lirc_rate;
  int lirc_fast;

  void reinit();
  int read_lirc(char*, int);
public:
  void run();
  bool init();

  Lirc();
  ~Lirc();

  std::string name()
  {
    return "lirc";
  }

  void suspend();
  void wake_up();
};


#endif
