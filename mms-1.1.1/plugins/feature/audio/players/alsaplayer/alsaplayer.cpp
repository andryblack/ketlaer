#include "config.h"

#include "alsaplayer.hpp"

#include "audio.hpp"
#include "audio_s.hpp"
#include "global_options.hpp"
#include "playlist_parser.hpp"
#include <alsaplayer/control.h>

#include <signal.h>
#include <unistd.h>
#include <iostream>

using std::string;

void Alsaplayer::reconfigure()
{
  stop_alsaplayer();
  start_alsaplayer();
}

void Alsaplayer::start_alsaplayer()
{
  GlobalOptions *go = S_GlobalOptions::get_instance();
  Config *conf = S_Config::get_instance();

  starting = true;

  global_audio_device gad = go->get_audio_device_audio();

  string audio_dev = gad.dev_path;

  string audio_output_type = gad.dev_output;

  run::external_program((alsaplayer_conf->p_alsaplayer_path() + " -i daemon -s \"mms_daemon\" -d " +
	  audio_dev + " -o " + audio_output_type + " " +
	  alsaplayer_conf->p_alsaplayer_options() + " --nosave >/dev/null&").c_str(), false);

  session_id = -1;

  while (session_id == -1) {
    // wait for alsaplayer to start up so that mms can find it :/
    mmsUsleep(500*1000);
    ap_find_session("mms_daemon", &session_id);
  }

  // alsaplayer sometimes still has something old stuck inside it's
  // buffer. Get rid of that here. Shouldn't be needed with the latest
  // alsaplayer but you never know...
  ap_stop(session_id);
  ap_clear_playlist(session_id);

  setvol(); // restore volume on restart of alsaplayer

  ap_set_looping(session_id, 0);

  starting = false;
  running = true;
}

void Alsaplayer::stop_alsaplayer()
{
  running = false;
  ap_quit(session_id);
}

Alsaplayer::~Alsaplayer()
{
  stop_alsaplayer();
  running = false;
}

Alsaplayer::Alsaplayer()
  : AudioPlayer("", "", "", 0, 0, 0),
    running(false), starting(false), volume(0), session_id(-1), started_a_new_track(false)
{
  alsaplayer_conf = S_AlsaplayerConfig::get_instance();
}

void Alsaplayer::init()
{
  start_alsaplayer();

  S_Global::get_instance()->register_reconfigure(boost::bind(&AudioPlayer::reconfigure, this));

  cur_nr.path = "";
}

string Alsaplayer::cd_track_path(int track_nr)
{
  std::ostringstream tmp;

  if (track_nr < 10)
    tmp << "Track 0" << track_nr << ".cdda";
  else
    tmp << "Track " << track_nr << ".cdda";

  return tmp.str();
}

int Alsaplayer::is_playing()
{
  if (starting)
    return 1;

  int i = 0;
  ap_is_playing(session_id, &i);

  if (i != 0)
    started_a_new_track = false;

  return i;
}

int Alsaplayer::is_buffering()
{
  // buffer until we have confirmation that we are playing
  return started_a_new_track;
}

void Alsaplayer::gather_info()
{
  char title_[AP_TITLE_MAX];
  char artist_[AP_ARTIST_MAX];
  char album_[AP_ALBUM_MAX];
  int position_, length_ = 0;

  ap_get_title(session_id, title_);
  ap_get_artist(session_id, artist_);
  ap_get_album(session_id, album_);
  ap_get_position(session_id, &position_);
  ap_get_length(session_id, &length_);

  title = title_;
  artist = artist_;
  album = album_;
  cur_time = position_;
  total_time = length_;
}

bool Alsaplayer::supports_rtp() const
{
  return false;
}

void Alsaplayer::collect_info(const string& filename)
{
  if (!running)
    start_alsaplayer();

  ap_clear_playlist(session_id);
  ap_add_and_play(session_id, filename.c_str());
  ap_stop(session_id);
  gather_info();
}

bool Alsaplayer::loaded()
{
  return running;
}

void Alsaplayer::release_device()
{
  stop_alsaplayer();
}

void Alsaplayer::restore_device()
{
  start_alsaplayer();
}

void Alsaplayer::addfile(const Simplefile &file)
{
  if (!check_media(file))
    return;

  if (!running)
    start_alsaplayer();

  Audio_s *audio_state = S_Audio_s::get_instance();

  audio_state->set_playing(false);

  ap_clear_playlist(session_id);

  string _str = file.path.c_str();
  if (file.type != "media-track"){
    vector<string> urls = PlaylistParser::resolve_playlist(_str);
    if (!urls.empty())
      _str = urls[0]; /* TODO: Handle multiple playlist entries here */
  }

  ap_add_and_play(session_id, _str.c_str());

  started_a_new_track = true;

  audio_state->set_playing(true);
  audio_state->set_pause(false);
}

void Alsaplayer::play()
{
  if (!running)
    start_alsaplayer();

  if (!cur_nr.path.empty())
    addfile(cur_nr);
}

void Alsaplayer::stop_player()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  audio_state->set_playing(false);

  started_a_new_track = false;

  ap_clear_playlist(session_id);
  ap_stop(session_id);
  ap_get_volume(session_id, &volume);
  stop_alsaplayer();
}

void Alsaplayer::pause()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  if (audio_state->p_pause()) {
    ap_unpause(session_id);
    audio_state->set_pause(false);
  } else {
    ap_pause(session_id);
    audio_state->set_pause(true);
  }
}

void Alsaplayer::setvol(int vol)
{
  volume = vol/100.0;
  setvol();
}

void Alsaplayer::setvol()
{
  ap_set_volume(session_id, volume);
}

string Alsaplayer::p_title() const
{
  return string_format::convert(title);
}

string Alsaplayer::p_artist() const
{
  return string_format::convert(artist);
}

string Alsaplayer::p_album() const
{
  return string_format::convert(album);
}

void Alsaplayer::volup()
{
  volume += 0.04;
  if (volume > 1)
    volume = 1;
  setvol();
}

void Alsaplayer::voldown()
{
  volume -= 0.04;
  if (volume < 0)
    volume = 0;
  setvol();
}

int Alsaplayer::getvol()
{
  return round_to_int(volume*100);
}

int Alsaplayer::is_mute()
{
  int v = getvol();

  if (v == 0) {
    return 1;
  } else {
    return 0;
  }
}

void Alsaplayer::mute()
{
  int v = getvol();

  if (v == 0) {
    // set back to what it was before (unmute)
    volume = old_volume/100.0;
    setvol();
  } else {
    old_volume = v;
    volume = 0;
    setvol();
  }
}

void Alsaplayer::ff()
{
  gather_info();

  if (!((cur_time + 10) > total_time))
    ap_set_position_relative(session_id, 10);
}

void Alsaplayer::fb()
{
  gather_info();

  if (cur_time == 0)
    return;

  if (!((cur_time - 10) < 0))
    ap_set_position_relative(session_id, -10);
}

int Alsaplayer::getpos()
{
  int position_ = 0;
  ap_get_position(session_id, &position_);
  return position_;
}

void Alsaplayer::setpos(int p)
{
  ap_set_position(session_id, p);
}
