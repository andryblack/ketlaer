/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <qscreendriverplugin_qws.h>
#include <qscreenrtd_qws.h>
#include <qstringlist.h>

#ifndef QT_NO_LIBRARY
QT_BEGIN_NAMESPACE

class GfxrtdDriver : public QScreenDriverPlugin
{
public:
    GfxrtdDriver();

    QStringList keys() const;
    QScreen *create(const QString&, int displayId);
};

GfxrtdDriver::GfxrtdDriver()
: QScreenDriverPlugin()
{
}

QStringList GfxrtdDriver::keys() const
{
    QStringList list;
    list << "RTD";
    return list;
}

QScreen* GfxrtdDriver::create(const QString& driver, int displayId)
{
#ifndef QT_NO_QWS_RTD
    if (driver.toLower() == "rtd")
        return new QrtdScreen(displayId);
#else //QT_NO_QWS_RTD
    printf("QT buildt with QT_NO_QWS_RTD. No screen driver returned\n");
#endif //QT_NO_QWS_RTD
    return 0;
}

Q_EXPORT_STATIC_PLUGIN(GfxrtdDriver)
Q_EXPORT_PLUGIN2(qgfxrtd, GfxrtdDriver)

QT_END_NAMESPACE

#endif //QT_NO_LIBRARY
