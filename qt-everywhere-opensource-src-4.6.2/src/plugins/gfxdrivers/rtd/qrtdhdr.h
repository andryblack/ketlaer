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

#ifndef QRTDHDR_H
#define QRTDHDR_H

#include <QtGui/qcolor.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_QWS_TEMP_DIR
#define QT_QWS_TEMP_DIR "/tmp"
#endif

#ifdef QT_PRIVATE_QWS
#define QT_RTD_DATADIR(DISPLAY) QString("%1/qtembedded-%2-%3") \
                                .arg(QT_QWS_TEMP_DIR).arg(getuid()).arg(DISPLAY)

#define QT_RTD_MAP(DISPLAY)           QT_RTD_DATADIR(DISPLAY) \
                                      .append("/qtvfb_map")
#define QT_RTD_SOUND_PIPE(DISPLAY)    QT_RTD_DATADIR(DISPLAY) \
                                      .append("/qt_soundserver")
#define QTE_PIPE(DISPLAY)             QT_RTD_DATADIR(DISPLAY) \
                                      .append("/QtEmbedded")
#define QTE_PIPE_QRTD(DISPLAY)        QTE_PIPE(DISPLAY)
#else
#define QT_RTD_DATADIR(DISPLAY) QString("%1/qtembedded-%2") \
                                .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QT_RTD_MAP(DISPLAY)           QString("/tmp/.qtrtd_map-%1").arg(DISPLAY)
#define QT_RTD_SOUND_PIPE(DISPLAY)    QString("/tmp/.qt_soundserver-%1").arg(DISPLAY)
#define QTE_PIPE(DISPLAY)             QT_RTD_DATADIR(DISPLAY)   \
                                      .append("/QtEmbedded-%1") \
                                      .arg(DISPLAY)
#define QTE_PIPE_QRTD(DISPLAY)        QString("/tmp/qtembedded-%1/QtEmbedded-%1") \
                                      .arg(DISPLAY)
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QRTDHDR_H
