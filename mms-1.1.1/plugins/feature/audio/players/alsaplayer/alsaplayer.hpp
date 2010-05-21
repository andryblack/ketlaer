#ifndef ALSAPLAYER_HPP
#define ALSAPLAYER_HPP

#include "audio_player.hpp"

#include "alsaplayer_config.hpp"

class Alsaplayer : public AudioPlayer
{
public:
  std::string cd_track_path(int track_nr);

  int is_mute();
  int is_playing();
  int is_buffering();
  void gather_info();
  void addfile(const Simplefile &file);
  void play();
  void stop_player();
  void pause();
  void ff();
  void fb();

  int getpos();
  void setpos(int p);

  void mute();
  void volup();
  void voldown();
  int getvol();
  virtual void setvol(int vol);

  virtual std::string p_title() const;
  virtual std::string p_artist() const;
  virtual std::string p_album() const;

  bool supports_rtp() const;

  //  void update_cdrom(const std::string& cdrom) {}; // doesn't do anything atm.
  void reconfigure();

  void collect_info(const std::string& filename);

  bool loaded();
  void release_device();
  void restore_device();

  void init();

  ~Alsaplayer();
  Alsaplayer();

private:

  AlsaplayerConfig *alsaplayer_conf;

  void setvol();
  void start_alsaplayer();
  void stop_alsaplayer();

  bool running;
  // value which tells if alsaplayer is "booting"
  bool starting;

  bool started_a_new_track;

  float volume;

  int session_id;
};

#endif
