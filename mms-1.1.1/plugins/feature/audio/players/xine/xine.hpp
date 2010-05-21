#ifndef XINE_HPP_
#define XINE_HPP_
#include "audio_player.hpp"
#include "common.hpp"
#include <string>
#include <xine.h>
#include <xine/xineutils.h>
#include "simplefile.hpp"
#include "audio.hpp"
#include "audio_s.hpp"
class Xine : public AudioPlayer
{
public:
  Xine();
  virtual ~Xine();

  /* Members taken from the original xine class */
  int is_playing();
  int is_buffering();
  int is_mute();
  void gather_info();
  void addfile(const Simplefile &f);
  void play();
  void resume_playback(const Simplefile &f, int p, bool paused);
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

  void reconfigure();

  void collect_info(const std::string& filename);

  bool loaded();
  void release_device();
  void restore_device();
  void init();
  std::string cd_track_path(int track_nr);
  /* end of routines from the original Xine class */

  /* tell check_audio_state() thread that we handle end of track events ourselves */
  bool delegate_eo_track(){return false;}



private:
  pthread_mutex_t stream_mutex;
  pthread_cond_t  stream_switch;
  pthread_t thread_loop;

  volatile bool running;
  bool initted;
  bool retry_track;
  std::string cd_device;
  Cd * cd_manager;

typedef enum{
  _XINE_REQUEST_NONE,
  _XINE_REQUEST_PLAY,
  _XINE_REQUEST_PLAY_AT,
  _XINE_REQUEST_STOP,
  _XINE_REQUEST_RELEASE_DEVICE,
  _XINE_REQUEST_PAUSE,
  _XINE_REQUEST_FF,
  _XINE_REQUEST_FB
} Xine_request_t;

  volatile Xine_request_t xine_request;
  xine_t *xine;
  xine_stream_t* stream;
  xine_video_port_t *vo_port;
  xine_audio_port_t *ao_port;
  xine_event_queue_t *event_queue;
  volatile bool quitting;

  /* Private routines */
  static void * pre_run(void * ptr); /* new thread */
  void run(); /* main loop */

  bool validate_stream();
  bool make_new_stream();
  void gather_info_internal();

  static void event_listener(void *user_data, const xine_event_t *event); /* this processes xine events, prints error messages, etc. */


  /* other private members */
  int volume;
  int is_muted;
  bool mixer_cap;
  std::string configfile;
  volatile bool playing;
  std::string _title;
  std::string _artist;
  std::string _album;
  unsigned int _cur_time;
  unsigned int _total_time;
  //std::string file;
  Simplefile file;
  Simplefile oldfile;
  bool seamless;
  bool start_paused;
#if 0  
  /* delete the following */
  std::string title;
  std::string artist;
  std::string album;
  unsigned int cur_time;
  unsigned int total_time;
#endif
  
};

#endif /*XINE_HPP_*/
