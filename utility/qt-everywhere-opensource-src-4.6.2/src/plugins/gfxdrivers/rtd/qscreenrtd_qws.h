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

#ifndef QSCREENRTD_QWS_H
#define QSCREENRTD_QWS_H

#include <QtGui/qscreen_qws.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_QWS_RTD

#define RTDBLIT

class QrtdScreenPrivate;

class Q_GUI_EXPORT QrtdScreen : public QScreen
{
public:
    explicit QrtdScreen(int display_id);
    virtual ~QrtdScreen();
    virtual bool connect(const QString &displaySpec);
    virtual void disconnect();
    virtual bool initDevice();
    virtual bool supportsDepth(int) const;
    virtual void solidFill(const QColor &color, const QRegion &region);
#ifdef RTDBLIT
    virtual void blit(const QImage &image, const QPoint &topLeft, const QRegion &region);
#endif
    virtual void setMode(int nw,int nh,int nd);
#ifdef QTOPIA_RTD_BRIGHTNESS
    static void setBrightness(int b);
#endif

private:
    QrtdScreenPrivate *d_ptr;
};

#endif // QT_NO_QWS_RTD

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSCREENRTD_QWS_H
