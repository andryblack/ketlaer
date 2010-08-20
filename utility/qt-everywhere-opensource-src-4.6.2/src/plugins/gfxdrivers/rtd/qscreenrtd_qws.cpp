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

#include <libketlaer.h>

QT_BEGIN_NAMESPACE

/***********************
 * Playback Controller *
 ***********************/

static char *evcodes[] = {
  "EV_FILE_EOF", "FE_RECORD_STOP", "FE_RECORD_VOBU", 
  "FE_RECORD_CELL", "FE_RECORD_VOB", "FE_RECORD_FULL", "EV_DISK_FULL", 
  "FE_RECORD_NOSIGNAL", "FE_RECORD_SIGNAL", "FE_RECORD_COPYFREEDATA", 
  "FE_RECORD_COPYPROCTECTEDDATA", "FE_RECORD_NTSC", "FE_RECORD_PAL", 
  "FE_RECORD_VIDEO_FORMAT_CHANGE", "FE_RECORD_NO_OUTPUT_SPACE", 
  "FE_RECORD_OUTPUT_SPACE_AVAILABLE", "FE_RECORD_DISC_ERROR", 
  "FE_Playback_ResetSpeed", "FE_Playback_VideoEOS", "FE_Playback_AudioEOS", 
  "FE_Playback_IndexGenEOS", "FE_Playback_Completed", 
  "FE_Playback_DomainChange", "FE_Playback_TitleChange", 
  "FE_Playback_ChapterChange", "FE_Playback_TimeChange", 
  "FE_Playback_RateChange", "FE_Playback_CommandUpdate", 
  "FE_Playback_RequestTmpPml", "FE_Playback_SetAutoStop", 
  "FE_Playback_JPEGDecEOS", "FE_Playback_ResetRepeatMode", 
  "FE_Playback_FatalError", "FE_Playback_ReadError", 
  "FE_Playback_BitstreamError", "FE_Playback_StreamingInfoUpdate", 
  "FE_Playback_BufferingStart", "FE_Playback_BufferingRestart", 
  "FE_Playback_BufferingEnd", "FE_Playback_SetAudioPreference", 
  "FE_Playback_NoSignal", "FE_Playback_DRM_NOT_AUTH_USER", 
  "FE_Playback_DRM_RENTAL_EXPIRED", "FE_Playback_DRM_REQUEST_RENTAL", 
  "FE_Playback_AudioFatalError", "FE_Playback_BackwardToBeginning", 
  "FE_Playback_UnsupportedFormat", "FE_Playback_InvalidFile", 
  "FE_Playback_SpicFrame", "FE_Playback_ContentEncrypt", 
  "FE_Timeshift_PlaybackCatchRecord", "FE_Timeshift_RecordCatchPlayback", 
  "FE_Timeshift_LivePauseWrapAround", "FE_TRANSCODE_STOP", 
  "FE_TRANSCODE_VE_STOP", "FE_TRANSCODE_AE_STOP"
};


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

static int fixret(int r)
{
  if (r == S_OK) r = 0;
  return r;
}

void QrtdPlaybackServer::handleRequest(char *arg, char *res)
{
  int ret = 0;
  CPlaybackFlowManager *pFManager = playback->m_pFManager;

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
    ret = playback->LoadMedia(arg+10);
    playback->m_pFManager->SetRate(256);
    playback->m_pAudioOut->SetFocus();
    playback->m_pAudioOut->SetVolume(0);
    playback->SetVideoOut(VO_VIDEO_PLANE_V1, 0, 0);
    playback->ScanFileStart(false);
    sprintf(res, "OK:%d\n", fixret(ret));
  }
  //Play
  else if (strcmp(arg, "Play") == 0) {
    if (pFManager)
      ret = pFManager->Run();
    sprintf(res, "OK:%d\n", fixret(ret));
  }
  //Pause
  else if (strcmp(arg, "Pause") == 0) {
    if (pFManager)
      ret = pFManager->Pause();
    sprintf(res, "OK:%d\n", fixret(ret));
  }
  //Stop
  else if (strcmp(arg, "Stop") == 0) {
    if (pFManager)
      ret = pFManager->Stop();
    sprintf(res, "OK:%d\n", fixret(ret));
  }
  //GetEvent
  else if (strncmp(arg, "GetEvent ", 9) == 0) {
    int     timeoutms;
    EVId    evid = -1;
    EVCode  evcode = (EVCode)-1;
    long    paramsize = 0;
    long   *pparams = 0;;
    long    evsubcode = 0;
    char   *evname = "TIMEDOUT";

    sscanf(arg, "%*s %d", &timeoutms);
    if (pFManager) {
      ret = pFManager->GetEvent(&evid, 
				&evcode, 
				&paramsize, 
				&pparams,
				timeoutms);
      if (ret == S_OK) {
	if (pparams) 
	  evsubcode = *pparams;
	if (evcode >= 0 && evcode <= FE_TRANSCODE_AE_STOP) 
	  evname = evcodes[evcode];
      }
    }
    else {
      ret = -1;
      usleep(timeoutms * 1000);
    }
    sprintf(res, 
	    "OK:%d:%ld:%d:%s:%ld:%ld", 
	    fixret(ret), 
	    evid,
	    evcode, 
	    evname, 
	    evsubcode,
	    (long)pparams);
  }
  //FreeEvent
  else if (strncmp(arg, "FreeEvent ", 10) == 0) {
    EVId evid;
    sscanf(arg, "%*s %ld", &evid);
    if ((evid != -1) && pFManager)
      pFManager->FreeEventParams(evid);
    sprintf(res, "OK:0\n");
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
  playback = getVideoPlayback();
  for(;;) {
    server->waitForNewConnection(-1);
    handleClient();
  }
}

/****************
 * Private Data *
 ****************/

class QrtdScreenPrivate
{
public:
  QrtdScreenPrivate();
  ~QrtdScreenPrivate();

  void ScreenConnect();
  void ScreenDisconnect();

public:
  QWSMouseHandler    *mouse;
#ifndef QT_NO_QWS_KEYBOARD
  QWSKeyboardHandler *keyboard;
#endif
  QrtdPlaybackServer *server;
};

QrtdScreenPrivate::QrtdScreenPrivate()
{
  mouse     = 0;
#ifndef QT_NO_QWS_KEYBOARD
  keyboard  = 0;
#endif
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
  server = new QrtdPlaybackServer();
  server->start();
}

void QrtdScreenPrivate::ScreenDisconnect()
{
  server->terminate();
  //server->wait();
  delete server;
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
  init_libketlaer();
  d_ptr->ScreenConnect();
  data = (uchar*)DG_GetSurfaceRowDataPointer(getScreenSurface());

  dw = w = getScreenRect()->width;
  dh = h = getScreenRect()->height;
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
  uninit_libketlaer();
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
  const HANDLE hSurface = getScreenSurface();

  for (int i = 0; i < rects.size(); ++i) {
    const QRect r = rects.at(i);
    DG_DrawRectangle(hSurface,
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
