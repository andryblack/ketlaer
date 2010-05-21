#ifndef _h_libketlaer_h
#define _h_libketlaer_h

#include "asusinc.h"

#define RESERVED_COLOR_KEY 0x00080810

void init_libketlaer();
void uninit_libketlaer();

int  ir_getfd();
int  ir_getkey(bool translate=true); //0==none

HANDLE         getScreenSurface();
VO_RECTANGLE  *getScreenRect();
VideoPlayback *getVideoPlayback();

#endif
