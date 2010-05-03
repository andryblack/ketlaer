#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <assert.h>

#include <lib_RPC.h>
#include <lib_OSAL.h>
#include <lib_GPIO.h>
#include <lib_DirectGraphics.h>
#include <lib_DrawScale.h>
#include <lib_MoveDataEngine.h>
#include <lib_StreamingEngine.h>
#include <lib_setupdef.h>
#include <lib_Board.h>
#include <lib_stream.h>
#include <lib_AudioUtil.h>
#include <lib_VoutUtil.h>
#include <lib_PlaybackAppClass.h>
#include <lib_HDMIControl.h>

#include "SetupClass.h"
#include "hdmihelper.h"

static HDMICallbacks *g_pHDMICallbacks = NULL;

/*system/Include/Platform_Lib/Graphics/TimerHandler.h*/
class _command_buffer;

class TimerHandler {
public:
  virtual int HandleTimer(_command_buffer * );
  virtual ~TimerHandler();
};

/*system/Include/Platform_Lib/Graphics/user_cmd.h*/
class _command_buffer {
public:
  char length;
  char type;
  union {
    int32_t idApp;
    TimerHandler * pHandler;
  } ;
  int64_t timestamp;
  char repeat;
  char parm[113];
};

typedef _command_buffer COMMAND_BUFFER;

/*system/Include/Platform_Lib/TimerSubsystem/TimerSubsystem.cpp*/
class timer {
public:
  TimerHandler * pHandler;
  uint32_t idTimer;
  uint32_t uTimeout;
  int64_t dwClockExpires;
  int next;
};

int64_t ExtendNextTimeoutValue(TimerHandler * pHandler, uint32_t idTimer, uint32_t uTimeout);

int64_t TimerGetTickCount();

uint32_t SetTimer(TimerHandler * pHandler, uint32_t idTimer, uint32_t uTimeout);

bool KillTimer(TimerHandler * pHandler, uint32_t idTimer);

uint32_t GetNextTimeoutValue();

bool GetLatestTimer(_command_buffer * command, int64_t * nextPeriod);

void TimerInit();

void ResetStartTicks();

bool GetTimerStatus(TimerHandler * pHandler, uint32_t idTimer);

class AbstractAP
{
public:
  static int control_pipe[2];
};

int AbstractAP::control_pipe[2] = { -1, -1 };

class ShowStatusAP
{
public:
  static HANDLE ShowStatus(int, char *, int, int)
  {
    return NULL;
  }
  ShowStatusAP()
  {
    ShowStatus(0, "init", 0, 0);
  }
};

ShowStatusAP showStatusApp;

static void *ProcessTimer(COMMAND_BUFFER *pcmd)
{
  if (pcmd->pHandler) {
    pcmd->pHandler->HandleTimer(pcmd);
  }
}

static bool g_stopMAIN = false;

void *MAIN_thread(void*)
{
  pollfd pfds[1];
  int timer_advantage;
  COMMAND_BUFFER cmd;
  int oldstate;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);

  pfds[0].fd = AbstractAP::control_pipe[0];
  pfds[0].events = POLLIN;
  while(!g_stopMAIN) {
    if(poll(pfds, 1, 100) <= 0) {
      if(GetLatestTimer(&cmd, NULL) == true) {
	ProcessTimer(&cmd);
      }
      continue;
    }
    if(pfds[0].revents & POLLIN) {
      read(AbstractAP::control_pipe[0], &(cmd.length), 14);
      if (cmd.type == 125) {
	/* is this ok? on my DVI it does not work!
	   setup->SetTvSystem((ENUM_VIDEO_SYSTEM)cmd.parm[0]);
	   setup->SetTvStandard((ENUM_VIDEO_STANDARD)cmd.parm[1]);
	*/
	if (g_pHDMICallbacks) {
	  g_pHDMICallbacks->TvSystemChanged();
	}
      }
      else {
	printf("unhandled event %d\n", cmd.type);
      }
    }
  }
  return NULL;
}

extern void *HDMI_thread(void *param);

static pthread_t thread_id_MAIN = (pthread_t)-1;
static pthread_t thread_id_HDMI = (pthread_t)-1;

void InitHDMI(HDMICallbacks *pCallbacks)
{
  g_pHDMICallbacks = pCallbacks;
  pipe(AbstractAP::control_pipe);
  TimerInit();
  g_stopMAIN = false;
  pthread_create(&thread_id_MAIN, NULL, MAIN_thread, (void *)NULL);
  pthread_create(&thread_id_HDMI, NULL, HDMI_thread, (void *)NULL);
  sleep(2);
}

void DeInitHDMI()
{
  if (thread_id_HDMI != -1) {
    pthread_cancel(thread_id_HDMI);
  }
  if (thread_id_MAIN != -1) {
    g_stopMAIN = true;
    pthread_join(thread_id_MAIN, NULL);
  }
}
