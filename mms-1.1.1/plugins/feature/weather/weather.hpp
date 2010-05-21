#ifndef WEATHER_HPP
#define WEATHER_HPP

#include "module.hpp"
#include "options.hpp"

class CData {
private:
  std::string city;
  std::string date;
  std::string text;
  std::string humid;
  std::string wind;
  std::string unit;
  struct {
    int hi;
    int low;
    std::string icon;
    std::string name;
  } day[5];

public:
    // Update script, City Code, XML File with result, Unit
    //CData ( std::string, std::string, std::string, std::string);
    CData ();
    void Update();
    std::string get_icon ( int );
    int get_high_temp ( int );
    int get_low_temp ( int );
    std::string get_name ( int );
    std::string get_city ( void );
    std::string get_country ( void );
    std::string get_date (void);
    std::string get_text (void);
    std::string get_humid (void);
    std::string get_wind (void);
    std::string weather_file;
    time_t weather_file_last_mod_time;
    std::string get_unit(void);
};


class Weather : public Module{
public:

  Weather();

  Options* get_opts()
  {
    return 0;
  }

  void startup_updater()
  {}
  std::string Print( int, int );
  std::string ThemePath();
  int Wget( std::string );
  std::string mainloop();

  int update_idle();
  void update_wdata();

#ifdef use_notify_area
  void ShowNotifyArea();
#endif

  void exit();

  bool exit_loop;

private:

  std::string header_font;

  std::string Unit;
  int curlayer;
  CData wdata;
  bool enabled;
  std::string weather_file;
  std::string user_agent;
  std::string codepage;
};

#endif

