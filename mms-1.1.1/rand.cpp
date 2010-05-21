#include "rand.hpp"

#include <time.h>

// random
#define srand48 srand

pthread_mutex_t Rand::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Rand::Rand()
{
  srand(time(0));
}
