/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   This is part of the mms software                                      *
 *   This has been coded by lorenzodes@fastwebnet.it                       *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef SSAVEROBJ_H_
#define SSAVEROBJ_H_

#include "common.hpp"
#include "config.hpp"
#include "singleton.hpp"

#include <sys/time.h>

class SSaverobj
{

 private:
  int countsecs;
  bool initialised;
  static void * ThreadLoop(void * ptr);
  void SetOpacity();
  signed long long lastevent;
  int truncatefloat(float t){
    return static_cast<int>(t);
  }

 public:

  static pthread_mutex_t singleton_mutex;

  SSaverobj();
  virtual ~SSaverobj();

  signed long long lasttime(){
    Config *conf = S_Config::get_instance();
    if (conf->p_last_key() > lastevent)
      return conf->p_last_key();
    else
      return lastevent;
  }

  bool Init();

  void LockMutex();
  void UnlockMutex();

  void StopCounter();
  void ResetCounter();

  bool IsScreenBlank();
  Overlay ssoverlay;
  bool use_hw_shader;
};

typedef Singleton<SSaverobj> S_SSaverobj;

#endif /*SSAVEROBJ_H_*/
