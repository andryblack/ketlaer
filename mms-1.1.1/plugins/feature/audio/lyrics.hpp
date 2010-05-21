#ifndef LYRICS_HPP
#define LYRICS_HPP

#include "lyrics_fetch.hpp"

#include "module.hpp"
#include "singleton.hpp"

#include <string>
#include <vector>

class LyricsLoop : public ost::Thread
{
private:
  virtual void run();
};

class Lyrics : public Module
{
private:

  int idle_status();

  //void calculate();

  std::string cache_artist;
  std::string cache_title;

  int px_count;
  int tm_old;
  int total_time;
  int current_time;
  int display_height;
  int ln_displace;
  int font_size;
  float period_fix;

  bool enabled;
  bool running;
  
  LyricsFetch* lf;

  int get_real_line();
  int get_total_lines();

public:

  // Constructor adds itself to the updater loop
  Lyrics();

  std::vector<std::string> vecLyric;

  void check_status();
  bool find_lyric(std::string artist, std::string title);
  std::string mainloop();

  bool get_status();
  bool is_enabled();

  void clear();

  void update_px_count();

  void play();
  void pause();

  void set_displace(int step);
  void set_total_time(int time);
  void set_current_time(int time);
  void set_display_height(int h);

  float	get_frequency();
  float get_period();
  int get_line();
  int get_displace();
  int display_lines();
  int max_line();
  int get_anim();

  Options* get_opts()
  {
    return 0;
  }

  void startup_updater()
  {}

  static pthread_mutex_t singleton_mutex;
};

typedef Singleton<Lyrics> S_Lyrics;

#endif
