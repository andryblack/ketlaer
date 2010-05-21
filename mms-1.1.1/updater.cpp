#include "updater.hpp"

#include "config.hpp"

#include "boost.hpp"

// time
#include <time.h>

#include <string>

#include <iostream>
#include "common.hpp"
using std::string;
using std::list;
using namespace time_helper;
#define min_wait_time 250
#define max_wait_time 1000


pthread_mutex_t ScreenUpdater::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t BackgroundUpdater::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Updater::Updater()
  : progressbar(Overlay("progressbar")), status_progressbar(0),
    total_progressbar(0), header(""), updater_name("Updater"),
    quitting(false), running(false)
{
  timer.hold_timers();
  /* start the updater thread */
  pthread_create(&thread_run, NULL, (void *(*)(void *))(pre_run), this);
  timer.release_timers();
}

Updater::~Updater(){
  quit();
}


void Updater::activate()
{
  int rc = pthread_mutex_lock(&timer.elements_mut);
  running = true;
  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    timer.release_timers();
}

void Updater::deactivate()
{
  running = false;
}

void Updater::quit()
{
  timer.hold_timers();
  if (quitting){
    timer.release_timers();
    /* fprintf(stderr, "Updater::quit() was called twice\n"); */
    return;
  }
  quitting = true;
  timer.release_timers();
  pthread_join(thread_run, NULL);
}

void Updater::run_once(const boost::function<void (void)>& func)
{
  int rc = pthread_mutex_lock(&timer.elements_mut);
  run_once_functions.push_back(func);
  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    timer.release_timers();
}

void * Updater::pre_run(void * ptr){
  Updater * thisobj = reinterpret_cast<Updater*>(ptr);
  thisobj->run();
  return 0;
}

void Updater::run()
{
  timer.elements_bool = false;
  pthread_mutex_lock(&timer.elements_mut);
  struct timespec ts = compute_interval(max_wait_time);
  int ret;
  uint64_t next_timeout = 0;
  //fprintf(stderr, "Updater thread initted\n");
  while (!quitting ){
    ret = pthread_cond_timedwait(&timer.elements_switch,
         &timer.elements_mut, &ts);
    
    if (!running){
      ts = compute_interval(max_wait_time);
      continue;
    }
    else if (quitting)
      break;
    
    if (ret != ETIMEDOUT && !timer.elements_bool)
      continue; /* we're not finished waiting */
    
    timer.elements_bool = false;
    
    /* If we're here it means that either timer has elapsed for some event
    *  or another thread has added/removed an event */

    std::list<boost::function<void (void)> > old_run_once_functions = run_once_functions;
    run_once_functions.clear();
    pthread_mutex_unlock(&timer.elements_mut);
    foreach (boost::function<void (void)>& func, old_run_once_functions)
      func();
    pthread_mutex_lock(&timer.elements_mut);

    clock_gettime(CLOCK_REALTIME, &ts);
    
    uint64_t ms_now = static_cast<uint64_t>(ts.tv_sec)*1000 + ts.tv_nsec/1000000;
    uint64_t diff = max_wait_time;
    next_timeout = diff;

    /* We might have to call routines that mess with timer.elements.
     * This might lead to undefined behaviours, crashes, and lots of unpleasant stuff.
     * This is why we create a double of timer.elements */

    std::list<TimeElement> old_elements = timer.elements;

    foreach (TimeElement& element, old_elements) {
      if (element.active){

	unsigned int thiselement_id = element.element_id;

	assert (thiselement_id > 0);

        diff = diff_timeval(ts, element.next_time);
	if (diff < 5){ /* less than 5 milliseconds is so marginal that it is not worth waiting till next loop */
	  pthread_mutex_unlock(&timer.elements_mut);
	  element.callback();
	  pthread_mutex_lock(&timer.elements_mut);
	  diff = element.time();
	/*  fprintf(stderr, "Updater '%s':: Returned from Timer element '%s' (wants a %lld ms timeout)\n",
	      updater_name.c_str(),
	      element.name.c_str(), diff);*/ 
	  element.next_time = compute_interval(ts, diff); 

	  /* Let's update the next_time value of the "official" TimeElement if  it is still there */
	  list<TimeElement>::iterator real_element = timer.elements.begin();

	  for (unsigned int t = 0; t < timer.elements.size(); t++){
	    if (real_element->element_id == thiselement_id){
	      real_element->next_time =  element.next_time;
	      break;
	    }
	    real_element++;
	  }
	}
      }
      if (next_timeout > diff)
	next_timeout = diff;
    }
    if (min_wait_time > next_timeout)
      next_timeout = min_wait_time; /* make sure this loop waits at least min_wait_time */
    
    else if (next_timeout > max_wait_time)
      next_timeout = max_wait_time; /* make sure this loop waits no longer than max_wait_time */


    /* fprintf(stderr, "Updater '%s':: Will sleep for %lld ms (timeout at %lld)\n",
	updater_name.c_str(), next_timeout, ms_now + next_timeout); */
    ts = compute_interval(ts, next_timeout);
  }

  /* thread ends here */
  pthread_mutex_unlock(&timer.elements_mut);
}

uint64_t Updater::diff_timeval(const struct timespec& t1, const struct timespec& t2)
{
  uint64_t a, b;
  a = static_cast<uint64_t>(t1.tv_sec)*1000 + t1.tv_nsec/1000000;
  b = static_cast<uint64_t>(t2.tv_sec)*1000 + t2.tv_nsec/1000000;
  if (a > b)
    return 0;
  return b-a;
}

Updater::Timer::Timer(){

  pthread_mutexattr_t mta;

  pthread_mutexattr_init(&mta);

  pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_ERRORCHECK);

  pthread_mutex_init(&elements_mut, &mta);

  pthread_mutexattr_destroy(&mta); /* we don't need it anymore */

  pthread_cond_init(&elements_switch, NULL);

  lastelement_id = 0;
}
Updater::Timer::~Timer(){
  pthread_mutex_destroy(&elements_mut);
  pthread_cond_destroy(&elements_switch);
}

void Updater::Timer::hold_timers(){
  int rc = pthread_mutex_lock(&elements_mut);
  if (rc != 0){
    fprintf(stderr, "Updater class: Could not lock elements_mut\n");
    fprintf(stderr, "Updater class: Error code number %d\n", rc);
    fprintf(stderr, "Updater class: Please report this bug\n");
    assert(false);
  }
}

void Updater::Timer::release_timers(){
  elements_bool = true;
  pthread_cond_broadcast(&elements_switch); /* wakes thread up */
  int rc = pthread_mutex_unlock(&elements_mut);
  if (rc != 0){
    fprintf(stderr, "Updater class: We're unlocking elements_mut, which wasn't previously locked\n");
    fprintf(stderr, "Updater class: Please report this bug\n");
    assert(false);
  }

}

void Updater::Timer::add(TimeElement te)
{
  te.next_time = compute_interval(te.time());
  int rc = pthread_mutex_lock(&elements_mut);
  te.element_id = ++lastelement_id;
  elements.push_back(te);
  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    release_timers();
}

void Updater::Timer::del(string name)
{
  int rc = pthread_mutex_lock(&elements_mut);

  for (list<TimeElement>::iterator iter = elements.begin(), end = elements.end();
       iter != end; ++iter)
    if (iter->name == name) {
      elements.erase(iter);
      break;
    }

  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    release_timers();
}

void Updater::Timer::activate(const string& name)
{

  int rc = pthread_mutex_lock(&elements_mut);

  foreach (TimeElement& element, elements)
    if (element.name == name) {
      element.next_time = compute_interval(element.time());
      element.active = true;
      break;
    }
  
  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    release_timers();
}

void Updater::Timer::deactivate(const string& name)
{
  int rc = pthread_mutex_lock(&elements_mut);

  foreach (TimeElement& element, elements)
    if (element.name == name) {
      element.active = false;
      if (element.cleanup != 0) {
	element.cleanup();
      }
      break;
    }

  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    release_timers();
}

void Updater::Timer::run_and_deactivate(const string& name)
{
  hold_timers();

  foreach (TimeElement& element, elements)
    if (element.name == name) {
      element.callback();
      element.active = false;
      break;
    }

  release_timers();
}

bool Updater::Timer::status(const string& name)
{
  int rc = pthread_mutex_lock(&elements_mut);
  bool active_element = false;
  foreach (TimeElement& element, elements)
    if (element.name == name) {
      active_element = element.active;
      break;
    }
  if (rc != EDEADLK) /* we unlock and signal only if this thread has locked the mutex once */
    release_timers();
  return active_element;
}

// trigger


void Updater::Trigger::hold_timers(){
  pthread_mutex_lock(&elements_mut);
}

void Updater::Trigger::release_timers(){
  pthread_mutex_unlock(&elements_mut);
}


void Updater::Trigger::add(const TriggerElement& te)
{
  hold_timers();
  elements.push_back(te);
  release_timers();
}

void Updater::Trigger::del(const string& name)
{
  hold_timers();

  for (list<TriggerElement>::iterator iter = elements.begin(), end = elements.end();
       iter != end; ++iter)
    if (iter->name == name) {
      if (iter->cleanup)
	iter->cleanup();
      elements.erase(iter);
      break;
    }
  release_timers();
}

void Updater::Trigger::del_all()
{
  hold_timers();
  elements.clear();
  release_timers();
}

void Updater::Trigger::trigger_input(const string& trigger)
{
  if (!enabled)
    return;

  hold_timers();

  foreach (TriggerElement& element, elements) {
    if (element.triggers.size() == 1 && element.triggers.front().empty()) { // trigger
								      // on all input
      element.trigger();
    } else {
      foreach (string& the_trigger, element.triggers)
	if (the_trigger == trigger) {
	  element.trigger();
	  break;
	}
    }
  }

  release_timers();
}

void Updater::Trigger::enable()
{
  enabled = true;
}

void Updater::Trigger::disable()
{
  enabled = false;
}

Updater::Trigger::Trigger()
  : enabled(true){

  pthread_mutex_init(&elements_mut, NULL);
}
Updater::Trigger::~Trigger(){
  pthread_mutex_destroy(&elements_mut);
}

BackgroundUpdater::BackgroundUpdater(){
  updater_name = "BackgroundUpdater";
}

void BackgroundUpdater::set_nicelevel()
{
  Config *conf = S_Config::get_instance();

  if (conf->p_priority_change())
    nice(10); //Lower priority to keep audio from skipping
}

ScreenUpdater::ScreenUpdater(){
  updater_name = "ScreenUpdater";
}

void ScreenUpdater::set_nicelevel()
{
  Config *conf = S_Config::get_instance();

  if (conf->p_priority_change())
    nice(3); //Lower priority to keep audio from skipping
}
