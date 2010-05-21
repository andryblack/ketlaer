#ifndef BENCH_HPP
#define BENCH_HPP

#include <iostream>
#include <sys/time.h>

class Bench
{
private:
  timeval timeval1, timeval2;
public:
  void start() { gettimeofday(&timeval1, 0); }
  void stop() { gettimeofday(&timeval2, 0); }
  void print(const std::string& type);
};

#endif
