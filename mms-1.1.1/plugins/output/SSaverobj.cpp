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

#include "SSaverobj.h"
#include <cstdlib>
#include <sys/ioctl.h>
#include <sstream>
#include <unistd.h>
#include "renderer.hpp"
#include "config.h"
#include "rectangle_object.hpp"

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t initmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  main_switch = PTHREAD_COND_INITIALIZER;
pthread_t thread1;
int state;
int maxopacity;
int red;
int green;
int blue;
int width;
int height;
int currentopa;

pthread_mutex_t SSaverobj::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

using namespace std;

SSaverobj::SSaverobj()
  : ssoverlay("ScreenSaverObj")
{
  state = 0;
  initialised=false;
  Config * conf = S_Config::get_instance();
  width = conf->p_h_res();
  height = conf->p_v_res();
  red =   conf->p_blankcolor_r();
  green = conf->p_blankcolor_g();
  blue = conf->p_blankcolor_b();
  maxopacity = conf->p_blankopacity();
  countsecs = conf->p_blanktime();
  currentopa = 0;

}

SSaverobj::~SSaverobj()
{
}

void SSaverobj::ResetCounter()
{
  struct timeval tv;
  pthread_mutex_lock(&mymutex);
  gettimeofday(&tv,NULL);
  lastevent = tv.tv_sec;

  if (state==0)
    pthread_cond_signal(&main_switch);

  state = 1;
  pthread_mutex_unlock(&mymutex);
}

void SSaverobj::StopCounter()
{
  pthread_mutex_lock(&mymutex);
  state = 0;

  pthread_mutex_unlock(&mymutex);
}

bool SSaverobj::Init()
{
  if (countsecs == 0)
    return false;
  
  pthread_mutex_lock(&initmutex);
  if (!initialised){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    lastevent = tv.tv_sec;

    initialised = true;
    state = 1;
    if (maxopacity < 100)
      maxopacity = 100;

    pthread_create( &thread1, NULL, (void *(*)(void *))(ThreadLoop), this);
  }

  pthread_mutex_unlock(&initmutex);
  return true;
}

void SSaverobj::LockMutex()
{
  pthread_mutex_lock(&mymutex);
}

void SSaverobj::UnlockMutex()
{
  pthread_mutex_unlock(&mymutex);
}

void SSaverobj::SetOpacity()
{
  int maxopa = maxopacity;
  float step = static_cast<float>(maxopa)/21;
  Render * ren = S_Render::get_instance();

  switch (state){
  case 0:
  case 1: //make sure screensaver is not on
    if (use_hw_shader){
      ren->device->stop_shader();
    }
    else if (currentopa !=0){
      ren->wait_and_aquire();
      if   (ssoverlay.elements.size() > 0){
	ssoverlay.cleanup();
      }
      ren->image_mut.leaveMutex();
      currentopa = 0;
    }
    break;

  case 21: //make sure that opacity == maxopa
    if (currentopa != maxopa && ! use_hw_shader){
      ren->wait_and_aquire();
      if (ssoverlay.elements.size() > 0)
	ssoverlay.partial_cleanup();

      ssoverlay.add(new RObj(0, 0, width, height, red, green, blue, maxopa, 9));
      ren->draw_and_release("ScreenSaverObj", true);
      currentopa = maxopa;
    }

    break;

  case 2:
    if (use_hw_shader){
      ren->device->start_shader(maxopa, red, green, blue, 300);
      break;
    }

  default:
    if (!use_hw_shader && currentopa != truncatefloat((step+0.5)*(state-1))){
      ren->wait_and_aquire();
      if (ssoverlay.elements.size() > 0)
	ssoverlay.partial_cleanup();
      ssoverlay.add(new RObj(0,0,width,height,red,green,blue,
			     truncatefloat((step+0.5)*(state-1)), 9));

      ren->draw_and_release("ScreenSaverObj", true);
      currentopa = truncatefloat((step+0.5)*(state-1));
    }
    break;
  }
}

void * SSaverobj::ThreadLoop(void * ptr)
{
  SSaverobj * thisobj  = (SSaverobj*) ptr;
  Render * ren = S_Render::get_instance();
  thisobj->use_hw_shader = ren->device->supports_shader();

  struct timeval tv;
  while(true){
    pthread_mutex_lock(&mymutex);
    switch (state){

    case 0: //screensaver is disabled
      thisobj->SetOpacity();
      pthread_cond_wait(&main_switch, &mymutex);
      pthread_mutex_unlock(&mymutex);
      break;

    case 1:
      thisobj->SetOpacity();
      gettimeofday(&tv,NULL);
      if (thisobj->lasttime() + thisobj->countsecs > tv.tv_sec){
	pthread_mutex_unlock(&mymutex);
	mmsSleep(thisobj->lasttime() + thisobj->countsecs - tv.tv_sec);
      }
      else{
	state++;
	pthread_mutex_unlock(&mymutex);
      }

      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      gettimeofday(&tv,NULL);
      if (thisobj->lasttime() + thisobj->countsecs > tv.tv_sec){
	state=1;
      }
      else {
	thisobj->SetOpacity();
	state++;
      }
      pthread_mutex_unlock(&mymutex);
      mmsUsleep(30000);
      break;
    case 21: //here we poll just to see if there are new keypresses
      thisobj->SetOpacity();
      gettimeofday(&tv,NULL);
      if (thisobj->lasttime() + thisobj->countsecs > tv.tv_sec){
	state=1;
      }

      pthread_mutex_unlock(&mymutex);
      mmsUsleep(250000);
      break;
    default:
      state = 1;
      break;
    }
  }
}
