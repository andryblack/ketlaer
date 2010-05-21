#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <pthread.h>

template <class T>
class Singleton
{
public:
  static T* get_instance() {
    pthread_mutex_lock(&T::singleton_mutex);
    static T _instance;
    pthread_mutex_unlock(&T::singleton_mutex);
    return &_instance;
  }
private:

  Singleton();
  ~Singleton();
  Singleton(Singleton const&);
  Singleton& operator=(Singleton const&);
};

#endif
