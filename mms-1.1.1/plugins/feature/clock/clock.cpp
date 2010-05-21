// used for #ifdef
#include "config.h"

// Modules Deps
#ifdef c_music
#include "audio.hpp"
#endif

#include "clock.hpp"
#include "clock_config.hpp"
#include "alarm_config.hpp"

#include "config.hpp"
#include "common.hpp"
#include "common-feature.hpp"
#include "image.hpp"
#include "renderer.hpp"
#include "gettext.hpp"
#include "graphics.hpp"
#include "plugins.hpp"

#include <cc++/numbers.h>
#include <string>
#include <list>
#include <vector>

#include "boost.hpp"

using std::vector;
using std::string;

#ifdef c_music
Alarm::Alarm()
{
  ost::Date date;
  dayWeek = date.getDayOfWeek();
}

Alarm::Alarm(int day, int hour, int min) {
   dayWeek = day;
   time  = ost::Time (hour, min, 0);
}

Alarm::Alarm(int day, string strhour) {
   dayWeek = day;
   int i = strhour.rfind(':');
   int hour = conv::atoi(strhour.substr(0, i+1));
   int min  = conv::atoi(strhour.substr(i+1));
   time = ost::Time (hour, min, 0);
}

bool Alarm::operator<(const Alarm& rhs) const
{
  if (this->dayWeek == rhs.dayWeek) 
    return (this->time.getTime() < rhs.time.getTime());
  return this->dayWeek < rhs.dayWeek;
}

bool Alarm::operator==(const Alarm& rhs) const
{
  return this->dayWeek == rhs.dayWeek && this->time.getTime() == rhs.time.getTime();
}

bool Alarm::operator!=(const Alarm& rhs) const
{
  return this->dayWeek != rhs.dayWeek || this->time.getTime() != rhs.time.getTime();
}
#endif

pthread_mutex_t Clock::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;
#ifdef c_music
Clock::Clock() : have_alarms(false), ringing(false), active_alarm(false),
	o(Overlay("add alarm")), alarm_pos(0), min(0),hour(0)
#else
Clock::Clock()
#endif
{

#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-clock", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-clock", nl_langinfo(CODESET));
#endif

#ifdef use_notify_area
  nArea->add(NotifyElement("Clock", boost::bind(&Clock::ShowNotifyArea, this))); 
#endif

  list_font = "Vera/" + conv::itos(resolution_dependant_font_size(16, conf->p_v_res()));
  list_size = string_format::calculate_string_size("abcltuwHPMjJg", list_font);

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

#ifdef c_music

  clock_conf = S_ClockConfig::get_instance();
  clock_alarms = S_ClockAlarms::get_instance();

  get_alarms();
  find_next_alarm();
  check_alarm();
  check_ring();

  screen_updater->timer.add(TimeElement("clock alarm", 
	boost::bind(&Clock::idle_ring, this),
        boost::bind(&Clock::check_ring, this)));

  screen_updater->timer.add(TimeElement("clock alarm", 
	boost::bind(&Clock::idle_alarm, this),
        boost::bind(&Clock::check_alarm, this))); 
#endif
}

#ifdef c_music
int Clock::idle_alarm()
{
  return 1000; // ms
}


int Clock::idle_ring()
{
  ost::Time time;
  return 1000 * (60 - time.getSecond()); // ms
}

void Clock::check_alarm()
{
  if (opts.alarm() == dgettext("mms-clock", "off")) {
    deactivate_alarm();
    return;
  }
  if (!active_alarm) 
    activate_alarm();
}

void Clock::deactivate_alarm()
{
  if (!active_alarm) return;
  call_script("2008-00-00 02:**:**");
  active_alarm = false;
}


void Clock::activate_alarm()
{

  if (!have_alarms) {
    deactivate_alarm();
    return;
  }
  ost::Datetime nextDT = calc_next_alarm(-2);
  string str_date_alarm = strDatetime(nextDT);
  call_script(str_date_alarm);
  active_alarm = true;
}

void Clock::get_alarms() 
{
  std::list<MyPair> cfg_alarms = clock_alarms->p_alarms();

  foreach (MyPair& cur_alarm, cfg_alarms) {
	Alarm nAlarm = Alarm(conv::atoi(cur_alarm.first), cur_alarm.second);
	alarms.push_back(nAlarm);
  }
}

void Clock::find_next_alarm() 
{
  if (S_Config::get_instance()->p_debug_level() > 2)
    std::cout << "Clock::find_next_alarm()" << std::endl;

  have_alarms = (alarms.size() > 0 ? true : false);

  if (!have_alarms) 
	return;

  // Sorting Alarms
  sort(alarms.begin(), alarms.end());
  
  // Current Time
  ost::Date date;
  ost::Time time;
  Alarm new_alarm;
  Alarm currTime =  Alarm(date.getDayOfWeek(), time.getHour(), time.getMinute());

  // Finding Next Alarm
  bool finded_alarm = false;
  foreach (Alarm alarm, alarms) {
    if (currTime < alarm) {
      finded_alarm = true;
      new_alarm = alarm;
      break;
    }
  }
  if (!finded_alarm) 
    new_alarm = *(alarms.begin());
 
  if (next_alarm != new_alarm)
    next_alarm = new_alarm;
}

ost::Datetime Clock::calc_next_alarm(int disp_mins) 
{
  ost::Date date;
  ost::Time time;

  // Calculate Days 
  int days = next_alarm.dayWeek - date.getDayOfWeek();
  if (days < 0) {
    days += 6;
  } else if (days == 0 && next_alarm.time < time) days = 7;

  time_t next_t = date.getDate();
  // Updating Date
  next_t += 60 * 60 * 24 * days; // set day
  // Updating Time
  next_t += 60 * 60 * next_alarm.time.getHour(); // set hour
  next_t += 60 * (next_alarm.time.getMinute() + disp_mins); // set time + displace mins
  return ost::Datetime(next_t);
}

void Clock::check_ring()
{

  ost::Date date;
  ost::Time time;

  Alarm currTime =  Alarm(date.getDayOfWeek(), time.getHour(), time.getMinute());
  if (next_alarm < currTime) {
    find_next_alarm();
    activate_alarm();
  }

  if (opts.alarm() == dgettext("mms-clock", "off") || !have_alarms) return;
  
  if (currTime == next_alarm) {
    if (!ringing) {
      ringing = true;
      std::cout << "Clock: RIIIIIIIIIINGGGGGGGGGGGGGGGGGGGGGGGGGGGGG!!!!" << std::endl;

      Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));
     
      // load playlist 
      if (opts.playlist() != dgettext("mms-clock","current")) {
           audio->load_playlist(opts.playlist());
      }

      Input cmd;
      cmd.mode = "playback";
       // stop movie playback if needed
      if (S_Global::get_instance()->is_playback("Movie")) {
        cmd.command = "stop";
        S_Global::get_instance()->check_commands(cmd);
      }
      cmd.command = "play";
      S_Global::get_instance()->set_playback("Audio");
      S_Global::get_instance()->check_commands(cmd);
    }
  } else 
    ringing = false;
}

void Clock::call_script(string date) {

  if (last_set_alarm == date) 
    return;

  last_set_alarm == date;

  if (!clock_conf->p_alarm_script().empty() && file_exists(clock_conf->p_alarm_script())) {
    run::external_program(clock_conf->p_alarm_script() + " '" + date+ "'");
  } else {
    print_warning(dgettext("mms-clock", "Could not find alarm update program"), "Clock");
  }
}

void Clock::add_alarm(Alarm &nAlarm)
{
  // FIXME: verify if exist
  alarms.push_back(nAlarm);
}

void Clock::delete_alarm(int &pos)
{
  vector<Alarm>::iterator i = alarms.begin() + pos;
  alarms.erase(i);

  // this is alarms.size(), not -1, because we just deleted a alarm
  if (pos != 0 && pos == alarms.size())
    --pos;
}

void Clock::save_alarms() {
     
     std::ofstream file;

      std::string plist = conf->p_homedir() + "ClockAlarms";
	std::cout << "Clock::save_alarms() into " << plist << std::endl;
      file.open(plist.c_str());

      if (!file) {
	print_critical(dgettext("mms-clock", "Could not write alarms to file: ") + plist, "CLOCK");
      } else {
	file << "### CLOCK ALARMS ### " << std::endl 
	     << "# don't edit manually" << std::endl << std::endl
	     << "version = " << MMS_CONFIG_VERSION << std::endl << std::endl
	     << "# Alarms" << std::endl;
        foreach (Alarm alarm, alarms) {
	    file << "alarm = " << alarm.dayWeek << ", " << doubleZero(alarm.time.getHour()) << ":" << doubleZero(alarm.time.getMinute()) << std::endl;
	}
      }

      file.close();
}
#endif

std::string Clock::strDatetime(ost::Datetime DT)
{
  string str_date = conv::itos(DT.getYear())
	+ "-" + doubleZero(DT.getMonth())
	+ "-" + doubleZero(DT.getDay())
	+ " " + doubleZero(DT.getHour()) 
	+ ":" + doubleZero(DT.getMinute()) 
	+ ":00";
  return str_date;
}

#ifdef use_notify_area
void Clock::ShowNotifyArea() 
{ 
  ost::Date date;
  ost::Time time;

  string strMsg;
  std::pair<int, int> size;
  int ow, oh;  

  render->image_mut.enterMutex();

#ifdef c_music
  // ALARM Notification 
  if  (opts.alarm() == dgettext("mms-clock", "on")) {
    if (ringing)
      strMsg = dgettext("mms-clock", "Riiiing!");
    else if (have_alarms) {
      // calculate tomorrow number day
      int tomorrow = date.getDayOfWeek() + 1;
      if (tomorrow > 6) 
        tomorrow = 0;
      if (next_alarm.dayWeek == tomorrow) 
	  strMsg = dgettext("mms-clock", "tomorrow");
      else if (next_alarm.dayWeek == date.getDayOfWeek() && next_alarm.time > time) 
  	  strMsg = dgettext("mms-clock", "today");
      else 
          strMsg = dayOfWeek(next_alarm.dayWeek);

      strMsg = strMsg + " " + 
  	doubleZero(next_alarm.time.getHour()) + ":" + doubleZero(next_alarm.time.getMinute());
    }
    if (strMsg.size() != 0) {
      size = string_format::calculate_string_size(strMsg, "Vera/12");
      nArea->status_overlay.add(new TObj(strMsg, "Vera/12", 
			conf->p_h_res() - size.first,
			conf->p_v_res() - NotifyArea::height - size.second/2 + 2, 
			themes->notify_area_font1, 
			themes->notify_area_font2, 
			themes->notify_area_font3, 5));
    }
  }
#endif

  strMsg = doubleZero(time.getHour()) + ":" + doubleZero(time.getMinute());
  size = string_format::calculate_string_size(strMsg, "Vera/28");
  nArea->status_overlay.add(new TObj(strMsg, "Vera/28", 
			conf->p_h_res() - NotifyArea::width/2 - size.first/3,
			conf->p_v_res() - NotifyArea::height/4 - size.second, 
			themes->notify_area_font1, 
			themes->notify_area_font2, 
			themes->notify_area_font3, 5));

  
  strMsg = doubleZero(date.getDay());
  size =  string_format::calculate_string_size(strMsg, "Vera/18");
  oh = conf->p_v_res() - size.second;
  ow = conf->p_h_res() - size.first + 5;
  nArea->status_overlay.add(new TObj(strMsg, "Vera/18", 
			ow , oh, 
			themes->notify_area_font1, 
			themes->notify_area_font2, 
			themes->notify_area_font3, 5));



  strMsg = dayOfWeek(date.getDayOfWeek());
  size = string_format::calculate_string_size(strMsg, "Vera/14");
  ow -= size.first - 5;
  nArea->status_overlay.add(new TObj(strMsg, "Vera/14", 
			ow, oh, 
			themes->notify_area_font1, 
			themes->notify_area_font2, 
			themes->notify_area_font3, 5));

  render->image_mut.leaveMutex();
}
#endif

string Clock::doubleZero(int time)
{
  char buffer [512];
  snprintf(buffer, 512, "%02d", time);
  return buffer;
}

string Clock::dayOfWeek(int date)
{
  string buff;
  switch (date) {
	case 0: buff = dgettext("mms-clock","sunday"); break;
	case 1: buff = dgettext("mms-clock","monday"); break;
	case 2: buff = dgettext("mms-clock","tuesday"); break;
	case 3: buff = dgettext("mms-clock","wednesday"); break;
	case 4: buff = dgettext("mms-clock","thursday"); break;
	case 5: buff = dgettext("mms-clock","friday"); break;
	case 6: buff = dgettext("mms-clock","saturday"); break;
  }
  return buff;
}

