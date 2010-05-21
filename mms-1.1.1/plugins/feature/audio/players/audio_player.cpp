//#define DLOG

#include "config.h"

#include "audio_player.hpp"

#include "audio.hpp"
#include "audio_s.hpp"

#include "dbaudiofile.hpp"

#include "lastfm.hpp"

#include "common.hpp"
#include "rand.hpp"
#include "gettext.hpp"

#include <vector>
#include <string>
#include <fstream>

using std::vector;
using std::string;

AudioPlayer::AudioPlayer(const string& t, const string& art,
			 const string& al, int ct, int tt, int bs) :
  buffering_state(bs), is_streaming(false), play_next_track(false),
  old_volume(0), title(t), artist(art), album(al), cur_time(ct), total_time(tt)
{
  audio_conf = S_AudioConfig::get_instance();
}

bool AudioPlayer::check_media_helper(const Simplefile& file)
{
  bool status = true;

  if (file.type == "media-track"){
    Cd *cd = S_Cd::get_instance();
    CD_Tag *cdtag = new CD_Tag;
    cdtag->verbosity= VERBOSE_ALL;
    if ((cdtag->TagCD(cd->get_device().c_str()) == -1) ||
	cdtag->CDDB_Id() != file.media_id) {
      status = false;
    }
    delete cdtag;
  }

  return status;
}

bool AudioPlayer::check_media(const Simplefile& file)
{
  if (!check_media_helper(file)) {
    DialogWaitPrint pdialog(dgettext("mms-audio", "Track not present on the current media"), 2000);
    stop();
    return false;
  } else
    return true;
}

void AudioPlayer::submit_lastfm_info()
{
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));
  Audio_s *audio_state = S_Audio_s::get_instance();

  string artist, album, title, buffer;
  audio->get_audiotrack_info(buffer, artist, album, title);

  LastFM::end_of_song(artist, album, title, audio_state->p->p_total_time(), audio_state->p->p_cur_time());
}

void AudioPlayer::stop(bool submit)
{
  Audio_s *audio_state = S_Audio_s::get_instance();
  audio_state->set_pause(false);

  if (submit)
    submit_lastfm_info();

  stop_player();

  mmsSleep(1);
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));
  audio->pos_umount();
}

void AudioPlayer::next()
{
  Audio_s *audio_state = S_Audio_s::get_instance();
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));

  if (!audio_state->p_playing())
    return;

  for (int skipped = 0; skipped < audio->playlist_size(); ++skipped) {

    if (!audio_state->check_repeat())
      audio_state->set_playing(false);

    if (audio_state->p_playing()) {

      submit_lastfm_info();

      cur_nr = audio->next_helper(true);

      if (!check_media_helper(cur_nr))
	continue;

      audio->check_mount_before(cur_nr.type);

      addfile(cur_nr);

      audio->check_mount_after(cur_nr.type);

      break;

    } else {
      stop();

      audio->check_shutdown();

      // make pressing play start from the first track again
      audio->playlist_pos_int = 0;
      Simplefile s;
      set_cur_nr(s);

      break;
    }
  }
}

void AudioPlayer::prev()
{
  Audio_s *audio_state = S_Audio_s::get_instance();
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));

  if (!audio_state->p_playing())
    return;

  for (int skipped = 0; skipped < audio->playlist_size(); ++skipped) {

    if (audio_state->p_playing()) {

      submit_lastfm_info();

      audio->prev_helper();

      if (!check_media_helper(cur_nr))
	continue;

      audio->check_mount_before(cur_nr.type);

      addfile(cur_nr);

      audio->check_mount_after(cur_nr.type);

      break;

    } else {
      stop();
      break;
    }
  }
}

void AudioPlayer::resume_playback(const Simplefile &f, int p, bool suspended)
{
  dprintf("Autoplayer resume_playback() '%s'\n", f.path.c_str());
  
  addfile(f);
  setpos(p);
  if(suspended)
    pause();
}


std::pair<int, bool> AudioPlayer::load_runtime_settings()
{
  Config *conf = S_Config::get_instance();

  // sane defaults
  int volume = 52, playlist_pos = 0;
  bool play_now_warning = true;

  std::string path = conf->p_var_data_dir() + "options/AudioRuntime";

  std::ifstream in;
  in.open(path.c_str(), std::ios::in);

  if (!in.is_open()) {
    print_critical(dgettext("mms-audio", "Could not open options file ") + path, "AUDIO PLAYER");
  } else {

    string name, value, str;

    while (getline(in, str)) {

      string::size_type comma = str.find(",");

      if (comma == string::npos)
        continue;

      name = str.substr(0,comma);
      value = str.substr(comma+1);

      if (name == "volume")
        volume = conv::atoi(value);
      else if (name == "playlist_pos")
	playlist_pos = conv::atoi(value);
      else if (name == "play_now_warning")
	play_now_warning = conv::stob(value);
    }
    in.close();
  }

  setvol(volume);

  return std::make_pair(playlist_pos, play_now_warning);
}
