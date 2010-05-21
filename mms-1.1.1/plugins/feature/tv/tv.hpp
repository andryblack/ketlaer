#ifndef TV_HPP
#define TV_HPP

#include "module.hpp"
#include "options.hpp"

class Tv : public Module
{
 public:
  Tv();

  Options* get_opts()
  {
    return 0;
  }

  void startup_updater()
  {}

  std::string mainloop();
};

#endif
