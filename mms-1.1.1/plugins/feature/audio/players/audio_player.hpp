#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <cc++/thread.h>

#include "simplefile.hpp"

#include <string>

class AudioConfig;

class AudioPlayer
{
public:
  virtual bool play_next()
  {
    if (play_next_track) {
      play_next_track = false;
      return true;
    } else
      return false;
  }

  bool streaming()
  {
    return is_streaming;
  }

  void set_streaming(bool s)
  {
    is_streaming = s;
  }

  virtual bool supports_buffering() { return true; }

  virtual std::string cd_track_path(int track_nr) = 0;

  // helper function
  bool check_media_helper(const Simplefile& file);
  bool check_media(const Simplefile& file);

  virtual int is_playing() = 0;
  virtual int is_buffering() = 0;
  virtual int is_mute() = 0;
  virtual void gather_info() = 0;
  virtual void addfile(const Simplefile &file) = 0;
  virtual void play() = 0;
  virtual void resume_playback(const Simplefile &f, int p, bool suspended);
  void stop(bool submit = true);
  virtual void stop_player() = 0;
  virtual void pause() = 0;
  virtual void ff() = 0;
  virtual void fb() = 0;

  virtual int getpos()=0;
  virtual void setpos(int p)=0;

  virtual void mute() = 0;
  virtual void volup() = 0;
  virtual void voldown() = 0;
  virtual int getvol() = 0;
  virtual void setvol(int vol) = 0;

  virtual bool supports_rtp() const = 0;

  //  virtual void update_cdrom(const std::string& cdrom) = 0;

  // used for reconfiguring the audio player after the audio device has
  // been changed
  virtual void reconfigure() = 0;

  // for audio cd playback
  virtual void collect_info(const std::string& filename) = 0;

  virtual bool loaded() = 0;
  virtual void release_device() = 0;
  virtual void restore_device() = 0;

  virtual void init() = 0;

  virtual ~AudioPlayer() {};

  void next();
  void prev();

  Simplefile p_cur_nr() const { return cur_nr; }
  void set_cur_nr(const Simplefile &state) { cur_nr = state; }

  virtual std::string p_title() const { return title; }
  virtual std::string p_artist() const { return artist; }
  virtual std::string p_album() const { return album; }

  void set_title(const std::string& t) { title = t; }
  void set_artist(const std::string& a) { artist = a; }
  void set_album(const std::string& a) { album = a; }

  int p_cur_time() const { return cur_time; }
  int p_total_time() const { return total_time; }


  /* true   == a thread triggered by the main updater loop, will poll the player for end of track 
   * false  == no polling, the player will signal end of track itself */
  virtual bool delegate_eo_track(){return true;}

  // in percent
  int buffering_state;

  std::pair<int, bool> load_runtime_settings();

protected:

  void submit_lastfm_info();

  bool is_streaming;

  // implemented in terms of:
  AudioConfig *audio_conf;

  AudioPlayer(const std::string& t, const std::string& art,
	      const std::string& al, int ct, int tt, int bs);

  bool play_next_track;

  std::string last_type;

  int old_volume;

  Simplefile cur_nr;

  std::string title;
  std::string artist;
  std::string album;

  // in seconds
  int cur_time;
  int total_time;
};

#endif
