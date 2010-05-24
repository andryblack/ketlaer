#include "rtdmov.hpp"
#include "simple_movie.hpp"
#include "common-feature.hpp"
#include "movie_config.hpp"
#include "SSaverobj.h"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <fstream>

#include <libketlaer.h>

using std::string;

static void do_play(const char *file)
{
  VideoPlayback *pb = getVideoPlayback();
  CFlowManager *pFManager;
  bool stop = false;
  bool playing = true;
  bool pause = false;

  printf("[RTDMOV]do_play '%s'\n", file);

  if (pb->LoadMedia((char*)file) == S_OK) {
    printf("[RTDMOV]media loaded.\n");
    pFManager = pb->m_pFManager;
    pFManager->SetRate(256);
    pb->m_pAudioOut->SetFocus();
    pb->m_pAudioOut->SetVolume(0);
    pb->SetVideoOut(VO_VIDEO_PLANE_V1, 0, 0);
    pb->ScanFileStart(false);
    pFManager->Run();
    while(!stop) {
      EVId    EventId;
      EVCode  EventCode;
      long    ParamSize;
      long*   pParams;

      if(pFManager->GetEvent(&EventId, &EventCode, &ParamSize, &pParams, 10) == S_OK) {
	switch(EventCode) {
	case FE_Playback_FatalError:
	  stop = true;
	  break;
	case FE_Playback_Completed:
	  stop = true;
	  break;
	default:
	  printf("[RTDMOV]playback event %d not handled\n", EventCode);
	  break;
	}
	pFManager->FreeEventParams(EventId);
      }
    }
    pFManager->Stop();
  }
  else
    printf("[RTDMOV]unable to load media\n");
}

static void play_file(const string& file)
{
  init_libketlaer();
  S_SSaverobj::get_instance()->StopCounter();
  run::aquire_exclusive_access();
  S_Render::get_instance()->device->unlock();
  DG_DrawRectangle(getScreenSurface(), 
		   0, 
		   0, 
		   getScreenRect()->width, 
		   getScreenRect()->height, 
		   RESERVED_COLOR_KEY, 
		   NULL);
  do_play(file.c_str());
  S_Render::get_instance()->device->lock();
  S_Render::get_instance()->complete_redraw();
  run::release_exclusive_access();
  S_SSaverobj::get_instance()->ResetCounter();
  uninit_libketlaer();
}

RtdMov::RtdMov()
  : MoviePlayer(true, true, true, true)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-rtdmov", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-rtdmov", nl_langinfo(CODESET));
#endif
}

RtdMov::~RtdMov()
{
}

void RtdMov::play_movie(const string& paths, bool window)
{
  printf("[RTDMOV]play_movie %s\n", paths.c_str());
  string file = "file://" + paths.substr(1);
  play_file(file);
}

void RtdMov::play_disc(const string& play_cmd)
{
  printf("[RTDMOV]play_disc %s\n", play_cmd.c_str());
  play_file(play_cmd);
}

void RtdMov::play_vcd(const string& device)
{
  printf("[RTDMOV]play_disc %s\n", device.c_str());
  play_file(device);
}

void RtdMov::play_dvd(const string& device)
{
  printf("[RTDMOV]play_dvd %s\n", device.c_str());
  play_dvd(device);
}
