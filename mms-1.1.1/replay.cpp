#include "replay.hpp"

#include "config.hpp"

#include <iostream>
#include <string>

pthread_mutex_t ReplaySaver::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

ReplaySaver::ReplaySaver()
  : file_ok(true)
{
  Config *conf = S_Config::get_instance();
  if (!conf->p_replay_write().empty() && !list_only_contains(conf->p_input(), std::string("replay"))) {
    file.open((conf->p_var_data_dir() + conf->p_replay_write()).c_str());
    clock_gettime(0, &start_time);
  } else
    file_ok = false;
}

ReplaySaver::~ReplaySaver()
{
  if (file_ok)
    file.close();
}

void ReplaySaver::add_event(const Input& i)
{
  if (file_ok) {
    timespec tmp;
    clock_gettime(0, &tmp);
    int sec = tmp.tv_sec-start_time.tv_sec;
    int msec = (tmp.tv_nsec-start_time.tv_nsec)/1000000;
    if (msec < 0) {
      sec -= 1;
      msec += 1000;
    }
    file << sec << "," << msec << ":" << i.to_string() << std::endl;
  }
}

