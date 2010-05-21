#include "config.h"

#include "audio_config.hpp"

#include "audio.hpp"
#include "audio_s.hpp"
#include "gettext.hpp"

#include <iostream>

using std::vector;
using std::string;

pthread_mutex_t Audio_s::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

// destructor
Audio_s::~Audio_s()
{
  AudioConfig *audio_conf = S_AudioConfig::get_instance();
  audio_conf->s_audio_started(false);
}

Audio_s::Audio_s()
  : playback("playback"), has_played(false),
    playback_fullscreen("playback_fullscreen"), playback_fullscreen_shown(false),
    fullscreen_info(false), mute("volume"), volume("volume"), volume_shown(false),
    playing(false), paused(false), is_external_source(false)
{}

void Audio_s::set_audio_player(AudioPlayer *player)
{
  p = player;

  // FIXME: if xine
//   if (conf->p_priority_change())
//     nice(-15); // Temporary raise priorities for Xine

//   p = new Xine;

//   if (conf->p_priority_change())
//     nice(15); //and back down again

  AudioConfig *audio_conf = S_AudioConfig::get_instance();
  audio_conf->s_audio_started(true);
}

void Audio_s::add_track_to_played(const Simplefile& e)
{
  std::stack<Simplefile> temp = played_tracks;

  bool found = false;
  while (!temp.empty()) {
    Simplefile top = temp.top();
    temp.pop();
    if (top == e)
      found = true;
  }

  if (!found)
    played_tracks.push(e);
}

void Audio_s::remove_track_from_played(const Simplefile& e)
{
  std::stack<Simplefile> temp;

  while (!played_tracks.empty()) {
    Simplefile top = played_tracks.top();
    if (top != e)
      temp.push(top);
    played_tracks.pop();
  }

  played_tracks = temp;
}

Simplefile Audio_s::prev_track_played()
{
  Simplefile e;

  if (played_tracks.size() == 0)
    return e;
  else {
    e = played_tracks.top();
    played_tracks.pop();
    return e;
  }
}

void Audio_s::add_track_to_queue(const Simplefile& e)
{
  if (queue_pos(e) == 0) // not already queued
    queued_tracks.push_back(e);
}

void Audio_s::remove_track_from_queue(const Simplefile& e)
{
  for (std::list<Simplefile>::iterator i = queued_tracks.begin(),
	 iend = queued_tracks.end(); i != iend; ++i)
    if (e == *i) {
      queued_tracks.erase(i);
      break;
    }
}

Simplefile Audio_s::next_in_queue(bool modify)
{
  Simplefile q = queued_tracks.front();
  if (modify)
    queued_tracks.pop_front();
  return q;
}

int Audio_s::queue_size()
{
  return queued_tracks.size();
}

int Audio_s::queue_pos(const Simplefile& s)
{
  int pos = 0;
  foreach (Simplefile& track, queued_tracks) {
    ++pos;
    if (s == track)
      return pos;
  }

  return 0;
}

void Audio_s::remove_queued_tracks()
{
  queued_tracks.clear();
}

bool Audio_s::check_repeat()
{
  if (audio->playlist_size() == 0)
    return false;

  bool status = true;

  if (queue_size() > 0)
    return true;

  if (!conv::stob(audio->get_opts()->repeat())) {
    if (audio->get_opts()->shuffle() == dgettext("mms-audio", "off")
	&& audio->last_element_in_playlist())
      status = false;
    else if (audio->get_opts()->shuffle() != dgettext("mms-audio", "off")
	     && played_tracks.size() >= audio->playlist_size()) {
      if (audio->get_opts()->shuffle() == dgettext("mms-audio", "real random"))
	status = true;
      else
	status = false;
    } else
      status = true;

    if (!status)
      empty_played_tracks();

  } else {
    if (played_tracks.size() >= audio->playlist_size())
      empty_played_tracks();
  }

  return status;
}

bool Audio_s::check_shutdown()
{
  return conv::stob(audio->get_opts()->shutdown());
}

void Audio_s::empty_played_tracks()
{
  while (!played_tracks.empty())
    played_tracks.pop();
}

void Audio_s::suspend_normal_audio_and_play_track(const Simplefile &file)
{
  suspend_playback();
  external_plugin_playback(file);
}

void Audio_s::suspend_playback()
{
  suspended = true;

  pos = p->getpos();
  track = p->p_cur_nr();
  p->stop(false);

  mmsSleep(1);
}

void Audio_s::restore_playback(bool paused)
{
  if (suspended) {
    audio->check_mount_before(track.type);
    p->resume_playback(track, pos, paused);
    p->set_cur_nr(track);
    suspended = false;

    if (is_external_source)
      is_external_source = false;
  }
}

bool Audio_s::suspended_playback()
{
  return suspended;
}

void Audio_s::external_plugin_playback(const Simplefile &file)
{
  p->addfile(file);
  p->set_cur_nr(file);
  is_external_source = true;
}
