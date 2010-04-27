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

#define RESERVED_COLOR_KEY 0x00080810

void firmware_init();
void firmware_uninit();
void audio_firmware_configure();
void video_firmware_configure();
void sendDebugMemoryAndAllocateDumpMemory(long videoDumpSize, long audioDumpSize);

SetupClass *SetupClass::m_ptr = new SetupClass();

QT_BEGIN_NAMESPACE

class QrtdScreenPrivate
{
public:
  QrtdScreenPrivate();
  ~QrtdScreenPrivate();

  QWSMouseHandler *mouse;
#ifndef QT_NO_QWS_KEYBOARD
  QWSKeyboardHandler *keyboard;
#endif

  VO_RECTANGLE rect;
  HANDLE hDisplay;
  HANDLE hSurface;
};

QrtdScreenPrivate::QrtdScreenPrivate()
    : mouse(0)

{
#ifndef QT_NO_QWS_KEYBOARD
  keyboard = 0;
#endif
  hDisplay = 0;
  hSurface = 0;
}

QrtdScreenPrivate::~QrtdScreenPrivate()
{
  delete mouse;
#ifndef QT_NO_QWS_KEYBOARD
  delete keyboard;
#endif
  if (hSurface)
    DG_CloseSurface(hSurface);
  if (hDisplay)
    DG_ReleaseDisplayHandle(hDisplay);
}

QrtdScreen::QrtdScreen(int display_id)
    : QScreen(display_id), d_ptr(new QrtdScreenPrivate)
{
  printf("QrtdScreen::QrtdScreen()\n");
  data = 0;
}

QrtdScreen::~QrtdScreen()
{
  delete d_ptr;
}

static HANDLE GetSurfaceHandle (int width, int height, PIXEL_FORMAT pixFormat)
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

bool QrtdScreen::connect(const QString &displaySpec)
{
  printf("connect\n");

  CLNT_STRUCT clnt;
  HRESULT *res = NULL;
  VoutUtil *vo = NULL;

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
  assert(res); free(res);
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
  vo = new VoutUtil();
  VO_COLOR bgColor;
  bgColor.c1 = 0x00; // red
  bgColor.c2 = 0x00; // green
  bgColor.c3 = 0x00; // blue
  bgColor.isRGB = true;
  vo->SetBackground( bgColor, true);
  short RGB2YUVcoeff[12] = {132, 258, 50, 948, 875,
			    225, 225, 836, 988, 128, 128, 16};
  vo->ConfigureOSD(VO_OSD_LPF_TYPE_DROP, RGB2YUVcoeff);
  vo->SetDeintMode(VO_VIDEO_PLANE_V1, VO_DEINT_MODE_AUTO);
  vo->SetDeintMode(VO_VIDEO_PLANE_V2, VO_DEINT_MODE_AUTO);
  vo->SetBrightness(32);
  vo->SetContrast(32);
  vo->SetTVtype(VO_TV_TYPE_16_BY_9_AUTO);
  CVideoOutFilter::SetSystemRescaleMode(VO_RESCALE_MODE_KEEP_AR_AUTO);
  VO_RECTANGLE rectNTSC = { 0, 0, 720,  480  };
  VO_RECTANGLE rectPAL  = { 0, 0, 720,  576  };
  VO_RECTANGLE rect720  = { 0, 0, 1280, 720  };
  VO_RECTANGLE rect1080 = { 0, 0, 1920, 1080 };
  VO_RECTANGLE rect800x600 = { 0, 0, 800, 600 };
  d_ptr->rect = rect720;
  vo->SetRescaleMode(VO_VIDEO_PLANE_V1, VO_RESCALE_MODE_KEEP_AR_AUTO, d_ptr->rect);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_NTSC, rectNTSC);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_PAL, rectPAL);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD720_50HZ, rect720);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD720_60HZ, rect720);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD1080_50HZ, rect1080);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_HD1080_60HZ, rect1080);
  vo->UpdateVoutRectangleSetup (VO_VIDEO_PLANE_V1, VIDEO_SVGA800x600, rect800x600);
  vo->ApplyVideoStandardSetup();
  bgColor.c1 = 0x00; // red
  bgColor.c2 = 0x00; // green
  bgColor.c3 = 0xff; // blue
  bgColor.isRGB = true;
  vo->ApplyVoutDisplayWindowSetup(bgColor, 0);
  vo->ConfigGraphicCanvas(VO_GRAPHIC_OSD,rect720,rect720,false);
  SetAudioSpeakerMode(TWO_CHANNEL);
  SetAudioAGCMode(AUDIO_AGC_DRC_OFF);
  SetAudioSPDIFMode(SPDIF_LPCM);
  SetAudioVolume(31);
  SetAudioForceChannelCtrl(31);
  setFormatSCART(false);
  board_codec_mute(false);
  board_dac_mute(false);

  HDMI_Controller hdmi;
  hdmi.Send_AudioMute(HDMI_SETAVM); //get spdif output

  d_ptr->hSurface = GetSurfaceHandle(d_ptr->rect.width,
				     d_ptr->rect.height,
				     Format_32);

  DG_DrawRectangle(d_ptr->hSurface,
		   0,
		   0,
		   d_ptr->rect.width,
		   d_ptr->rect.height,
		   RESERVED_COLOR_KEY,
		   NULL);

  d_ptr->hDisplay = DG_GetDisplayHandle();

  DG_DisplayArea (d_ptr->hDisplay,
		  0,
		  0,
		  d_ptr->rect.width,
		  d_ptr->rect.height,
		  d_ptr->hSurface,
		  0,
		  0,
		  Alpha_Constant,
		  0xff,
		  ColorKey_Src,
		  RESERVED_COLOR_KEY);

  data = (uchar*)DG_GetSurfaceRowDataPointer(d_ptr->hSurface);

  dw = w = d_ptr->rect.width;
  dh = h = d_ptr->rect.height;
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
  firmware_uninit();
  board_uninit();
  DG_UnInit();
  se_close();
  md_close();
  pli_freeAllMemory();
  pli_resetRPC();
  pli_close();
}

bool QrtdScreen::initDevice()
{
  printf("initDevice.\n");

#ifndef QT_NO_QWS_MOUSE_QRTD
  d_ptr->mouse = new QrtdMouseHandler(QLatin1String("QrtdMouse"));
  qwsServer->setDefaultMouse("None");
  if (d_ptr->mouse)
    d_ptr->mouse->setScreen(this);
#endif

#if !defined(QT_NO_QWS_KBD_QRTD) && !defined(QT_NO_QWS_KEYBOARD)
  d_ptr->keyboard = new QrtdKeyboardHandler();
  qwsServer->setDefaultKeyboard("None");
#endif
#ifndef QT_NO_QWS_CURSOR
  //QScreenCursor::initSoftwareCursor();
#endif
  return true;
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

inline void qt_memconvert(quint32 *dest, const quint32 *src, int count)
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

inline void qt_rectconvert(quint32 *dest, const quint32 *src,
                           int x, int y, int width, int height,
                           int dstStride, int srcStride)
{
  char *d = (char*)(dest + x) + y * dstStride;
  const char *s = (char*)(src);
  for (int i = 0; i < height; ++i) {
    qt_memconvert((quint32*)d, (const quint32*)s, width);
    d += dstStride;
    s += srcStride;
  }
}

void QrtdScreen::blit(const QImage &image, const QPoint &topLeft, const QRegion &region)
{
  QWSDisplay::grab();
  quint32 *dest = reinterpret_cast<quint32*>(base());
  const int screenStride = linestep();
  const int imageStride = image.bytesPerLine();

  if (region.rectCount() == 1) {
    const QRect r = region.boundingRect();
    const quint32 *src = reinterpret_cast<const quint32*>(image.scanLine(r.y()))
      + r.x();
    qt_rectconvert(dest, src,
		   r.x() + topLeft.x(), r.y() + topLeft.y(),
		   r.width(), r.height(),
		   screenStride, imageStride);
  } else {
    const QVector<QRect> rects = region.rects();

    for (int i = 0; i < rects.size(); ++i) {
      const QRect r = rects.at(i);
      const quint32 *src = reinterpret_cast<const quint32*>(image.scanLine(r.y()))
	+ r.x();
      qt_rectconvert(dest, src,
		     r.x() + topLeft.x(), r.y() + topLeft.y(),
		     r.width(), r.height(),
		     screenStride, imageStride);
    }
  }
  QWSDisplay::ungrab();
}

void QrtdScreen::setMode(int ,int ,int)
{
}

void QrtdScreen::setBrightness(int)
{
}

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
#endif // QT_NO_QWS_QRTD

QT_END_NAMESPACE
