#include "mpeg.hpp"

#include "config.h"

#ifdef benchmark_output
#include "bench.hpp"
#endif

#include "print.hpp"
#include "gettext.hpp"
#include "config.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern "C"
{
#ifdef ffmpeg_include_split
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#else
#include <avcodec.h>
#include <swscale.h>
#endif
}

UINT8 *mpg_buf, *pic_dat;
AVFrame *yuv_buf;
static AVCodecContext *codec_context;
static AVPicture pic, temp_pic;
static mpeg_frame cur_frame;
static int width = 720, height = 540;
static bool anamorph = false;

static struct SwsContext *img_convert_ctx = NULL;

#include <iostream>

void mpeg_init(bool ntsc)
{
  Config *conf = S_Config::get_instance();

  if (conf->p_h_res() == 960)
  {
    width = 720;
    anamorph = true;
  } else
    width = conf->p_h_res();

  height = conf->p_v_res();

  avcodec_init();
  avcodec_register_all();

  AVCodec *codec = avcodec_find_encoder(CODEC_ID_MPEG2VIDEO);
  if (!codec) {
    print_critical(gettext("Can't find codec mpeg2"), "MPEG");
    exit(1);
  }

  codec_context = avcodec_alloc_context();
  avcodec_get_context_defaults(codec_context);

  codec_context->bit_rate = 10000000;
 
  codec_context->width = width;
  codec_context->height = height;
  codec_context->pix_fmt = PIX_FMT_YUV420P;

  AVRational r;

  if (ntsc) {
    r.num = 1001;
    r.den = 30000;
  } else {
    r.num = 1;
    r.den = 25;
  }

  codec_context->time_base = r;

  codec_context->gop_size = 0; /* I-Frame */
  codec_context->flags = CODEC_FLAG_LOW_DELAY;
  codec_context->mb_decision=2;
  codec_context->qmin=2;
  codec_context->qmax=4;

  if (avcodec_open(codec_context, codec) < 0) {
    print_critical(gettext("Could not open codec"), "MPEG");
    exit(1);
  }

  /* prepare necessary buffers */
  pic_dat = reinterpret_cast<UINT8 *>(malloc(avpicture_get_size(PIX_FMT_YUV420P, width, height)));
  memset(pic_dat, 0, avpicture_get_size(PIX_FMT_YUV420P, width, height));
  avpicture_fill(&pic, pic_dat, PIX_FMT_YUV420P, width, height);

  mpg_buf = reinterpret_cast<UINT8 *>(malloc(5000000)); /* if segfaults are generated in encode_video, raise this size */

  /* prepare an AVPicture out of our buffers */
  yuv_buf = avcodec_alloc_frame();

  /* the lower the better quality, but also larger images => slower to transfer */
  yuv_buf->quality = 0;

  /* copy the data from avpicture to avframe */
  for (int i=0; i<4; ++i){
    yuv_buf->data[i]= pic.data[i];
    yuv_buf->linesize[i]= pic.linesize[i];
  }

  cur_frame.data = mpg_buf; // point to the mpg_buf
}

mpeg_frame mpeg_draw(UINT8* buf)
{
#ifdef benchmark_output
  Bench bench;
#endif

#ifdef benchmark_output
  bench.start();
#endif
  AVPicture * pFrame = &temp_pic;
  AVPicture * dFrame = &pic;

  if (anamorph) {
    // scale down to 720x540
    avpicture_fill(&temp_pic, buf, PIX_FMT_RGBA32, 960, height);
    img_convert_ctx = sws_getCachedContext(img_convert_ctx,
					   960, height, PIX_FMT_RGBA32,
					   width, height, PIX_FMT_YUV420P,
					   SWS_BICUBIC,
					   NULL, NULL, NULL);

    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, height,
	      dFrame->data, dFrame->linesize);

  } else {
    avpicture_fill(&temp_pic, buf, PIX_FMT_RGBA32, width, height);
    img_convert_ctx = sws_getCachedContext(img_convert_ctx,
					   width, height,
					   PIX_FMT_RGBA32, width, height,
					   PIX_FMT_YUV420P,
					   SWS_BICUBIC,
					   NULL, NULL, NULL);

    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, height,
	      dFrame->data, dFrame->linesize);
  }

#ifdef benchmark_output
  bench.stop();
  bench.print("convert image from RGBA32 to YUV420P");
#endif

  /* copy the data from avpicture to avframe */
  for (int i=0; i<4; ++i){
    yuv_buf->data[i]= pic.data[i];
    yuv_buf->linesize[i]= pic.linesize[i];
  }

#ifdef benchmark_output
  bench.start();
#endif
  cur_frame.size = avcodec_encode_video(codec_context, mpg_buf, 500000, yuv_buf);

#ifdef benchmark_output
  bench.stop();
  bench.print("encoded a picture to mpeg1");
#endif

  return cur_frame;
}

void mpeg_deinit(){
  avcodec_close(codec_context);
  free(pic_dat);
  free(mpg_buf);
}
