#ifndef MYGST_HPP
#define MYGST_HPP

#define KETLAER

#include "audio_player.hpp"

class Rtd : public AudioPlayer
{
public:
  std::string cd_track_path(int track_nr);

  virtual bool supports_buffering() { return false; }

  int is_playing();
  int is_buffering();
  int is_mute();
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
  void setvol(int vol);

  bool supports_rtp() const;

  //  void update_cdrom(const std::string& cdrom);
  void reconfigure();

  void collect_info(const std::string& filename);

  bool loaded();
  void release_device();
  void restore_device();

  void init();

  Rtd();
  ~Rtd();

private:

  bool is_loaded;
};

#endif
