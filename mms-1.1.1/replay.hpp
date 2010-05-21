#ifndef REPLAY_HPP
#define REPLAY_HPP

#include "input.hpp"
#include "singleton.hpp"

#include <fstream>

class ReplaySaver
{
private:
  std::ofstream file;
  timespec start_time;
  bool file_ok;

public:

  static pthread_mutex_t singleton_mutex;

  ReplaySaver();
  ~ReplaySaver();

  void add_event(const Input& i);
};

typedef Singleton<ReplaySaver> S_ReplaySaver;

#endif
