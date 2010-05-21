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

#include "thumbnailer.h"
#define OBJOUTPUT(a) ( std::string( std::string("MMS Player: <") +  __func__ + "> " + (a) ).c_str())

#define BLANK_LEVEL_UNIT 1.26


thumbnailer::thumbnailer()
{
  initted = false;
  isfileopen = false;
  img_convert_ctx = NULL;
  skipblanks = 4;
}

thumbnailer::~thumbnailer()
{
  if (img_convert_ctx)
     sws_freeContext(img_convert_ctx);
  img_convert_ctx = NULL;
}

bool thumbnailer::init(bool verb){
  avcodec_init();
  av_register_all(); //register all codecs
  avcodec_register_all();
  av_log_set_level( AV_LOG_QUIET ); /* don't print warnings from ffmpeg */
  initted = true;
  verbosemode = verb;
  deepval = 1;
  return true;
}

bool thumbnailer::openmovie(const std::string& filename, movie_specs * mspecs){

  if (!initted){
    fprintf(stderr,OBJOUTPUT("Call init() first!!!!\n"));
    return false;
  }

  if (isfileopen){
    fprintf(stderr,OBJOUTPUT("Can't open a new file, already parsing %s\n"), curmovie.c_str());
    return false;
  }
  isfileopen = false;
  if (filename.empty())
    return false;
  AVFormatParameters _ap, *ap = &_ap;
  memset(ap, 0, sizeof(*ap));
  ap->time_base= (AVRational){1, 25}; //default
  ap->width = 320; //default
  ap->height = 256; //defaul
  ap->pix_fmt = PIX_FMT_NONE;

  if(av_open_input_file(&pFormatCtx, filename.c_str(), NULL, 0, ap)!=0){
    fprintf(stderr,OBJOUTPUT("Unable to open file %s\n"), filename.c_str());
    return false;
  }
  if(av_find_stream_info(pFormatCtx)<0){
    fprintf(stderr,OBJOUTPUT("Unable to get stream information %s\n"), filename.c_str());
    av_close_input_file(pFormatCtx);
    return false;
  }

  video_index = -1;
  if(verbosemode)
    fprintf(stderr,OBJOUTPUT("\nProcessing file '%s' which is a '%s' file\n"),
		    filename.c_str(), pFormatCtx->iformat->name);
  //fprintf(stderr, "Number of streams is %d\n", pFormatCtx->nb_streams);
  if(verbosemode)
    fprintf(stderr,OBJOUTPUT("This file contains %d streams\n"),pFormatCtx->nb_streams);

  int _z = (pFormatCtx->nb_streams &0x7fff);
  for(int i = 0; i< _z; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) { //we select the 1st video stream as stream to parse
      video_index=i;
      break;
  }
  if (video_index == -1){
    fprintf(stderr,OBJOUTPUT("File %s has no video stream\n"), filename.c_str());
    av_close_input_file(pFormatCtx);
    return false;
  }
  if(verbosemode)
    fprintf(stderr,OBJOUTPUT("Processing stream #%d which is the 1st available video stream\n"), video_index);

  icodecctx=pFormatCtx->streams[video_index]->codec;
  char buf[1024];
  if(verbosemode){
    avcodec_string(buf, sizeof(buf), icodecctx, false);
    fprintf(stderr,OBJOUTPUT("Codec info: ' %s ' (codecID: %d)\n"), buf, icodecctx->codec_id);
  }

  if (icodecctx->codec_id == CODEC_ID_RV30 || icodecctx->codec_id == CODEC_ID_RV40){
  fprintf(stderr,OBJOUTPUT("rv30 and rv40 formats not supported yet\n"));
    av_close_input_file(pFormatCtx);
    return false;
   }

  //find the right video codec
  ivideocodec = avcodec_find_decoder(icodecctx->codec_id);
  if (ivideocodec == NULL){
    fprintf(stderr,OBJOUTPUT("Unable to find codec %s\n"), icodecctx->codec_name);
    av_close_input_file(pFormatCtx);
    return false;
  }

  video_st = pFormatCtx->streams[video_index];
  if (pFormatCtx->streams[video_index]->duration != static_cast<int64_t>(AV_NOPTS_VALUE))
    streamlen = pFormatCtx->streams[video_index]->duration * av_q2d(pFormatCtx->streams[video_index]->time_base);
  else if (pFormatCtx->duration != static_cast<int64_t>(AV_NOPTS_VALUE))
    streamlen = pFormatCtx->duration / AV_TIME_BASE;
  else
    streamlen = 0;
  if (streamlen &&  static_cast<uint64_t>(pFormatCtx->start_time) != AV_NOPTS_VALUE)
    streamlen+=(pFormatCtx->start_time / AV_TIME_BASE);

  if(verbosemode)
      fprintf(stderr,OBJOUTPUT("Further stream details:\n  width = %d\n  height = %d\n  Duration = %lld\n"),
	 pFormatCtx->streams[video_index]->codec->width, pFormatCtx->streams[video_index]->codec->height, streamlen);

  if (mspecs){
    mspecs->size_x = icodecctx->width;
    mspecs->size_y = icodecctx->height;
    mspecs->length = streamlen;
  }
  if(avcodec_open(icodecctx, ivideocodec)<0){
    fprintf(stderr,OBJOUTPUT("Unable to open codec %s\n"), icodecctx->codec_name);
    av_close_input_file(pFormatCtx);
    return false;
  }

  isfileopen = true;

  curmovie = filename;
  if (icodecctx->codec_id == CODEC_ID_FLV1){
    seekmode = AVSEEK_FLAG_ANY;
  }
  else
    seekmode = AVSEEK_FLAG_BACKWARD;
  return true;
}

bool thumbnailer::closemovie(){
  if (!isfileopen){
    fprintf(stderr,OBJOUTPUT("No movie to close!!!\n"));
    return false;
  }
  isfileopen = false;
  avcodec_close(icodecctx);

  av_close_input_file(pFormatCtx);
  return true;
}

bool thumbnailer::createthumbnail(int64_t position, //milliseconds
    unsigned int sizex,
    unsigned int sizey,
    bool keepaspect,
    const std::string & filename){ // here we open the stream, go to the selected frame, convert it, open the proper encoder
                                   // and save the thumbnail in jpg format. Sounds easy, doesn't it?

  if (!isfileopen){
    fprintf(stderr,OBJOUTPUT("First you must a select a movie file!!!\n"));
    return false;
  }
  int maxblanks = 0;
  switch (skipblanks){
  case 0:
    break;
  case 1:
    deviance_limit = BLANK_LEVEL_UNIT*3;
    maxblanks = 300;
    break;
  case 2:
    deviance_limit = BLANK_LEVEL_UNIT*5;
    maxblanks = 500;
  break;
  case 3:
    deviance_limit = BLANK_LEVEL_UNIT*7;
    maxblanks = 1000;
    break;
  case 4:
    deviance_limit = BLANK_LEVEL_UNIT*8;
    maxblanks = 1500;
    break;
  case 5:
    deviance_limit = BLANK_LEVEL_UNIT*10;
    maxblanks = 2000;
    break;
  case 6:
    deviance_limit = BLANK_LEVEL_UNIT*12;
    maxblanks = 2200;
    break;
  case 7:
    deviance_limit = BLANK_LEVEL_UNIT*14;
    maxblanks = 2500;
    break;
  case 8:
    deviance_limit = BLANK_LEVEL_UNIT*15;
    maxblanks = 2700;
    break;
  case 9:
    deviance_limit = BLANK_LEVEL_UNIT*17;
    maxblanks = 2800;
    break;
  case 10:
    deviance_limit = BLANK_LEVEL_UNIT*20;
    maxblanks = 3000;
    break;
  default:
    deviance_limit = BLANK_LEVEL_UNIT*2*(skipblanks+2);
    maxblanks = 3000 + skipblanks*2;

  };

  float ratio1, ratio2;
  int  wantedx=sizex, wantedy = sizey;
  if (keepaspect){
    ratio1 = static_cast<float>(icodecctx->width)/sizex;
    ratio2 = static_cast<float>(icodecctx->height)/sizey;
    //printf("ratio1 is %f, ratio2 is %f\n", ratio1, ratio2);
    if (ratio1 > ratio2){
      sizey=icodecctx->height/ratio1;
      //sizex=icodecctx->width/ratio1;
    }
    else if (ratio1 != ratio2){
      sizex=icodecctx->width/ratio2;
      //sizey=icodecctx->height/ratio2;
    }
  }
  //printf("sizex is %d, sizey is %d\n", sizex, sizey);
  uint8_t * buffer;
  uint8_t * jbuffer;
  AVFrame * dFrame=avcodec_alloc_frame();
  AVFrame * pFrame=avcodec_alloc_frame();
  int64_t position2 = (position * AV_TIME_BASE)/1000;
  int ret=0;

  /* we allocate the destination framebuffer for the desired format */
  int numBytes=avpicture_get_size(PIX_FMT_YUVJ420P, wantedx, wantedy);
  buffer = static_cast<uint8_t*>(av_mallocz(numBytes + FF_INPUT_BUFFER_PADDING_SIZE));
  avpicture_fill(reinterpret_cast<AVPicture*>(dFrame), buffer, PIX_FMT_YUVJ420P,
      wantedx, wantedy);

  AVPacket packet;
  AVPacket * pkt =&packet;
  int secondframe = deepval;
  int frameFinished=0;

  /* Read the 1st valid frame, this allows it for ffmpeg to properly parse problematic streams */
  seekframe(0);
  for (int t = 0; t < 50; t++){
    if (av_read_frame(pFormatCtx, pkt)>=0 &&
        pkt->stream_index == video_index &&
        avcodec_decode_video(icodecctx, pFrame, &frameFinished, pkt->data, pkt->size) > 0 && 
        frameFinished > 0)
      break;
    else
      frameFinished = 0;
  }

  if (!frameFinished){
    av_free(buffer);
    av_free(dFrame);
    av_free(pFrame);
    return false;
  }

  /* move to the desired position in stream */
  if (!seekframe(position2)){
    av_free(buffer);
    av_free(dFrame);
    av_free(pFrame);
    return false;
  }

  while(true){
    ret = av_read_frame(pFormatCtx, pkt);
    if (ret < 0){
      fprintf(stderr,OBJOUTPUT("Could not read stream packets from file '%s'\n"), curmovie.c_str());
      av_free(buffer);
      av_free(dFrame);
      av_free(pFrame);
      return false;
    }
    if(pkt->stream_index == video_index){
      if (avcodec_decode_video(icodecctx, pFrame, &frameFinished, pkt->data, pkt->size) < 1){
        fprintf(stderr,OBJOUTPUT("Error decoding packet\n"));
      }
    }

    if(frameFinished > 0 && --secondframe < 0){
/* Basically we grab the 1st frame
   whose deviance > MAX_BLANK_LEVEL
   among the following "maxblanks" ones.
   If no frame matches our criteria, take the
   "maxblanks"'th one
   This should reduce, although not eliminate,
   the cases of thumbnails with very low details
   in them or that are just blank
*/
      if (skipblanks && --maxblanks > 0){
        if (blanklevel(pFrame) >= deviance_limit){
          break;
        }
      }
      else
        break;
    }
  }

  //we convert and scale our video frame....

  // do we have to add black bands to the picture?
  int bottomband = ((wantedy - sizey)/2)&0xffff;
  int topband = (wantedy - sizey - bottomband);
  int leftband = ((wantedx - sizex )/2)&0xffff;
  int rightband = (wantedx - sizex - leftband);
  if (rightband > leftband) leftband = rightband;
  if (topband > bottomband) bottomband = topband;
  if (bottomband&01) bottomband++;
  if (rightband&01) rightband++;

  //printf("borders are:\ntop: %d\nbottom: %d\nleft: %d\nright: %d\n", topband, bottomband, leftband, rightband);
  //printf("height is %d\n", topband+bottomband+sizey);

  img_convert_ctx = sws_getCachedContext(img_convert_ctx, icodecctx->width, icodecctx->height,
		    icodecctx->pix_fmt, sizex, sizey, PIX_FMT_YUVJ420P /*icodecctx->pix_fmt*/,SWS_BICUBIC, NULL, NULL, NULL);

  if (!img_convert_ctx){
     fprintf(stderr,OBJOUTPUT("Could not create SWScale context\n"));
      av_free(buffer);
      av_free(dFrame);
      av_free(pFrame);
      return false;
  }

  if (bottomband > 0 || leftband > 0){
  //we create a temp AVFrame
    AVFrame ttFrame;
    AVFrame * tFrame = &ttFrame;
    avcodec_get_frame_defaults(tFrame);

    int tnumBytes=avpicture_get_size(PIX_FMT_YUVJ420P, sizex, sizey);
    uint8_t * tbuffer = static_cast<uint8_t*>(av_malloc(tnumBytes));
    avpicture_fill(reinterpret_cast<AVPicture*>(tFrame), tbuffer, PIX_FMT_YUVJ420P,
	      sizex, sizey);

    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, icodecctx->height, tFrame->data, tFrame->linesize);

  //and finally add black bands;

    int padcolor[3] = {16,128,128}; //black padding color
    av_picture_pad(reinterpret_cast<AVPicture*>(dFrame), reinterpret_cast<AVPicture*>(tFrame), wantedy, wantedx,
                   PIX_FMT_YUVJ420P, topband, bottomband, leftband, rightband, padcolor);
    av_free(tbuffer);
  }
  else
     sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, icodecctx->height,
	 dFrame->data, dFrame->linesize);


  //Initialize the encoder...
  ocodecctx = avcodec_alloc_context();
  if (!ocodecctx){
    fprintf(stderr,OBJOUTPUT("Could not allocate the output codec context\n"));
    av_free(buffer);
    av_free(dFrame);
    av_free(pFrame);
    return false;
  }

  ocodecctx->bit_rate = icodecctx->bit_rate;
  ocodecctx->width = wantedx;
  ocodecctx->height = wantedy;
  ocodecctx->pix_fmt = PIX_FMT_YUVJ420P;
  ocodecctx->codec_id = CODEC_ID_MJPEG;
  ocodecctx->codec_type = CODEC_TYPE_VIDEO;
  ocodecctx->time_base = icodecctx->time_base;
  if (ocodecctx->bit_rate_tolerance < ocodecctx->bit_rate*av_q2d(ocodecctx->time_base))
    ocodecctx->bit_rate_tolerance = 1+ocodecctx->bit_rate*av_q2d(ocodecctx->time_base);
  oCodec = avcodec_find_encoder(CODEC_ID_MJPEG);
  if (!oCodec){
    fprintf(stderr,OBJOUTPUT("Output codec not found\n"));
    av_free(buffer);
    av_free(dFrame);
    av_free(pFrame);
    av_free(ocodecctx);
    return false;
  }

  if (avcodec_open(ocodecctx, oCodec) < 0) {
    fprintf(stderr,OBJOUTPUT("Failed opening output codec\n"));
    av_free(buffer);
    av_free(dFrame);
    av_free(pFrame);
    av_free(ocodecctx);
    return false;
  }

  //we set some output codec stuff....
  ocodecctx->qmin = ocodecctx->qmax = 3;
  ocodecctx->mb_lmin = ocodecctx->lmin = ocodecctx->qmin * FF_QP2LAMBDA;
  ocodecctx->mb_lmax = ocodecctx->lmax = ocodecctx->qmax * FF_QP2LAMBDA;
  ocodecctx->flags = CODEC_FLAG_QSCALE;
  ocodecctx->global_quality = ocodecctx->qmin * FF_QP2LAMBDA;

  // Encode the frame in JPEG format...

  numBytes=avpicture_get_size(ocodecctx->pix_fmt, wantedx, wantedy);
  if (numBytes < FF_MIN_BUFFER_SIZE)
    numBytes = FF_MIN_BUFFER_SIZE;
  jbuffer = static_cast<uint8_t*>(av_mallocz(numBytes + FF_INPUT_BUFFER_PADDING_SIZE));
  //fprintf(stderr,"numBytes = %0x\n", numBytes);

  int numjBytes = avcodec_encode_video(ocodecctx, jbuffer, numBytes , dFrame);
  //fprintf(stderr, "numjBytes = %0x\n", numjBytes);
  if (numjBytes < 0){
    fprintf(stderr,OBJOUTPUT("FFMPEG encoder (MJPEG) error!\n"), filename.c_str());
    av_free(buffer);
    av_free(jbuffer);
    av_free(dFrame);
    av_free(pFrame);
    avcodec_close(ocodecctx);
    av_free(ocodecctx);
    return false;
  }

  //write to file...
  FILE * jfile = fopen(filename.c_str(), "wb");
  if (!jfile){
    fprintf(stderr,OBJOUTPUT("Couldn't create file %s!\n"), filename.c_str());
    av_free(buffer);
    av_free(jbuffer);
    av_free(dFrame);
    av_free(pFrame);
    avcodec_close(ocodecctx);
    av_free(ocodecctx);
    return false;
  }

  ret = fwrite(jbuffer, sizeof(uint8_t), numjBytes, jfile);
  fclose(jfile);
  av_free(buffer);
  av_free(jbuffer);
  av_free(dFrame);
  av_free(pFrame);
  avcodec_close(ocodecctx);
  av_free(ocodecctx);

//  int t = 0/0;
  if (ret != numjBytes){
    fprintf(stderr,OBJOUTPUT("Couldn't write to file %s!\n"), filename.c_str());
    return false;
  }

  return true;

}

bool thumbnailer::seekframe(int64_t pos){
  if (!streamlen){
    fprintf(stderr,OBJOUTPUT("Stream lenght is unavailable, not seeking\n"));
    return true;
  }

  if(av_seek_frame(pFormatCtx, -1, pos, seekmode)>=0){
    if (ivideocodec->flush != NULL)
      ivideocodec->flush(icodecctx);
    return true;
  }
  avcodec_flush_buffers(icodecctx);
  fprintf(stderr,OBJOUTPUT("Could not seek to position %0.3f!\n"), static_cast<double>(pos)/AV_TIME_BASE);
  return false;
}

double thumbnailer::blanklevel(AVFrame * sFrame){
    AVFrame ttFrame;
    AVFrame * tFrame = &ttFrame;
    int tnumBytes=avpicture_get_size(PIX_FMT_RGB24, icodecctx->width, icodecctx->height);
    uint8_t * tbuffer = static_cast<uint8_t*>(av_malloc(tnumBytes));
    avpicture_fill(reinterpret_cast<AVPicture*>(tFrame), tbuffer, PIX_FMT_RGB24,
		            icodecctx->width, icodecctx->height);
    img_convert_ctx = sws_getCachedContext(img_convert_ctx, icodecctx->width, icodecctx->height,
                                           icodecctx->pix_fmt, icodecctx->width, icodecctx->height,
                                           PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(img_convert_ctx, sFrame->data, sFrame->linesize, 0, icodecctx->height, tFrame->data, tFrame->linesize);

    // We have a buffer in RGB24 format
    // we gotta compute the deviance
    // I am not a math expert, but this is how I think it should work

    //1st: compute average value for each color
    uint64_t av_r = 0, av_g = 0, av_b = 0, dev_r = 0, dev_g = 0, dev_b = 0;

    uint32_t offsg = icodecctx->width * icodecctx->height/24; // number of bytes in each color "plane"

for (unsigned int t = 0, offs = 0; t < offsg; t++){
      av_r += tbuffer[offs++];
      av_g += tbuffer[offs++];
      av_b += tbuffer[offs++];
    }

    av_r /=offsg; // average
    av_g /=offsg; // average
    av_b /=offsg; // average

    //2nd: find deviance

    for (unsigned int t = 0, offs = 0; t < offsg; t++){
      dev_r += abs(tbuffer[offs++] - av_r);
      dev_g += abs(tbuffer[offs++] - av_g);
      dev_b += abs(tbuffer[offs++] - av_b);
    }

    double deviance = static_cast<double>(dev_r + dev_g + dev_b)/3/offsg;
//    fprintf(stderr,"deviance = %f\n", deviance);
    av_free(tbuffer);

    return deviance;


}



