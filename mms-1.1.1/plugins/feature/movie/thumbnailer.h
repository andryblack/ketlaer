/***************************************************************************
 *   Copyright (C) 2008 - lorenzodes<at>fastwebnet.it                      *
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

#ifndef THUMBNAILER_H_
#define THUMBNAILER_H_

#include "config.h"

#define INT64_C(c)     (c ## LL)
extern "C" {
#ifdef ffmpeg_include_split
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#else
#include <avcodec.h>
#include <avformat.h>
#include <swscale.h>
#endif
}

#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <typeinfo>

class thumbnailer
{
  
private:
  bool initted;
  AVCodecContext *icodecctx;
  AVCodec *iCodec;
  bool isfileopen;
  AVFormatContext *pFormatCtx;
  std::string curmovie;
  int video_index;
  AVStream *video_st;
  AVCodec  *ivideocodec;
  AVCodecContext * ocodecctx;
  AVCodec *oCodec;
  SwsContext *img_convert_ctx;
  int64_t streamlen;
  bool seekframe(int64_t pos);
  double blanklevel(AVFrame * sframe);
  bool verbosemode;
  int seekmode;
  double deviance_limit;

public:
  typedef struct movie_specs{
    uint64_t length;
    int size_x;
    int size_y;
  };
  
  int skipblanks;
  int deepval;
  thumbnailer();
  virtual ~thumbnailer();
  bool init(bool verb = false);
  void deinit();
  bool openmovie(const std::string& filename, movie_specs * mspecs);
  bool closemovie();
  bool createthumbnail(int64_t position, unsigned int sizex, unsigned int sizey, bool keepaspect, const std::string & filename);
};

#endif /*THUMBNAILER_H_*/
