#include "config.h"

#include "config.hpp"
#include "common.hpp"

#include "lyrics.hpp"
#include "lyrics_fetch.hpp"

#include "common.hpp"
#include "gettext.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "input.hpp"

#include "audio.hpp"
#include "startmenu.hpp"

#include "tinyxml.h"
#include "tinystr.h"

#include "boost.hpp"

#include "wget_wrapper.hpp"

// saving
#include <fstream>
#include <iostream>


using std::string;

pthread_mutex_t Lyrics::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

void LyricsLoop::run()
{
  Lyrics *lyrics = S_Lyrics::get_instance();
  while(true) {
    lyrics->update_px_count();
    mmsUsleep(lyrics->get_period()*1000); // ms
  }
}

Lyrics::Lyrics()
  : enabled(false), running(false), lf(0)
{
  vecLyric.clear();

  string screensaver_lyric_font = "Vera/" + conv::itos(resolution_dependant_font_size(22, conf->p_v_res()));

  std::pair<int, int> size =  string_format::calculate_string_size("(asdfghjkjlASDFGHJKL):", screensaver_lyric_font);

  font_size = size.second;

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.add(TimeElement("audio_lyric_status", boost::bind(&Lyrics::idle_status, this),
					boost::bind(&Lyrics::check_status, this)));
  LyricsLoop *ll = new LyricsLoop;
  ll->start();

}

int Lyrics::idle_status()
{
  return 500; // ms
}

void Lyrics::check_status()
{
  if (enabled) {
    if (lf && !lf->is_busy() && lf->get_ready()) {
	vecLyric = lf->get_lyric();
	enabled = false;
    }
  }
  
/*  
    std::cout << "Lyrics::check_status()" << std::endl;
    Load() || search_result();
    if (buff_text.size() && !vecLyric.size())
      parse_text(buff_text);
  }
  enabled = false;*/
}

string Lyrics::mainloop()
{
  return "";
}

bool Lyrics::find_lyric(string artist, string title)
{

  if (cache_artist == artist && cache_title == title)
    return true;

  cache_artist = artist;
  cache_title = title;
  vecLyric.clear();
  px_count = 0;
  tm_old = 0;
  total_time = 0;
  current_time = 0;
  display_height = 0;
  ln_displace = 0;
  period_fix = 0;
  enabled = true;

  if (lf != 0)
    delete lf;
  
  lf = new LyricsFetch(artist, title);

  return true;
}

bool Lyrics::is_enabled()
{
  return (vecLyric.size() ? true : false);
}

void Lyrics::set_displace(int step)
{
  int nvalue = ln_displace + step;
  if (std::abs(nvalue) < vecLyric.size() - display_lines())
    ln_displace += step;
  px_count = float(current_time) * get_frequency(); // FORCE PIXEL COUNT IF CHANGE LN_DISPLACE
}

int Lyrics::get_displace()
{
  return ln_displace;
}

int Lyrics::get_total_lines()
{
  return vecLyric.size() - ln_displace;
}

void Lyrics::set_total_time(int time)
{
  total_time = time;
}

void Lyrics::set_current_time(int time)
{
  current_time = time;

  if (total_time != 0) {
    int tm = current_time * 100 / total_time;
    int df = tm - tm_old;
    // resync lyric with track time when rewind or fastforward
    if (std::abs(df) > 2) {
      px_count = current_time * get_frequency();
    }
    tm_old = tm;
    // end of count
    //update_px_count();
  } else
    px_count = 0;
}

void Lyrics::update_px_count()
{
  if (running && get_line() < max_line())
    px_count++;
}

void Lyrics::play()
{
  running = true;
}

void Lyrics::pause()
{
  running = false;
}


void Lyrics::set_display_height(int h)
{
  display_height = h;
}

float Lyrics::get_frequency()
{
  float pixel = float(get_total_lines()) * float(font_size);
  float freq  = total_time ? pixel / (total_time) : 0;
  return freq;
}

float Lyrics::get_period()
{
  float freq = get_frequency();
  float per = (freq ?  1 / freq * 1000 : 0);
  per -=  period_fix;
  period_fix = per - int(per);
  return (per ? per : 250); // ms
}

/**
  *
  * return int numbers of lines to display
  */
int Lyrics::display_lines() 
{
  return display_height / font_size + 1;
}

/**
  *
  * return int numbers of lines max
  *
  */
int Lyrics::max_line()
{
  return get_total_lines() - display_lines() + ln_displace;
}

int Lyrics::get_real_line() 
{
return px_count / font_size + ln_displace;
}

int Lyrics::get_line()
{
  int val = get_real_line();

  if (val > 0)
    return (val < max_line() ? val : max_line());
  else
    return 0;
}


int Lyrics::get_anim()
{
  int val = get_real_line();
  return 0 <= val  ? px_count % font_size : 0;
}
