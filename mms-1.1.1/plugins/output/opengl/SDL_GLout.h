/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   This is part of the mms software                                      *
 *                                                                         *
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



#ifndef SDL_GLOUT_H_
#define SDL_GLOUT_H_

#define GLX_GLXEXT_PROTOTYPES


#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h> 
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glu.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#define NUM_OF_LAYOUTS 20
#define SHADER_LAYOUT NUM_OF_LAYOUTS -1
#define BUFFER_OFFSET(i) (static_cast<char *>(NULL) + (i))

#define OBJOUTPUT(a) ( std::string( std::string("OpenGL: <") +  __func__ + "> " + (a) ).c_str())


//

  static pthread_mutex_t sdl_glout_singletonmutex = PTHREAD_MUTEX_INITIALIZER; //this is initialized before program execution

//

typedef struct {
  int x;
  int y;
  int w;
  int h;
  int type;
  int planes;
  char * bitmpap;
  bool freeme;
}draw_rect;


  enum GL_Action_t {GL_Action_Base, GL_Action_Fade, GL_Action_Spin, GL_Action_Move, GL_Action_Zoom, GL_Action_Slide};
class layoutobj {
public:
  GLuint id;
  GLuint buffer_id;
  bool active;
  GLenum type;
  int     planes;
  GLint width, height;
  GLfloat pos_x, pos_y;
  GLfloat angle;
  GLfloat alpha;
  GLfloat scale_x;
  GLfloat scale_y;
  GLfloat red, green, blue;
  bool centered;
  bool dirty;
  bool needrealloc;
  char * PBObuffer;
  bool Fit_to_Screen;
  std::vector<draw_rect> draw_rects;
  void ClearActions(){
    for (unsigned int t = 0; t < Actions.size(); t++){
      delete Actions[t];
    }
     Actions.clear();
  }

  inline void copytothis(const layoutobj * t){
    this->alpha = t->alpha;
    this->angle = t->angle;
    this->buffer_id = t->buffer_id;
    this->centered = t->centered;
    this->draw_rects = t->draw_rects;
    this->height = t->height;
    this->id = t->id;
    this->needrealloc = t->needrealloc;
    this->pos_x = t->pos_x;
    this->pos_y = t->pos_y;
    this->scale_x = t->scale_x;
    this->scale_y = t->scale_y;
    this->type = t->type;
    this->width = t->width;
    this->height = t->height;
    this->Fit_to_Screen = t->Fit_to_Screen;
    this->dirty = t->dirty;
    this->red = t->red;
    this->green = t->green;
    this->blue = t->blue;
    //this->Actions = t->Actions;
    this->Fit_to_Screen = t->Fit_to_Screen;
  }

  class GL_Action{
  protected:
    GL_Action_t ActionType;
  public:
    GL_Action() { ActionType = GL_Action_Base ; Expired = false; deferred = false;};
    virtual ~GL_Action(){};
    bool Aknowledged;
    uint64_t Start_at;
    bool deferred;
    unsigned int AllottedTime;//  1/100 of seconds 
    bool Expired;
    bool deactivateatend;
    virtual void PerformAction(layoutobj* , uint64_t ){};
  };

public:

 std::vector<GL_Action *> Actions;

  layoutobj(GLuint _id, GLuint _buffer_id)
  : id(_id),buffer_id(_buffer_id), active(false), type(GL_RGBA), planes(4), width(0), height(0), pos_x(0), pos_y(0),  
  angle(1.0f), alpha(1.0f), scale_x(1.0), scale_y(1.0), red(1.0), green(1.0), blue(1.0),
  centered(true), dirty(false), needrealloc(true), PBObuffer(NULL) 
  {
    draw_rects.clear();
  };
  ~layoutobj();
};

class SDL_GLout
{

private:
  int sdl_x, sdl_y;
  int real_x, real_y;
  bool requestedfullscreen;
  std::string sdl_caption;
  bool initted;
  int glrecmode;
  std::vector<layoutobj*> layouts;
  int bpp; //bits per pixel
  const SDL_VideoInfo* screeninfo;
  int screen_w, screen_h;
  SDL_Surface *screen;
  pthread_mutex_t initmutex;

  pthread_cond_t outputdone_switch;
  volatile bool outputdone;
  pthread_cond_t refreshdone_switch;
  pthread_cond_t needsdrawing_switch; 
  pthread_mutex_t outputdone_mutex;
  pthread_mutex_t refreshdone_mutex;
  pthread_mutex_t needsdrawing_mutex;
  pthread_t draw_thread;
  pthread_t input_thread;
  bool wantsinput;
  int state;
  int original_screen_x;
  int original_screen_y;
  int original_screen_bpp;


  //GL extensions are linked *dynamically*
  PFNGLGENBUFFERSARBPROC _glGenBuffersARB;
  PFNGLDELETEBUFFERSARBPROC _glDeleteBuffersARB;
  PFNGLBINDBUFFERARBPROC _glBindBufferARB;
  PFNGLBUFFERDATAARBPROC _glBufferDataARB;
  PFNGLBUFFERSUBDATAARBPROC _glBufferSubDataARB;
  PFNGLMAPBUFFERARBPROC _glMapBufferARB;
  PFNGLUNMAPBUFFERARBPROC _glUnmapBufferARB;

  //same as GLX APIs
  PFNGLXGETVIDEOSYNCSGIPROC _glXGetVideoSyncSGI;
  PFNGLXWAITVIDEOSYNCSGIPROC _glXWaitVideoSyncSGI;

  virtual ~SDL_GLout();
  SDL_GLout();//we hide the constructor, since this is going to be "singletoned" 
  bool _Init();//for internal use only
  bool CheckGLExt(const std::string& name);
  void PrintGLStuff();
  bool IsPo2(int value);

  static void * OpenGLThreadLoop(void * ptr);
  static void * KeyboardThreadLoop(void * ptr);
  volatile bool KeyboardLoopRunning;
  volatile bool OpenGLLoopRunning;
  int SDLWaitEvent(SDL_Event *event);
  void DrawLayout(unsigned int layoutnum);
  inline bool _UpdateSurface(unsigned int layoutnum) __attribute__((always_inline));
  void ShowDraws();
  volatile bool NeedsDrawing;
  volatile bool NeedsFullScreenToggling;
  volatile bool IsOnHold;
  volatile bool NeedsPumping;
  void ParseActions(unsigned int layoutnum );
  inline void ParseAction(unsigned int layoutnum, int actionnum, uint64_t thistime );
  bool SetTextureFromSurface(unsigned int layoutnum, int x, int y, SDL_Surface *surface, int type);
  bool SetTextureFromSurface(unsigned int layoutnum, int x, int y,  int off_x, int off_y, SDL_Surface *surface, int type);
  bool ToggleFullScreen();
  bool FullScreenStatus;
  volatile bool refreshdone;
  bool ChangeScreenSize();
  int newscreen_x;
  int newscreen_y;
  int newscreen_sx;
  int newscreen_sy;
  bool NeedsNewScreen;

  /* Benchmark stuff */
  uint64_t bench_timer;

  void Reset_Timer();

  uint64_t Get_Timer();


public:

  bool undefer;
  uint32_t sdlvideoflags;
  uint32_t mousestatusfs;
  uint32_t mousestatuswn;
  uint32_t benchmark;
  bool showglstuff;
  GLuint vsyncFrames;
  int openglmode; //0 DMA, 1 Memory Mapping, 2 compatibility mode.  
  int GetTransferMode(){ return openglmode;};
  void WantsPump();
  bool Init(int x, int y, bool fullscreen, const std::string& caption, bool handle_input=false, int real_x=0, int real_y=0);
  bool IsInitted(){ return initted; };
  bool WaitForInit();
  void Terminate();
  bool LoadTextureFromFile(unsigned int layoutnum, const std::string& filename);
  void ReallocateLayout(unsigned int layoutnum, int w, int h, bool setactive = false);
  void ReallocateLayout(unsigned int layoutnum, int w, int h, GLint type, int planes, bool setactive = false);
  
  bool SetTextureFromABGRSurface(unsigned int layoutnum, int x, int y, SDL_Surface *surface);
  bool SetTextureFromRGBASurface(unsigned int layoutnum, int x, int y, SDL_Surface *surface);
  bool SetTextureFromBGRASurface(unsigned int layoutnum, int x, int y, SDL_Surface *surface);
  bool SetTextureFromRGBSurface(unsigned int layoutnum, int x, int y, SDL_Surface *surface);
  bool SetTextureFromABGRSurface(unsigned int layoutnum, int x, int y, int ww, int hh, SDL_Surface *surface);
  bool SetTextureFromRGBASurface(unsigned int layoutnum, int x, int y, int ww, int hh, SDL_Surface *surface);
  bool SetTextureFromBGRASurface(unsigned int layoutnum, int x, int y, int ww, int hh, SDL_Surface *surface);
  
  void SetLayoutAlpha(unsigned int layoutnum, GLfloat val);
  void SetLayoutAngle(unsigned int layoutnum, GLfloat val);
  void SetLayoutPosition(unsigned int layoutnum, GLfloat val_x,GLfloat val_y);
  void SetLayoutScale(unsigned int layoutnum, GLfloat val_x,GLfloat val_y);
  void SetLayoutActiveStatus(unsigned int layoutnum, bool active);
  void SetLayoutRGB(unsigned int layoutnum, GLfloat r,GLfloat g, GLfloat b);
  void SetLayoutDefaults(unsigned int layoutnum);
  void SetLayoutCentered(unsigned int layoutnum, bool b = true);
  void SetLayoutFitScreen(unsigned int layoutnum, bool fits);
  void ForceInputHandling();


  void HoldDrawing(){
    if (!quit){
      pthread_mutex_lock(&needsdrawing_mutex);
      refreshdone = false;
      outputdone = false;
    }
  };
  void ReleaseDrawing(){
    pthread_cond_broadcast(&needsdrawing_switch);
    pthread_mutex_unlock(&needsdrawing_mutex);
  };
  void WaitforOutputDone();
  void WaitforRefreshDone();
  void SwapLayers(unsigned int layoutnum1, unsigned int layoutnum2);
  void AddLayerAction(unsigned int layoutnum, layoutobj::GL_Action *action, bool defer = false);
  bool IsActionInProgress(unsigned int layoutnum);
  bool IsActionInProgress();
  void MarkActionsExpired(unsigned int layoutnum);
  void MarkActionsExpired();
  void MakeActionsExpire(unsigned int layoutnum);
  void MakeActionsExpire();
  void FullScreenToggle();
  bool GetFullScreenStatus();
  void RefreshScreen(){
    HoldDrawing();
    NeedsDrawing = true;
    ReleaseDrawing();
  }
  void ChangeScreenRes(int newx, int newy, int newsx, int newsy);
 
  static SDL_GLout *get_instance(){ //Singleton
    pthread_mutex_lock(&sdl_glout_singletonmutex);
    static SDL_GLout _inst;
    pthread_mutex_unlock(&sdl_glout_singletonmutex);
    return &_inst;
  }
  

  //Render device stuff
//  void redraw(){};
//  void deinit(){};
//  void wait_for_output_done();
//  void output_is_ready();
//  bool supports_wid(){return false;};
//  int wid() { return 0; }
//  int fs_wid() { return 0; }
//
//  void show_window_video()
//  {}
//  void hide_window_video()
//  {}
//
//  void show_fs_video()
//  {}
//  void hide_fs_video()
//  {}
//
//  void hide_video()
//  {}
//
//  void wait_for_not_drawing();

protected:
	void _run();
	
	bool quit;
};



class GL_ActionSpin : public layoutobj::GL_Action {
public:  
  GL_ActionSpin(GLfloat original_angle, GLfloat final_angle, int decsecs, bool deactivate = false){
    Aknowledged = false;
    ActionType = GL_Action_Spin;
    angle1 = original_angle;
    angle2 = final_angle;
    AllottedTime = decsecs;
    deactivateatend = deactivate;
  };
  GLfloat angle1;
  GLfloat angle2;
  ~GL_ActionSpin(){};
  void PerformAction(layoutobj* t, uint64_t thistime){
    if (thistime >= Start_at + AllottedTime){
      Expired = true;
      t->angle = angle2;
      if (deactivateatend)
        t->active = false;
      return;
    }
    else if (thistime == Start_at){
      t->angle = angle1;
      return;
    }
    else {
      unsigned  int zz = thistime - Start_at;
      t->angle = float((angle2 - angle1))*zz/AllottedTime+angle1;
    }
  }
};

class GL_ActionMove : public layoutobj::GL_Action {
public:  
  GL_ActionMove(GLfloat original_x,GLfloat original_y, GLfloat final_x,GLfloat final_y, int decsecs, bool deactivate = false){
    Aknowledged = false;
    ActionType = GL_Action_Move;
    x1 = original_x;
    x2 = final_x;
    y1 = original_y;
    y2 = final_y;
    AllottedTime = decsecs;
    deactivateatend = deactivate;
  };
  GLfloat x1;
  GLfloat x2;
  GLfloat y1;
  GLfloat y2;
  ~GL_ActionMove(){};
  void PerformAction(layoutobj* t, uint64_t thistime){
    if (thistime >= Start_at + AllottedTime){
      Expired = true;
      t->pos_x = x2;
      t->pos_y = y2;
      if (deactivateatend)
        t->active = false;
      return;
    }
    else if (thistime == Start_at){
      t->pos_x = x1;
      t->pos_y = y1;
      return;
    }
    else {
      unsigned  int zz = thistime - Start_at;
      t->pos_x = float((x2 - x1))*zz/AllottedTime+x1;
      t->pos_y = float((y2 - y1))*zz/AllottedTime+y1;
    }

  }
};

class GL_ActionZoom : public layoutobj::GL_Action {
public:
  GL_ActionZoom(GLfloat original_x,GLfloat original_y, GLfloat final_x,GLfloat final_y, int decsecs, bool deactivate = false){
    Aknowledged = false;
    ActionType = GL_Action_Zoom;
    x1 = original_x;
    x2 = final_x;
    y1 = original_y;
    y2 = final_y;
    AllottedTime = decsecs;
    deactivateatend = deactivate;
  }
  GLfloat x1;
  GLfloat x2;
  GLfloat y1;
  GLfloat y2;
  ~GL_ActionZoom(){};

  void PerformAction(layoutobj* t, uint64_t thistime){
    if (thistime >= Start_at + AllottedTime){
      Expired = true;
      t->scale_x = x2;
      t->scale_y = y2;
      if (deactivateatend)
        t->active = false;
      return;
    }
    else if (thistime == Start_at){
      t->scale_x = x1;
      t->scale_y = y1;
      return;
    }
    else {
      unsigned  int zz = thistime - Start_at;
      t->scale_x = float((x2 - x1))*zz/AllottedTime+x1;
      t->scale_y = float((y2 - y1))*zz/AllottedTime+y1;
    }

  }
};


class GL_ActionFade :  public layoutobj::GL_Action {
public:
  GL_ActionFade(GLfloat original_alpha, GLfloat final_alpha, int decsecs, bool deactivate = false){
    Aknowledged = false;
    ActionType = GL_Action_Fade;
    alpha1 = original_alpha;
    alpha2 = final_alpha;
    AllottedTime = decsecs;
    deactivateatend = deactivate;
  };
  ~GL_ActionFade(){};
  GLfloat alpha1;
  GLfloat alpha2;
  void PerformAction(layoutobj* t, uint64_t thistime){
    if (thistime >= Start_at + AllottedTime){
      Expired = true;
      t->alpha = alpha2;
      if (deactivateatend)
        t->active = false;
      return;
    }
    else if (thistime == Start_at){
      t->alpha = alpha1;
      return;
    }
    else {
      unsigned  int zz = thistime - Start_at;
      t->alpha = float((alpha2 - alpha1))*zz/AllottedTime+alpha1;
    }

  }
};


#endif /*SDL_GLOUT_H_*/
