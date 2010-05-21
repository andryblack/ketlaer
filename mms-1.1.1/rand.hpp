#ifndef RAND_HPP
#define RAND_HPP

#include "singleton.hpp"

#include <stdlib.h>
#include <algorithm>

class Rand
{
public:

  static pthread_mutex_t singleton_mutex;

  Rand();

  inline int number(int max) { return rand()/(RAND_MAX/max+1); }
};

typedef Singleton<Rand> S_Rand;

#endif
