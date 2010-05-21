#ifndef _MMS_SIGNALS_HPP
#define _MMS_SIGNALS_HPP
#include "singleton.hpp"
#include "version.h"
class mmsSignals{
public:
  static pthread_mutex_t singleton_mutex;
  mmsSignals();

  virtual ~mmsSignals();
  void init();
  void clean_up(bool full_deinit);

private:
  
  bool deinitted;
  volatile bool process_sigs;
  static void * pre_run(void * ptr);
  void run();

  static void crash_handler(int signum, siginfo_t * siginfo, void *context);
  static void sigint_handler(int);

  pthread_t thread_loop;
  sigset_t signal_set;
  void deinit_mms(bool full_deinit);
  bool do_die;

  
};


typedef Singleton<mmsSignals> S_mmsSignals;

#endif
