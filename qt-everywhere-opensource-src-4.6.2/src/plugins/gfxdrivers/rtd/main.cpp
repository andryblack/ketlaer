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

class ScreenRtdDriver : public QScreenDriverPlugin
{
public:
    ScreenRtdDriver();

    QStringList keys() const;
    QScreen *create(const QString&, int displayId);
};

ScreenRtdDriver::ScreenRtdDriver()
: QScreenDriverPlugin()
{
  printf("plugin loaded.\n");
}

QStringList ScreenRtdDriver::keys() const
{
  printf("plugin keys.\n");
  QStringList list;
  list << "RTD";
  return list;
}

QScreen* ScreenRtdDriver::create(const QString& driver, int displayId)
{
  printf("plugin create.\n");
  if (driver.toLower() == "rtd")
    return new QrtdScreen(displayId);
  printf("fail!\n");
  return 0;
}

Q_EXPORT_STATIC_PLUGIN(ScreenRtdDriver)
Q_EXPORT_PLUGIN2(qscreenrtd, ScreenRtdDriver)

QT_END_NAMESPACE
#endif //QT_NO_LIBRARY
