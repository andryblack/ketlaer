#ifndef _h_libketlaer_h
#define _h_libketlaer_h

#define protected public
#define private   public
#include "asusinc.h"
#undef protected
#undef private

#define RESERVED_COLOR_KEY 0x00080810

void init_libketlaer();
void uninit_libketlaer();

int ir_getfd();
int ir_getkey();
int it_getkey(bool &repeat);

HANDLE getScreenSurface();
HANDLE getSurfaceHandle (int width, int height, PIXEL_FORMAT pixFormat);
VO_RECTANGLE *getScreenRect();
VideoPlayback *getVideoPlayback();
void zoomVideo(VO_RECTANGLE*);
void setSpdifSampleRate(int rate);

typedef HRESULT (*IOPluginOpen)(IOPLUGIN*);

void setIOPluginOpen(IOPluginOpen func);

#endif
