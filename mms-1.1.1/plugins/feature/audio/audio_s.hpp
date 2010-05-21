#ifndef AUDIO_S_HPP
#define AUDIO_S_HPP

#include "simplefile.hpp"
#include "audio_player.hpp"

#include "singleton.hpp"

// overlay
#include "image.hpp"

#include <cc++/thread.h>

#include <string>
#include <stack>
#include <list>

class Audio;

class Audio_s
{
public:

  static pthread_mutex_t singleton_mutex;

  void suspend_normal_audio_and_play_track(const Simplefile &file);

  void suspend_playback();
  void restore_playback(bool paused = false);

  bool suspended_playback();
  bool is_external_source;

  // play some file without putting it into the playlist
  void external_plugin_playback(const Simplefile &file);

  // common
  bool p_playing() const { return playing; }
  void set_playing(bool state) { playing = state; }

  bool p_pause() const { return paused; }
  void set_pause(bool state) { paused = state; }

  void set_audio_player(AudioPlayer *player);

  // FIXME: ugly
  AudioPlayer *p;

  // real random
  void remove_track_from_played(const Simplefile& e);
  void add_track_to_played(const Simplefile& e);
  Simplefile prev_track_played();

  // queue
  void add_track_to_queue(const Simplefile& e);
  void remove_track_from_queue(const Simplefile& e);
  Simplefile next_in_queue(bool modify);
  int queue_size();
  int queue_pos(const Simplefile& s);
  void remove_queued_tracks();

  // real random
  enum real_random_state { ADDED, TAKEN };
  real_random_state direction;

  Audio_s();
  ~Audio_s();

  bool check_repeat();
  bool check_shutdown();

  Overlay playback;
  bool has_played;

  Overlay playback_fullscreen;
  bool playback_fullscreen_shown;

  bool fullscreen_info;

  Audio *get_audio()
  {
    return audio;
  }

  void set_audio(Audio *a)
  { audio = a; }

  Overlay mute;

  Overlay volume;
  bool volume_shown;

  std::stack<Simplefile> played_tracks;
  void empty_played_tracks();

private:

  Audio *audio;

  bool playing;

  bool paused;

  Simplefile track;
  int pos;
  bool suspended;

  std::list<Simplefile> queued_tracks;
};

typedef Singleton<Audio_s> S_Audio_s;

#endif
