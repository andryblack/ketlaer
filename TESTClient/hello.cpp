#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
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

#include <SetupClass.h>
#include <hdmihelper.h>
#include <ConfigFile.h>
#include <RemoteMap.h>

void firmware_init();
void firmware_uninit();
void audio_firmware_configure();
void video_firmware_configure();
void sendDebugMemoryAndAllocateDumpMemory(long videoDumpSize, long audioDumpSize);

#define RESERVED_COLOR_KEY 0x842

VoutUtil        *g_vo = NULL;
HANDLE           g_hs = NULL;
HANDLE           g_hd = NULL;
VideoPlayback   *g_pb = NULL;

/*Configuration file class*/
ConfigFile Config;
IrMapFile RemoteConfig;

VO_RECTANGLE rect;
/*callbacks*/
extern "C" HRESULT *AUDIO_RPC_ToSystem_AudioHaltDone_0_svc(long *resvered, RPC_STRUCT *rpc, HRESULT *retval)
{
  printf("AUDIO_RPC_ToSystem_AudioHaltDone_0_svc\n");
  *retval = 0x10000000;
  return retval;
}

extern "C" HRESULT *VIDEO_RPC_ToSystem_VideoHaltDone_0_svc(long *reserved, RPC_STRUCT *rpc, HRESULT *retval)
{
  printf("AVIDEO_RPC_ToSystem_VideoHaltDone_0_svc\n");
  *retval = 0x10000000;
  return retval;
}
/***/

static void LoadTvConfig()
{
  printf("LoadTVConfig..\n");
  VO_COLOR bgColor;
  bgColor.c1 = 0x00; // red
  bgColor.c2 = 0x00; // green
  bgColor.c3 = 0x00; // blue
  bgColor.isRGB = true;
  g_vo->SetBackground(bgColor, true);
  short RGB2YUVcoeff[12] = {132, 258, 50, 948, 875,
			    225, 225, 836, 988, 128, 128, 16};
  g_vo->ConfigureOSD(VO_OSD_LPF_TYPE_DROP, RGB2YUVcoeff);
  g_vo->SetDeintMode(VO_VIDEO_PLANE_V1, VO_DEINT_MODE_AUTO);
  g_vo->SetDeintMode(VO_VIDEO_PLANE_V2, VO_DEINT_MODE_AUTO);
  g_vo->SetBrightness(setup->GetBrightness());
  g_vo->SetContrast(setup->GetContrast());
  VO_RECTANGLE rectNTSC = { 0, 0, 720,  480  };
  VO_RECTANGLE rectPAL  = { 0, 0, 720,  576  };
  VO_RECTANGLE rect720  = { 0, 0, 1280, 720  };
  VO_RECTANGLE rect1080 = { 0, 0, 1920, 1080 };
  VO_RECTANGLE rect800x600 = { 0, 0, 800, 600 };
  switch(setup->GetTvSystem()) {
  case VIDEO_NTSC:
    rect = rectNTSC;
    break;
  case VIDEO_PAL:
    rect = rectPAL;
    break;
  case VIDEO_HD720_50HZ:
    rect = rect720;
    break;
  case VIDEO_HD720_60HZ:
    rect = rect720;
    break;
  case VIDEO_HD1080_50HZ:
    rect = rect1080;
    break;
  case VIDEO_HD1080_60HZ:
    rect = rect1080;
    break;
  case VIDEO_HD1080_24HZ:
    rect = rect1080;
    break;
  case VIDEO_SVGA800x600:
    rect = rect800x600;
    break;
  }
  VO_RESCALE_MODE rescaleMode;
  VO_TV_TYPE      tvType;
  switch(setup->GetAspectRatio()) {
  case PS_4_3:
    tvType = VO_TV_TYPE_4_BY_3;
    rescaleMode = VO_RESCALE_MODE_KEEP_AR_PS_CNTR;
    break;
  case LB_4_3:
    tvType = VO_TV_TYPE_4_BY_3;
    rescaleMode = VO_RESCALE_MODE_KEEP_AR_LB_CNTR;
    break;
  case Wide_16_9:
  default:
    tvType = VO_TV_TYPE_16_BY_9_AUTO;
    rescaleMode = VO_RESCALE_MODE_KEEP_AR_AUTO;
    break;
  }
  g_vo->SetTVtype(tvType);
  CVideoOutFilter::SetSystemRescaleMode(rescaleMode);
  g_vo->SetRescaleMode(VO_VIDEO_PLANE_V1, rescaleMode, rect);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_NTSC, rectNTSC);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_PAL, rectPAL);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD720_50HZ, rect720);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD720_60HZ, rect720);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD1080_50HZ, rect1080);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD1080_60HZ, rect1080);
  g_vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_SVGA800x600, rect800x600);
  g_vo->ApplyVideoStandardSetup();
  bgColor.c1 = 0x00; // red
  bgColor.c2 = 0x00; // green
  bgColor.c3 = 0xff; // blue
  bgColor.isRGB = true;
  g_vo->ApplyVoutDisplayWindowSetup(bgColor, 0);
  g_vo->ConfigGraphicCanvas(VO_GRAPHIC_OSD,rect,rect,false);
  setFormatSCART(setup->GetVideoOutput() == VOUT_RGB);
  if (getHDMIPlugged()) {
    HDMI_Controller hdmi;
    switch(setup->GetSpdifMode()) {
    case SPDIF_RAW:
    case SPDIF_LPCM:
      hdmi.Send_AudioMute(HDMI_SETAVM);
      break;
    case HDMI_RAW:
    case HDMI_LPCM:
      hdmi.Send_AudioMute(HDMI_CLRAVM);
      break;
    default:
      break;
    }
  }
}

class MyHDMICallbacks : public HDMICallbacks {
  virtual void TvSystemChanged() {
    LoadTvConfig();
  }
};

static void LoadAudioConfig()
{
  SetAudioSpeakerMode(setup->GetSpeakerOut());
  SetAudioAGCMode(setup->GetAudioAGCMode());
  SetAudioSPDIFMode(setup->GetSpdifMode());
  SetAudioVolume(31); //volume is controlled by playback class
  SetAudioForceChannelCtrl(31);//??
}

static void Init()
{
  CLNT_STRUCT clnt;
  HRESULT *res = NULL;

  pli_setThreadName("MAIN");
  pli_init();
  //  preAllocateMemory();
  md_open();
  se_open();
  DG_Init();
  board_init();
  firmware_init();
  sendDebugMemoryAndAllocateDumpMemory(0, 0);
  VIDEO_INIT_DATA initData;
  initData.boardType = getTVE();
  clnt = prepareCLNT(BLOCK_MODE | USE_INTR_BUF | SEND_VIDEO_CPU, VIDEO_SYSTEM, VERSION);
  VIDEO_RPC_ToAgent_VideoCreate_0(&initData, &clnt);
  BYTE *audioDebugFlag;
  unsigned long audioPhyAddr;
  BYTE *videoDebugFlag;
  unsigned long videoPhyAddr;
  pli_allocContinuousMemoryMesg("DBF", 4, (BYTE**)&audioDebugFlag, &audioPhyAddr);
  pli_allocContinuousMemoryMesg("DBF", 4, (BYTE**)&videoDebugFlag, &videoPhyAddr);
#define AUDIO_DEBUG_FLAG        0x00000001
#define VIDEO_DEBUG_FLAG        0x00000001
  pli_IPCWriteULONG((BYTE*)audioDebugFlag, AUDIO_DEBUG_FLAG);
  pli_IPCWriteULONG((BYTE*)videoDebugFlag, VIDEO_DEBUG_FLAG);
  clnt = prepareCLNT(NONBLOCK_MODE | USE_POLL_BUF | SEND_AUDIO_CPU, D_PROGRAM, D_VERSION);
  set_debug_flag_0((int*)&audioPhyAddr, &clnt);
  clnt = prepareCLNT(NONBLOCK_MODE | USE_POLL_BUF | SEND_VIDEO_CPU, D_PROGRAM, D_VERSION);
  set_debug_flag_0((int*)&videoPhyAddr, &clnt);
  audio_firmware_configure();
  video_firmware_configure();
  g_vo = new VoutUtil();
  LoadTvConfig();
  LoadAudioConfig();
  board_codec_mute(false);
  board_dac_mute(false);
  InitHDMI(new MyHDMICallbacks());
}

static void UnInit()
{
  delete g_vo;

  DeInitHDMI();
  firmware_uninit();
  board_uninit();
  DG_UnInit();
  se_close();
  md_close();
  pli_freeAllMemory();
  pli_close();
}

HANDLE GetSurfaceHandle (int width, int height, PIXEL_FORMAT pixFormat)
{
  SURFACEDESC desc;
  HANDLE ret;

  // Create surface for bitmap memory which will be select to the psd->addr
  memset(&desc, 0, sizeof(desc));
  desc.dwSize = sizeof(SURFACEDESC);
  desc.dwHeight = height;
  desc.dwWidth =  width;
  desc.dwColorKey = RESERVED_COLOR_KEY;
  desc.pixelFormat = pixFormat;
  desc.storageMode = 1;

  ret = DG_CreateSurface(&desc);
  return ret;
}

int main(int argc, char **argv)
{
  int ret;

  Init();

  g_hs = GetSurfaceHandle(rect.width, rect.height, Format_16);

  DG_DrawRectangle(g_hs,
		   0,
		   0,
		   rect.width,
		   rect.height,
		   RESERVED_COLOR_KEY,
		   NULL);

  g_hd = DG_GetDisplayHandle();

  DG_DisplayArea (g_hd,
		  0,
		  0,
		  rect.width,
		  rect.height,
		  g_hs,
		  0,
		  0,
		  Alpha_Constant,
		  0xff,
		  ColorKey_Src,
		  RESERVED_COLOR_KEY);

  printf("load video..\n");
  g_pb = new VideoPlayback(MEDIATYPE_None);
  char file[4096];
  if (argc == 2) {
    sprintf(file, "file://%s", argv[1]);
  } else {
    printf("no media file passed. using default.\n");
    //strcpy(file, "file:///tmp/DATA/media/audio/Various/Sex Pistols - My Way.mp3");
    strcpy(file, "file:///tmp/DATA/media/video/kinder/lights.wmv");
  }
  printf("play %s\n", file);
  g_pb->LoadMedia(file);
  g_pb->m_pFManager->Run();
  g_pb->m_pFManager->SetRate(256);
  g_pb->m_pAudioOut->SetFocus();
  g_pb->m_pAudioOut->SetVolume(0);
  g_pb->SetVideoOut(VO_VIDEO_PLANE_V1, 0, 0);
  g_pb->ScanFileStart(false);

  while(1) {
    char cmd[256];
    printf("cmd:");
    fflush(stdout);
    fgets(cmd, sizeof(cmd), stdin);
    cmd[strlen(cmd)-1] = 0;
    if (strcmp(cmd, "quit") == 0) break;
    g_pb->m_pSource->ExecuteCommandLine(cmd);
  }

  g_pb->m_pFManager->Stop();

  UnInit();

  printf("end.\n");
}
