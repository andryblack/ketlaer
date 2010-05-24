#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#include "libketlaer.h"

#include "hdmihelper.h"
#include "ConfigFile.h"
#include "RemoteMap.h"

void firmware_init();
void firmware_uninit();
void audio_firmware_configure();
void video_firmware_configure();
void sendDebugMemoryAndAllocateDumpMemory(long videoDumpSize, long audioDumpSize);

static VoutUtil      *g_vo = NULL;
static IrMapFile     *g_pIrMap = NULL;
static int            g_irfd = -1;
static HANDLE         g_hScreen = NULL;
static HANDLE         g_hDisplay = NULL;
static VO_RECTANGLE   rect;
static VideoPlayback *g_pb = NULL;

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

HANDLE getSurfaceHandle (int width, int height, PIXEL_FORMAT pixFormat)
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

static void Init()
{
  CLNT_STRUCT clnt;
  HRESULT *res = NULL;

  pli_setThreadName("MAIN");
  pli_init();
  /*
  void *adr1, *adr2;
  adr1 = pli_allocContinuousMemoryMesg("PreAlloc", 16*1024*1024, 0, 0);
  adr2 = pli_allocContinuousMemoryMesg("PreAlloc", 16*1024*1024, 0, 0);
  pli_freeContinuousMemoryMesg("PreAlloc", adr1);
  pli_freeContinuousMemoryMesg("PreAlloc", adr2);
  */
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
  g_hScreen = getSurfaceHandle(rect.width, rect.height, Format_32);
  DG_DrawRectangle(g_hScreen,
		   0,
		   0,
		   rect.width,
		   rect.height,
		   RESERVED_COLOR_KEY,
		   NULL);
  g_hDisplay = DG_GetDisplayHandle();
  DG_DisplayArea (g_hDisplay,
		  0,
		  0,
		  rect.width,
		  rect.height,
		  g_hScreen,
		  0,
		  0,
		  Alpha_Constant,
		  0xff,
		  ColorKey_Src,
		  RESERVED_COLOR_KEY);
  g_pb = new VideoPlayback(MEDIATYPE_None);
}

static void UnInit()
{
  delete g_pb;
  DG_ReleaseDisplayHandle(g_hDisplay);
  DG_CloseSurface(g_hScreen);
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

static int initcount = 0;

void init_libketlaer()
{
  if (!initcount++) {
    ConfigFile cfg;
    g_pIrMap = new IrMapFile(cfg.GetRemoteType());
    Init();
    g_irfd = open("/dev/venus_irrp", O_RDWR);
    ioctl(g_irfd, VENUS_IR_IOC_FLUSH_IRRP, NULL);
    ioctl(g_irfd, VENUS_IR_IOC_SET_PROTOCOL, g_pIrMap->GetProtocol());
    ioctl(g_irfd, VENUS_IR_IOC_SET_DEBOUNCE, 100); 
    printf("ir protocol=%d\n", g_pIrMap->GetProtocol());
  }
}

void uninit_libketlaer()
{
  if (!--initcount) {
    close(g_irfd);
    UnInit();
    delete g_pIrMap;
  }
}

int ir_getfd()
{
  return g_irfd;
}

int ir_getkey(bool &repeated)
{
  int key = 0, rep = 0, qtkey;

  read(g_irfd, &key, sizeof(key));
  read(g_irfd, &rep, sizeof(rep));
  repeated = rep == 1;
  qtkey = g_pIrMap->GetQtKey(key);
  printf("[LIBKETLAER]ir %lx %lx %lx\n", key, rep, qtkey);
  return qtkey;
}

int ir_getkey()
{
  bool dummy;
  return ir_getkey(dummy);
}

HANDLE getScreenSurface()
{
  return g_hScreen;
}

VO_RECTANGLE *getScreenRect()
{
  return &rect;
}

VideoPlayback *getVideoPlayback()
{
  return g_pb;
}
