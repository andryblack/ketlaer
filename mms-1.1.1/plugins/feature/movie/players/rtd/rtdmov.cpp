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
#include <qtkeys.h>

using std::string;
using std::vector;

typedef vector<string> filearray;

static void do_seek(VideoPlayback *pb, int offset)
{
  CNavigationFilter *pNav = pb->m_pSource;
  NAVPLAYBACKSTATUS  stat;
  int                newpos;
  uint32_t           cmdid;

  if (pNav) {
    if (pNav->GetPlaybackStatus(&stat) == S_OK) {
      newpos = stat.elapsedTime.seconds + offset;
      if (newpos < 0) 
	newpos = 0;
      pNav->PlayAtTime(-1, newpos, 0, &cmdid);
    }
  }
}

static int do_play(const char *file)
{
  int            ret = Key_MediaNext;
  VideoPlayback *pb = getVideoPlayback();
  CFlowManager  *pFManager;
  bool           stop = false;
  bool           playing = true;
  bool           pause = false;

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

      if(pFManager->GetEvent(&EventId, 
			     &EventCode, 
			     &ParamSize, 
			     &pParams, 
			     10) == S_OK) {
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
      if (!stop) {
	fd_set  set;
	timeval tv;
	int     fd = ir_getfd();
	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	if (select(fd+1, &set, NULL, NULL, &tv)) {
	  int key = ir_getkey();

	  switch(key) {
	  case Key_MediaNext:
	    ret = key;
	    stop = true;
	    break;
	  case Key_MediaPrevious:
	    ret = key;
	    stop = true;
	    break;
	  case Key_MediaStop:
	    ret = key;
	    stop = true;
	    break;
	  case Key_Less:
	    do_seek(pb, -1 * 60);
	    break;
	  case Key_Greater:
	    do_seek(pb, +5 * 60);
	    break;
	  }
	}
      }
    }
    pFManager->Stop();
    pb->ClearEventQueue();
  }
  else
    printf("[RTDMOV]unable to load media\n");
  return ret;
}

static void add_element(const string& file, filearray &files)
{
  if (file.find("://") == string::npos) 
    files.push_back("file://"+file);
  else
    files.push_back(file);
}

static void split_list(const string& list, filearray &files)
{
  string element;
  const char *s = list.c_str();
  bool is_string = false;

  while(*s) {
    if (*s == ' ' && !is_string) {
      if (element.length())
	add_element(element, files);
      element = "";
      s++;
    }
    else {
      if (*s == '\\' && s[1]) {
	s++;
	element += *s++;
      }
      else if (*s == '"') {
	is_string = !is_string;
	s++;
      }
      else {
	element += *s++;
      }
    }
  }
  if (element.length())
    add_element(element, files);
}

static void do_playlist(const string& list)
{
  filearray files;
  int       idx = 0;
  bool      stop = false;

  split_list(list, files);
  while (!stop) {
    switch(do_play(files[idx].c_str())) {
    case Key_MediaPrevious:
      if (idx) 
	idx--;
      else
	stop = true;
      break;
    case Key_MediaNext:
      if (idx < files.size() -1)
	idx++;
      else
	stop = true;
      break;
    case Key_MediaStop:
      stop = true;
      break;
    }
  }
}

static void play_file(const string& file, bool isList)
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
  if (isList)
    do_playlist(file);
  else
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
  printf("[RTDMOV]init\n");
}

RtdMov::~RtdMov()
{
  printf("[RTDMOV]deinit\n");
}

void RtdMov::play_movie(const string& paths, bool window)
{
  printf("[RTDMOV]play_movie %s\n", paths.c_str());
  play_file(paths, true);
}

void RtdMov::play_disc(const string& play_cmd)
{
  printf("[RTDMOV]play_disc %s\n", play_cmd.c_str());
  play_file(play_cmd, false);
}

void RtdMov::play_vcd(const string& device)
{
  printf("[RTDMOV]play_disc %s\n", device.c_str());
  play_file(device, false);
}

void RtdMov::play_dvd(const string& device)
{
  printf("[RTDMOV]play_dvd %s\n", device.c_str());
  play_file(device, false);
}
