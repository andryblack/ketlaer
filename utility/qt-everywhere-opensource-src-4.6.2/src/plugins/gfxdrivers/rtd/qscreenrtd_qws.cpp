/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_QWS_RTD

#define QTOPIA_RTD_BRIGHTNESS

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <qrtdhdr.h>
#include <qscreenrtd_qws.h>
#include <qkbdrtd_qws.h>
#include <qmousertd_qws.h>
#include <qwindowsystem_qws.h>
#include <qsocketnotifier.h>
#include <qapplication.h>
#include <qscreen_qws.h>
#include <qmousedriverfactory_qws.h>
#include <qkbddriverfactory_qws.h>
#include <qwsdisplay_qws.h>
#include <qdebug.h>
#include <qthread.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>

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
#include <ConfigFile.h>
#include <hdmihelper.h>

#define RESERVED_COLOR_KEY 0x00080810

void firmware_init();
void firmware_uninit();
void audio_firmware_configure();
void video_firmware_configure();
void sendDebugMemoryAndAllocateDumpMemory(long videoDumpSize, long audioDumpSize);

static VoutUtil     *g_vo = NULL;
static VO_RECTANGLE rect;

static ConfigFile *g_pConfig = NULL;

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
  default:
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
  CLNT_STRUCT  clnt;
  HRESULT     *res = NULL;

  if (!g_pConfig)
    g_pConfig = new ConfigFile(); //loads config to SetupClass

  pli_setThreadName("MAIN");
  pli_init();
  md_open();
  se_open();
  DG_Init();
  board_init();
  firmware_init();
  sendDebugMemoryAndAllocateDumpMemory(0, 0);
  VIDEO_INIT_DATA initData;
  initData.boardType = getTVE();
  clnt = prepareCLNT(BLOCK_MODE | USE_INTR_BUF | SEND_VIDEO_CPU, VIDEO_SYSTEM, VERSION);
  res = VIDEO_RPC_ToAgent_VideoCreate_0(&initData, &clnt);
  free(res);
  BYTE *audioDebugFlag;
  unsigned long audioPhyAddr;
  BYTE *videoDebugFlag;
  unsigned long videoPhyAddr;
  pli_allocContinuousMemoryMesg("DBF", 4, (BYTE**)&audioDebugFlag, &audioPhyAddr);
  pli_allocContinuousMemoryMesg("DBF", 4, (BYTE**)&videoDebugFlag, &videoPhyAddr);
  pli_IPCWriteULONG((BYTE*)audioDebugFlag, 1);
  pli_IPCWriteULONG((BYTE*)videoDebugFlag, 1);
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
  DeInitHDMI();
  delete g_vo;
  firmware_uninit();
  board_uninit();
  DG_UnInit();
  se_close();
  md_close();
  pli_freeAllMemory();
  pli_close();
}

QT_BEGIN_NAMESPACE

/***********************
 * Playback Controller *
 ***********************/

#define CMD_BUFFER (PATH_MAX+128)

class QrtdPlaybackServer : public QThread 
{
public:
  void run();
private:
  void handleClient();
  void handleRequest(char *arg, char *res);

  QLocalServer  *server;
  VideoPlayback *playback;
};

void QrtdPlaybackServer::handleRequest(char *arg, char *res)
{
  //Version
  if (strncmp(arg, "Version ", 8) == 0) {
    int version = -1;
    sscanf(arg, "%*s %d", &version);
    printf("client version %d\n", version);
    sprintf(res, "OK\n");
  }
  //GetReservedColorKey
  else if (strcmp(arg, "GetReservedColorKey") == 0) {
    sprintf(res, "OK:%d\n", RESERVED_COLOR_KEY);
  }
  //LoadMedia
  else if (strncmp(arg, "LoadMedia ", 10) == 0) {
    int ret = playback->LoadMedia(arg+10);
    playback->m_pFManager->SetRate(256);
    playback->m_pAudioOut->SetFocus();
    playback->m_pAudioOut->SetVolume(0);
    playback->SetVideoOut(VO_VIDEO_PLANE_V1, 0, 0);
    playback->ScanFileStart(false);
    sprintf(res, "OK:%d\n", ret);
  }
  //Play
  else if (strcmp(arg, "Play") == 0) {
    int ret = playback->m_pFManager->Run();
    sprintf(res, "OK:%d\n", ret);
  }
  //Pause
  else if (strcmp(arg, "Pause") == 0) {
    int ret = playback->m_pFManager->Pause();
    sprintf(res, "OK:%d\n", ret);
  }
  //Stop
  else if (strcmp(arg, "Stop") == 0) {
    int ret = playback->m_pFManager->Stop();
    sprintf(res, "OK:%d\n", ret);
  }
  //BAD
  else {
    sprintf(res, "BAD:%s\n", arg);
  }
}

void QrtdPlaybackServer::handleClient()
{
  QLocalSocket *client = server->nextPendingConnection();
  if (client) {
    char arg[CMD_BUFFER];
    char res[CMD_BUFFER];

    client->waitForReadyRead();
    client->readLine(arg, sizeof(arg));
    int len = strlen(arg);
    if (len) arg[len-1] = 0;
    handleRequest(arg, res);
    client->write(res, strlen(res));
    client->flush();
    delete client;
  }
}

void QrtdPlaybackServer::run()
{
  server = new QLocalServer();
  unlink("/tmp/.playback");
  server->listen(".playback");
  playback = new VideoPlayback(MEDIATYPE_None);
  for(;;) {
    server->waitForNewConnection(-1);
    handleClient();
  }
}

/****************
 * Private Data *
 ****************/

class QrtdScreenPrivate : public QObject
{
  Q_OBJECT

public:
  QrtdScreenPrivate();
  ~QrtdScreenPrivate();

  void ScreenConnect();
  void ScreenDisconnect();

private Q_SLOTS:
  void handleRequest();

public:
  QWSMouseHandler    *mouse;
#ifndef QT_NO_QWS_KEYBOARD
  QWSKeyboardHandler *keyboard;
#endif
  void               *hDisplay;
  void               *hSurface;
  QrtdPlaybackServer *server;
};

QrtdScreenPrivate::QrtdScreenPrivate()
  : QObject()
{
  mouse     = 0;
#ifndef QT_NO_QWS_KEYBOARD
  keyboard  = 0;
#endif
  hDisplay  = 0;
  hSurface  = 0;
  server    = 0;
}

QrtdScreenPrivate::~QrtdScreenPrivate()
{
  delete mouse;
#ifndef QT_NO_QWS_KEYBOARD
  delete keyboard;
#endif
}

void QrtdScreenPrivate::ScreenConnect()
{
  SURFACEDESC desc;

  memset(&desc, 0, sizeof(desc));
  desc.dwSize      = sizeof(SURFACEDESC);
  desc.dwWidth     = rect.width;
  desc.dwHeight    = rect.height;
  desc.dwColorKey  = RESERVED_COLOR_KEY;
  desc.pixelFormat = Format_32;
  desc.storageMode = 1;

  hSurface = DG_CreateSurface(&desc);

  DG_DrawRectangle(hSurface,
		   0,
		   0,
		   rect.width,
		   rect.height,
		   RESERVED_COLOR_KEY,
		   NULL);

  hDisplay = DG_GetDisplayHandle();

  DG_DisplayArea (hDisplay,
		  0,
		  0,
		  rect.width,
		  rect.height,
		  hSurface,
		  0,
		  0,
		  Alpha_Constant,
		  0xff,
		  ColorKey_Src,
		  RESERVED_COLOR_KEY);
  server = new QrtdPlaybackServer();
  server->start();
}

void QrtdScreenPrivate::ScreenDisconnect()
{
  server->terminate();
  server->wait();
  delete server;
  DG_ReleaseDisplayHandle(hDisplay);
  DG_CloseSurface(hSurface);
}

/*************************
 * Screen Implementation *
 *************************/

QrtdScreen::QrtdScreen(int display_id)
  : QScreen(display_id, BGRPixel), d_ptr(new QrtdScreenPrivate)
{
  data = 0;
}

QrtdScreen::~QrtdScreen()
{
  delete d_ptr;
}

bool QrtdScreen::connect(const QString &displaySpec)
{
  Init();
  d_ptr->ScreenConnect();
  data = (uchar*)DG_GetSurfaceRowDataPointer(d_ptr->hSurface);

  dw = w = rect.width;
  dh = h = rect.height;
  d = 32;
  lstep = w * 4;
  size = lstep * h;
  mapsize = size;
  screencols = 0;
  setPixelFormat(QImage::Format_ARGB32);
  pixeltype = BGRPixel;

  QStringList displayArgs = displaySpec.split(QLatin1Char(':'));

  // Handle display physical size spec.
  int dimIdxW = -1;
  int dimIdxH = -1;
  for (int i = 0; i < displayArgs.size(); ++i) {
    if (displayArgs.at(i).startsWith(QLatin1String("mmWidth"))) {
      dimIdxW = i;
      break;
    }
  }
  for (int i = 0; i < displayArgs.size(); ++i) {
    if (displayArgs.at(i).startsWith(QLatin1String("mmHeight"))) {
      dimIdxH = i;
      break;
    }
  }
  if (dimIdxW >= 0) {
    bool ok;
    int pos = 7;
    if (displayArgs.at(dimIdxW).at(pos) == QLatin1Char('='))
      ++pos;
    int pw = displayArgs.at(dimIdxW).mid(pos).toInt(&ok);
    if (ok) {
      physWidth = pw;
      if (dimIdxH < 0)
	physHeight = dh*physWidth/dw;
    }
  }
  if (dimIdxH >= 0) {
    bool ok;
    int pos = 8;
    if (displayArgs.at(dimIdxH).at(pos) == QLatin1Char('='))
      ++pos;
    int ph = displayArgs.at(dimIdxH).mid(pos).toInt(&ok);
    if (ok) {
      physHeight = ph;
      if (dimIdxW < 0)
	physWidth = dw*physHeight/dh;
    }
  }
  if (dimIdxW < 0 && dimIdxH < 0) {
    const int dpi = 72;
    physWidth = qRound(dw * 25.4 / dpi);
    physHeight = qRound(dh * 25.4 / dpi);
  }

  qDebug("RTD(%s): %d x %d x %d %dx%dmm (%dx%ddpi)", displaySpec.toLatin1().data(),
	 w, h, d, physWidth, physHeight, qRound(dw*25.4/physWidth), qRound(dh*25.4/physHeight) );

  return true;
}

void QrtdScreen::disconnect()
{
  data = 0;
  d_ptr->ScreenDisconnect();
  UnInit();
}

bool QrtdScreen::initDevice()
{
#ifndef QT_NO_QWS_MOUSE_RTD
  d_ptr->mouse = new QrtdMouseHandler(QLatin1String("QrtdMouse"));
  qwsServer->setDefaultMouse("None");
  if (d_ptr->mouse)
    d_ptr->mouse->setScreen(this);
#endif

#if !defined(QT_NO_QWS_KBD_RTD) && !defined(QT_NO_QWS_KEYBOARD)
  d_ptr->keyboard = new QrtdKeyboardHandler();
  qwsServer->setDefaultKeyboard("None");
#endif
#ifndef QT_NO_QWS_CURSOR
  QScreenCursor::initSoftwareCursor();
#endif
  return true;
}

bool QrtdScreen::supportsDepth(int d) const
{
  return d == 32;
}

void QrtdScreen::solidFill( const QColor &color, const QRegion &region)
{
  QWSDisplay::grab();
  const QRegion reg = region.translated(-offset()) & QRect(0, 0, dw, dh);
  const quint32 c = color.rgba();
  const QVector<QRect> rects = region.rects();

  for (int i = 0; i < rects.size(); ++i) {
    const QRect r = rects.at(i);
    DG_DrawRectangle(d_ptr->hSurface,
		     r.x(),
		     r.y(),
		     r.width(),
		     r.height(),
		     c,
		     NULL);
  }
  QWSDisplay::ungrab();
}

#ifdef RTDBLIT

template <class DST, class SRC>
static inline void rtd_memconvert(DST *dest, const SRC *src, int count)
{
  /* Duff's device */
  int n = (count + 7) / 8;
  switch (count & 0x07) {
  case 0: do { *dest++ = htonl(*src++);
  case 7:      *dest++ = htonl(*src++);
  case 6:      *dest++ = htonl(*src++);
  case 5:      *dest++ = htonl(*src++);
  case 4:      *dest++ = htonl(*src++);
  case 3:      *dest++ = htonl(*src++);
  case 2:      *dest++ = htonl(*src++);
  case 1:      *dest++ = htonl(*src++);
             } while (--n > 0);
  }
}

template <class DST, class SRC>
static inline void rtd_rectconvert(DST *dest, const SRC *src,
				   int x, int y, int width, int height,
				   int dstStride, int srcStride)
{
  char *d = (char*)(dest + x) + y * dstStride;
  const char *s = (char*)(src);
  for (int i = 0; i < height; ++i) {
    rtd_memconvert<DST,SRC>((DST*)d, (const SRC*)s, width);
    d += dstStride;
    s += srcStride;
  }
}

template <typename DST, typename SRC>
static inline void rtd_blit_template(QScreen *screen, 
				     const QImage &image,
				     const QPoint &topLeft, 
				     const QRegion &region)
{
  DST *dest = reinterpret_cast<DST*>(screen->base());
  const int screenStride = screen->linestep();
  const int imageStride = image.bytesPerLine();

  if (region.rectCount() == 1) {
    const QRect r = region.boundingRect();
    const SRC *src = reinterpret_cast<const SRC*>(image.scanLine(r.y()))
      + r.x();
    rtd_rectconvert<DST, SRC>(dest, src,
			      r.x() + topLeft.x(), r.y() + topLeft.y(),
			      r.width(), r.height(),
			      screenStride, imageStride);
  } else {
    const QVector<QRect> rects = region.rects();

    for (int i = 0; i < rects.size(); ++i) {
      const QRect r = rects.at(i);
      const SRC *src = reinterpret_cast<const SRC*>(image.scanLine(r.y()))
	+ r.x();
      rtd_rectconvert<DST, SRC>(dest, src,
				r.x() + topLeft.x(), r.y() + topLeft.y(),
				r.width(), r.height(),
				screenStride, imageStride);
    }
  }
}

void QrtdScreen::blit(const QImage &img, const QPoint &topLeft, const QRegion &reg)
{
  QWSDisplay::grab();
  const QRect bound = (region() & QRect(topLeft, img.size())).boundingRect();
  rtd_blit_template<quint32, quint32>(this, img, topLeft-offset(),
				      (reg & bound).translated(-topLeft));
  QWSDisplay::ungrab();
}
#endif

void QrtdScreen::setMode(int ,int ,int)
{
}

#ifdef QTOPIA_RTD_BRIGHTNESS
void QrtdScreen::setBrightness(int)
{
}
#endif

/*callbacks*/
extern "C" HRESULT *AUDIO_RPC_ToSystem_AudioHaltDone_0_svc(long *, RPC_STRUCT *, HRESULT *retval)
{
  printf("AUDIO_RPC_ToSystem_AudioHaltDone_0_svc\n");
  *retval = 0x10000000;
  return retval;
}

extern "C" HRESULT *VIDEO_RPC_ToSystem_VideoHaltDone_0_svc(long *, RPC_STRUCT *, HRESULT *retval)
{
  printf("AVIDEO_RPC_ToSystem_VideoHaltDone_0_svc\n");
  *retval = 0x10000000;
  return retval;
}
/***/
#endif // QT_NO_QWS_RTD

QT_END_NAMESPACE
