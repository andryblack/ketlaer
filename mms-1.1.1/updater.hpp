#ifndef UPDATER_HPP
#define UPDATER_HPP

#include "plugins/output/image.hpp"
#include "singleton.hpp"

#include "boost.hpp"

#include <cc++/thread.h>

#include <time.h>

#include <list>
#include <string>
#include <cassert>

class TimeElement
{
public:
  typedef boost::function<void (void)> callback_func;
  typedef boost::function<int (void)> time_func;
  typedef boost::function<void (void)> cleanup_func;

  TimeElement(const std::string& n, const time_func& t, const callback_func& c)
    : name(n), element_id(0), active(true), time(t), callback(c), cleanup(0)
  {}

  TimeElement(const std::string& n, const time_func& t, const callback_func& c,
	      const cleanup_func& cl)
    : name(n), element_id(0), active(true), time(t), callback(c), cleanup(cl)
  {}

  // FIXME: add ability to add function which only runs once

  std::string name;
  unsigned int element_id;
  bool active;
  time_func time;
  callback_func callback;
  cleanup_func cleanup;
  struct timespec next_time;
};

class TriggerElement
{
public:
  typedef boost::function<void (void)> trigger_func;
  typedef boost::function<void (void)> cleanup_func;

  TriggerElement(const std::string& n, const std::list<std::string>& trig,
		 const trigger_func& t, const cleanup_func& c)
    : name(n), triggers(trig), trigger(t), cleanup(c)
  {
    assert(t != 0);
    // clean_up can be empty if it's time controlled instead
  }

  std::string name;
  std::list<std::string> triggers;
  trigger_func trigger;
  cleanup_func cleanup;
};

class Updater //: public ost::Thread
{
public:
  Updater();
  virtual ~Updater();

  void activate();
  void deactivate();
  void quit();
  void run_once(const boost::function<void (void)>& func);

  class Timer
  {
  public:
    Timer();
    ~Timer();
    void add(TimeElement te);  /* this can be executed by code launched by Timer */
    void del(std::string name); /* this can be executed by code launched by Timer */
    void hold_timers();
    void release_timers();

    // for individual elements
    void activate(const std::string& name); /* this can be executed by code launched by Timer */
    void deactivate(const std::string& name); /* this can be executed by code launched by Timer */
    void run_and_deactivate(const std::string& name); /* this can be executed by code launched by Timer */
    bool status(const std::string& name); /* this can be executed by code launched by Timer */

    struct timeval current_time;

    std::list<TimeElement> elements;

    pthread_mutex_t elements_mut;
    pthread_cond_t  elements_switch;
    volatile bool   elements_bool;
  private:
    uint32_t lastelement_id;

    //struct timeval set_timeout(const struct timeval& t, int wait);
  };

  class Trigger
  {
  public:
    Trigger();
    ~Trigger();

    void add(const TriggerElement& te);
    // cleanup
    void del(const std::string& name);
    void del_all();
    // while running
    void trigger_input(const std::string& trigger);
    void hold_timers();
    void release_timers();

    void disable();
    void enable();

    pthread_mutex_t elements_mut;
  private:
    bool enabled;

    //ost::Mutex elements_mut;

    std::list<TriggerElement> elements;
  };

  Timer timer;
  Trigger trigger;

  Overlay progressbar;
  int status_progressbar;
  int total_progressbar;
  std::string header;
  std::string updater_name;
private:

  uint64_t diff_timeval(const timespec&, const timespec&);
  std::list<boost::function<void (void)> > run_once_functions;

  static void * pre_run(void * ptr);

  void run();

  virtual void set_nicelevel() = 0;
  pthread_t thread_run;

  volatile bool quitting;
  bool running;
};

class BackgroundUpdater : public Updater
{
public:
  static pthread_mutex_t singleton_mutex;
  BackgroundUpdater();

private:
  void set_nicelevel();
};

class ScreenUpdater : public Updater
{
public:
  static pthread_mutex_t singleton_mutex;
  ScreenUpdater();

private:
  void set_nicelevel();
};

typedef Singleton<ScreenUpdater> S_ScreenUpdater;
typedef Singleton<BackgroundUpdater> S_BackgroundUpdater;

#endif
