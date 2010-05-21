#ifndef CLOCK_HPP
#define CLOCK_HPP

// used for #ifdef
#include "config.h"

#include "module.hpp"
#include "options.hpp"
#include "singleton.hpp"
#include "clock_config.hpp"
#include "alarm_config.hpp"

#include <cc++/numbers.h>
#include <string>
#ifdef c_music
class Alarm 
{
public: 
  int dayWeek;
  ost::Time time;

   Alarm();
   Alarm(int day, int hour, int min);
   Alarm(int day, std::string strhour);
   bool operator<(const Alarm& rhs) const;
   bool operator==(const Alarm& rhs) const;
   bool operator!=(const Alarm& rhs) const;
};
#endif
class Clock : public Module
{
  friend class Audio;
  public:

#ifdef c_music
  class ClockOpts : public Options
  {
  public:
    ClockOpts();
    ~ClockOpts();

    void update_playlists();

    std::string alarm()
    {
      return alarm_p->values[alarm_p->pos];
    }

    std::string playlist()
    {
      return playlist_p->values[playlist_p->pos];
    }

    std::vector<Option*> values()
    {
      return val;
    }

    void alarms_mainloop();

  private:
    Option *alarm_p, *playlist_p; 
  };
#endif

  Clock();  

#ifdef c_music
  ClockOpts* get_opts()
  {
    opts.update_playlists();
    return &opts;
  }
#else
 Options* get_opts()
  {
    return 0;
  }
#endif

    void startup_updater()
  {}

    std::string mainloop() 
  {
    return "";
  }

#ifdef c_music
   // GUI ALARMS
   void alarms_mainloop();
#endif

   static pthread_mutex_t singleton_mutex;


private:

    std::string list_font;
    std::pair<int, int> list_size;

#ifdef c_music
    ClockOpts opts;
    
    Alarm next_alarm;
    bool have_alarms;
    bool ringing;
    bool active_alarm;
    std::string old_opts_alarm;
    std::string last_set_alarm;

    std::vector<Alarm> alarms;

    ClockConfig *clock_conf;
    ClockAlarms *clock_alarms;

    void activate_alarm();
    void deactivate_alarm();

    int idle_ring();
    int idle_alarm();
    void check_alarm();
    void check_ring();
    void call_script(const std::string dte);
    ost::Datetime calc_next_alarm(int disp_mins = 0);
    void find_next_alarm();

    void get_alarms();
    void add_alarm(Alarm &nAlarm);
    void delete_alarm(int &pos);
    void save_alarms();

    // GUI ALARMS
    Overlay o;

    int alarm_pos;
    int dow; // date of week
    int hour;
    int min;    

    bool exit_edit_alarms_loop;
    Alarm cur_alarm;
    int int_position;

    bool add_alarm_mainloop(Alarm &alarm); 

    void print_add_alarm();
    void print_edit_alarm(const std::vector<Alarm>& cur_files, const Alarm& position, const int int_position);
    void print_edit_alarm_string_element(const Alarm& r, const Alarm& position, int y);

    void exit_edit_alarms();
    void cleanup();
#endif

#ifdef use_notify_area
    void ShowNotifyArea();
#endif
    std::string doubleZero(int time);  
    std::string dayOfWeek(int date);
    std::string strDatetime(ost::Datetime DT);
};

#endif
