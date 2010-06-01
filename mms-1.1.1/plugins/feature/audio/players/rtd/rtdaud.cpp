#include "rtdaud.hpp"

#include "audio_s.hpp"
#include "config.hpp"
#include "global_options.hpp"
#include "updater.hpp"
#include "shoutcast.hpp"

#include <string>

#include <iostream>
#include <sstream>

#include <libketlaer.h>

static bool           g_bRunning = false;
static VideoPlayback *g_pb = NULL;

using std::string;

static bool do_playfile(const string &file)
{
  bool isFile = false;
  string path;

  if (file.find("http://www.shoutcast.com/sbin/tunein-station.pls") != string::npos) 
    path = Shoutcast::get_instance()->get_playlist(file).second;
  else if (file.find("://") != string::npos) 
    path = file;
  else {
    isFile = true;
    path = "file://" + file;
  }

  printf("[RTDAUD]do_playfile %s\n", path.c_str());
  if (g_pb->LoadMedia((char*)path.c_str()) == S_OK) {
    if (!isFile) {
      long startupfullness = 16 * 1024;
      g_pb->m_pSource->SetGetProperty(NAVPROP_NAV_SET_STARTUP_FULLNESS,
				      (void*)&startupfullness,
				      sizeof(startupfullness),
				      NULL,
				      0,
				      NULL);
    }
    g_bRunning = true;
    g_pb->m_pFManager->SetRate(256);
    g_pb->m_pAudioOut->SetFocus();
    g_pb->m_pAudioOut->SetVolume(0);
    g_pb->SetVideoOut(VO_VIDEO_PLANE_V1, 0, 0);
    g_pb->ScanFileStart(false);
    g_pb->m_pFManager->Run();
    return true;
  }
  else {
    printf("[RTDAUD]can not load media\n");
    return false;
  }
}


RtdAud::RtdAud()
  : AudioPlayer("", "", "", 0, 0, 0), is_loaded(true)
{
  printf("[RTDAUD]init\n");
  init_libketlaer();
  g_pb = getVideoPlayback();
}

RtdAud::~RtdAud()
{
  printf("[RTDAUD]deinit\n");
  uninit_libketlaer();
}

void RtdAud::init()
{
}

string RtdAud::cd_track_path(int track_nr)
{
  std::ostringstream tmp;

  tmp << "cdda://" << track_nr;

  return tmp.str();
}

int RtdAud::is_playing()
{
  Audio_s *audio_state = S_Audio_s::get_instance();
  CPlaybackFlowManager *pFManager = g_pb->m_pFManager;

  if (pFManager) {
    EVId    EventId;
    EVCode  EventCode;
    long    ParamSize;
    long*   pParams;

    if(pFManager->GetEvent(&EventId, 
			   &EventCode, 
			   &ParamSize, 
			   &pParams, 
			   10) == S_OK) {
      switch(EventCode) {
      case FE_Playback_FatalError:
	g_bRunning = false;
	break;
      case FE_Playback_Completed:
	g_bRunning = false;
	break;
      default:
	printf("[RTDAUD]playback event %d not handled\n", EventCode);
	break;
      }
      pFManager->FreeEventParams(EventId);
    }
  }
  if (audio_state->p_pause())
    return 1;

  return g_bRunning;
}

int RtdAud::is_buffering()
{
  return 0;
}

int RtdAud::is_mute()
{
  return (old_volume != 0);
}

void RtdAud::gather_info()
{
  cur_time = getpos();
}

void RtdAud::addfile(const Simplefile &file)
{
  printf("[RTDAUD]addfile %s\n", path.c_str());
  path = file.path;
  if (!check_media(file))
    return;
  stop_player();
  if (path.length()) {
    do_playfile(path);
    Audio_s *audio_state = S_Audio_s::get_instance();
    audio_state->set_playing(true);
    audio_state->set_pause(false);
  }
}

void RtdAud::play()
{
  printf("[RTDAUD]play\n");
  Audio_s *audio_state = S_Audio_s::get_instance();
  if (!cur_nr.path.empty())
    addfile(cur_nr);
}

void RtdAud::stop_player()
{
  printf("[RTDAUD]stop_player\n");
  if (g_pb->m_pFManager) {
    g_pb->m_pFManager->Stop();
    g_pb->ClearEventQueue();
  }
  Audio_s *audio_state = S_Audio_s::get_instance();
  audio_state->set_playing(false);
  audio_state->p->set_title("");
  audio_state->p->set_artist("");
  audio_state->p->set_album("");
}

void RtdAud::pause()
{
  Audio_s *audio_state = S_Audio_s::get_instance();
  
  printf("[RTDAUD]pause\n");
  if (audio_state->p_pause()) {
    if (g_pb->m_pFManager)
      g_pb->m_pFManager->Run();
    audio_state->set_pause(false);
  } else {
    if (g_pb->m_pFManager)
      g_pb->m_pFManager->Pause();
    audio_state->set_pause(true);
  }
}

void RtdAud::ff()
{
  printf("[RTDAUD]ff\n");
  gather_info();

  /*
  if (!((cur_time + 10) > total_time))
  */
  setpos(cur_time + 10);
}

void RtdAud::fb()
{
  printf("[RTDAUD]fb\n");
  gather_info();

  if (cur_time == 0)
    return;

  if ((cur_time - 10) < 0)
    setpos(0);
  else
    setpos(cur_time - 10);
}

int RtdAud::getpos()
{
  int                ret = 0;
  CNavigationFilter *pNav = g_pb->m_pSource;

  if (pNav) {
    NAVPLAYBACKSTATUS stat;

    if (pNav->GetPlaybackStatus(&stat) == S_OK)
      ret = stat.elapsedTime.seconds;
  } 
  return ret;
}

void RtdAud::setpos(int p)
{
  uint32_t cmdid;

  printf("[RTDAUD]setpos %d seconds\n", p);
  if (g_pb->m_pSource) 
    g_pb->m_pSource->PlayAtTime(-1, p, 0, &cmdid);
}

void RtdAud::mute()
{
  int vol = getvol();

  if (vol == 0) {
    setvol(old_volume);
    old_volume = 0;
  } else {
    old_volume = vol;
    setvol(0);
  }
}

void RtdAud::volup()
{
  int vol = getvol() + 4;
  if (vol > 100)
    vol = 100;
  setvol(vol);
}

void RtdAud::voldown()
{
  int vol = getvol() - 4;
  if (vol < 0)
    vol = 0;
  setvol(vol);
}

void RtdAud::setvol(int volume)
{
  printf("[RTDAUD]setvol %d\n", volume);
}

int RtdAud::getvol()
{
  printf("[RTDAUD]getvol\n");
  return 100;
}

bool RtdAud::supports_rtp() const
{
  // FIXME: does it?
  return false;
}

void RtdAud::reconfigure()
{
  printf("[RTDAUD]reconfigure\n");
}

void RtdAud::collect_info(const string& filename)
{
  printf("[RTDAUD]collect_info\n");
}

bool RtdAud::loaded()
{
  return is_loaded;
}

void RtdAud::release_device()
{
  printf("[RTDAUD]release_device\n");
  stop();
  is_loaded = false;
}

void RtdAud::restore_device()
{
  printf("[RTDAUD]restore_device\n");
  is_loaded = true;
}

