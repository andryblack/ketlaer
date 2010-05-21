#ifndef RESOLUTION_HPP
#define RESOLUTION_HPP

#include "singleton.hpp"

#include "boost.hpp"

#include <vector>

class ResolutionManagement
{
public:
  typedef boost::function<void ()> resolution_callback_func;

  void register_callback(const resolution_callback_func& res_func);
  void resolution_change();

  static pthread_mutex_t singleton_mutex;

private:

  std::vector<resolution_callback_func> callbacks;
};

typedef Singleton<ResolutionManagement> S_ResolutionManagement;

#endif
