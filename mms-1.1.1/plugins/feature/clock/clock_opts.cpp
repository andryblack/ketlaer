#include "clock.hpp"

#include "audio.hpp"
#include "module.hpp"
#include "common.hpp"
#include "input.hpp"
#include "image.hpp"
#include "renderer.hpp"
#include "gettext.hpp"
#include "graphics.hpp"

#include <cc++/numbers.h>

using std::vector;
using std::string;


Clock::ClockOpts::ClockOpts()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the e1nvironment variables */
  bindtextdomain("mms-clock", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-clock", nl_langinfo(CODESET));
#endif
  
  // header
  header = "Clock Options";
  translated_header = dgettext("mms-clock", "Clock Options");
  save_name = "ClockOptions";

  // values
  vector<string> alarm_values;

  alarm_values.push_back(dgettext("mms-clock", "on"));
  alarm_values.push_back(dgettext("mms-clock", "off"));

  vector<string> english_alarm_values;

  english_alarm_values.push_back("on");
  english_alarm_values.push_back("off");

  alarm_p = new Option(false, dgettext("mms-clock", "alarm"), "alarm", 0,
		      alarm_values, english_alarm_values, boost::bind(&ClockOpts::alarms_mainloop, this));
  val.push_back(alarm_p);

  // values
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));
  std::vector<std::string> saved_playlists = audio->list_playlists();

  vector<string> playlist_values;
  vector<string> english_playlist_values;

  playlist_values.push_back(dgettext("mms-clock", "current"));
  english_playlist_values.push_back("current");

  foreach (string& el, saved_playlists) {
    playlist_values.push_back(dgettext("mms-clock", el.c_str()));
    english_playlist_values.push_back(el);
  }

  playlist_p = new Option(false, dgettext("mms-clock", "playlist"), "playlist", 0,
		      playlist_values, playlist_values);
  val.push_back(playlist_p);
}

void Clock::ClockOpts::update_playlists() 
{
  if (S_Config::get_instance()->p_debug_level() > 2)
    std::cout << "Clock::ClockOpts::update_playlists() run" << std::endl;
// values
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));
  std::vector<std::string> saved_playlists = audio->list_playlists();
  bool finded;
  foreach (string& el, saved_playlists) {
    finded = false;
    foreach (string& in_list, playlist_p->english_values) {
	if (in_list == el) {
		finded = true;
		break;
	}
    }
    if (!finded) {
      playlist_p->values.push_back(dgettext("mms-clock", el.c_str()));
      playlist_p->english_values.push_back(el);
    }
  }
}

void Clock::ClockOpts::alarms_mainloop()
{
  Clock *clk = get_class<Clock>(dgettext("mms-clock", "Clock"));
  clk->alarms_mainloop();
  S_BusyIndicator::get_instance()->idle();
}


Clock::ClockOpts::~ClockOpts()
{
  save();

  delete alarm_p;
  delete playlist_p;
}
