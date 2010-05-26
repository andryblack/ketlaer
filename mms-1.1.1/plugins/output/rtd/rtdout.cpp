#include "config.h"
#include "rtdout.hpp"
#include "touch.hpp"
#include "common-feature.hpp"
#include "busy_indicator.hpp"
#include "print.hpp"
#include "gettext.hpp"
#include "themes.hpp"
#include "theme.hpp"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <libketlaer.h>

#define MAX_LOOP_INTERVAL 250

RTDdev::RTDdev()    
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-rtd-out", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-rtd-out", nl_langinfo(CODESET));
#endif
}

RTDdev::~RTDdev()
{
}

void RTDdev::init(){
  init_libketlaer();
  width = getScreenRect()->width;
  height = getScreenRect()->height;
  line_len = 4 * width;
  render = S_Render::get_instance();
}

void RTDdev::deinit()
{
  uninit_libketlaer();
}

template <class DST, class SRC>
static inline void rtd_memconvert(DST *dest, const SRC *src, int count)
{
  /* Duff's device */
  int n = (count + 7) / 8;
  switch (count & 0x07) {
  case 0: do { *dest++ = htonl(*src++);
  case 7:      *dest++ = htonl(*src++);
  case 6:      *dest++ = htonl(*src++);
  case 5:      *dest++ = htonl(*src++);
  case 4:      *dest++ = htonl(*src++);
  case 3:      *dest++ = htonl(*src++);
  case 2:      *dest++ = htonl(*src++);
  case 1:      *dest++ = htonl(*src++);
             } while (--n > 0);
  }
}

template <class DST, class SRC>
static inline void rtd_rectconvert(DST *dst, const SRC *src,
				   int x, int y, int width, int height,
				   int dstStride, int srcStride)
{
  char *d = (char*)(dst + x) + y * dstStride;
  const char *s = (char*)(src + x) + y * srcStride;
  for (int i = 0; i < height; ++i) {
    rtd_memconvert<DST,SRC>((DST*)d, (const SRC*)s, width);
    d += dstStride;
    s += srcStride;
  }
}

void RTDdev::draw_rectangle(unsigned *src, int x, int y, int w, int h)
{
  unsigned *dst = (unsigned*)DG_GetSurfaceRowDataPointer(getScreenSurface());

  if (w > conf->p_h_res())
    w = conf->p_h_res();

  if (h > conf->p_v_res())
    h = conf->p_v_res();

  rtd_rectconvert<unsigned, unsigned>(dst, src, 
				      x, y, w, h, 
				      line_len, line_len);
}

void RTDdev::rtd_draw()
{
  if (render->draw_rects.size() == 0) // optimization
    return;

  unsigned *src = reinterpret_cast<unsigned*>(imlib_image_get_data_for_reading_only());

  if (!src) {
    printf("[RTDOUT] src == NULL\n");
    return;
  }

  foreach (rect& r, render->draw_rects)
    draw_rectangle(src, r.x, r.y, r.w, r.h);
}

void RTDdev::run()
{
  long long l = 0;
  while(true) {
    drawing = false;
    new_to_draw = false;
    output_done.signal();
    output_ready.wait();
    output_ready.reset();
    //l = pli_getPTS();
    rtd_draw();
    //printf("%lld\n", pli_getPTS() -l );
  }
}

