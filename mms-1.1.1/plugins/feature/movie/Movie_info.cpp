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

#include "Movie_info.h"
#include <cstring>
#include <sys/stat.h>
#include <math.h>
#include <stdarg.h>
#include <errno.h>             /* per  errno */
#include "endianesstools.h"
#include "ftyp.h"
//#define __func__  __PRETTY_FUNCTION__

//#define MPEG_DEEP_PARSE	// if defined, mpeg files are parsed till end of file; 
							// this makes parsing much slower and not necessarily more accurate,
							// but it can solve problems for some files


// routine to convert a double into a string
// rounded to the given precision and with a
// given min number of decimals
// we need this because stringstreams are overall cool,
// but when it comes to formatting numbers they are
// crap

std::string double_to_str(double x, unsigned int prec, unsigned int decs=0){
	int mod=1;
	if (prec>0)
		for(unsigned int t = 0; t < prec; t++)
			mod=mod*10;
	if (x > 0.0)
		x = (double)(floor( x * mod + 0.5 ) / mod);
	else if (x < 0.0) //you can't round 0.0 :P
		x = (double)(floor( x * mod - 0.5 ) / mod);
	ostringstream ostr;
	ostr << x;
	if (decs == 0)
		//just exits
		return ostr.str();
	
	//we make sure we have at least "decs" decimals
	std::string str;
	size_t pos = ostr.str().find(".");
	if (pos == std::string::npos){
		pos = ostr.str().size();
		ostr << '.';
	}
	pos = ostr.str().size() - pos -1;
	
	for (unsigned int t = pos; t < decs; t++)
		ostr << "0";
	return ostr.str();
}

std::string time_to_str(double secs){
			ostringstream ostr;
			unsigned int _hours = ((int)secs)/3600;
			unsigned int _mins = ((int)secs)/60%60;
			unsigned int _secs = ((int)secs)%60;
			double		 _msecs =(secs-(int)secs+_secs);
			if (_msecs > 0.01) ++_secs;//
			ostr.width(2);
			ostr.fill('0');
			ostr << right << _hours <<":";
			ostr.width(2);
			ostr.fill('0');
			ostr << right << _mins << ":";
			if (_secs < 10) ostr << "0";
//			ostr <<  double_to_str(_msecs,2);
			ostr << _secs;
			return ostr.str();			
}

// Now the real stuff...
Movie_info::Movie_info(){
	//we start allocating the buffer
	buf_len = INITIAL_BUFFERS_SIZE;
	verbosity = MOVIE_INFO_VERBOSE_ALL;
	initialized = false;
	//Check for endianess
	ProbeEndianess();
	ResetAll();
	
}


Movie_info::~Movie_info()
{
	if(initialized)
		free(file_buf);
}


void Movie_info::ResetAll(){
	got_image_size=false;
	got_movie_duration=false;
	got_aspect_ratio=false;
	got_container_type=false;
	got_audio_type=false;
	got_audio_channels=false;
	got_audio_bitrate=false;
	general_info.str("");
	audio_info.str("");
	got_info = false;
	video_info.str("");
	if(initialized){
		free(file_buf);
	}
	file_buf = (char*) malloc(INITIAL_BUFFERS_SIZE);
	initialized =  (!(file_buf==NULL));
}



bool Movie_info::Fetch_Movie_data(MOVIE_RESPONSE_INFO * ptrMovie_info_data){
	
	if (!got_info) return false;

	ptrMovie_info_data->aspect_ratio = aspect_ratio;
	ptrMovie_info_data->audio_channels = audio_channels;
	ptrMovie_info_data->audio_bitrate = audio_bitrate;
	ptrMovie_info_data->audio_info=audio_info.str();
	ptrMovie_info_data->audio_type = audio_type;
	ptrMovie_info_data->container_type = container_type;
	ptrMovie_info_data->general_info = general_info.str();
	ptrMovie_info_data->got_aspect_ratio = got_aspect_ratio;

	ptrMovie_info_data->got_audio_channels = got_audio_channels;
	ptrMovie_info_data->got_audio_bitrate = got_audio_bitrate;
	ptrMovie_info_data->got_audio_type = got_audio_type;
	ptrMovie_info_data->got_container_type = got_container_type;
	ptrMovie_info_data->got_image_size = got_image_size;
	ptrMovie_info_data->got_movie_duration = got_movie_duration;
	ptrMovie_info_data->image_size_h = image_size_h;
	ptrMovie_info_data->image_size_w = image_size_w;
	ptrMovie_info_data->movie_duration = movie_duration;
	ptrMovie_info_data->video_info = video_info.str();
	
	if (got_aspect_ratio){
		ptrMovie_info_data->aspect_ratio_str = double_to_str(aspect_ratio,2,2) + ":1";
	}
	
	if (got_image_size){
		ostringstream str;
		str << image_size_w << "x" << image_size_h;
		ptrMovie_info_data->resolution = str.str();
	}
	
	if (got_movie_duration){
		ptrMovie_info_data->movie_duration_str = time_to_str(movie_duration);
	}

	return true;
	
}

bool  Movie_info::bit_cmp( char  *buf1,  const char  buf2[], unsigned int bits){
	unsigned int bytes=bits/8;
	for (unsigned int t = 0; t < bytes; t++){
		//printf ("%x  ---> %x\n", buf1[t]&0xff,buf2[t]&0xff);
		if (buf1[t]!=buf2[t])
			return false;
	}
	int rol = bits%8;
	if (rol == 0)
		return true;
	
	char mask = '\xff';
	mask = mask << rol;
	return ((buf1[bytes]&mask) == (buf2[bytes]&mask));

}

uint64_t  Movie_info::fileat (){
	return cur_data_offs;
}


bool Movie_info::fileat (uint64_t pos){
	cur_data_offs = pos;
	return ((fseeko(pFile,pos, SEEK_SET))==0);
}


bool Movie_info::fileread(uint64_t len){
	if (len > buf_len){
		free(file_buf);

		file_buf = (char*) malloc(len+8);
		if (file_buf == NULL){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Out of memory\n", __func__);
			initialized=false;
			return false;
		}
		buf_len= len;
	}
	clearerr(pFile);
	cur_data_offs=ftello(pFile);
	cur_data_len= fread(file_buf, 1, len, pFile);

	if (ferror(pFile)!=0){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: I/O error - '%s'\n", __func__, strerror(errno));
		return false;
	}
	bufpos = 0;
	return true;//(feof(pFile)==0);
}

bool Movie_info::readbyte(uint8_t * byte){

	if (bufpos >= cur_data_len){
		if (!fileat(fileat()+bufpos) || !fileread(1024) || cur_data_len == 0) //we always read at least 1024 bytes
			return false;
	}

	(*byte) = file_buf[bufpos++];

	return true;
		
}

uint8_t Movie_info::readbyte(){
	uint8_t byte;
	if (readbyte(&byte))
		return byte;
	else
		return 0;
}

bool Movie_info::skipbytes(uint64_t bytes){
	
	bufpos += bytes;
	if (bufpos > cur_data_len){
		if (!fileat(fileat()+bufpos) || !fileread(1024)) //we always read at least 1024 bytes
			return false;
	}
	return true;
}

uint64_t Movie_info::getfilepos(){
	return (bufpos + fileat());
}


bool Movie_info::probe_movie(const string &file){
	ResetAll();
	if (!initialized){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Initialization error, unable to proceed\n", __func__ );
		return false;
	}
		
	struct stat64  _stat;

	if ((lstat64(file.c_str(), &_stat)) == -1){
    	Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: lstat failed while accessing %s\n", __func__, strerror(errno),file.c_str());
    	return false;
    	}
  	
  	else if (!S_ISREG(_stat.st_mode)) { //it's there, but is it a regular file?
    	Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: '%s' is not a regular file\n", __func__, file.c_str());
    	return false;
  	}
  	
  	file_len = _stat.st_size;
  	
 
	
	if ((pFile = fopen (file.c_str(),"r"))==NULL){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: I/O error - '%s' while accessing '%s'\n", __func__, strerror(errno),file.c_str());

		return false;
	}
	
	if(!fileat(0)){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: I/O error - '%s' while accessing '%s'\n", __func__, strerror(errno),file.c_str());
		fclose(pFile);
		return false;
	}

	if (!fileread(1024)){
		fclose(pFile);
		return false;
	}
	
	
	if (cur_data_len < 64){ // file is probably too short to be a movie
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File '%s' is too short\n", __func__, file.c_str());
		fclose(pFile);
		return false;
	}
	find_magic(file);
	fclose(pFile);
	return got_info;
}
	
	//we now start probing....

bool Movie_info::find_magic(const std::string& file){	
	if (bit_cmp(file_buf,MOVIE_MAGIC_ASF_1, sizeof(MOVIE_MAGIC_ASF_1-1)*8)){
		Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Possible ASF file type\n", __func__);
		got_info =  probe_asf();
	}
	
	else if (bit_cmp(file_buf,MOVIE_MAGIC_RIFF, 32) && bit_cmp(file_buf+8,MOVIE_MAGIC_AVI, 32)){
		 Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Possible AVI file type\n", __func__);
		got_info = probe_avi();
	}
		
	else if (bit_cmp(file_buf,MOVIE_MAGIC_RIFF, 32) && bit_cmp(file_buf+8,MOVIE_MAGIC_CDXA, 32)){
		 Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Possible CDXA file type\n", __func__);
		 got_info = probe_cdxa();
	}
	else if (bit_cmp(file_buf+4,"moov", 32)||
		bit_cmp(file_buf+4,"free", 32)||
		bit_cmp(file_buf+4,"wide", 32)||
		bit_cmp(file_buf+4,"ftyp", 32)||
		bit_cmp(file_buf+4,"skip", 32))
		{
			Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Possible Quicktime file type\n", __func__);
			got_info =  probe_mov();
		}
		
	else if (bit_cmp(file_buf,"\x00\x00\x01\xba", 32)||
		bit_cmp(file_buf,"\x00\x00\x01\xbe", 32)){
			got_info = probe_mpg();
	}
	
	else if(bit_cmp(file_buf, "\x49\x44\x33\x02", 32)){
		got_info = probe_id3();
	} 
	
	else if(bit_cmp(file_buf, "\x1A\x45\xDF\xA3", 32) &&
			bit_cmp(file_buf+8, "matroska", 32)){
		 Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Possible Matroska file type\n", __func__);
		 
		 got_info = probe_mkv();


	}
	else{	
		got_info = probe_mpg(0, true);
	}
	
	return got_info;
	
	
}

double get_pack_ts(char *file_buf, bool mpeg2){
		uint64_t a = 0;
 
		if (!mpeg2){
			
			a  = ((uint8_t)file_buf[4]&0xe);
			a = a<<29;
			a |= ((uint8_t)file_buf[5] <<22);
			a |= (((uint8_t)file_buf[6] &0xfe)<<14);
			a |= ((uint8_t)file_buf[7] << 7);
			a |= ((uint8_t)file_buf[8] >> 1);

//			printf ("TS is %lld\n", a/90000);
		}
		
		else {
			a  = ((uint8_t)file_buf[4]&0x38)<<27;
			a |= ((uint8_t)file_buf[4]&0x3) <<28;
			a |= ((uint8_t)file_buf[5] <<20);
			a |= (((uint8_t)file_buf[6] &0xf8)<<12);
			a |= (((uint8_t)file_buf[6] &0x03)<<13);
			a |= ((uint8_t)file_buf[7] << 5);
			a |= ((uint8_t)file_buf[8] >> 3);
//			printf ("TS is %lld\n", a/90000);
		
		}


//		printf("0x%02x%02x%02x%02x%02x\n", (uint8_t)file_buf[4],(uint8_t)file_buf[5],
//			(uint8_t)file_buf[6],(uint8_t)file_buf[7],(uint8_t)file_buf[8] );
		return a/(double)(90000);	
	
}



bool Movie_info::probe_mpg(uint32_t startfrom, bool skips){
	//regard mpeg support as experimental,
	//official info about this format is messy and far from complete
	//well, it may also be that I was unable to find it

	//we define an arbitrary max size of file we bother to probe
	const uint64_t	maxprobe = 1024*1024;//1 Mbyte
	if (cur_data_len < 64)
	         return false;
	uint64_t offs=startfrom;
	uint64_t next_stuff = 0;
	uint64_t old_offs = offs;
	bool mpeg2_5 = false;
	bool got_video = false;
	bool got_audio = false;
	bool got_pack = false;
	double first_pack_ts = 0.0;
//	TODO: show container type info for non PES files 

	for (;;){

//	printf("offset is %llx\n", fileat()+offs);	
//	printf("0x%02x%02x%02x%02x\n", (uint8_t)file_buf[offs],(uint8_t)file_buf[offs+1],(uint8_t)file_buf[offs+2],(uint8_t)file_buf[offs+3]);
	if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xba", 32)){//pack header, we skip it
		uint32_t _skip;
		bool mpeg2 = false;
		next_stuff = 0;
		if (((uint8_t)file_buf[offs+4] & 0xF0) == 0x20) {//mpeg 1
			_skip=12;
		}
		else if (((uint8_t)file_buf[offs+4]& 0xC0) == 0x40) {//mpeg 2
			mpeg2=true;
			if ((offs+13) > cur_data_len){
				if (!fileat(fileat()+offs)){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
					return false;
				}
				if (!fileread(256)||cur_data_len < 14){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
					return false;
				}
				offs = 0;
			}
				
			_skip = 14 + ((uint8_t)file_buf[offs+13]&0x7);
		}
		else{//	printf("offset is %llx\n", fileat()+offs);
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
			return false;
		}
		
//		printf("skip is %d\n", _skip);	
		if (!got_container_type){
			got_container_type=true;;

			general_info << "MPEG-";
			if (mpeg2) general_info << "2";
			else general_info << "1";

			general_info <<" sequence (multiplexed - PES)";
		}


		//let's see if we can fetch the time stamp
		if (!got_pack){
			got_pack = true;
			
			first_pack_ts=get_pack_ts(file_buf+offs, mpeg2); 
		}

		
		offs+=_skip;

	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xbb", 32)){//system header, we skip it
		offs+=6+be_val16((uint16_t&)file_buf[offs+4]);
	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xbe", 32)){//padding stream, we skip it
		
		offs+=6+be_val16((uint16_t&)file_buf[offs+4]);
	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xbd", 32)){//private stream1, we may use this
		
		uint64_t offs2=6+be_val16((uint16_t&)file_buf[offs+4]);
		if (!got_audio){
			uint32_t t = 9+((uint8_t)file_buf[offs+8]);
			if ((t+8) >= offs2){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
				return false;
			}
			 
			if (offs+offs2 > cur_data_len){
				fileat(fileat()+offs);
				if (!fileread(offs2+4)||cur_data_len <(offs2+4)){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file3\n", __func__);
					return false;
				}
				offs=0;
			}
			uint32_t substream_type = (uint8_t)file_buf[offs+t]; 		
			
			if (substream_type >= 0x20 && substream_type <= 0x3f){
				//subpictures, we don't need it
			}
			else if (substream_type >= 0x80 && substream_type <= 0x87){
				//AC3 audio
				t+=4;
				if (bit_cmp(&file_buf[offs+t], "\x0b\x77", 16)){
					got_audio=true;
					audio_info << "AC3 Audio";

					got_audio_type = true;
					audio_type = "AC3 Audio";

					uint8_t index = (uint8_t)file_buf[offs+t+4];
					uint32_t samplerate = mpg_ac3audio_samplerate[index>>5];
					uint32_t bitrate = 0;
					if ((index&0x3f) <=0x25)
						bitrate=mpg_ac3audio_bitrate[index&0x3f];
						
						if (bitrate !=0 || samplerate != 0){
							audio_info << 	", bitrate: ";
							if (bitrate!=0) {
							  audio_info << bitrate << " kb" ; 
							  got_audio_bitrate = true;
							  ostringstream tmp;
							  tmp << bitrate << " kb";
							  audio_bitrate = tmp.str();
							}
							
							if (samplerate!=0 && bitrate!=0)  audio_info << ", " << samplerate << "Hz";
							else if (samplerate!=0)  audio_info << samplerate << "Hz";
							
						}
				
				//TODO: fetch num of channels
				}
			}
			else if (substream_type >= 0x88 && substream_type <= 0x8F){
				//DTS audio stream
				got_audio_type = true;
				audio_type = "DTS Audio";
				audio_info << audio_type;
				//TODO: parse it ffs!
			}
			
			else if (substream_type >= 0xA0 && substream_type <= 0xA7){
				//LPCM audio stream
				got_audio_type = true;
				audio_type = "LPCM Audio";
				audio_info << audio_type;
				//TODO: parse it ffs!
			}
			
		}
		offs+=offs2;		
	}
	
	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xbf", 32)){//private stream2, we skip it
		offs+=6+be_val16((uint16_t&)file_buf[offs+4]);
	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xb3", 32)){//sequence header, we need it
		//TODO: make sure we have the whole header
		uint32_t w = be_val16((uint16_t&)file_buf[offs+4])>>4;
		uint32_t h = be_val16((uint16_t&)file_buf[offs+5])&0xfff;
		if (!got_video){
			got_video=true;
			got_image_size=true;
			got_aspect_ratio=true;
			image_size_w=w;
			image_size_h=h;
			w = (uint8_t)file_buf[offs+7]>>4;

			aspect_ratio = mpg_aspect_ratio[w];
			if (aspect_ratio==0&& image_size_w!=0 && image_size_h!=0) {//file doesn't provide a valid aspect ratio
								  //so we compute it
				Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Invalid aspect ratio, using workaround\n", __func__);
				aspect_ratio=(double)image_size_w / image_size_h;
			}
				
			video_info << "Mpeg Video aspect: " << image_size_w << "x" << image_size_h << " (" <<
				double_to_str(aspect_ratio,2,2) << ":1)";
			w = (uint8_t)file_buf[offs+7]&0xf;
			video_info << " (" << double_to_str(mpg_frame_rate[w],2,2) << "fps)";
		}

		offs+=(12-4+((((uint8_t)file_buf[offs+11]&0x03)!=0)?64:0));
		goto skip_brutal;
	}
	
	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xe0", 28)){

		next_stuff = 6+ be_val16((uint16_t&)file_buf[offs+4]);
		if (true){
			fileat(fileat()+offs);
			if (!fileread(next_stuff+4)||cur_data_len <(next_stuff+4)){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
				return false;
			}
		offs=0;
		}
		next_stuff+=offs;
		uint32_t header_len = 8;
		bool mpeg2 = ((uint8_t)file_buf[offs+7]&0xc0==0x80);
		if (mpeg2){	
			uint32_t xx = (uint8_t)file_buf[offs+7]>>6;//PTS_DTS flags
		
			if (xx == 1){
				header_len+=5;
//				printf("PTS_DTS flags\n");
			
			}
	
			else if (xx == 3){
				header_len+=10;
//				printf("PTS_DTS flags\n");
			}
			
			else if (xx!=0){
				xx = (uint8_t)file_buf[offs+7];
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Invalid PTS/DTS flag in video stream %x (offs is %llx)\n", __func__, xx, fileat()+offs+7);
				offs=next_stuff;
				next_stuff=0;
				goto reloop;
			}
	
			xx = ((uint8_t)file_buf[offs+7]>>5)&0x1;//ESCR flag
			if (xx!=0){
				header_len+=6; //Don't ask me why but some broken files have this set even if
	//					ESCR is not present.... grrrr
				
			}
	
			xx = ((uint8_t)file_buf[offs+7]>>4)&0x1;//ES-rate flag
			if (xx!=0){
				header_len+=3;
				printf("ES-rate flag\n");
			}
	
			xx = ((uint8_t)file_buf[offs+7]>>3)&0x1;//DSM trick mode flag
			if (xx!=0){
				header_len++;
				printf("DSM trick mode flag\n");
			}
	
			xx = ((uint8_t)file_buf[offs+7]>>2)&0x1;//Additional copy info flag
			if (xx!=0){
				header_len++;
				printf("Additional copy info flag\n");
			}
	
			xx = ((uint8_t)file_buf[offs+7]>>1)&0x1;//PES CRC flag
			if (xx!=0){
				header_len+=2;
				printf("PES CRC flag\n");
	
			}
	
			xx = ((uint8_t)file_buf[offs+7])&0x1;//PES extension flag
			if (xx!=0){
				printf("PES extension flag\n");
				xx=((uint8_t)file_buf[offs+header_len]);
				header_len++;
				if ((xx>>7)!=0) //PES private data flag
					header_len+=16;
	
				if ((xx>>6)&0x1!=0) //pack header field flag
					header_len++;
	
				if ((xx>>5)&0x1!=0) //program packet sequence counter flag
					header_len+=2;
	
				if ((xx>>4)&0x1!=0) //P-STD buffer flag 
					header_len+=2;
	
				if (xx&0x1!=0){ //PES_extension_field_flag
					header_len+=(1+(uint8_t)file_buf[offs+header_len]&0x7f);
				}
			}
		}
		else header_len=0x11;

		if (offs+header_len+4 > cur_data_len){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
			return false;
		}

		uint64_t t = header_len + offs;
		bool found = false;
		while (t < (next_stuff -3)){
			if (bit_cmp(&file_buf[offs+t], "\x00\x00\x01\xb3", 32)){
				next_stuff+=fileat();
				offs=t;
				found = true;
				break;
			}
			t++;
		}

		if (!found){

			offs=next_stuff;
			next_stuff=0;
		}
	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xc0", 28)|| bit_cmp(&file_buf[offs], "\x00\x00\x01\xd0", 28)){
		next_stuff = be_val16((uint16_t&)file_buf[offs+4]);
		//we make sure we have the whole stuff
		uint8_t layer = 0, mpegtype=0;
				
		if (!got_audio){
			if ((offs+next_stuff+6) < cur_data_len){
				fileat(fileat());
				if (!fileread(next_stuff+6+offs)||cur_data_len <(next_stuff+6+offs)){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
					return false;
				}	
			}
			uint64_t offs2=offs+6;
		
			for (uint32_t t=0; t < next_stuff; t++){
				
				
				if (((uint8_t)file_buf[offs2]==0xFF)&&
						(((uint8_t)file_buf[offs2+1]& 0xF0)==0xF0)&&
						((uint8_t)file_buf[offs2+1]!=0xFF)) 
				{
					got_audio=true;
					break;
				}
				else if (((uint8_t)file_buf[offs2]==0xFF)&&(((uint8_t)file_buf[offs2+1] & 0xE0)==0xE0)&&
						((uint8_t)file_buf[offs2+1]!=0xFF)) {
					mpeg2_5 = true;
	                		got_audio=true;
		            		break;
				}
		
				if ((t+4) > next_stuff){

					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
					return false;
				}
				offs2++;
		
			}
		
			if (!got_audio ){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
				return false;
			}

			//this seems audio stuff
			got_audio_type = true;
			audio_type = "mp";

			audio_info << "Mpeg Audio";
			if ((uint8_t)file_buf[offs2+1] & 0x08) {
				if (!mpeg2_5) {
					mpegtype=1;
					audio_info << " v. 1.0";
				}
				else {
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
					return false;
				}
			}
			else {
				if (!mpeg2_5){
					mpegtype=2;
					audio_info << " v. 2.0";
				}
				else{
					mpegtype=3;
					audio_info << " v. 2.5";
				}
			}
	
			//layer?
			audio_info << ", layer ";
			layer = ((uint8_t)file_buf[offs2+1] & 0x06)>>1;
			switch (layer){
				case 0: Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file\n", __func__);
					return false;
					break;
				case 1:	audio_info << "III";
				        audio_type += "3";
					break;
				case 2:	audio_info << "II";
				        audio_type += "2";
					break;
				case 3:	audio_info << "I";
				        audio_type += "1";
			}
			uint8_t mode = (uint8_t)file_buf[offs2+3]>>6;
			
			switch (mode){
				case 0: audio_info << ", Stereo"; break;
				case 1: audio_info << ", Joint Stereo"; break;
				case 2: audio_info << ", Dual Channel"; break;
				case 3: audio_info << ", Mono"; break;
			}
			
			uint32_t index = ((uint8_t)file_buf[offs2+2]>>4);
			uint32_t temp = 0; 
			
			if (mpegtype==1 && layer==3)
				temp=mpg_audio_bitrate[index][0];
			else if (mpegtype==1 && layer==2)
				temp=mpg_audio_bitrate[index][1];
			else if (mpegtype==1 && layer==1)
				temp=mpg_audio_bitrate[index][2];
			else if (mpegtype==2 && layer==3)
				temp=mpg_audio_bitrate[index][3];
			else if  (mpegtype >=2)
				temp=mpg_audio_bitrate[index][4];
			
			if (temp == 0){
				Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Invalid audio bitrate value\n", __func__);
			}
			else {
				audio_info << ", bitrate: "<< double_to_str((double)temp,3,0) << " kb"; 
				got_audio_bitrate = true;
				audio_bitrate = double_to_str((double)temp,0,0) + " kb";
			}
			temp = 0;
			index = ((uint8_t)file_buf[offs2+2]&0xf)>>2;
			if (index <3)
				temp = mpg_audio_samplerate[index][mpegtype-1];
			
			if (temp ==0)
				Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Invalid audio samplerate value\n", __func__);
			
			else {
				audio_info << ", " << temp << "Hz";
			}
	
		}

		offs+=next_stuff+6;
		next_stuff=0;
	}


	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xb8", 32)){
		offs+=8;
	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xb2", 32)){// we skip this brutally
			
skip_brutal:
		uint64_t offs2 = offs+4;
		
		for (;;) {
			if (offs2+4 > cur_data_len){
				
				fileat(fileat());
				if (!fileread(1024+offs2))
					return false;
				if (cur_data_len < 14){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
					return false;
				}
			}

			if (bit_cmp(&file_buf[offs2], "\x00\x00\x01", 24)){
				offs=offs2;
				break;
			}
			offs2++;
		}
	}

	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xb5", 32)){//extension header, we skip it

		switch ((uint8_t)file_buf[offs+4] >> 4) {
			case 1: 
				offs+=10;
				break;
			case 2:
				if (file_buf[offs+4]& 1 ==1)
					offs+=12;
				else offs+=9;
				break;
			case 8:
				if ((file_buf[offs+8]>>6&1)==1)
					offs+=11;
				else
					offs+=9;
				break;
			default:
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid mpg file6\n", __func__);
				return false;
		}
	}
	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01", 24)&& ((uint8_t)file_buf[offs+3]< 0xb9)){//we jump to the next frame
		if (next_stuff==0){
			goto skip_brutal;
			
			
		}
		else{
			if(!fileat(next_stuff)){
				Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Reached end of file\n", __func__);
				return true;
			}
			offs=0;
			cur_data_len=0;
			next_stuff = 0;
		}
	}
		
	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01\xb9", 32)){//end of stream
		break;

	}
	
	else if (bit_cmp(&file_buf[offs], "\x00\x00\x01", 24)){
		Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Found unknown mpeg header 0x%02x%02x%02x%02x at offset 0x%llx\n",  __func__, 
			(uint8_t)file_buf[offs],(uint8_t)file_buf[offs+1],(uint8_t)file_buf[offs+2],(uint8_t)file_buf[offs+3], offs+fileat());
		goto skip_brutal;
	}
	
	else if (file_buf[offs]=='\x00'||file_buf[offs]=='\xff'){

		offs++;
	}
	
	else {	
		if (skips) {
			offs+=1;
//			printf("offs is %llx\n", offs);
		}
		else {
			//printf("offset is %llx\n", fileat()+offs);
			//printf("0x%02x%02x%02x%02x\n", (uint8_t)file_buf[offs],(uint8_t)file_buf[offs+1],(uint8_t)file_buf[offs+2],(uint8_t)file_buf[offs+3]);
			break;
		}
	}
reloop:
	if (offs+12> cur_data_len){
		if(!fileat(fileat()+offs)){
			Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Reached end of file\n", __func__);
			break;
		}
		
		if (!fileread(4096))
		              break;
		
		offs=0;
		if (cur_data_len ==0){
			Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Reached end of file\n", __func__);
			break;
		}
		
		if (cur_data_len >= 4 && (bit_cmp(&file_buf[offs], "\x00\x00\x01\xb9", 32)||
			bit_cmp(&file_buf[offs], "\x00\x00\x01\xb7", 32))){
			Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Reached end of file\n", __func__);
			break;
		}
		
			

		if (cur_data_len < 12){
			break;
		}
		
	}


#ifndef MPEG_DEEP_PARSE

	if ((got_audio && got_video)||(offs+fileat()> maxprobe))
		break;
#endif 


	if (old_offs+fileat() == offs){
		Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Endless loop condition detected in mpeg parsing: aborting\n", __func__);
		break;
	}
	old_offs=(fileat()+offs);
	}

	if (!got_audio && ! got_video)
		return false;
		
	if (got_pack){
		
		//We try to retrieve movie duration, but only for PES files (it wouldn't be reliable otherwise)
		//This is what we do: we find the last pack header in file
		//and retrieve its timestamp
		
		
		if (file_len < (uint64_t)128000)
			fileat(0);
		
		else fileat(file_len-128000);

		if (fileread(128000) && cur_data_len > 12){
			
			offs=cur_data_len-12;
			bool found = false;
			while ( offs !=0){
				if ((bit_cmp(&file_buf[offs], "\x00\x00\x01\xba", 32))&&(
					(((uint8_t)file_buf[offs+4] & 0xF0) == 0x20) ||
					(((uint8_t)file_buf[offs+4] & 0xC0) == 0x40)))
					
				{
						found = true;
						break;
				}
			offs--;
			}
			
			if (found){
				bool mpeg2 = (((uint8_t)file_buf[offs+4]& 0xC0) == 0x40);
				got_movie_duration = true;
				movie_duration = get_pack_ts(file_buf+offs, mpeg2) ;
				
				if (first_pack_ts > movie_duration){
					Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: One or more pack headers have a wrong Time Stamp!!!\n", __func__);
					Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Movie duration might be unreliable\n", __func__);
				}
				else
					movie_duration -= first_pack_ts;
				
				general_info <<", duration: " << time_to_str(movie_duration);
//				printf("offset of last pack header is 0x%llx\n", offs+fileat());
			} 
			
		}
	} 

	Log_Msg(MOVIE_INFO_MSG_INFO, "General info-> %s\n", general_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Video streams info-> %s\n",video_info.str().c_str());
    Log_Msg(MOVIE_INFO_MSG_INFO, "Audio streams info-> %s\n",audio_info.str().c_str());
	return (got_video==true || got_audio == true);

}



#if 1
bool Movie_info::probe_cdxa(){
	if (cur_data_len < 64)
		return false;

	if (!bit_cmp(file_buf+12,"fmt",3*8))
		return false;

        if (!fileat(fileat()+0x2c))
		return false;
	
	if (!fileread(2352))
		return false;
	
	bool _bool = true;
	while (_bool){
	        if (cur_data_len < 2352)
	                return false;
		
		if (!bit_cmp(file_buf,"\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00", 12*8))  //CDXA sync
			return false;

		if (bit_cmp(file_buf+0x18, "\x00\x00\x01\xba", 32))  //we want CDXA pack header code
															 //as start of mpeg data
		{
			general_info << "(RIFF - CDXA) ";
			return probe_mpg(0x18, true);

		}
		if (!fileat(fileat()+2352))
			return false;

		_bool=fileread(2352);
	}
	return true;
	
}
#endif


bool Movie_info::probe_id3(){
	if (cur_data_len < 64)
		return false;
	
	unsigned long a;
	
	a  = (((uint8_t)file_buf[6] & 0x7f)<<21);
	a |= (((uint8_t)file_buf[7] & 0x7f)<<14);
	a |= (((uint8_t)file_buf[8] & 0x7f)<<7);
	a |= ((uint8_t)file_buf[9] & 0x7f);
	a += 10;


	if ((a+6) >  cur_data_len){
		if (!fileat(fileat()+a)){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
				return false;
		}
		
		if (!fileread(256)||cur_data_len < 14){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
				return false;
		}
				a = 0;
	}
	
	return probe_mpg(a);
	
}




bool Movie_info::probe_avi(){

	if (cur_data_len < 64)
		return false;
	
	if (!bit_cmp(file_buf+0x14, "hdrlavih", 64)){
		//we don't have the main avi header
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
		return false;
	}
	
	//we now download the whole 1st LIST chunk...
	if (!fileat(fileat()+0x18)){
		//file is broken?
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
		return false;
	}
	
	uint32_t to_download;
	to_download = le_val32((uint32_t&) file_buf[0x10]);

	if(to_download <  (sizeof(AVIMAINHEADER) + sizeof(AVISTREAMHEADER))){
		//file is broken?
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
		return false;
	}
		
	
	if (!fileread(to_download)){
		//file is broken?
		//TODO: Error message
		return false;
	}
	
	if (cur_data_len < to_download){
		//file is broken?
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
		return false;
	}
		
	bool got_audio = false;
	bool got_video = false;
	
	AVIMAINHEADER avimainhead;
	AVISTREAMHEADER avistreamhead;
	
	//we fill the relevant structures...
	memcpy(&avimainhead, file_buf, sizeof(AVIMAINHEADER));
	if (le_val32(avimainhead.cb) != (sizeof(AVIMAINHEADER)-8)){
		
		//this header is borked and we can't trust it
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid AVI file1\n", __func__);
		return false;
	}
		
	
	int _offs=sizeof(AVIMAINHEADER);
	got_container_type=true;
	container_type= "RIFF AVI File";
	general_info <<  container_type <<  " - streams: " << le_val32(avimainhead.dwStreams);
	got_image_size=true;
	image_size_w=le_val32(avimainhead.dwWidth);
	image_size_h=le_val32(avimainhead.dwHeight);
	general_info << ", aspect: "<< image_size_w << "x" << image_size_h  << " (";
	long double a=(double)image_size_w / image_size_h;
	got_aspect_ratio=true;
	aspect_ratio=a;

	general_info << double_to_str(a,2,2) <<":1)";
	
	
	for (unsigned int t = 0; t < le_val32(avimainhead.dwStreams); t++ ){
		int _offs2 = 0;
		if(!Movie_info::bit_cmp(file_buf+_offs,"LIST",32)){
			
			//Missing mandatory stream header
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Missing mandatory LIST object\n", __func__);
			return false;
		}

		_offs2+=4;
		if ((le_val32((uint32_t&)file_buf[_offs+_offs2])+_offs+8) > cur_data_len){
			//We should have got the whole avimainheader + stream header stuff,
			//but apparently it's not so. Data is not reliable so we exit.
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid AVI file\n", __func__);
			return false;
		}
		
		_offs2+=4;
		if(!Movie_info::bit_cmp(file_buf+_offs+_offs2,"strl",32)){
			
			//Missing mandatory stream header (strl)
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Missing mandatory stream header (strl)\n", __func__);
			return false;
		}
	
		_offs2+=4;
		
		memcpy(&avistreamhead, file_buf+_offs+_offs2, sizeof(AVISTREAMHEADER));	
		if(le_val32(avistreamhead.cb) < (sizeof(AVISTREAMHEADER)-8)){
			//this header is borked and we can't trust it
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid AVI file\n", __func__);
			return false;
		}
		
		// what kind of stream is this?

		
		if (le_val32(avistreamhead.fccType) == AVI_STREAM_MIDS){
		}
			
		
		else if (le_val32(avistreamhead.fccType) == AVI_STREAM_VIDS){
			if (got_video)
				video_info << '\n';
			got_video = true;
			
			video_info << "Stream " << t+1 << " out of " << le_val32(avimainhead.dwStreams) << ": " <<
				fourcc_to_str(avistreamhead.fccHandler).c_str();
			video_info << ", aspect: "  << (le_val32(avistreamhead.rcFrame.right) - le_val32(avistreamhead.rcFrame.left)) <<"x" <<
			(le_val32(avistreamhead.rcFrame.bottom) - le_val32(avistreamhead.rcFrame.top)) ;
			movie_duration=(double)le_val32(avistreamhead.dwRate)/le_val32(avistreamhead.dwScale);
			movie_duration=(double)le_val32(avistreamhead.dwLength)/movie_duration;
			got_movie_duration=true;
			video_info << ", duration: " << time_to_str(movie_duration) <<" (";
	
			a=(double)avistreamhead.dwRate/avistreamhead.dwScale;
			video_info << double_to_str(a,2) << " fps)";
		}
		
		else if (le_val32(avistreamhead.fccType) == AVI_STREAM_AUDS){
		//for audio, the audiostream header is useless,
		//we actually need the stream format data
		//but first some safety checks
			if (cur_data_len < (_offs+_offs2+sizeof(AVISTREAMHEADER)+sizeof(WAVEFORMATEX)+8)){
				//this header is borked and we can't trust it
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is damaged or this is not a valid AVI file\n", __func__);
				return false;
			}
			_offs2=_offs2+sizeof(AVISTREAMHEADER);
			
			if (!bit_cmp(file_buf+_offs+_offs2,"strf",32)){
				//missing mandatory stream format data
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Missing mandatory stream format data\n", __func__);
				return false;
			}
			_offs2+=8;
			if (got_audio)
				audio_info << '\n';
			got_audio = true;
			WAVEFORMATEX wave;
			memcpy(&wave, file_buf+_offs+_offs2, sizeof(WAVEFORMATEX));
			
			audio_info <<  "Stream " << t+1 << " out of " << le_val32(avimainhead.dwStreams) << ": " <<
			"type " << le_val16(wave.wFormatTag) <<", " << le_val16(wave.nChannels) << " channels, bitrate: ";
			audio_info << double_to_str((double)le_val32(wave.nAvgBytesPerSec)*8/1000, 3) << " kb";
			if (!got_audio_bitrate) {
			  got_audio_bitrate = true;
			  audio_bitrate = double_to_str((double)le_val32(wave.nAvgBytesPerSec)*8/1000, 0) + " kb";
			}

			if (!got_audio_type){
				ostringstream _tmp;
				got_audio_type=true;
				_tmp << "type " << le_val16(wave.wFormatTag);
				audio_type = _tmp.str();
				got_audio_channels=true;
				audio_channels=le_val16(wave.wBitsPerSample);
			}
			if (le_val16(wave.wBitsPerSample)!=0)
				audio_info << ", " << le_val16(wave.wBitsPerSample) << " bits per sample";
			
			if (le_val32(wave.nSamplesPerSec)!=0)
				audio_info << ", " << wave.nSamplesPerSec << "Hz";

		}
		
		else if (le_val32(avistreamhead.fccType) == AVI_STREAM_TXTS){
			//FIXME: we should handle it
		}

		_offs+=	le_val32((uint32_t&)file_buf[_offs+4])+8;
	};
	Log_Msg(MOVIE_INFO_MSG_INFO, "General info-> %s\n", general_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Video streams info-> %s\n",video_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Audio streams info-> %s\n",audio_info.str().c_str());

	return true;	

}

bool Movie_info::probe_asf(){
	//This is madness! This is blasphemy!!!
	//Madness? THIS IS THE ASF FORMAT!!!!
	if (cur_data_len < 64){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
		return false;
	}
		
	//we already checked the main header GUID
	//we now download, if necessary, the top-level stuff, i.e.
	//everything till before the first data packet
	uint64_t headers_len= le_val64((uint64_t &)file_buf[16])+8;
	uint64_t offs=0;
	
	if (cur_data_len <  headers_len){
			if (!fileat(0)|| !fileread(headers_len)){
			//file is broken?
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: I/O error - '%s' \n", __func__, strerror(errno));
				return false;
			}
	}
	
	if (cur_data_len < headers_len){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
			return false;
	}
	
	vector<ASF_STREAM_V_STORAGE> v_streams;
	vector<ASF_STREAM_A_STORAGE>	a_streams;
	vector<ASF_EXTENDED_STREAM_PROPERTIES> ext_streams_p;

	ASF_HEADEROBJECT head_obj;
	memcpy(&head_obj, &file_buf[offs], sizeof(ASF_HEADEROBJECT));

	unsigned int num_of_objects = le_val32(head_obj.Number_of_Header_Objects);
	//We skip the main header object, nothing else useful
	//in there	
	offs+= sizeof(ASF_HEADEROBJECT);
	
	if (cur_data_len < (offs+sizeof(GUID)+8)){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
		return false;
	}
	
	for (unsigned int t = 0; t <  num_of_objects; t++){
		if (cur_data_len < (offs+sizeof(GUID)+8)){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
			return false;
		}
#if 0	
		char zz;
			for (int t=0; t < 16; t++){
			zz=file_buf[t+offs];
			printf ("\\x%02x", (zz&0xff));
			}
			printf("\n");
#endif
		
		if (bit_cmp(&file_buf[offs],ASF_GUID_FILE_PROP, 128)){
			if (cur_data_len < (offs+sizeof(ASF_FILEPROPERTIES_OBJECT))){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
					return false;
					}
		
			ASF_FILEPROPERTIES_OBJECT obj;
			obj = ((ASF_FILEPROPERTIES_OBJECT&) file_buf[offs]);
		
			uint64_t duration = (le_val64(obj.Send_Duration)  -  le_val64(obj.Preroll));///10000000;
			if (duration==0|| (le_val32(obj.Flags)&0x80000000))
			 	Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Missing or unset duration time property\n", __func__);
			else{
				got_movie_duration=true;
				movie_duration=((double)duration)/10000000;
			}
		}
	
		else if (bit_cmp(&file_buf[offs],ASF_GUID_STREAM_PROP, 128)){
			//we have a stream object
			if (cur_data_len < (offs+sizeof(ASF_STREAM_PROPERTIES_OBJECT))){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
				return false;
			}
			unsigned int stream_num = le_val32(((ASF_STREAM_PROPERTIES_OBJECT &)file_buf[offs]).Flags);
			stream_num=stream_num&0x7f;
						
			if (bit_cmp(&file_buf[offs+0x18],ASF_GUID_STREAM_VIDEO_MEDIA, 128)){//VIDEO STREAM
				if (cur_data_len < (offs+sizeof(ASF_STREAM_PROPERTIES_OBJECT)+
					sizeof(ASF_STREAM_VIDEO)+sizeof(BITMAPINFOHEADER))){
						Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
						return false;
				}
				ASF_STREAM_V_STORAGE s_video;
				s_video=(ASF_STREAM_VIDEO&) file_buf[offs+sizeof(ASF_STREAM_PROPERTIES_OBJECT)];
				s_video.stream_num = stream_num;
				s_video.type = ((BITMAPINFOHEADER&)file_buf[offs+sizeof(ASF_STREAM_PROPERTIES_OBJECT)+sizeof(ASF_STREAM_VIDEO)]).biCompression;
//				
//				printf("%c%c%c%c\n", s_video.type&0xff, (s_video.type>>8)&0xff,
//						(s_video.type>>16)&0xff, (s_video.type>>24)&0xff);
				
				
				
				v_streams.push_back(s_video);
//			printf("Image size is %d x %d\n", s_video.obj.Encoded_Image_Width, s_video.obj.Encoded_Image_Height);
			
			}

			else if(bit_cmp(&file_buf[offs+0x18], ASF_GUID_STREAM_AUDIO_MEDIA, 128)){//AUDIO STREAM
				if (cur_data_len < (offs+sizeof(ASF_STREAM_PROPERTIES_OBJECT)+
					+sizeof(WAVEFORMATEX))){
						Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
						return false;
				}

				ASF_STREAM_A_STORAGE s_audio;
				s_audio=(WAVEFORMATEX&) file_buf[offs+sizeof(ASF_STREAM_PROPERTIES_OBJECT)];
				s_audio.stream_num = stream_num;
				a_streams.push_back(s_audio);
			}

			//We don't bother of other stream types.
		}
		
		else if (bit_cmp(&file_buf[offs],ASF_GUID_HEADER_EXTENSION, 128)){
		//first thing our usual sanity check....
			if (cur_data_len < (offs+sizeof(ASF_HEADER_EXTENSION_OBJECT))){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
				return false;
			}
			//second sanity check....
			if (cur_data_len < (offs+sizeof(ASF_HEADER_EXTENSION_OBJECT)+(uint64_t&)file_buf[offs+16])){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
				return false;
			}
			
			ASF_HEADER_EXTENSION_OBJECT ext_obj;
			memcpy(&ext_obj, &file_buf[offs], sizeof(ASF_HEADER_EXTENSION_OBJECT));

			unsigned int z = sizeof(ASF_HEADER_EXTENSION_OBJECT);
			while (z  < (le_val64(ext_obj.Header_Extension_Data_Size)+sizeof(ASF_HEADER_EXTENSION_OBJECT))){
				if (bit_cmp(&file_buf[offs+z], ASF_GUID_EXT_STREAM_PROPERTIES, 128)){
					ASF_EXTENDED_STREAM_PROPERTIES ext_stream;
					memcpy(&ext_stream, &file_buf[offs+z], sizeof(ASF_EXTENDED_STREAM_PROPERTIES));
					ext_streams_p.push_back(ext_stream);
				}
				z+=le_val64((uint64_t&)file_buf[offs+z+16]);
			}
		}
		
		//we simply ignore other objects
		offs+=(uint64_t&)file_buf[offs+16];
	}
	got_container_type=true;
	container_type="Advanced Systems Format (ASF) file";
	general_info << container_type << " - streams: " << (v_streams.size() + a_streams.size());
	if (got_movie_duration){

		general_info << ", duration " << time_to_str(movie_duration);
	}
		
	//process video streams
	int z = 0;
	if (v_streams.size()>0)
	for (vector<ASF_STREAM_V_STORAGE>::iterator iter = v_streams.begin(); iter != v_streams.end();iter++){
		int strnum=((ASF_STREAM_V_STORAGE)*iter).stream_num;
		int w=le_val32(((ASF_STREAM_V_STORAGE)*iter).obj.Encoded_Image_Width);
		int h=le_val32(((ASF_STREAM_V_STORAGE)*iter).obj.Encoded_Image_Height);
		long double a=(double)w/h;
		
		if (!got_image_size){
			got_image_size=true;
			got_aspect_ratio=true;
			image_size_w=w;
			image_size_h=h;
			general_info << ", aspect: "<< w << "x" << h  << " (";
			general_info << double_to_str(a,2,2) <<":1)";
			aspect_ratio =a;


		}
		if(z>0)
			video_info << "\n";
			
		video_info << "Stream " << strnum << " out of " << (v_streams.size() + a_streams.size()) << ": ";
		char* c_tmp= (char *) &((ASF_STREAM_V_STORAGE)*iter).type;
		video_info << c_tmp[0] <<  c_tmp[1] <<  c_tmp[2] <<  c_tmp[3]  <<", aspect: ";
		
		video_info << w << "x" << h <<" (";
		video_info<< double_to_str(a,2,2) <<":1)";
		//TODO: add fps			
		z++;	
	}
	//process audio streams
	z=0;
	if (a_streams.size()>0)
	for (vector<ASF_STREAM_A_STORAGE>::iterator iter = a_streams.begin(); iter != a_streams.end();iter++){
		int strnum=((ASF_STREAM_A_STORAGE)*iter).stream_num;
		audio_info << "Stream " << strnum << " out of " << (v_streams.size() + a_streams.size()) << ": " <<
			"type " << le_val16(((ASF_STREAM_A_STORAGE)*iter).obj.wFormatTag) <<", " <<
			le_val16(((ASF_STREAM_A_STORAGE)*iter).obj.nChannels) << " channels";
		if (le_val16(((ASF_STREAM_A_STORAGE)*iter).obj.wBitsPerSample)!=0)
	                audio_info << ", " << le_val16(((ASF_STREAM_A_STORAGE)*iter).obj.wBitsPerSample) << " bits per sample";
               
		if (le_val32(((ASF_STREAM_A_STORAGE)*iter).obj.nSamplesPerSec)!=0)
			audio_info << ", " << le_val32(((ASF_STREAM_A_STORAGE)*iter).obj.nSamplesPerSec) << "Hz";
			audio_info << ", bitrate: " <<
			double_to_str((double)le_val32(((ASF_STREAM_A_STORAGE)*iter).obj.nAvgBytesPerSec)*8/1000, 3) << " kb";
			got_audio_bitrate = true;
			audio_bitrate = double_to_str((double)le_val32(((ASF_STREAM_A_STORAGE)*iter).obj.nAvgBytesPerSec)*8/1000, 0) + " kb";
	}

	Log_Msg(MOVIE_INFO_MSG_INFO, "General info-> %s\n", general_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Video streams info-> %s\n",video_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Audio streams info-> %s\n",audio_info.str().c_str());
	return true;
}

bool  Movie_info::probe_mov(){
        if (cur_data_len < 64){
	    Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is too short\n", __func__);
	    return false;
	}
	uint64_t atomsize, offs=0;
	char atomname[5];	
//	const uint64_t max_atom_len = 0xfffff; //max size of an atom we bother to parse, data and children atoms included
	bool got_ftyp=false;
	bool got_moov=false;
	uint64_t atom_head;
	unsigned int tracknum=0;
	double fps=0.0, track_duration;


	uint32_t timescale = 0,  track_duration_track=0;
	got_container_type=true;
	container_type="Quicktime file (generic)";
	uint8_t  tracktype=0;// 0 means undefined
						 // 1 means audio track
						 // 2 means video track
	bool got_audio=false, got_video=false;
	
	const char spurious_msg[]="%s: Spurious '%s' atom: aborting\n";
	const char unexp_msg[]="%s: Unexpected end of file\n";
	

	while (true){
//	printf("offs is %llx, cur_data_len is %llx\n", offs, cur_data_len);
	
	if ((offs+8) > cur_data_len){
		if(!fileat(fileat()+offs)){
			Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Reached end of file\n", __func__);
			break;
		}

		if (!fileread(8)){
			break;
		}
		if (cur_data_len ==0){
			 Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Reached end of file\n", __func__);
			 break;
		}
		if (cur_data_len < 8){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Unexpected end of file\n", __func__);
			return false;
		}
	offs=0;
	}

	atomsize=be_val32((uint32_t&)file_buf[offs]);
	if (atomsize==1){
		atom_head=16;
		if (cur_data_len < offs+16){
			if(!fileat(fileat()+offs)||
			   !fileread(offs+64)||cur_data_len < (offs+16))// we want the whole atom header
			{
				Log_Msg(MOVIE_INFO_MSG_ERROR, unexp_msg, __func__);
				return false;
			}
			offs=0;
		}
		atomsize=be_val64((uint64_t&)file_buf[offs+8]);
	}
	else {atom_head=8;}

	if (atomsize >0 && atomsize < atom_head){
		Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is corrupted or not a valid QuickTime format\n", __func__);
		return false;
	}
       
	memcpy(atomname, file_buf+4+offs,4);
        atomname[4]='\0';
#if 0
        Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Found atom '%s' at offs 0x%llx (len: 0x%llx)\n", __func__, atomname, (fileat()+offs), atomsize);
#endif
	if (atomsize==0){
     	Log_Msg(MOVIE_INFO_MSG_INFO, "%s: Zero size atom\n", __func__);
        break;//we've read all
	}


//This prevents probing from taking too long on many files. Downside is you can't retrieve metadata from some
#if 0
	if (bit_cmp(atomname,"mdat", 32)){
		//we got to the raw movie data
		//and we end probing
		//WARNING: metadata may as well come after raw movie data (especially if it is an edited file)
		///but we want this probe to be fast, so we end
		Log_Msg(MOVIE_INFO_MSG_INFO, "%s: We stop at the 'mdat' atom\n", __func__);
		break; //this is not an error
	}
	
	if (atomsize>max_atom_len){
		Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Found an atom which is 0x%llx bytes long: aborting probe\n", __func__, atomsize);
		break;//it's not an error, but we want this to be fast.
	}
#endif
	
	if (bit_cmp(atomname,"mdat", 32)||bit_cmp(atomname,"free", 32)){  //we skip the raw movie data
										//this should make the "stop at mdat" stuff unnecessary
										//and probing fast enough
										
		if(!fileat(fileat()+offs+atomsize)){
			Log_Msg(MOVIE_INFO_MSG_ERROR, unexp_msg, __func__);
		    return false;
		}
		offs=0;
		cur_data_len=0;
		continue;
	}
		
	
	if (cur_data_len < (offs+atomsize)){
		if(!fileat(fileat()+offs)||
		!fileread(atomsize) || (cur_data_len <atomsize)){
		       Log_Msg(MOVIE_INFO_MSG_ERROR, unexp_msg, __func__);
		       return false;
	       }
	       offs=0;
	}
	
	if (bit_cmp(atomname,"ftyp", 32)){
		if (got_ftyp){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Duplicate 'ftyp' atom: aborting\n", __func__);
			return false;
		}
		got_ftyp=true;
		
		
		//we make sure it has at least the so called "major brand" (4 bytes)
		if (atomsize < atom_head+4){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: File is corrupted or not a valid QuickTime format\n", __func__);
			return false;
		}
		//let's see what kind of file type this is....
		for (unsigned int t = 0; t < sizeof(mov_ftyps); t++)
			if (bit_cmp(file_buf+offs+atom_head,mov_ftyps[t][0], 32)){
				container_type=mov_ftyps[t][1];
				//printf("%s\n", container_type.c_str());
				break;
			}
	}
	
	else if (bit_cmp(atomname,"moov", 32)){
		if (got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "moov");
			return false;
		}
		got_moov=true;
		offs+=atom_head;
		continue;
	}

	else if (bit_cmp(atomname,"cmov", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "cmov");
			return false;
		}
		container_type+=" (compressed metadata)";
		Log_Msg(MOVIE_INFO_MSG_WARNING, "%s: Metadata stored in compressed format: aborting\n", __func__);
		break;
	}


	else if (bit_cmp(atomname,"mvhd", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "mvhd");
			return false;
		}
		got_movie_duration=true;
		unsigned int a;
		a=be_val32((uint32_t&)file_buf[offs+atom_head+16]);
		timescale =be_val32((uint32_t&)file_buf[offs+atom_head+12]);
		if (timescale != 0 && a!=0){
			got_movie_duration=true;
			
			movie_duration=((double)a)/timescale;
//			printf ("duration is %d\n", a);
		}
	}
	
	else if (bit_cmp(atomname,"trak", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "trak");
			return false;
		}
		offs+=atom_head;
		tracktype=0;
		continue;
	}

	else if (bit_cmp(atomname,"tkhd", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "tkhd");
			return false;
		}
		tracknum++;
		tracktype=0;
	}
			
	else if (bit_cmp(atomname,"minf", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "minf");
			return false;
		}
		offs+=atom_head;
		continue;
	}
	
	else if (bit_cmp(atomname,"vmhd", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "vmhd");
			return false;
		}
		if (got_video)
			video_info << "\n";
			video_info << "Track " << tracknum;
		tracktype=2;
	}
		
	else if (bit_cmp(atomname,"smhd", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "smhd");
			return false;
		}
		if (got_audio)
			audio_info << "\n";
			audio_info << "Track " << tracknum;
		tracktype=1;
	}
	
	else if (bit_cmp(atomname,"stbl", 32)){
		if (!got_moov){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "stbl");
			return false;
		}
		offs+=atom_head;
		continue;
	}

	else if (bit_cmp(atomname,"stsd", 32)){
		if (!got_moov|| tracknum==0){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "stsd");
			return false;
		}
		//stsd atom is at least 0x10 bytes  in lenght...
		if (atomsize < 0x10 || (offs+0x10) > cur_data_len){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "Illegal 'stsd' atom size\n", __func__);
			return false;
		}
		uint32_t entries_num = be_val32((uint32_t&)file_buf[offs+atom_head+4]);
		if (entries_num < 1){
			Log_Msg(MOVIE_INFO_MSG_WARNING,"%s: Found empty 'stsd' atom\n",  __func__);
			offs+=atomsize;
			continue;
		}
		//the 'stsd' may atom contain multiple entries inside the Sample description table
		//we look at the first one only though
		//I haven't found a file for which this is a problem yet.
		uint64_t offs2=offs+atom_head+0x8;
		char codec[5];
		memcpy(codec, file_buf+offs2+4,4);
		codec[4]='\0';
		uint32_t structsize=0;

		if (tracktype == 1){//audio track
			uint32_t channels=0, samplerate=0, samplesize=0;
			got_audio=true;
			QT_SAMPLE_AUDIO0* sample0 = &(QT_SAMPLE_AUDIO0&)file_buf[offs2];
			
			if (offs2+sizeof(QT_SAMPLE_AUDIO0)>cur_data_len){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
				return false;
			}

			if (be_val16(sample0->version) < 2){
				channels = be_val16(sample0->channels);
				samplerate = (be_val32(sample0->samplerate)>>16);
				samplesize= be_val16(sample0->samplesize);
				if (be_val16(sample0->version)==1)
					structsize=sizeof(QT_SAMPLE_AUDIO1);
				else
					structsize=sizeof(QT_SAMPLE_AUDIO0);
					
			}
			else if (be_val16(sample0->version) == 2){
				if (offs2+sizeof(QT_SAMPLE_AUDIO2)>cur_data_len){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
					return false;
				}
				structsize=sizeof(QT_SAMPLE_AUDIO2);
				QT_SAMPLE_AUDIO2* sample2 = &(QT_SAMPLE_AUDIO2&)file_buf[offs2];
				channels=be_val32(sample2->channels);
				samplerate = (uint32_t) int2dbl(be_val64(sample2->samplerate));
				samplesize = be_val32(sample2->bitsperchannel);
			}
			audio_info << ": type " << codec <<", " << channels  << " channels";
			audio_info << ", " << samplesize << " bits per sample";	
			audio_info << ", " << samplerate << "Hz";
			got_audio_type=true;
			got_audio_channels=true;
			audio_type=codec;
			audio_channels=channels;

			if (bit_cmp(codec,"mp4a", 32) && (offs2+structsize+8 < cur_data_len)){
			
				while (true){
					offs2+=structsize;
#if 0
					char mp4codec[5];
					memcpy(mp4codec, file_buf+offs2+4,4);
					mp4codec[4]='\0';
					Log_Msg(MOVIE_INFO_MSG_INFO, "Got Mp4 box '%s'\n", mp4codec);
#endif			
					if((structsize=be_val32((uint32_t&)file_buf[offs2]))+offs2 > cur_data_len){
						Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
						return false;
					}
				
					if (bit_cmp(file_buf+offs2+4, "esds", 32)){
						QT_SAMPLE_ESD * esd =&(QT_SAMPLE_ESD&)file_buf[offs2];
						
						audio_info << ", bitrate: " << double_to_str(((double)be_val32(esd->avg_bitrate)/1000),0,0) << "kb";
						got_audio_bitrate = true;
						audio_bitrate = double_to_str(((double)be_val32(esd->avg_bitrate)/1000),0,0) + "kb";
						break;
					}
					
					else if(bit_cmp(file_buf+offs2+4, "wave", 32)){
						structsize=8;

					}
					else if ((offs2+structsize+8)>(offs+atomsize))
						break;
					
				}
			}
		}

		else if (tracktype == 2) {//video track
			if (offs2+sizeof(QT_SAMPLE_VIDEO)>cur_data_len){
				Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
				return false;
			}
			got_video=true;
			QT_SAMPLE_VIDEO* sample = &(QT_SAMPLE_VIDEO&)file_buf[offs2];
			uint32_t w=be_val16(sample->width), h=be_val16(sample->height);			
			long double a=(double)w/h;
			video_info << " " << codec << ", aspect: " << w <<"x"<<h <<
				" (" <<double_to_str(a,2,2) << ":1)";
			if (!got_image_size){
				got_image_size=true;
				image_size_w=w;
				image_size_h=h;
				got_aspect_ratio=true;
				aspect_ratio=a;
			}
		}
	}
		
	else if (bit_cmp(atomname,"mdia", 32)){
		if (tracknum==0){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "mdia");
			return false;
		}
		offs+=atom_head;
		continue;
	}
	
	else if (bit_cmp(atomname,"mdhd", 32)){
		if (tracknum==0){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "mdhd");
			return false;
		}
		if (offs+sizeof(QT_MEDIAHEADERATOM)>cur_data_len){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
			return false;
		}
		
		QT_MEDIAHEADERATOM * atom = &(QT_MEDIAHEADERATOM&)file_buf[offs];
		timescale=be_val32(atom->TimeScale);
		if (timescale!=0 && atom->Duration!=0){
			track_duration=((double)be_val32(atom->Duration)/timescale);
			track_duration_track=tracknum;
		}
		else
			track_duration_track=0;
	}
		
	else if (bit_cmp(atomname,"stts", 32)){
		if (tracknum==0){
			Log_Msg(MOVIE_INFO_MSG_ERROR, spurious_msg, __func__, "stts");
			return false;
		}
		if (atomsize < (atom_head+8)||offs+atomsize > cur_data_len){
			Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
			return false;
		}

		if (track_duration_track ==tracknum && tracktype==2){//we have to be sure we have duration details for THIS track
			
			uint32_t maxdura=0;
			uint32_t mindura=(uint32_t)-1;
			uint64_t framescount=0;
			uint64_t framesdura=0;
			QT_TIME_TO_SAMPLE_ENTRY* entry;
			uint64_t start=offs+atom_head+8;
			uint64_t iterations=be_val32((uint32_t&)file_buf[offs+atom_head+4]);
			for (uint64_t t = 0; t < iterations*sizeof(QT_TIME_TO_SAMPLE_ENTRY); t+=sizeof(QT_TIME_TO_SAMPLE_ENTRY)){
				if (start+t+sizeof(QT_TIME_TO_SAMPLE_ENTRY)> cur_data_len){
					Log_Msg(MOVIE_INFO_MSG_ERROR, "%s: Atom is too long\n", __func__);
					return false;
				}
				entry=&(QT_TIME_TO_SAMPLE_ENTRY&)file_buf[start+t];
				framescount+=be_val32(entry->samplecount);
				uint32_t val = be_val32(entry->sampleduration);
				if (val < mindura) mindura=val;
				if (val > maxdura) maxdura=val;
				framesdura+=val;
			}
			if (maxdura==mindura){
				fps=(double)timescale/mindura;
			}
			else {
				fps=((double)framescount/track_duration);
				Log_Msg(MOVIE_INFO_MSG_INFO, "Track with variable frame rate\n", __func__);
			}

			video_info << ", " <<  double_to_str(fps,2) << " fps";			
		}
	}

	offs+=atomsize;
	}
	general_info <<  container_type;
	if (tracknum !=0){
		general_info << " - tracks: " << tracknum;
	}
	if (got_movie_duration){
		general_info << ", duration: " << time_to_str(movie_duration);

	}
	
	if (got_aspect_ratio){
		general_info << ", aspect: " << image_size_w << "x" << image_size_h << " (" <<double_to_str(aspect_ratio,2,2) << ":1)";
	}

	Log_Msg(MOVIE_INFO_MSG_INFO, "General info-> %s\n", general_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Video streams info-> %s\n",video_info.str().c_str());
	Log_Msg(MOVIE_INFO_MSG_INFO, "Audio streams info-> %s\n",audio_info.str().c_str());
	return true;
}

//Matroska stuff
//This is NOT intended to be a real EBML parser
//Some shortcuts have been taken to make it fast
//It only knows of some EBML "atoms" and value types
//needed to retrieve basic movie format data from a given file

int  Movie_info::mkv_vint(uint64_t * vint){
	uint8_t bitmask = 0x7f;
	uint8_t byte;
	if ((byte = readbyte()) == 0)
		return 0;
	int count = 1;
	while ((byte & bitmask) == byte){
		count++;
		bitmask >>=1;
	}
	uint64_t a=(byte & bitmask);
	
	for (int t = 1; t < count; t++)	{
		a <<=8;
		a |= readbyte();
	}
	(*vint) = a;
	return count;
}

int  Movie_info::mkv_vid(uint64_t * vid){
	uint8_t bitmask = 0x7f;
	uint8_t byte; 
	if ((byte = readbyte()) == 0)
		return 0;
	int count = 1;
	while ((byte & bitmask) == byte){
		count++;
		bitmask >>=1;
	}
	uint64_t a = byte;
	for (int t = 1; t < count; t++)	{
		a <<=8;
		a |= readbyte();
	}
	(*vid) = a;
	return count;
}

uint64_t Movie_info::mkv_val(int val_len){
	uint64_t x = 0;
	
	for (int t = 0; t < val_len; t++){
		x <<= 8;
		x |= readbyte();
	}
	
	return x;
}

double Movie_info::mkv_float(int val_len){

	if (val_len > 8)
		return 0.0;
	if (val_len == 4){
		return int2flt(mkv_val(val_len)); 
	}
	else
		return int2dbl(mkv_val(val_len));	 

}



bool Movie_info::mkv_next_track(vector<MKV_ATOM> * atoms, MKV_TRK * track = NULL){
	int atnum;
	int a = 0, b = 0;
	bool looper;
	uint64_t vint = 0;
	uint64_t vid = 0;
	MKV_ATOM  atom;	
redo:
	if (track != 0){
		track->track_number = 0;
		track->track_type = 0;
		track->codec = "";
		track->bits = 0;
		track->channels = 0;
		track->freq = 0;
		track->outputfreq = 0;
		track->x = 0;
		track->y = 0;
		track->duration = 0.;
		track->timecodescale = 0;
	}

	atnum = atoms->size() -1;
	if (atnum > 0){
		fileat(atoms->at(atnum).endoffs);
		cur_data_len=0;
		bufpos = 0;
		atoms->erase(atoms->begin() + atnum);
	}
	looper = true;
	while (looper){
		a = mkv_vid(&vid);
		b = mkv_vint(&vint);

		if (a == 0 || b == 0 || getfilepos() > 6*1024*1024)
			return false;

//				printf("ATOM %llx, Size %llx at %llx\n", vid, vint+a+b, getfilepos()-a-b);

			//let's clean the vector;
			for (vector<MKV_ATOM>::iterator iter = atoms->begin(); iter != atoms->end();iter++){
				if ((iter)->endoffs < getfilepos()){
					atoms->erase(iter);
					iter--;
				}
			}
			atnum = -1 + atoms->size();
				switch (vid) {

				case 0x18538067: //Segment
					atom.vid = vid;
					atom.endoffs = (vint + getfilepos());
					atoms->push_back(atom);
					break;
				case 0x1549A966: //Segment info
					if (atnum < 0 || atoms->at(atnum).vid != 0x18538067)
						return false;
					atom.vid = vid;
					atom.endoffs = (vint + getfilepos());
					atoms->push_back(atom);
					break;
				
				case 0x4489: //segment duration
					if (atnum < 1 || atoms->at(atnum).vid != 0x1549A966)
						return false;
					if (track != NULL) track->duration = mkv_float(vint);
					else skipbytes(vint);
					break;
					
				case 0x2AD7b1://timecode scale
					if (atnum < 1 || atoms->at(atnum).vid != 0x1549A966)
						return false;
					if (track != NULL) track->timecodescale = mkv_val(vint);
					else skipbytes(vint);
					break;

				case 0x1654ae6b: //Track

					if (atnum < 0 || atoms->at(atnum).vid != 0x18538067)
						return false;
					atom.vid = vid;
					atom.endoffs = (vint + getfilepos());
					atoms->push_back(atom);
					break;

				case 0xae: //Track entry
					if (atnum < 1 || atoms->at(atnum).vid != 0x1654ae6b || atoms->at(atnum - 1).vid != 0x18538067)
						return false;
					atom.vid = vid;
					atom.endoffs = (vint + getfilepos());
					atoms->push_back(atom);

					break;
				
				//entries below this line are level 3 stuff
				
				case 0xd7: //Track number
					if (atnum < 2 || atoms->at(atnum).vid !=0xae || atoms->at(atnum - 1).vid != 0x1654ae6b || atoms->at(atnum - 2).vid != 0x18538067)
						return false;

					if (track != NULL) track->track_number = (int) mkv_val(vint);
					else skipbytes(vint);
					break;
					
				case 0x83: //Track type
					if (atnum < 2 || atoms->at(atnum).vid !=0xae || atoms->at(atnum - 1).vid != 0x1654ae6b || atoms->at(atnum - 2).vid != 0x18538067)
						return false;

					if (track != NULL) {
						track->track_type = (int) mkv_val(vint);
					}
					else skipbytes(vint);
					break;
				
				case 0xb9: //is this track enabled?
					if (atnum < 2 || atoms->at(atnum).vid !=0xae || atoms->at(atnum - 1).vid != 0x1654ae6b || atoms->at(atnum - 2).vid != 0x18538067)
						return false;

					if (mkv_val(vint) != 1)
						goto redo;
					break;
				
				case 0x86: //Codec
					if (atnum < 2 || atoms->at(atnum).vid !=0xae || atoms->at(atnum - 1).vid != 0x1654ae6b || atoms->at(atnum - 2).vid != 0x18538067)
						return false;

					if (track != NULL){
						track->codec = "";
						for (unsigned int t = 0; t < vint; t++)
							track->codec += readbyte();
					}
					else skipbytes(vint);
					break;
				
				case 0x23e383:
					if (atnum < 2 || atoms->at(atnum).vid !=0xae || atoms->at(atnum - 1).vid != 0x1654ae6b || atoms->at(atnum - 2).vid != 0x18538067)
						return false;
					if (track != NULL) {
						track->default_dura = (int) mkv_val(vint);
					}
					else skipbytes(vint);
					break;
					
				case 0xe0 : //start of video/audio track specific info...
				case 0xe1 :
					if (atnum < 2 || atoms->at(atnum).vid !=0xae || atoms->at(atnum - 1).vid != 0x1654ae6b || atoms->at(atnum - 2).vid != 0x18538067)
						return false;
					atom.vid = vid;
					atom.endoffs = (vint + getfilepos());
					atoms->push_back(atom);
					looper = false;
					break;

				default:

					skipbytes(vint);
		}
	}

	return true;
	
}


bool Movie_info::probe_mkv(){
	
	int a = 0, b = 0;
	vector<MKV_ATOM>  atoms;
	vector <MKV_TRK>  tracks;
	MKV_TRK track;
	bool looper;
	double segdura = 0.;
	uint64_t segscale = 0;

	while (mkv_next_track(&atoms, &track)){
		looper = true;
		if (atoms.size() > 0){  //we're inside a "video/audio track entry" atom
			int tracktype = atoms.at(atoms.size() - 1).vid;
			
			if (track.duration != 0.) segdura = track.duration;
			else track.duration = segdura;
			
			if (track.timecodescale !=0) segscale = track.timecodescale; 
			else track.timecodescale = segscale;
			
			while (getfilepos() < atoms.at(atoms.size() - 1).endoffs &&  looper){
				uint64_t vint = 0;
				uint64_t vid = 0;
				a = mkv_vid(&vid);
				b = mkv_vint(&vint);
				if (a == 0 || b == 0)
					break;
				if (tracktype == 0xe0) {
					
					switch (vid){
						
					case 0xb0: //PixelWidth 
						track.x = mkv_val(vint);
						break;
					
					case 0xba: //PixelHeight
						track.y = mkv_val(vint);
						break;
					
					default:
						skipbytes(vint);
						
					}
				}

				else {
					
					switch (vid){
					
					case 0x9f: //channels
						track.channels = mkv_val(vint);
						break;
						
					case 0x6264: //Bits per channel
						track.bits = mkv_val(vint);
						break;
						
					case 0xb5: //frequency
						track.freq = mkv_float(vint);
						break;
						
					case 0x78b5://output frequency
						track.outputfreq = mkv_float(vint);
						break;

					default:
						skipbytes(vint);
						
					}
				}
			}
		}
		tracks.push_back(track);
	}

	if (tracks.size() >0 ){
		
		got_container_type=true;
		container_type= "Matroska File";
		general_info <<  container_type <<  " - audio/video tracks: " << tracks.size();
		int t = 1;
		for (vector<MKV_TRK>::iterator iter = tracks.begin(); iter != tracks.end();iter++){
			segdura = iter->duration;
			segscale = iter->timecodescale;
			if (segscale != 0 && segscale != 1000000)
				segdura = segdura/segscale*1000000;
			
			segdura /=1000;
							
			if (!got_movie_duration && segdura != 0.){
				movie_duration = segdura;
				got_movie_duration = true;
			}
				
			if (iter->track_type == 1) {// video
				if (!got_image_size){
					
					got_image_size = true;
					image_size_w = iter->x;
					image_size_h = iter->y;
					got_aspect_ratio = true;
					aspect_ratio=(double)image_size_w / image_size_h;

				}
				else
					video_info << "\n";
				
				video_info << "Track " << t << " out of " << tracks.size() <<": " << iter->codec;
				if (segdura != 0.)
					video_info << ", duration: " << time_to_str(segdura);
			
				video_info << ", aspect: "  << iter->x <<"x" << iter->y <<" (" << double_to_str((double)image_size_w / image_size_h,2,2) << ":1)";
				video_info << ", " << (double) 1000000000 *(1. /iter->default_dura) << " fps";
			}
			
			else if (iter->track_type == 2) {//audio
				if (!got_audio_type){
					got_audio_type = true;
					audio_type = iter->codec;
					got_audio_channels = true;
					audio_channels = iter->channels;
					got_audio_bitrate = true;
					audio_bitrate = double_to_str(iter->freq,2,2);
				}
				else
					audio_info << "\n";

				audio_info << "Track " << t << " out of " << tracks.size() <<": " << iter->codec;
				if (segdura != 0.)
					audio_info << ", duration: " << time_to_str(segdura);
				audio_info <<", channels: " <<  iter->channels <<", " << double_to_str(iter->freq,2,2) <<"Hz";
				if (iter->bits > 0)
				audio_info << ", " << iter->bits << " bits";
					
			}
			t++;	
		}

		Log_Msg(MOVIE_INFO_MSG_INFO, "General info-> %s\n", general_info.str().c_str());
		Log_Msg(MOVIE_INFO_MSG_INFO, "Video streams info-> %s\n",video_info.str().c_str());
		Log_Msg(MOVIE_INFO_MSG_INFO, "Audio streams info-> %s\n",audio_info.str().c_str());
		return true;
	}
	
	return false;

}


int Movie_info::Log_Msg(int msgtype, const char *fmt, ...){
	string str;
	char *p, *np;
	va_list ap;
	int  n;
	size_t size, nextsize;
	size = 32;
	
	if (( p =(char*) malloc(size)) == NULL){
		fprintf(stderr, "%s, Out of memory error in Movie_info::Log_Msg()\n", __func__); //warn user no matter what
		return -1;
	}
	while (true){
		va_start(ap, fmt);
		n = vsnprintf (p, size, fmt, ap);
		va_end(ap);
		if (n == -1){// buffer is too small but we don't know how much we should allocate;
			nextsize = size*2;
		}
		
		else if ((unsigned int) n == size){ //buffer is too small because eol couldn't fit
			nextsize = size+3;
		}
		
		else if ((unsigned int) n == size -1){	//This is ambiguous. It may mean that the output string
			nextsize = size*2;					//was truncated to fit the buffer
		}

		else if ((unsigned int) n > size) {	//this is telling us how much the buffer should be
			nextsize = (unsigned int)n +3;  //3 bytes more just to stay on the safe side
		}

		else {//string fits in the buffer!!!!!!!
			break;
		}
		if ((np = (char*) realloc (p, nextsize)) == NULL) {
			free(p);
			fprintf(stderr, "%s: Out of memory error in Movie_info::Log_Msg()\n",__func__);
			return -1;
		} else {
			size = nextsize;
			p = np;
		}
	}
	if (msgtype == MOVIE_INFO_MSG_ERROR){
		ResetErr();
		last_error << p;
	}
	if (verbosity >= msgtype)
		fprintf(stderr,"Movie_info: %s", p);
	free(p);
	return n;					
}

void Movie_info::ResetErr(){
	last_error.clear();
	last_error.str("");
}
