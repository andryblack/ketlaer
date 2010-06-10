#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <assert.h>

#include "libk.h"
#include <libketlaer.h>

void libk_init()
{
  init_libketlaer();
}

void libk_uninit()
{
  //  uninit_libketlaer();
}

void libk_getscreensize(int *w, int *h)
{
  *w = getScreenRect()->width;
  *h = getScreenRect()->height;
}

static inline
unsigned rgb16to32(unsigned pixel)
{
  return ((((pixel) & 0xF800)      ) |
	  (((pixel) & 0x07E0) << 13) |
	  (((pixel) & 0x001F) << 27)   );
}

static inline 
void rtd_memconvert_16(unsigned *dst, const unsigned short *src, int count)
{
  /* Duff's device */
  int n = (count + 7) / 8;
  switch (count & 0x07) {
  case 0: do { *dst++ = rgb16to32(*src++);
  case 7:      *dst++ = rgb16to32(*src++);
  case 6:      *dst++ = rgb16to32(*src++);
  case 5:      *dst++ = rgb16to32(*src++);
  case 4:      *dst++ = rgb16to32(*src++);
  case 3:      *dst++ = rgb16to32(*src++);
  case 2:      *dst++ = rgb16to32(*src++);
  case 1:      *dst++ = rgb16to32(*src++);
             } while (--n > 0);
  }
}

static inline 
void rtd_rectconvert_16(unsigned *dst, const unsigned short *src,
			int x, int y, int width, int height,
			int dstStride, int srcStride)
{
  char *d = (char*)(dst + x) + y * dstStride;
  const char *s = (char*)(src + x) + y * srcStride;
  for (int i = 0; i < height; ++i) {
    rtd_memconvert_16((unsigned*)d, (const unsigned short*)s, width);
    d += dstStride;
    s += srcStride;
  }
}

void libk_copyrect_16(void *src, int pitch, int x, int y, int w, int h)
{
  void *dst = DG_GetSurfaceRowDataPointer(getScreenSurface());
  int dstStride = 4 * getScreenRect()->width;

  rtd_rectconvert_16((unsigned*)dst,
		     (unsigned short*)src,
		     x,
		     y,
		     w,
		     h,
		     dstStride,
		     pitch);
}

static inline 
void rtd_memconvert_32(unsigned *dst, const unsigned *src, int count)
{
  /* Duff's device */
  int n = (count + 7) / 8;
  switch (count & 0x07) {
  case 0: do { *dst++ = htonl(*src++);
  case 7:      *dst++ = htonl(*src++);
  case 6:      *dst++ = htonl(*src++);
  case 5:      *dst++ = htonl(*src++);
  case 4:      *dst++ = htonl(*src++);
  case 3:      *dst++ = htonl(*src++);
  case 2:      *dst++ = htonl(*src++);
  case 1:      *dst++ = htonl(*src++);
             } while (--n > 0);
  }
}

static inline 
void rtd_rectconvert_32(unsigned *dst, const unsigned *src,
			int x, int y, int width, int height,
			int dstStride, int srcStride)
{
  char *d = (char*)(dst + x) + y * dstStride;
  const char *s = (char*)(src + x) + y * srcStride;
  for (int i = 0; i < height; ++i) {
    rtd_memconvert_32((unsigned*)d, (const unsigned*)s, width);
    d += dstStride;
    s += srcStride;
  }
}

void libk_copyrect_32(void *src, int pitch, int x, int y, int w, int h)
{
  void *dst = DG_GetSurfaceRowDataPointer(getScreenSurface());
  int dstStride = 4 * getScreenRect()->width;

  rtd_rectconvert_32((unsigned*)dst,
		     (unsigned*)src,
		     x,
		     y,
		     w,
		     h,
		     dstStride,
		     pitch);
}

