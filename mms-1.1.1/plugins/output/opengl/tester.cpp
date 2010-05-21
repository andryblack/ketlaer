#include <SDL/SDL.h>
#include <GL/gl.h>
#include <cstdlib>
#include <unistd.h>
//#include <math.h>
#include "SDL_GLout.h"



int  main (){

  SDL_GLout * SDLGL = SDL_GLout::get_instance();
  SDLGL->Init(1024,768, false, "Ciao");
  SDLGL->WaitForInit();
  

  for (int t = 0; t < 3; t++){
    SDLGL->HoldDrawing();
    SDLGL->SetLayoutActiveStatus(SHADER_LAYOUT, false);
    SDLGL->LoadTextureFromFile(1,"test2.bmp");
//    SDLGL->LoadTextureFromFile(0,"test.bmp");

    SDLGL->SetLayoutDefaults(1);
    SDLGL->SetLayoutDefaults(0);
    SDLGL->SetLayoutActiveStatus(0, false);

    SDLGL->SetLayoutRGB(0,1,1,1);
    SDLGL->AddLayerAction(1, (new GL_ActionFade(1,0,600)));
    SDLGL->AddLayerAction(1, (new GL_ActionZoom(1,1,10,10,200)));
    SDLGL->AddLayerAction(1, (new GL_ActionSpin(0,180,600)));
    SDLGL->SetLayoutActiveStatus(1, true);

    SDLGL->ReleaseDrawing();
    SDLGL->WaitforOutputDone();

    SDLGL->HoldDrawing();
    SDLGL->SetLayoutActiveStatus(1, false);
    SDLGL->SetLayoutActiveStatus(SHADER_LAYOUT, true);
    SDLGL->SetLayoutDefaults(SHADER_LAYOUT);
    SDLGL->SetLayoutRGB(SHADER_LAYOUT,0.0,0.0,0.0);
    SDLGL->AddLayerAction(SHADER_LAYOUT, (new GL_ActionFade(0,1,200)));
//    SDLGL->MarkActionsExpired();
    SDLGL->ReleaseDrawing();
    SDLGL->WaitforOutputDone();
 
 }
  SDLGL->HoldDrawing();
  SDLGL->SetLayoutActiveStatus(SHADER_LAYOUT, false);
  SDLGL->SetLayoutDefaults(0);
  SDLGL->SetLayoutRGB(0,1,1,1);
//  SDLGL->SetTextureFromBGRASurface(0, 300, 50, 300, 350,rgbsurface);
  SDLGL->ReleaseDrawing();
  SDLGL->WaitforOutputDone();

  
  
  sleep(10);
  
  return 0;	
} 
