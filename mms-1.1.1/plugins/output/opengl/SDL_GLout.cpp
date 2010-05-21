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



#include "SDL_GLout.h"
#include <sys/time.h>
#include <ctime>
#include <errno.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else

#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

using namespace std;

#define GL_GLEXT_PROTOTYPES 

#define glError() { \
        GLenum err = glGetError(); \
        while (err != GL_NO_ERROR) { \
                fprintf(stderr, "\n\n\n************** glError: %x caught at %s:%u **************\n", err, __FILE__, __LINE__-1); \
                err = glGetError(); \
        } \
}

layoutobj::~layoutobj(){
  for (unsigned int t = 0; t < Actions.size(); t++){
    delete Actions[t];
  }
  Actions.clear();
}

SDL_GLout::SDL_GLout()
  :initted(false), IsOnHold(false) {
    //these mutexes can be initted at runtime, as long as the single instance one is initted before.

    pthread_mutex_init(&initmutex,NULL);
    pthread_mutex_init(&needsdrawing_mutex,NULL);
    pthread_mutex_init(&outputdone_mutex,NULL);
    pthread_mutex_init(&refreshdone_mutex,NULL);
    pthread_cond_init(&outputdone_switch, NULL);
    pthread_cond_init(&refreshdone_switch, NULL);
    pthread_cond_init(&needsdrawing_switch, NULL);
    benchmark = 500;
    showglstuff = false;
    quit = false;
    KeyboardLoopRunning = false;
    OpenGLLoopRunning = false; 
    NeedsFullScreenToggling = false;
    NeedsNewScreen = false;
    sdlvideoflags = 0;
    mousestatusfs = SDL_DISABLE;
    mousestatuswn = SDL_ENABLE;
    openglmode = 1;
    vsyncFrames = 0; //Vertical sync is disabled
}

SDL_GLout::~SDL_GLout(){
  Terminate();
  for (unsigned int t = 0; t < layouts.size() ; t++){
    layouts[t]->ClearActions();
    delete layouts[t];
    }
  layouts.clear();
}

bool SDL_GLout::CheckGLExt(const std::string& name){
  const char *p =  reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
  if (p == NULL){
    fprintf(stderr,OBJOUTPUT("Critical error: no extentions reported by glGetString\nThis should not happen\n"));
    ::exit(1);
  }
  std::string str = p;
  if (str.empty())
    return false;
  //it's a list of names separated by a space; the last character of the string is a space too
  str = " " + str;
  return ( str.find(" " + name + " ", 0) != std::string::npos);
}

void SDL_GLout::PrintGLStuff(){ //Prints various OpenGL details
  const char *vendor =  reinterpret_cast<const char *>(glGetString(GL_VENDOR));
  const char *renderer =  reinterpret_cast<const char *>(glGetString(GL_RENDERER));
  const char *version = reinterpret_cast<const char *>( glGetString(GL_VERSION));
  const char *p =  reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
  if (vendor == NULL || renderer ==  NULL || version == NULL || p == NULL){
    fprintf(stderr,OBJOUTPUT("Critical error: glGetString failed\n"));
    ::exit(1);
  }
  //let's format the extension list a little better
  std::string str(p);
  std::string replacement1("\n    ");
  std::string replacement2(", ");
  unsigned int z = 0, x = 0;
  while ((z = str.find(" ", z))!= std::string::npos && z+1 < str.size()){
    if (x == 2){
      str.replace(z,1,replacement2 + replacement1);
      x = 0;
      z+=replacement1.size() + replacement2.size();
    }
    else{
      str.replace(z,1,replacement2);
      x++;
      z+=replacement2.size();
    }
  }

  fprintf(stderr,OBJOUTPUT("Opengl vendor: %s\n"), vendor);
  fprintf(stderr,OBJOUTPUT("Opengl renderer: %s\n"), renderer);
  fprintf(stderr,OBJOUTPUT("Opengl version: %s\n"), version);
  fprintf(stderr,OBJOUTPUT("Opengl extensions:%s%s\n\n"), replacement1.c_str(),str.c_str());

}


bool SDL_GLout::Init(int x, int y, bool fullscreen, const std::string& caption, bool handle_input, int _real_x, int _real_y){
	//purpose of this is to protect with locks the real init stuff
  pthread_mutex_lock(&initmutex);
  if(initted){
    pthread_mutex_unlock(&initmutex);
    return false;
  }
  //we now create TWO threads.
  //One handles SDL Input
  //One handles drawings
  sdl_x = x;
  sdl_y = y;
  real_x = _real_x;
  real_y = _real_y;
  requestedfullscreen = fullscreen;
  sdl_caption = caption;
  wantsinput = handle_input;
  pthread_create( &draw_thread, NULL, (void *(*)(void *))(OpenGLThreadLoop), this);
  if (wantsinput)
    pthread_create( &input_thread, NULL, (void *(*)(void *))(KeyboardThreadLoop), this);
  return true;
}

void SDL_GLout::Terminate(){
  if (!initted)
    return;
  quit = true;
  if (wantsinput)
    pthread_join(input_thread, NULL);
  pthread_join(draw_thread, NULL);
  initted = false;
}

void SDL_GLout::ForceInputHandling(){
  if (wantsinput)
    return;
  wantsinput = true;
  pthread_create( &input_thread, NULL, (void *(*)(void *))(KeyboardThreadLoop), this);
  while(!quit && !KeyboardLoopRunning)
    SDL_Delay(100);
}


bool SDL_GLout::_Init(){
  int x,y;

  if (real_x == 0 || real_y == 0){
    x = sdl_x;
    y = sdl_y;
    real_x = x;
    real_y = y;
  }
  else{
    x = real_x;
    y = real_y;
  }

  bool fullscreen = requestedfullscreen;
  std::string caption = sdl_caption;

  if (showglstuff)
    printf(OBJOUTPUT("Parameters x-> %d\ny-> %d\ncaption-> %s\n"), x,y,caption.c_str());

  //we start by creating the SDL context
  if(SDL_Init(SDL_INIT_VIDEO /*| SDL_INIT_EVENTTHREAD */) < 0) {
    fprintf(stderr, OBJOUTPUT("Video initialization failed: %s\n"), SDL_GetError());
    return false;
  }
  
  SDL_EventState (SDL_ALLEVENTS, SDL_IGNORE); //I know, this triggers a warning, blame it on SDL
  SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);
  SDL_EventState (SDL_KEYUP, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEBUTTONUP, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
  SDL_EventState (SDL_ACTIVEEVENT, SDL_ENABLE);
  SDL_EventState (SDL_VIDEOEXPOSE, SDL_ENABLE);

  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);

  int parent;
  if (SDL_GetWMInfo(&info) > 0) {
    info.info.x11.lock_func();


    parent = info.info.x11.window;
    int x11_width = WidthOfScreen(DefaultScreenOfDisplay(info.info.x11.display));
    int x11_height = HeightOfScreen(DefaultScreenOfDisplay(info.info.x11.display));
    info.info.x11.unlock_func();

    if ((x11_width < x || x11_height < y) &&fullscreen) {
      fprintf(stderr, OBJOUTPUT("Requested resolution is higher than X resolution!\n"));
      SDL_Quit();
      return false;
    }
  }

  screeninfo = SDL_GetVideoInfo();
  bpp = screeninfo->vfmt->BitsPerPixel;
  original_screen_bpp = bpp;
  original_screen_x = screeninfo->current_w;
  original_screen_y = screeninfo->current_h;

  //Now some OpenGL attributes 
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

 // SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  int flags = SDL_OPENGL | sdlvideoflags;

  //we create the window/screen...

  if((screen = SDL_SetVideoMode(x, y, bpp, flags)) == 0) {
    fprintf(stderr, "Video creation failed: %s\n", SDL_GetError());
    SDL_Quit();
    std::exit(1);
  }

  //we set the caption
  SDL_WM_SetCaption(caption.c_str(),caption.c_str());
  SDL_ShowCursor(mousestatuswn);
  //we init the OpenGL contest
  glViewport( 0, 0, x, y );
  glMatrixMode(GL_PROJECTION);
  glError();
  glLoadIdentity();
  glError();
  if (fullscreen)
     ToggleFullScreen();
  
  screen_w = sdl_x; screen_h = sdl_y;
  /* printf("screen_w is %d, screen_h is %d\n", screen_w, screen_h); */
  glMatrixMode(GL_MODELVIEW);
  glError();
  glLoadIdentity();
  glError();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //Clear screen
  glError();
  glDisable(GL_DEPTH_TEST); //We only do 2D rendering
  glError();
  if (showglstuff)
    PrintGLStuff();
  if (openglmode != 2 && !CheckGLExt("GL_ARB_pixel_buffer_object") && !CheckGLExt("GL_EXT_pixel_buffer_object")){
	fprintf(stderr,
    	    OBJOUTPUT("SDL_GLout::This video card does not support the extension: GL_ARB_pixel_buffer_object\n"));
	openglmode = 2;
	fprintf(stderr, 
	    OBJOUTPUT("SDL_GLout::Falling back to standard/compatibility mode\n"));
  }

 	//we check if this card supports GL_ARB_texture_rectangle
	//if not we exit
 if (!CheckGLExt("GL_ARB_texture_rectangle")){
	  fprintf(stderr, 
	      OBJOUTPUT("SDL_GLout::This video card does not support the required extension: GL_ARB_texture_rectangle\n"));
          fprintf(stderr,
	     OBJOUTPUT( "SDL_GLout::Quitting\n"));	  
	  SDL_Quit();
	  std::exit(1);
  }
 if (CheckGLExt("GL_ARB_texture_non_power_of_two")){
   glrecmode = GL_TEXTURE_2D;
   fprintf(stderr,
       OBJOUTPUT("SDL_GLout::Found optional extension: GL_ARB_texture_non_power_of_two\n"));
       }
 else
   glrecmode = GL_TEXTURE_RECTANGLE_ARB;

	//we dyamically link some OpenGL extension APIs
  _glGenBuffersARB = reinterpret_cast<PFNGLGENBUFFERSARBPROC>(SDL_GL_GetProcAddress("glGenBuffersARB"));
  _glDeleteBuffersARB = reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(SDL_GL_GetProcAddress("glDeleteBuffersARB"));
  _glBindBufferARB = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(SDL_GL_GetProcAddress("glBindBufferARB"));
  _glBufferDataARB = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(SDL_GL_GetProcAddress("glBufferDataARB"));
  _glBufferSubDataARB = reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(SDL_GL_GetProcAddress("glBufferSubDataARB"));
  _glMapBufferARB = reinterpret_cast<PFNGLMAPBUFFERARBPROC>(SDL_GL_GetProcAddress("glMapBufferARB"));
  _glUnmapBufferARB = reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(SDL_GL_GetProcAddress("glUnmapBufferARB"));


  //we dynamically link some GLX APIs for vertical sync

  _glXGetVideoSyncSGI = reinterpret_cast<PFNGLXGETVIDEOSYNCSGIPROC>(
      glXGetProcAddressARB(reinterpret_cast<const unsigned char*>("glXGetVideoSyncSGI")));

  _glXWaitVideoSyncSGI = reinterpret_cast<PFNGLXWAITVIDEOSYNCSGIPROC>(
      glXGetProcAddressARB(reinterpret_cast<const unsigned char*>("glXWaitVideoSyncSGI")));
  if (_glXGetVideoSyncSGI == NULL || _glXWaitVideoSyncSGI == NULL){
    fprintf(stderr,
        OBJOUTPUT("SDL_GLout::This system doesn't allow v-sync settings\n"));
    vsyncFrames = 0;
  }


  if (
      _glGenBuffersARB == NULL ||
      _glDeleteBuffersARB == NULL ||
      _glBindBufferARB == NULL ||
      _glBufferDataARB == NULL ||
      _glBufferSubDataARB == NULL ||
      _glMapBufferARB == NULL ||
      _glUnmapBufferARB == NULL
  ){
    fprintf(stderr,
        OBJOUTPUT("SDL_GLout::Critical: Unable to get address of one or more OpenGL extension APIs\n"));
    std::exit(1);
  }

  //we prepare the layouts
  GLuint texnums[NUM_OF_LAYOUTS];
  glGenTextures(NUM_OF_LAYOUTS, texnums);
  glError();

  //we prepare the buffers for fast GPU <--> RAM transfers...
  GLuint bufnums[NUM_OF_LAYOUTS];
  if (openglmode != 2){
    _glGenBuffersARB(NUM_OF_LAYOUTS, bufnums);
    glError();
  }


  for (int t = 0; t < NUM_OF_LAYOUTS ; t++){
	  layouts.push_back( new layoutobj(texnums[t], bufnums[t]));
  }

  //Initialise the layout to match window/screen resolution
  //size can be changed in a second time if needed
  for (int t = 0; t < NUM_OF_LAYOUTS ; t++){
	  ReallocateLayout(t, sdl_x, sdl_y, false);
  }

  int max_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
  glError();
  if (showglstuff)
    printf(OBJOUTPUT("Max texture size supported by this system is %d\n"),max_size);
  initted = true;
  return true;
}

void * SDL_GLout::OpenGLThreadLoop(void * ptr){
  //pretty simple loop
  SDL_GLout * thisobj = reinterpret_cast<SDL_GLout*>(ptr);
  if (!thisobj->_Init()){
    fprintf(stderr, 
	OBJOUTPUT("Unable to init OpenGL\n"));
    pthread_mutex_unlock(&thisobj->initmutex);
    thisobj->quit = true;
    return 0;
  }

  struct timespec ts;
  uint64_t ttimer;
  int idletime;
  unsigned int numofdrawings = 0;
  uint64_t ttimer2 = 0;
  uint64_t elapsed = 0;
  uint64_t maxtime = 0;
  thisobj->OpenGLLoopRunning = true;
  pthread_mutex_lock(&thisobj->needsdrawing_mutex);
  thisobj->NeedsNewScreen = false;
  thisobj->NeedsFullScreenToggling = false;
  thisobj->NeedsDrawing = false;
  thisobj->NeedsPumping = false;
  pthread_mutex_unlock(&thisobj->initmutex);
  while (!thisobj->quit){
    if (thisobj->NeedsPumping){
      thisobj->NeedsPumping = false;
      SDL_PumpEvents();
    }
    if (thisobj->NeedsNewScreen){
      thisobj->NeedsNewScreen = false;
      thisobj->ChangeScreenSize();
      thisobj->NeedsDrawing = true;
    }
    if (thisobj->NeedsFullScreenToggling){
      thisobj->NeedsFullScreenToggling = false;
      thisobj->ToggleFullScreen();
      thisobj->NeedsDrawing = true;
    }
    if (thisobj->NeedsDrawing){
      if (thisobj->benchmark != 0){
        numofdrawings++;
	thisobj->Reset_Timer();
      }
      thisobj->NeedsDrawing = false;
      for (int t = 0; t < NUM_OF_LAYOUTS; t++){
        thisobj->DrawLayout(t);
      }
      thisobj->ShowDraws();
      if (thisobj->benchmark != 0)
	ttimer2 = thisobj->Get_Timer();
    }
    pthread_mutex_lock(&thisobj->refreshdone_mutex);
    thisobj->refreshdone = true;
    pthread_cond_broadcast(&thisobj->refreshdone_switch);
    pthread_mutex_unlock(&thisobj->refreshdone_mutex);
    if (!thisobj->NeedsDrawing){
      pthread_mutex_lock(&thisobj->outputdone_mutex);
      thisobj->outputdone = true;
      pthread_cond_broadcast(&thisobj->outputdone_switch);
      pthread_mutex_unlock(&thisobj->outputdone_mutex);
      idletime = 1000;
    }
    else
      idletime = 20;
    clock_gettime(CLOCK_REALTIME, &ts);
    ttimer = static_cast<uint64_t>(ts.tv_sec)*1000 + ts.tv_nsec/1000000;
    if (ttimer2 != 0){
      elapsed += ttimer2;
      if (ttimer2 > maxtime)
        maxtime = ttimer2;
      ttimer2 = 0;
    }
    ttimer += idletime;
    do {
      ts.tv_sec = ttimer/1000;
      ts.tv_nsec = (ttimer%1000)*1000000;
      pthread_cond_timedwait(&thisobj->needsdrawing_switch, &thisobj->needsdrawing_mutex, &ts);
    } while (!thisobj->NeedsPumping && !thisobj->quit && !thisobj->NeedsNewScreen && !thisobj->NeedsFullScreenToggling && !thisobj->NeedsDrawing);
    if (numofdrawings >= thisobj->benchmark && thisobj->benchmark != 0){
      printf (OBJOUTPUT("Done %d drawings in %lld milliseconds. Average is %f milliseconds per drawing. Highest drawing time is %lld milliseconds\n"), 
	  numofdrawings,
	  elapsed,
	  float(elapsed)/numofdrawings, maxtime);
      numofdrawings = 0;
      elapsed = 0;
      maxtime = 0;
    }
  }

  pthread_mutex_unlock(&thisobj->needsdrawing_mutex);
  thisobj->OpenGLLoopRunning = false;
  //Restore screen to previous state...
  if(thisobj->showglstuff)
    printf(OBJOUTPUT
           ("Restoring screen to original settings: %dx%d, %d bpp\n"), thisobj->original_screen_x, thisobj->original_screen_y, thisobj->original_screen_bpp);
  SDL_SetVideoMode(thisobj->original_screen_x, thisobj->original_screen_y, thisobj->original_screen_bpp, SDL_OPENGL);

  //free buffers
  //get rid of textures
  GLuint texnums[NUM_OF_LAYOUTS];
  GLuint bufnums[NUM_OF_LAYOUTS];
  for (unsigned int t  = 0; t< thisobj->layouts.size(); t++){
    texnums[t] = thisobj->layouts[t]->id;
    bufnums[t] = thisobj->layouts[t]->buffer_id;
  }

  glDeleteTextures(NUM_OF_LAYOUTS, texnums);
  thisobj->_glDeleteBuffersARB(NUM_OF_LAYOUTS, bufnums);
  SDL_Quit(); 
  return 0;
}
int SDL_GLout::SDLWaitEvent(SDL_Event *event){
  if (!initted)
    return 0;
  int ret;
  while (!quit){
    WantsPump();
    if ((ret = SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_ALLEVENTS)) < 0)
      return 0;
    if (ret == 1)
      return ret;
    SDL_Delay(10);
  }
  return 0;
}

void * SDL_GLout::KeyboardThreadLoop(void * ptr){
  SDL_GLout * thisobj = reinterpret_cast<SDL_GLout*>(ptr);
  fprintf(stderr, OBJOUTPUT("SDL event handling is enabled\n"));
  if(!thisobj->WaitForInit())
    return 0;
  SDL_Event event[1];
  thisobj->KeyboardLoopRunning = true;
  while (!thisobj->quit){
    if (thisobj->SDLWaitEvent(event)){
      switch (event[0].type){
        case SDL_ACTIVEEVENT:
        case SDL_VIDEOEXPOSE:
          thisobj->HoldDrawing();
          thisobj->NeedsDrawing = true;
          thisobj->ReleaseDrawing();
          thisobj->WaitforRefreshDone();
          break;
        default:
          SDL_Delay(50);
      }
    }
  }
  thisobj->KeyboardLoopRunning = false;
  return 0;
}


/*
void * SDL_GLout::KeyboardThreadLoop(void * ptr){
  SDL_GLout * thisobj = reinterpret_cast<SDL_GLout*>(ptr);
  if(!thisobj->wantsinput){
    fprintf(stderr, OBJOUTPUT("SDL event handling is disabled\n"));
    return 0;
  }
  else
    fprintf(stderr, OBJOUTPUT("SDL event handling is enabled\n"));
  SDL_Event event[1];
  if(!thisobj->WaitForInit())
      return 0;
  pthread_mutex_lock(&thisobj->inputmutex);
  SDL_InitSubSystem( SDL_INIT_EVENTTHREAD);
  thisobj->KeyboardLoopRunning = true;
  while (!thisobj->quit) {
    pthread_mutex_unlock(&thisobj->inputmutex);
    if (thisobj->SDLWaitEvent(event)){
      pthread_mutex_lock(&thisobj->inputmutex);
      if (thisobj->quit)
        break;
      switch (event[0].type) {
      case SDL_QUIT:
        thisobj->HoldDrawing();
        thisobj->quit = true;
        thisobj->ReleaseDrawing();
        break;
      case SDL_ACTIVEEVENT:
      case SDL_VIDEORESIZE:
      case SDL_SYSWMEVENT:
      case  SDL_VIDEOEXPOSE:
        thisobj->HoldDrawing();
        thisobj->NeedsDrawing = true;
        thisobj->ReleaseDrawing();
        break;
      case SDL_KEYDOWN:
        switch( event[0].key.keysym.sym ){
        case SDLK_ESCAPE:
        case SDLK_q:
          break;

        case SDLK_F1:
//          thisobj->HoldDrawing();
//          thisobj->NeedsFullScreenToggling = true;
//          thisobj->ReleaseDrawing();

          break;
        default:
          break;
        }
      }
    }

  }
  thisobj->KeyboardLoopRunning = false;
  pthread_mutex_unlock(&thisobj->inputmutex);
  //and here it ends
  return 0;
}
*/
bool SDL_GLout::WaitForInit(){

  pthread_mutex_lock(&initmutex);
  bool ret = initted;
  pthread_mutex_unlock(&initmutex);
  return ret;
}


bool SDL_GLout::LoadTextureFromFile(unsigned int layoutnum, const std::string& filename){
  if (layoutnum >= layouts.size())
    return false;
  layouts[layoutnum]->active = false;
  SDL_Surface *surface = SDL_LoadBMP(filename.c_str());
  if (!surface){
    fprintf(stderr, OBJOUTPUT("Unable to load %s\n"), filename.c_str());
    return false;
  }

  bool needrealloc = false;
  if (layouts[layoutnum]->width < surface->w){
    needrealloc = true;
    layouts[layoutnum]->width = surface->w;
  }

  if (layouts[layoutnum]->height < surface->h){
    needrealloc = true;
    layouts[layoutnum]->height = surface->h;
  }


  if (needrealloc){
    ReallocateLayout(layoutnum, layouts[layoutnum]->width, layouts[layoutnum]->height, true);
  }

  bool ret = SetTextureFromABGRSurface(layoutnum, 0, 0, surface);
//  SDL_FreeSurface(surface);
  return ret;
}


//bool SDL_GLout::SetTextureFromSurface(unsigned int layoutnum, int x, int y, SDL_Surface *surface, int type){
//  if (layoutnum >= layouts.size())
//    return false;
//  layouts[layoutnum]->active = false;
//
//  if((surface->format->BytesPerPixel != 3) &&
//      (surface->format->BytesPerPixel != 4)){
//    fprintf(stderr, OBJOUTPUT("Surface has an unsupported colour depth\n"));
//    return false;
//  }
//  char * target;
//  if(surface->format->BytesPerPixel!=4 ){
//    target = static_cast<char *>(malloc(surface->h*surface->w*4));
//    if (target == NULL){
//      fprintf(stderr, OBJOUTPUT("Unable to allocate memory for the layout buffer\n"));
//      return false;
//    }
//  }
//  else target = static_cast<char *>(surface->pixels);
//  char * source = static_cast<char *>(surface->pixels);
//  //  printf("\n\n\nSurface type is %d and color depth is %d\n\n\n\n\n\n\n", type, surface->format->BytesPerPixel);
//
//  if (surface->format->BytesPerPixel == 4 && type == 0){
//    layouts[layoutnum]->type = GL_ABGR_EXT;
//  }
//
//  else if (surface->format->BytesPerPixel == 3 && (type == 0 || type == 1)){
//    for (int t = 0 ,  x = 0; t < surface->w*surface->h*4; t +=4, x +=3){ //from BGR to RGBA
//      target[t] = source[x+2]; //Target R
//      target[t+1] = source[x+1]; //Target G
//      target[t+2] = source[x]; //Target B
//      target[t+3] = -0x1; //opaque alpha channel
//    }
//    layouts[layoutnum]->type = GL_RGBA;
//  }
//
//  else if (surface->format->BytesPerPixel == 4 && type == 1){
//    layouts[layoutnum]->type = GL_BGRA;
//  }
//
//  else if (surface->format->BytesPerPixel == 4 && type == 2){
//    layouts[layoutnum]->type = GL_RGBA;
//  }
//
//  else{
//    for (int t = 0 ,  x = 0; t < surface->h*surface->w*4; t +=4, x +=3){ //from RGB to RGBA
//      target[t] = source[x]; //Target R
//      target[t+1] = source[x+1]; //Target G
//      target[t+2] = source[x+2]; //Target B
//      target[t+3] = -0x1; //opaque alpha channel
//    }
//    layouts[layoutnum]->type = GL_RGBA;
//  }
//
//  layouts[layoutnum]->active = true;
//  layouts[layoutnum]->dirty = true;
//  draw_rect dr;
//  dr.h = surface->h;
//  dr.w = surface->w;
//  dr.x = 0;
//  dr.y = 0;
//  dr.bitmpap = target;
//  dr.freeme = (surface->format->BytesPerPixel!=4 );
//  layouts[layoutnum]->draw_rects.push_back(dr);
//  NeedsDrawing = true;
//  return true;
//}

inline bool SDL_GLout::_UpdateSurface(unsigned int layoutnum){
  int x;
  int y;
  int w = layouts[layoutnum]->draw_rects[0].w;
  int h = layouts[layoutnum]->draw_rects[0].h;
  int tt;
  char * buf = layouts[layoutnum]->draw_rects[0].bitmpap;
  char * source = buf;
  char * target = layouts[layoutnum]->PBObuffer;
  int planes = layouts[layoutnum]->planes;
  int pp;
  if(layouts[layoutnum]->needrealloc && showglstuff)
    fprintf(stderr, OBJOUTPUT("reallocating layout %d to %dx%dx%d\n"), layoutnum, w, h, planes);

  switch (openglmode){
  
  case 0: //DMA transfer{
    glEnable(glrecmode);
    glError();
    
    glBindTexture(glrecmode,  layouts[layoutnum]->id);
    glError();
    x = layouts[layoutnum]->draw_rects[0].x;
    y = layouts[layoutnum]->draw_rects[0].y;
    buf = layouts[layoutnum]->draw_rects[0].bitmpap;
    tt = layouts[layoutnum]->draw_rects[0].type;
    pp = layouts[layoutnum]->draw_rects[0].planes;
    _glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, layouts[layoutnum]->buffer_id);
    glError();
    glBindTexture(glrecmode, layouts[layoutnum]->id);
    glError();
    if(!layouts[layoutnum]->needrealloc)
      _glBufferSubDataARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0, (w)*planes*(h),buf);
    else
      _glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT,(w)*planes*(h),buf,GL_STATIC_DRAW );
    glError();
    //GL_STREAM_DRAW);
    if(!layouts[layoutnum]->needrealloc)
      glTexSubImage2D(glrecmode, 0, 0, 0, w, h,tt, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
    else
      glTexImage2D(glrecmode, 0, layouts[layoutnum]->type,  w, h, 0, tt, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
    glError();
    layouts[layoutnum]->needrealloc = false;

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glError();

    //We initialize the layout data
    

//    layouts[layoutnum]->alpha = 1.0f;
//    layouts[layoutnum]->pos_x = 0;
//    layouts[layoutnum]->pos_y = 0;
//    layouts[layoutnum]->angle = 0;
//    layouts[layoutnum]->type = glrecmode;

    _glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
    break;


  case 1: //Mem mapping
    glEnable(glrecmode);
    glError();
    glBindTexture(glrecmode, layouts[layoutnum]->id);
    glError();
    x = layouts[layoutnum]->draw_rects[0].x;
    y = layouts[layoutnum]->draw_rects[0].y;
    buf = layouts[layoutnum]->draw_rects[0].bitmpap;
    tt = layouts[layoutnum]->draw_rects[0].type;
    pp = layouts[layoutnum]->draw_rects[0].planes;
    _glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, layouts[layoutnum]->buffer_id);
    glError();

    //Create Pixel buffer
    _glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT, (x+w)*planes*(y+h),
        NULL, GL_STREAM_DRAW);//this makes buffer mapping faster, but discards previous contents!!!
    glError();
    layouts[layoutnum]->PBObuffer = static_cast<char *> (_glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY));
    glError();
    assert(layouts[layoutnum]->PBObuffer);
    //Copy surface data to Pixel buffer
    source = buf;
    target = layouts[layoutnum]->PBObuffer;
    memcpy(target, source, h*w*(pp<planes?pp:planes));
    assert(_glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT));

    glBindTexture(glrecmode, layouts[layoutnum]->id);
    glError();
    if(!layouts[layoutnum]->needrealloc)
      glTexSubImage2D(glrecmode, 0, 0, 0, w, h,tt, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
    else
      glTexImage2D(glrecmode, 0, layouts[layoutnum]->type,  w, h, 0, tt, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
    glError();
    
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glError();

    //We initialize the layout data
    layouts[layoutnum]->active = true;

//    layouts[layoutnum]->alpha = 1.0f;
//    layouts[layoutnum]->pos_x = 0;
//    layouts[layoutnum]->pos_y = 0;
//    layouts[layoutnum]->angle = 0;
//    layouts[layoutnum]->type = glrecmode;

    _glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
    glError();
    break;


  case 2: //good old method
    glEnable(glrecmode);
    x = layouts[layoutnum]->draw_rects[0].x;
    y = layouts[layoutnum]->draw_rects[0].y;
    tt = layouts[layoutnum]->draw_rects[0].type;
 
    glBindTexture(glrecmode, layouts[layoutnum]->id);
    glError();
    if(!layouts[layoutnum]->needrealloc)
    glTexSubImage2D(glrecmode, 0, 0, 0, w, h,tt, GL_UNSIGNED_BYTE, layouts[layoutnum]->draw_rects[0].bitmpap);
    else
      glTexImage2D(glrecmode, 0, layouts[layoutnum]->type,  w, h, 0, tt, GL_UNSIGNED_BYTE,layouts[layoutnum]->draw_rects[0].bitmpap);
    glError();
    
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glError();

    //We initialize the layout data
    layouts[layoutnum]->active = true;
  }
  if (layouts[layoutnum]->draw_rects[0].freeme)
    free(buf);
  layouts[layoutnum]->draw_rects.clear();
  layouts[layoutnum]->active = true;
  layouts[layoutnum]->needrealloc= false;
  return true;
}

void SDL_GLout::DrawLayout(unsigned int layoutnum){
  if (layoutnum >= layouts.size())
    return;
  if(!layouts[layoutnum]->active)
    return;
  glEnable(glrecmode);
  glError();

  if (layouts[layoutnum]->dirty && layoutnum != SHADER_LAYOUT){
    layouts[layoutnum]->dirty = false;
    _UpdateSurface(layoutnum);
  }
  glError();
  ParseActions(layoutnum);
  glMatrixMode(GL_PROJECTION);
  glError();
  glLoadIdentity();
  glError();

  if (!layouts[layoutnum]->centered){
    glOrtho(0, screen_w, screen_h, 0, -1, 1); //We want GL coordinates to match screen coordinates
  }
  else{
    glOrtho(-float(screen_w)/2, float(screen_w)/2, 
            float(screen_h)/2, -float(screen_h)/2, -1, 1); 
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  if (layoutnum != SHADER_LAYOUT){ //SHADER_LAYOUT has no texture by definition
    glEnable(glrecmode);
    glBindTexture(glrecmode, layouts[layoutnum]->id);
    glError();
  }
  else {
    glDisable(glrecmode);
    glError();
    glBindTexture(glrecmode, 0);
    glError();
  }
  glTexParameteri(glrecmode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glError();
  glTexParameteri(glrecmode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glError();

  //	glLoadIdentity();
  glTranslatef(layouts[layoutnum]->pos_x, layouts[layoutnum]->pos_y, 0);// 
  if (layouts[layoutnum]->scale_x!=0 || layouts[layoutnum]->scale_y!=0)
    glScalef(layouts[layoutnum]->scale_x,layouts[layoutnum]->scale_y,0); 

  if (layouts[layoutnum]->angle!=0)
    glRotatef(layouts[layoutnum]->angle, 0, 0, 1);//
  glColor4f(layouts[layoutnum]->red, layouts[layoutnum]->green, layouts[layoutnum]->blue, layouts[layoutnum]->alpha);
  glError();
  float normw, normh;
  if (glrecmode == GL_TEXTURE_2D){
    normw = 1;
    normh = 1;
  }
  else{
    normw = layouts[layoutnum]->width;
    normh = layouts[layoutnum]->height;
  }
  glBegin(GL_QUADS);
  if (!layouts[layoutnum]->centered){
    glTexCoord2d(0, 0);
    glVertex2d(0, 0);

    if (layoutnum != SHADER_LAYOUT) glTexCoord2d(normw, 0);
    glVertex2d(layouts[layoutnum]->width, 0);

    if (layoutnum != SHADER_LAYOUT) glTexCoord2d(normw, normh);
    glVertex2d(layouts[layoutnum]->width, layouts[layoutnum]->height);

    if (layoutnum != SHADER_LAYOUT) glTexCoord2d(0, normh);
    glVertex2d(0, layouts[layoutnum]->height);

  }
  else {
    if (layoutnum != SHADER_LAYOUT) glTexCoord2d(0, 0);
    glVertex2d(-float(layouts[layoutnum]->width)/2, -float(layouts[layoutnum]->height)/2);

    if (layoutnum != SHADER_LAYOUT)glTexCoord2d(normw, 0);
    glVertex2d(float(layouts[layoutnum]->width)/2, -float(layouts[layoutnum]->height)/2);

    if (layoutnum != SHADER_LAYOUT) glTexCoord2d(normw, normh);
    glVertex2d(float(layouts[layoutnum]->width)/2, float(layouts[layoutnum]->height)/2);

    if (layoutnum != SHADER_LAYOUT) glTexCoord2d(0, normh);
    glVertex2d(-float(layouts[layoutnum]->width)/2, float(layouts[layoutnum]->height)/2);
  }

  glEnd();

  glDisable(GL_BLEND);
  glBindTexture(glrecmode, 0);
  //	_glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

}

void SDL_GLout::ShowDraws(){
  GLuint frameCount;
  if (vsyncFrames > 0 && _glXGetVideoSyncSGI(&frameCount) == 0) {
    _glXWaitVideoSyncSGI(vsyncFrames, frameCount % vsyncFrames, &frameCount);
  }

  SDL_GL_SwapBuffers();//as simple as that
  //	glMatrixMode(GL_MODELVIEW);
  //	glLoadIdentity();
  glClear( GL_COLOR_BUFFER_BIT); //Clear screen
}	

inline void SDL_GLout::ParseAction(unsigned int layoutnum, int actionnum, uint64_t thistime){
  layoutobj::GL_Action * act = layouts[layoutnum]->Actions[actionnum];
  if (undefer)
    act->deferred = false;

  if(act->Expired){
    delete act;
    layouts[layoutnum]->Actions.erase(layouts[layoutnum]->Actions.begin() + actionnum);
    return;
  }
  else if (act->deferred)
    return;
  else if (!act->Aknowledged){
    act->Start_at = thistime;
    act->Aknowledged = true;
  }
  act->PerformAction(layouts[layoutnum], thistime);

}

void SDL_GLout::ParseActions(unsigned int layoutnum ){
  if (layouts[layoutnum]->Actions.size() == 0)
    return;
  NeedsDrawing = true;
  struct timeval tv;
  gettimeofday(&tv,NULL);

  uint64_t _now = static_cast<uint64_t>(tv.tv_sec*100 + tv.tv_usec/10000); 
  for (int t = layouts[layoutnum]->Actions.size()-1; t >= 0; t--){
    ParseAction(layoutnum, t, _now);
  }
}

bool SDL_GLout::IsPo2(int value){
  return (( value | (value - 1)) + 1 == value * 2);
}

bool SDL_GLout::ToggleFullScreen(){

  screen = SDL_GetVideoSurface();// you never know, I am paranoid
  int w = screen->w;
  int h = screen->h;
  int bits = screen->format->BitsPerPixel;
  bool ret = true;
  if (showglstuff)
    fprintf(stderr, OBJOUTPUT("I'm about to toggle fullscreen status\n"));


  screen = SDL_SetVideoMode(w,h,bits, sdlvideoflags | SDL_OPENGL|(screen->flags&SDL_FULLSCREEN?0:SDL_FULLSCREEN));
  SDL_ShowCursor((screen->flags & SDL_FULLSCREEN) > 0 ? mousestatusfs:mousestatuswn);

  if (screen == NULL){
    fprintf(stderr, OBJOUTPUT("Unable to toggle fullscreen mode, SDL returned %s\n"),SDL_GetError());
    
  }
  FullScreenStatus = ((screen->flags&SDL_FULLSCREEN) != 0);
  
//  if ((screen->flags&SDL_FULLSCREEN) && (screen->w != w || screen->h != h) ){// we haven't managed to get the desidered resolution: undo
//    
//    screen = SDL_SetVideoMode(w,h,bits, SDL_OPENGL); //back to windowed mode
//    assert(screen != NULL);
//    ret = false;
//  }

  if (showglstuff)
    fprintf(stderr, OBJOUTPUT("Fullscreen status changed\n"));
  
  return ret;
}


bool SDL_GLout::SetTextureFromABGRSurface(unsigned int layoutnum, int , int , SDL_Surface *surface){
  //return SetTextureFromSurface(layoutnum, x, y, surface, 0);
  if (layoutnum >= layouts.size())
    return false;
  draw_rect dr;
  dr.h = surface->h;
  dr.w = surface->w;
  dr.x = 0;
  dr.y = 0;
  dr.bitmpap =  static_cast<char *>(surface->pixels);
  dr.freeme = false;
  dr.planes = 4;
  dr.type = GL_ABGR_EXT;
  layouts[layoutnum]->draw_rects.push_back(dr);
  NeedsDrawing = true;
  layouts[layoutnum]->active = true;
  layouts[layoutnum]->dirty = true;
  undefer = true;
  return true;
  
}

bool SDL_GLout::SetTextureFromRGBSurface(unsigned int layoutnum, int , int , SDL_Surface *surface){
  if (layoutnum >= layouts.size())
    return false;
  draw_rect dr;
#if 0
  void * target = malloc(surface->h*surface->w*3);
  void * source = static_cast<char *>(surface->pixels);
  memcpy(target, source, surface->h*surface->w*3);
  
 
  dr.h = surface->h;
  dr.w = surface->w;
  dr.x = 0;
  dr.y = 0;
  dr.bitmpap =  static_cast<char *>(target);
  dr.freeme = /*false;*/ true;
#endif
  dr.bitmpap = static_cast<char *>(surface->pixels);
  dr.h = surface->h;
  dr.w = surface->w;
  dr.x = 0;
  dr.y = 0;
  dr.type = GL_RGB;
  dr.freeme = false;
  dr.planes = 3;
  layouts[layoutnum]->draw_rects.push_back(dr);
  NeedsDrawing = true;
  layouts[layoutnum]->active = true;
  layouts[layoutnum]->dirty = true;
  undefer = true;
  return true;
}


bool SDL_GLout::SetTextureFromBGRASurface(unsigned int layoutnum, int , int , SDL_Surface *surface){
//#if 0
  if (layoutnum >= layouts.size())
    return false;
  draw_rect dr;
  dr.h = surface->h;
  dr.w = surface->w;
  dr.x = 0;
  dr.y = 0;
  dr.bitmpap =  static_cast<char *>(surface->pixels);
  dr.freeme = false;
  dr.type = GL_BGRA;
  dr.planes = 4;
  layouts[layoutnum]->draw_rects.push_back(dr);
  NeedsDrawing = true;
  layouts[layoutnum]->active = true;
  layouts[layoutnum]->dirty = true;
  undefer = true;
  return true;
//#endif 
//  return SetTextureFromSurface(layoutnum, x, y, surface, 1);
}

bool SDL_GLout::SetTextureFromRGBASurface(unsigned int layoutnum, int , int , SDL_Surface *surface){
  if (layoutnum >= layouts.size())
    return false;
  draw_rect dr;
  dr.h = surface->h;
  dr.w = surface->w;
  dr.x = 0;
  dr.y = 0;
  dr.bitmpap =  static_cast<char *>(surface->pixels);
  dr.freeme = false;
  dr.type = GL_BGRA;
  dr.planes = 4;
  layouts[layoutnum]->draw_rects.push_back(dr);
  NeedsDrawing = true;
  layouts[layoutnum]->active = true;
  layouts[layoutnum]->dirty = true;
  undefer = true;
  return true;

//  return SetTextureFromSurface(layoutnum, x, y, surface, 2); 
}

void SDL_GLout::WaitforOutputDone(){
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  uint64_t ttimer = static_cast<uint64_t>(ts.tv_sec)*1000 + ts.tv_nsec/1000000;

  pthread_mutex_lock(&outputdone_mutex);
//  if (showglstuff)
//    fprintf(stderr, OBJOUTPUT("(WaitforOutputDone) Waiting....\n"));
  while (!outputdone && !quit){
    ttimer +=400; 
    ts.tv_sec = ttimer/1000;
    ts.tv_nsec = (ttimer%1000)*1000000;
    pthread_cond_timedwait(&outputdone_switch, &outputdone_mutex, &ts);
  }
  pthread_mutex_unlock(&outputdone_mutex);
//  if (showglstuff)
//    fprintf(stderr, OBJOUTPUT("(WaitforOutputDone) Done waiting\n"));

}

void SDL_GLout::WaitforRefreshDone(){
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  uint64_t ttimer = static_cast<uint64_t>(ts.tv_sec)*1000 + ts.tv_nsec/1000000;
  pthread_mutex_lock(&refreshdone_mutex);
//  if (showglstuff)
//    fprintf(stderr, OBJOUTPUT("(WaitforRefreshDone) Waiting....\n"));

  while (!refreshdone && !quit){
    ttimer +=400;
    ts.tv_sec = ttimer/1000;
    ts.tv_nsec = (ttimer%1000)*1000000;
    pthread_cond_timedwait(&refreshdone_switch, &refreshdone_mutex, &ts);
  }
  pthread_mutex_unlock(&refreshdone_mutex);
//  if (showglstuff)
//    fprintf(stderr, OBJOUTPUT("(WaitforRefreshDone) Done waiting\n"));
}

void SDL_GLout::SetLayoutActiveStatus(unsigned int layoutnum, bool active){
  if (layoutnum >= layouts.size())
    return;
  layouts[layoutnum]->active = active;
}

void SDL_GLout::SetLayoutAlpha(unsigned int layoutnum, GLfloat val){
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  layouts[layoutnum]->alpha = val;
}

void SDL_GLout::SetLayoutAngle(unsigned int layoutnum, GLfloat val){
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  layouts[layoutnum]->angle = val;
}

void SDL_GLout::SetLayoutScale(unsigned int layoutnum, GLfloat val_x,GLfloat val_y){
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  layouts[layoutnum]->scale_x = val_x;
  layouts[layoutnum]->scale_y = val_y;
}
void SDL_GLout::SetLayoutCentered(unsigned int layoutnum, bool b){
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  layouts[layoutnum]->centered = b;
}


void SDL_GLout::SetLayoutPosition(unsigned int layoutnum, GLfloat val_x,GLfloat val_y){
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  layouts[layoutnum]->pos_x = val_x;
  layouts[layoutnum]->pos_y = val_y;
}

void SDL_GLout::SetLayoutRGB(unsigned int layoutnum, GLfloat r,GLfloat g, GLfloat b){
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  layouts[layoutnum]->red = r;
  layouts[layoutnum]->green = g;
  layouts[layoutnum]->blue = b;
}

void SDL_GLout::SetLayoutFitScreen(unsigned int layoutnum, bool fits){
  if (layoutnum >= layouts.size())
    return;
  layouts[layoutnum]->Fit_to_Screen = fits;
  NeedsDrawing = true;
}

void SDL_GLout::SetLayoutDefaults(unsigned int layoutnum){
  SetLayoutScale(layoutnum,1,1);
  SetLayoutPosition(layoutnum,0,0);
  SetLayoutAngle(layoutnum,0);
  SetLayoutAlpha(layoutnum,1);
  SetLayoutCentered(layoutnum);
  if (layoutnum != SHADER_LAYOUT)
    SetLayoutRGB(layoutnum, 1, 1, 1);
  else
    SetLayoutRGB(layoutnum, 0, 0, 0);
  SetLayoutFitScreen(layoutnum, true);
}

void SDL_GLout::ReallocateLayout(unsigned int layoutnum, int w,int h, bool setactive){//this will also clear the layout
  ReallocateLayout(layoutnum, w, h, GL_RGBA, 4, setactive);
}

void SDL_GLout::ReallocateLayout(unsigned int layoutnum, int w,int h, GLint type, int planes, bool setactive){//this will also clear the layout
  if (layoutnum >= layouts.size())
    return;
  NeedsDrawing = true;
  SetLayoutDefaults(layoutnum);
  layouts[layoutnum]->active = setactive;
  layouts[layoutnum]->width = w;
  layouts[layoutnum]->height = h;
  layouts[layoutnum]->type = type;
  layouts[layoutnum]->needrealloc = true;
  layouts[layoutnum]->planes = planes;
}

void SDL_GLout::SwapLayers(unsigned int layoutnum1, unsigned int layoutnum2){
  if (layoutnum1 >= layouts.size() || layoutnum2 >= layouts.size())
    return;
  layoutobj  * tmp = new layoutobj(0,0);
  layoutobj  * one = layouts[layoutnum1];
  layoutobj  * two = layouts[layoutnum2];
  tmp->copytothis(one);
  one->copytothis(two);
  two->copytothis(tmp);
  std::swap(one->Actions, two->Actions);
  delete tmp;
  NeedsDrawing = true;
}

void SDL_GLout::AddLayerAction(unsigned int layoutnum, layoutobj::GL_Action *action, bool defer){
  if (layoutnum >= layouts.size())
    return;
  action->deferred = defer;
  layouts[layoutnum]->Actions.push_back(action);
  NeedsDrawing = true;
}

bool SDL_GLout::IsActionInProgress(unsigned int layoutnum){
  if (layoutnum >= layouts.size())
    return false;
  return (layouts[layoutnum]->Actions.size() > 0);
}

bool SDL_GLout::IsActionInProgress(){
  for (unsigned int t = 0; t < layouts.size();t++){
    if (layouts[t]->Actions.size() > 0)
      return true;
  }
  return false;
}

void SDL_GLout::MarkActionsExpired(unsigned int layoutnum){
  fprintf(stderr,OBJOUTPUT("SDL_GLout::MarkActionsExpired is deprecated.\n"));
  if (layoutnum >= layouts.size())
    return;
  for (unsigned int t = 0; t < layouts[layoutnum]->Actions.size(); t++){
    layouts[layoutnum]->Actions[t]->Expired = true;
  }
  NeedsDrawing = true;
}

void SDL_GLout::MarkActionsExpired(){
  for (unsigned int t = 0; t < layouts.size();t++)
    MarkActionsExpired(t);
}

void SDL_GLout::FullScreenToggle(){
  NeedsFullScreenToggling = true;
  NeedsDrawing = true;
}

void SDL_GLout::ChangeScreenRes(int newx, int newy, int newsx, int newsy){
  newscreen_x = newx;
  newscreen_y = newy;
  newscreen_sx = newsx;
  newscreen_sy = newsy;
  NeedsNewScreen = true;
  NeedsDrawing = true;
}

bool SDL_GLout::ChangeScreenSize(){

  
  if (newscreen_x == 0)
    newscreen_x = real_x;
  
  if (newscreen_y == 0)
    newscreen_y = real_y;
  
  if (newscreen_sx == 0)
    newscreen_sx = sdl_x;
  
  if (newscreen_sy == 0)
    newscreen_sy = sdl_y;
  
  if (real_x == newscreen_x &&
      real_y == newscreen_y &&
      sdl_x == newscreen_sx &&
      sdl_y == newscreen_sy)
    return true; // nothing to do, new and old resolutions match
  
  int x = newscreen_x;
  int y = newscreen_y;
  screen = SDL_GetVideoSurface();
  int bits = screen->format->BitsPerPixel;
  if ((screen = SDL_SetVideoMode(x,y, bits, sdlvideoflags | SDL_OPENGL))==0){
    fprintf(stderr,
            OBJOUTPUT("SDL_GLout::Unable to change screen resolution\n"));
    return false;
  }
  real_x = newscreen_x;
  real_y = newscreen_y;
  sdl_x = newscreen_sx;
  sdl_y = newscreen_sy;
  screen_w = sdl_x; screen_h = sdl_y;
  glLoadIdentity();
  glViewport( 0, 0, x, y );
  glError();
  for (int t = 0; t < NUM_OF_LAYOUTS ; t++){
    ReallocateLayout(t, sdl_x, sdl_y, layouts[t]->active);
  } 
  return true;
}

void SDL_GLout::MakeActionsExpire(unsigned int layoutnum){
  if (layoutnum >= layouts.size())
    return;
  for (unsigned int t = 0; t < layouts[layoutnum]->Actions.size(); t++){
    layouts[layoutnum]->Actions[t]->AllottedTime = 0;
  }
  NeedsDrawing = true;
}

void SDL_GLout::MakeActionsExpire(){
  for (unsigned int t = 0; t < layouts.size();t++)
    MakeActionsExpire(t);
}

bool SDL_GLout::GetFullScreenStatus(){
  return FullScreenStatus;
  NeedsDrawing = true;
  
}

void SDL_GLout::_run(){
}


void SDL_GLout::WantsPump(){
  HoldDrawing();
  NeedsPumping = true;
  ReleaseDrawing();
  WaitforRefreshDone();
}


void SDL_GLout::Reset_Timer(){
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  bench_timer = static_cast<uint64_t>(ts.tv_sec)*1000L + ts.tv_nsec/1000000L;
}

uint64_t SDL_GLout::Get_Timer(){
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return static_cast<uint64_t>(ts.tv_sec)*1000L + ts.tv_nsec/1000000L - bench_timer;
} 
