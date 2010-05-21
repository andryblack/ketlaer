#include "opengl.hpp"
#include <cassert>
#include "opengl_config.hpp"
OpenGLdev::OpenGLdev(){
}

OpenGLdev::~OpenGLdev(){};

void OpenGLdev::init(){
  conf = S_Config::get_instance();
  width = conf->p_h_res();
  height  = conf->p_v_res();
  OpenglConfig * glconf = S_OpenglConfig::get_instance();
  glconf->parse_configuration_file(conf->p_homedir());
  //printf("Homedir is %s\n", conf->p_homedir().c_str());
  int rw = glconf->p_opengl_h();
  int rh = glconf->p_opengl_v();
  x_ratio = double(width)/(rw!=0?rw:width);
  y_ratio = double(height)/(rh!=0?rh:height);

  #ifdef use_x11
  XInitThreads();//TODO: make sure it actually inits multithreading mode

  #endif

  //we set mouse and window properties
  SDLGL = SDL_GLout::get_instance();
  if (glconf->p_borderless())
    SDLGL->sdlvideoflags |= SDL_NOFRAME;

  if (glconf->p_mousecursor() == "disabled" || (glconf->p_mousecursor() == "nofs" && glconf->p_borderless())){
    SDLGL->mousestatusfs = SDL_DISABLE;
    SDLGL->mousestatuswn = SDL_DISABLE;
  }

  else if (glconf->p_mousecursor() == "enabled"){
    SDLGL->mousestatusfs = SDL_ENABLE;
    SDLGL->mousestatuswn = SDL_ENABLE;
  }

  else if (glconf->p_mousecursor() == "nofs"){
    SDLGL->mousestatusfs = SDL_DISABLE;
    SDLGL->mousestatuswn = SDL_ENABLE;
  }

  else { //Print an error message
    
    SDLGL->mousestatusfs = SDL_DISABLE;
    SDLGL->mousestatuswn = SDL_ENABLE;
  }

  if (glconf->p_transfer_mode() == "DMA"){
    SDLGL->openglmode = 0;
    printf(OBJOUTPUT("Using DMA transfermode\n"));
  }
  
  else if (glconf->p_transfer_mode() == "mmap"){
    SDLGL->openglmode = 1;
    printf(OBJOUTPUT("Using memory mapping transfermode\n"));
  }
  else if (glconf->p_transfer_mode() == "standard"){
    SDLGL->openglmode = 2;
    printf(OBJOUTPUT("Using standard transfermode\n"));
  }
  else { //Print an error message
    SDLGL->openglmode = 2;
    printf(OBJOUTPUT("Using standard transfermode\n"));
  }

  SDLGL->benchmark = 100*glconf->p_glbenchmark();
  SDLGL->vsyncFrames = glconf->p_vsync();
  SDLGL->showglstuff = (glconf->p_verbose() || conf->p_debug_level() > 2);
  
  rgbsurface = SDL_CreateRGBSurface(SDL_SRCALPHA, width, height, 32,
         0,0,0,0/*0xFF000000, 0x00FF0000, 0x0000FF00,0x000000FF*/  );
  bool handleinput = !list_contains(conf->p_input(), std::string("keyboard"));
  SDLGL->Init(width,height, static_cast<bool>(conf->p_fullscreen()), "MMS", handleinput, rw, rh);
  if(!SDLGL->WaitForInit())
    std::exit(1);
  render = S_Render::get_instance();
  SDLGL->SetLayoutRGB(0,1,1,1);
  firstdraw = true;
  currentlayer = 0;
}


void OpenGLdev::opengldraw(){

  if (firstdraw){
    rgbsurface->pixels = reinterpret_cast<UINT8 *>(imlib_image_get_data_for_reading_only());
    SDLGL->HoldDrawing();
    SDLGL->SetTextureFromBGRASurface(currentlayer, width, height, rgbsurface);
    SDLGL->SetLayoutActiveStatus(currentlayer, true);
    firstdraw = false;
    SDLGL->AddLayerAction(currentlayer, (new GL_ActionFade(0,1,200)));
    SDLGL->AddLayerAction(currentlayer, (new GL_ActionZoom(6,6,1,1,90)));
  }
  else {
    SDLGL->HoldDrawing();
    rgbsurface->pixels = reinterpret_cast<UINT8 *>(imlib_image_get_data_for_reading_only());
    SDLGL->SetTextureFromBGRASurface(currentlayer, width, height, rgbsurface);
  }
  SDLGL->ReleaseDrawing();

}

void OpenGLdev::output_is_ready(){
  opengldraw();
}

void OpenGLdev::wait_for_output_done(){
 SDLGL->WaitforRefreshDone();
}


void OpenGLdev::deinit(){
  SDLGL->Terminate();
}

void OpenGLdev::switch_to_layer(int new_layer){
  currentlayer = new_layer;
  if(SDLGL->showglstuff)
    printf(OBJOUTPUT("Switch to layer %d\n"),currentlayer);

}

void OpenGLdev::swap_layers(int layer1, int layer2){
  SDLGL->SwapLayers(layer1, layer2);
}

void OpenGLdev::start_shader(int final_opa, int color_r, int color_g, int color_b, int time){
  SDLGL->HoldDrawing();
  SDLGL->MakeActionsExpire(SHADER_LAYOUT);
  SDLGL->SetLayoutActiveStatus(SHADER_LAYOUT, true);
  SDLGL->SetLayoutDefaults(SHADER_LAYOUT);
  SDLGL->SetLayoutRGB(SHADER_LAYOUT,float(color_r)/255,float(color_g)/255,float(color_b)/255);
  SDLGL->AddLayerAction(SHADER_LAYOUT, (new GL_ActionFade(-0.2,float(final_opa)/255,time)));
  SDLGL->AddLayerAction(SHADER_LAYOUT, (new GL_ActionZoom(-1,0,1,1,time)));
  SDLGL->AddLayerAction(SHADER_LAYOUT, (new GL_ActionSpin(720,0,time)));
  SDLGL->ReleaseDrawing();
}

void OpenGLdev::stop_shader(){
  SDLGL->HoldDrawing();
  SDLGL->MakeActionsExpire(SHADER_LAYOUT);
  SDLGL->SetLayoutActiveStatus(SHADER_LAYOUT, false);
  SDLGL->ReleaseDrawing();
}

void OpenGLdev::animation_section_begin(bool defer){
  SDLGL->HoldDrawing();
  SDLGL->MakeActionsExpire(currentlayer);
  SDLGL->undefer = false;
  defer_anim = defer;
}

void OpenGLdev::animation_section_end(bool wait_finished){
  SDLGL->ReleaseDrawing();
  if(wait_finished)
    SDLGL->WaitforOutputDone();
}

void OpenGLdev::animation_fade(float init_val, float end_val, int time, int layoutnum){
  if (layoutnum == -1)
    layoutnum = currentlayer;
  SDLGL->AddLayerAction(layoutnum, (new GL_ActionFade(init_val,end_val,time)), defer_anim);
}

void OpenGLdev::animation_spin(float init_angle, float end_angle, int time, int layoutnum){
  if (layoutnum == -1)
    layoutnum = currentlayer;
  SDLGL->AddLayerAction(layoutnum, (new GL_ActionSpin(init_angle,end_angle,time)), defer_anim);
}

void OpenGLdev::animation_zoom(float init_xval, float init_yval, float end_xval, float end_yval, int time, int layoutnum){
  if (layoutnum == -1)
    layoutnum = currentlayer;
  SDLGL->AddLayerAction(layoutnum, (new GL_ActionZoom(init_xval,init_yval,end_xval,end_yval,time)), defer_anim);
}

void OpenGLdev::animation_move(float init_xval, float init_yval, float end_xval, float end_yval, int time, int layoutnum){
  if (layoutnum == -1)
    layoutnum = currentlayer;
  SDLGL->AddLayerAction(layoutnum, (new GL_ActionMove(init_xval,init_yval,end_xval,end_yval,time)), defer_anim);
}

void OpenGLdev::layer_active_no_wait(bool setstatus){
  SDLGL->SetLayoutActiveStatus(currentlayer, setstatus);
}

void OpenGLdev::layer_active_no_wait(int layernum, bool setstatus){
  SDLGL->SetLayoutActiveStatus(layernum, setstatus);
}

void OpenGLdev::layer_active(int layernum, bool setstatus){
  SDLGL->HoldDrawing();
  SDLGL->SetLayoutActiveStatus(layernum, setstatus);
  SDLGL->ReleaseDrawing();
}

void OpenGLdev::layer_active(bool setstatus){
  SDLGL->HoldDrawing();
  SDLGL->SetLayoutActiveStatus(currentlayer, setstatus);
  SDLGL->ReleaseDrawing();
}

void OpenGLdev::reset_layout_attribs_nowait(int layoutnum){
  if (layoutnum == -1)
    layoutnum = currentlayer;
  SDLGL->SetLayoutDefaults(layoutnum);
}

void OpenGLdev::reset_layout_attribs(int layoutnum){
  SDLGL->HoldDrawing();
  if (layoutnum == -1)
    layoutnum = currentlayer;
  SDLGL->SetLayoutDefaults(layoutnum);
  SDLGL->ReleaseDrawing();
}

void OpenGLdev::set_layout_alpha(float f, int layoutnum){
  if (layoutnum == -1)
    layoutnum = currentlayer;

  SDLGL->SetLayoutAlpha(layoutnum, f);

}

void OpenGLdev::make_actions_expire(int layoutnum) {
  SDLGL->MakeActionsExpire(layoutnum);
}

void OpenGLdev::unlock(){//ok, this is supposed to be called just before an external player is run
                       //is this actually so?
  SDLGL->HoldDrawing();
  //printf("(Unlock) Fullscreen status is now %d\n\n", SDLGL->GetFullScreenStatus());
  if (SDLGL->GetFullScreenStatus())// go to windowed mode
    SDLGL->FullScreenToggle();
  SDLGL->ReleaseDrawing(); //we don't block here
}

void OpenGLdev::lock(){//shouldn't this be called only after the external player exits? Remember to ask Anders
  //printf("(lock) Fullscreen status is now %d\n\n", SDLGL->GetFullScreenStatus());
  SDLGL->HoldDrawing();
  if (static_cast<bool>(conf->p_fullscreen()) && !SDLGL->GetFullScreenStatus()){// go back to fullscreen mode
    SDLGL->FullScreenToggle();
  }
  SDLGL->ReleaseDrawing(); //we don't block here
}

void OpenGLdev::refresh_screen(){
  SDLGL->RefreshScreen();
}

unsigned int OpenGLdev::convert_xmouse(int x) {
  return static_cast<unsigned int>(double(x)*x_ratio);
}

unsigned int OpenGLdev::convert_ymouse(int y) {
  return static_cast<unsigned int>(double(y)*y_ratio);
}

void OpenGLdev::sdlpump(){
  SDLGL->WantsPump();
}


