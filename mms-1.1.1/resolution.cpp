#include "resolution.hpp"

pthread_mutex_t ResolutionManagement::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

void ResolutionManagement::register_callback(const resolution_callback_func& res_func)
{
  callbacks.push_back(res_func);
}

void ResolutionManagement::resolution_change()
{
  foreach (resolution_callback_func& callback, callbacks)
    callback();
}
