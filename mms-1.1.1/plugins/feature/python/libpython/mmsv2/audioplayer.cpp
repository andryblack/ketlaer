#include "config.h"
#include "plugins.hpp"
#include "simplefile.hpp"
#include "audio_config.hpp"
#include "audioplayer.hpp"
#include "audio_s.hpp"
#include <iostream>

using std::string;

pthread_mutex_t pymms::player::PythonAudioPlayer::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

pymms::player::PythonAudioPlayer::PythonAudioPlayer()
{
  #ifdef c_music
  m_bSet = false;
  Plugins *pPlugins = S_Plugins::get_instance();
  AudioConfig *pAudioConf = S_AudioConfig::get_instance();
  
  if(pPlugins->find(pPlugins->audio_players, pAudioConf->p_audio_player()))
    m_bSet = true;
  else
    std::cerr << "Audio support detected, but no audio player!" << std::endl;
  #else
  std::cerr << "No audio support detected" << std::endl;
  #endif
}

void pymms::player::PythonAudioPlayer::play(string strPath, string strTitle)
{
  #ifdef c_music
  if(m_bSet)
  {
    Simplefile track;
    track.path = strPath;
    track.name = strTitle;
    if( (strPath.find("http://", 0) != string::npos) || (strPath.find("rtsp://", 0) != string::npos) ||
        (strPath.find("rtp://", 0) != string::npos) || (strPath.find("mmsh://", 0) != string::npos) ||
        (strPath.find("mms://", 0) != string::npos) )
          track.type = "web";
    else
      track.type = "media-file";

    S_Audio_s::get_instance()->external_plugin_playback(track);
    S_Audio_s::get_instance()->p->set_streaming(track.type == "web");
  }
  #endif
}

void pymms::player::PythonAudioPlayer::stop()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->stop();

  #endif
}

void pymms::player::PythonAudioPlayer::pause()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->pause();
  #endif
}

void pymms::player::PythonAudioPlayer::mute()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->mute();
  #endif
}

void pymms::player::PythonAudioPlayer::volup()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->volup();
  #endif
}

void pymms::player::PythonAudioPlayer::voldown()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->voldown();
  #endif
}

void pymms::player::PythonAudioPlayer::ff()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->ff();
  #endif
}

void pymms::player::PythonAudioPlayer::fb()
{
  #ifdef c_music
  if(m_bSet)
    S_Audio_s::get_instance()->p->fb();
  #endif
}

int pymms::player::PythonAudioPlayer::isPlaying()
{
  #ifdef c_music
  if(m_bSet)
    return S_Audio_s::get_instance()->p->is_playing();
  #endif
}
