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

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <qrtdhdr.h>
#include <qkbdrtd_qws.h>

#ifndef QT_NO_QWS_KEYBOARD
#ifndef QT_NO_QWS_KBD_RTD

#include <qwindowsystem_qws.h>
#include <qsocketnotifier.h>
#include <qapplication.h>
#include <private/qcore_unix_p.h> // overrides QT_OPEN

#include <lib_ir.h>
#include <RemoteMap.h>

static IrMapFile *g_pIrMap = NULL;

QT_BEGIN_NAMESPACE

QrtdKeyboardHandler::QrtdKeyboardHandler()
    : QObject()
{
  if (!g_pIrMap)
    g_pIrMap = new IrMapFile();
  m_fd = open("/dev/venus_irrp", O_RDWR);
  ioctl(m_fd, VENUS_IR_IOC_FLUSH_IRRP, NULL);
  ioctl(m_fd, VENUS_IR_IOC_SET_PROTOCOL, g_pIrMap->GetProtocol());
  ioctl(m_fd, VENUS_IR_IOC_SET_DEBOUNCE, 100); 
  QSocketNotifier *notifier;
  notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
  connect(notifier, SIGNAL(activated(int)), this, SLOT(readKey()));
}

QrtdKeyboardHandler::~QrtdKeyboardHandler()
{
  close(m_fd);
}

void QrtdKeyboardHandler::readKey()
{
  int irkey, keycode, unicode;

  read(m_fd, &irkey, sizeof(irkey));
  if (irkey != 0) {
    keycode = g_pIrMap->GetQtKey(irkey);
    printf("irkey=%08lx qtkey=%08lx\n", irkey, keycode);
    if (keycode >= Qt::Key_A && keycode <= Qt::Key_Z)
      unicode = keycode - Qt::Key_A + 'a';
    else if (keycode >= Qt::Key_0 && keycode <= Qt::Key_9)
      unicode = keycode - Qt::Key_0 + '0';    
    else
      unicode = 0xffff;
    processKeyEvent(unicode, keycode, Qt::NoModifier, true, false);
    processKeyEvent(unicode, keycode, Qt::NoModifier, false, false);
  }
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_KBD_RTD
#endif // QT_NO_QWS_KEYBOARD
