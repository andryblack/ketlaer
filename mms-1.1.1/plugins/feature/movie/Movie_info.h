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

#ifndef MOVIE_INFO_H_
#define MOVIE_INFO_H_
#define __USE_FILE_OFFSET64
#define __USE_LARGE_FILE
#define _FILE_OFFSET_BITS 64 
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <sys/ioctl.h>
#include <vector>
using namespace std;

/* Some type definitions for older version of gcc */

#ifndef __uint32_t_defined

typedef unsigned int            uint32_t;
# define __uint32_t_defined
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
#if __WORDSIZE == 64
typedef unsigned long int       uint64_t;
#else
__extension__
typedef unsigned long long int  uint64_t;
#endif

#endif //ifndef __uint32_t_defined





#define INITIAL_BUFFERS_SIZE 4096


//Verbosity levels
#define MOVIE_INFO_VERBOSE_NO			0//Nothing is printed to terminal
#define MOVIE_INFO_VERBOSE_ERRS			1//Only errors are printed to terminal
#define MOVIE_INFO_VERBOSE_WARN			2//prints errors and warnings
#define MOVIE_INFO_VERBOSE_ALL			100//Errors, warnings and info are printed to terminal 

//Message types for Log_Msg()
#define MOVIE_INFO_MSG_ERROR			1
#define MOVIE_INFO_MSG_WARNING			2
#define MOVIE_INFO_MSG_INFO				3


//MAGIC VALUES
#define MOVIE_MAGIC_ASF_1	"\x30\x26\xB2\x75\x8E\x66\xCF\x11\xA6\xD9\x00\xAA\x00\x62\xCE\x6C"
#define MOVIE_MAGIC_AVI		"AVI LIST"
#define MOVIE_MAGIC_RIFF	"RIFF"
#define MOVIE_MAGIC_CDXA	"CDXA"

//Quicktime types
//#define MOVIE_MAGIC_MOV_1	"moov"
//#define MOVIE_MAGIC_MOV_2	"mdat"
//#define MOVIE_MAGIC_MOV_3	"ftyp"
//#define MOVIE_MAGIC_MOV_4	"cmov"    //this one is compressed



//other less common types
#define MOVIE_MAGIC_YUV4MPEG	"YUV4MPEG2"


//some matroska structs....

typedef	struct {
	bool got_image_size;
	unsigned int image_size_w;
	unsigned int image_size_h;
	
	bool got_movie_duration;
	double movie_duration; //in seconds
	std::string movie_duration_str;
	
	bool got_aspect_ratio;
	double aspect_ratio;
    std::string resolution;
    std::string aspect_ratio_str;
	
	bool got_container_type;
	std::string container_type;
	
	bool got_audio_type;
	std::string audio_type;
	
	bool got_audio_channels;
	unsigned int audio_channels;
	
	bool got_audio_bitrate;
	std::string audio_bitrate;

	std::string general_info;
	std::string video_info;
	std::string audio_info;
} MOVIE_RESPONSE_INFO;

typedef	struct {
	uint64_t vid;
	uint64_t startoffs;
	uint64_t endoffs;
} MKV_ATOM;

typedef struct	{
	int	track_type; //0 = undefined, 1 = video, 2 = audio, etc... see Matroska specs for a meaning.
	int track_number;
	string codec;
	uint64_t	timecodescale;
	double		duration;
	//for video
	int x;
	int y;
	uint64_t default_dura;
	
	//for audio
	int  channels;
	float freq;
	float outputfreq;
	int bits;

} MKV_TRK;




class Movie_info
{
private:
	bool initialized;
	char *file_buf;
        uint64_t  buf_len;	
	uint64_t cur_data_len; 
	FILE *pFile;
	uint64_t cur_data_offs;
	uint64_t file_len;
	bool got_info;
	uint64_t bufpos; // 
	
	

//private APIs
	bool fileread(uint64_t len);
	uint64_t  fileat ();
	bool fileat (uint64_t pos);
	std::string fourcc_to_str(uint32_t t){ string str; char c[5]; c[3]=(char)((t>>24)&0xff); c[2]=(char)((t>>16)&0xff); c[1]=(char)((t>>8)&0xff); c[0]=(char)(t&0xff); c[4]='\0';  str=c;return str;};
	bool  bit_cmp( char * buf1,  const char  buf2[], unsigned int bits);
	uint8_t  readbyte();
	bool	 readbyte(uint8_t * byte);
	bool	 skipbytes(uint64_t bytes);
	uint64_t getfilepos();

protected:
	bool find_magic(const std::string& file);
	bool probe_mpg(uint32_t startfrom=0, bool skips =false);
	bool probe_cdxa();
	bool probe_id3();
	bool probe_avi();
	bool probe_mov();
	bool probe_asf();
	bool probe_mkv();	
	int  mkv_vint(uint64_t * vint);
	int  mkv_vid(uint64_t * vint);
	uint64_t mkv_val(int val_len);
	double mkv_float(int val_len);
	bool mkv_next_track(vector<MKV_ATOM> * atoms, MKV_TRK * track);

	
	
	std::ostringstream general_info;
	std::ostringstream video_info;
//	std::ostringstream video_aspect;
	std::ostringstream audio_info;
	int Log_Msg(int msgtype, const char *fmt, ...);
	std::ostringstream last_error;
	void ResetErr();
	//FIXME:
	bool got_image_size;
	unsigned int image_size_w;
	unsigned int image_size_h;
	
	bool got_movie_duration;
	double movie_duration; //in seconds
	
	bool got_aspect_ratio;
	double aspect_ratio;

	bool got_container_type;
	std::string container_type;
	
	bool got_audio_type;
	std::string audio_type;
	
	bool got_audio_channels;
	unsigned int audio_channels;
	
	bool got_audio_bitrate;
	std::string audio_bitrate;

public:
	Movie_info();
	virtual ~Movie_info();
	
	bool  probe_movie(const string &file);
	int verbosity;
	void ResetAll();
	bool Fetch_Movie_data(MOVIE_RESPONSE_INFO * ptrMovie_info_data);
	
};




//stuff & structures for various containers

typedef  uint32_t DWORD;
typedef	 uint16_t	WORD;
typedef  uint32_t FOURCC;
typedef  uint64_t QWORD;
typedef  char BYTE;
typedef  uint32_t LONG;


typedef  struct  __attribute__((packed)){
    FOURCC fcc;
    DWORD  cb;
    DWORD  dwMicroSecPerFrame;
    DWORD  dwMaxBytesPerSec;
    DWORD  dwPaddingGranularity;
    DWORD  dwFlags;
    DWORD  dwTotalFrames;
    DWORD  dwInitialFrames;
    DWORD  dwStreams;
    DWORD  dwSuggestedBufferSize;
    DWORD  dwWidth;
    DWORD  dwHeight;
    DWORD  dwReserved[4];
} AVIMAINHEADER;


typedef struct  __attribute__((packed)){
     FOURCC fcc;
     DWORD  cb;
     FOURCC fccType;
     FOURCC fccHandler;
     DWORD  dwFlags;
     WORD   wPriority;
     WORD   wLanguage;
     DWORD  dwInitialFrames;
     DWORD  dwScale;
     DWORD  dwRate;
     DWORD  dwStart;
     DWORD  dwLength;
     DWORD  dwSuggestedBufferSize;
     DWORD  dwQuality;
     DWORD  dwSampleSize;
     struct __attribute__((packed)) {
         short int left;
         short int top;
         short int right;
         short int bottom;
     }  rcFrame;
} AVISTREAMHEADER;


typedef struct __attribute__((packed)) { 
  WORD  wFormatTag; 
  WORD  nChannels; 
  DWORD nSamplesPerSec; 
  DWORD nAvgBytesPerSec; 
  WORD  nBlockAlign; 
  WORD  wBitsPerSample; 
  WORD  cbSize; 
} WAVEFORMATEX; 

typedef struct __attribute__((packed)){
  DWORD  biSize; 
  LONG   biWidth; 
  LONG   biHeight; 
  WORD   biPlanes; 
  WORD   biBitCount; 
  DWORD  biCompression; 
  DWORD  biSizeImage; 
  LONG   biXPelsPerMeter; 
  LONG   biYPelsPerMeter; 
  DWORD  biClrUsed; 
  DWORD  biClrImportant; 
} BITMAPINFOHEADER; 





//AVI stream types
#define AVI_STREAM_VIDS	('v' | 'i' << 8 | 'd' << 16 | 's' << 24)
#define AVI_STREAM_AUDS ('a' | 'u' << 8 | 'd' << 16 | 's' << 24)
#define AVI_STREAM_TXTS ('t' | 'x' << 8 | 't' << 16 | 's' << 24)
#define AVI_STREAM_MIDS ('m' | 'i' << 8 | 'd' << 16 | 's' << 24) 




//ASF GUIDs and structures
typedef struct   __attribute__((packed)){ char v[16];} GUID;



#define ASF_GUID_HEADER  MOVIE_MAGIC_ASF_1
typedef  struct  __attribute__((packed))
{	
	GUID	Object_ID;
	QWORD  	Object_Size;
	DWORD  	Number_of_Header_Objects;
	BYTE	Reserved1 ;
	BYTE 	Reserved2 ;
}ASF_HEADEROBJECT ;

#define ASF_GUID_FILE_PROP      "\xa1\xdc\xab\x8c\x47\xa9\xcf\x11\x8e\xe4\x00\xc0\x0c\x20\x53\x65"
typedef struct __attribute__((packed)){
	GUID	Object_ID;
	QWORD	Object_Size;
	GUID	File_ID;
	QWORD	File_Size;
	QWORD	Creation_Date;
	QWORD	Data_Packets_Count;
	QWORD	Play_Duration;
	QWORD	Send_Duration;
	QWORD	Preroll;
	DWORD	Flags;
	DWORD	Minimum_Data_Packet_Size;
	DWORD	Maximum_Data_Packet_Size;
	DWORD	Maximum_Bitrate;
} ASF_FILEPROPERTIES_OBJECT;


#define ASF_GUID_STREAM_PROP   "\x91\x07\xdc\xb7\xb7\xa9\xcf\x11\x8e\xe6\x00\xc0\x0c\x20\x53\x65"
typedef struct __attribute__((packed)) {
	GUID	Object_ID;
	QWORD	Object_Size;
	GUID	Stream_Type;
	GUID	Error_Correction_Type;
	QWORD	Time_Offset;
	DWORD	Type_specific_Data_Length;
	DWORD	Error_Correction_Data_Length;
	WORD	Flags;
	DWORD	Reserved;
} ASF_STREAM_PROPERTIES_OBJECT;

#define ASF_GUID_STREAM_AUDIO_MEDIA "\x40\x9e\x69\xf8\x4d\x5b\xcf\x11\xa8\xfd\x00\x80\x5f\x5c\x44\x2b"

#define ASF_GUID_STREAM_VIDEO_MEDIA "\xc0\xef\x19\xbc\x4d\x5b\xcf\x11\xa8\xfd\x00\x80\x5f\x5c\x44\x2b"
typedef struct __attribute__((packed)) {
	DWORD	Encoded_Image_Width;
	DWORD	Encoded_Image_Height;
	BYTE	Reserved_Flags;
	WORD	Format_Data_Size;
} ASF_STREAM_VIDEO;

typedef struct __attribute__((packed)) {
	ASF_STREAM_VIDEO obj;
	unsigned int   stream_num;
	unsigned int   type;
	void operator=(const  ASF_STREAM_VIDEO &a) {
		this->obj.Encoded_Image_Width=a.Encoded_Image_Width;
		this->obj.Encoded_Image_Height=a.Encoded_Image_Height;
		this->obj.Reserved_Flags = a.Reserved_Flags;
		this->obj.Format_Data_Size = a.Format_Data_Size;
	};
} ASF_STREAM_V_STORAGE;

typedef struct __attribute__((packed)) {
	WAVEFORMATEX obj;
	unsigned int stream_num;
	void operator=(const  WAVEFORMATEX &a) {
		memcpy (&this->obj, &a, sizeof(WAVEFORMATEX));
	}

} ASF_STREAM_A_STORAGE; 

#define ASF_GUID_HEADER_EXTENSION "\xb5\x03\xbf\x5f\x2e\xa9\xcf\x11\x8e\xe3\x00\xc0\x0c\x20\x53\x65"
typedef struct __attribute__((packed)) {
	GUID	Object_ID;
	QWORD	Object_Size;
	GUID	Reserved_Field_1;
	WORD	Reserved_Field_2;
	DWORD	Header_Extension_Data_Size;
} ASF_HEADER_EXTENSION_OBJECT;


#define ASF_GUID_EXT_STREAM_PROPERTIES "\xcb\xa5\xe6\x14\x72\xc6\x32\x43\x83\x99\xa9\x69\x52\x06\x5b\x5a"
typedef struct __attribute__((packed)) {
	GUID	Object_ID;
	QWORD	Object_Size;
	QWORD	Start_Time;
	QWORD	End_Time;
	DWORD	Data_Bitrate;
	DWORD	Buffer_Size;
	DWORD	Initial_Buffer_Fullness;
	DWORD	Alternate_Data_Bitrate;
	DWORD	Alternate_Buffer_Size;
	DWORD	Alternate_Initial_Buffer_Fullness;
	DWORD	Maximum_Object_Size;
	DWORD	Flags;
	WORD	Stream_Number;
	WORD	Stream_Language_ID_Index;
	QWORD	Average_Time_Per_Frame;
	WORD	Stream_Name_Count;
	WORD	Payload_Extension_System_Count;
} ASF_EXTENDED_STREAM_PROPERTIES;


typedef struct __attribute__((packed)) {
	uint32_t	descsize;       /* total size of SoundDescription
	                                      including extra data */
	FOURCC		dataFormat;     /* sound format */
	int32_t		resvd1;         /* reserved, usually  zero */
	int16_t		resvd2;         /* reserved, usually  zero */
	int16_t		datarefindex;
	uint16_t	version;
	uint16_t	revision;
	uint32_t	vendor;// always 0
	uint16_t	channels; //if (version == 2), this is always 3
	uint16_t	samplesize;//8 or 16 bits; if (version == 2) this is always 16
	uint16_t	compressionid;//0 or 1; if (version == 2) this is always -2
	uint16_t	packetsize;//always 0
	uint32_t	samplerate;//if (version == 2) this is always 65536
} QT_SAMPLE_AUDIO0;

typedef struct __attribute__((packed)) {
	QT_SAMPLE_AUDIO0	base;
	    // fixed compression ratio information
	uint32_t	samplesperpacket;
    uint32_t	bytesperpacket;
	uint32_t	bytesperframe;
    uint32_t	bytespersample;
} QT_SAMPLE_AUDIO1;

typedef struct __attribute__((packed)) {
	QT_SAMPLE_AUDIO0        base;
	uint32_t	sizeofstruct;
	uint64_t	samplerate;//this is actually a 64 bit double precision number
	uint32_t	channels;
	int32_t		always7F000000;
	uint32_t	bitsperchannel;//if sound is compressed this equals to 0
	uint32_t	formatspecificflags;
	uint32_t	bytesperaudiopacket;
	uint32_t	LPCMFramesperaudiopacket;
} QT_SAMPLE_AUDIO2;


typedef struct __attribute__((packed)){
	uint32_t size;
	FOURCC	 name;
	char	 padding[8];
	uint8_t  version;
	uint32_t flags; // 24bit
	uint16_t es_id;
	uint8_t  stream_priority;
	uint8_t  object_type_id;
	uint8_t  stream_type;
	uint32_t buffer_size_db; // 24
	uint32_t max_bitrate;
	uint32_t avg_bitrate;
	uint8_t  decoder_cfg_len;
	uint8_t *decoder_cfg;
	uint8_t  sl_config_len;
	uint8_t *sl_config;
} QT_SAMPLE_ESD;





typedef struct   __attribute__((packed)){ uint8_t len; char v[31];} str31;
typedef struct __attribute__((packed)) {
	uint32_t 	idSize;       	/* total size of this structure */
	FOURCC		cType;        	/* compressor creator type */
	uint32_t	resvd1;       	/* reserved--must be set to 0 */
	uint16_t	resvd2;       	/* reserved--must be set to 0 */
	uint16_t	dataRefIndex; 	/* reserved--must be set to 0 */
	uint16_t	version;      	/* version of compressed data */
	uint16_t	revisionLevel;	/* compressor that created data */
	uint32_t	vendor;       	/* compressor developer that created data */
	uint32_t	temporalQuality;/* degree of temporal compression */ 
	uint32_t	spatialQuality; /* degree of spatial compression */ 
	uint16_t	width;          /* width of source image in pixels */
	uint16_t	height;         /* height of source image in pixels */
	uint32_t	hRes;           /* horizontal resolution of source image */
	uint32_t	vRes;           /* vertical resolution of source image */
	uint32_t	dataSize;       /* size in bytes of compressed data */
	uint16_t	frameCount;     /* number of frames in image data */
	str31		name;           /* name of compression algorithm */
	uint16_t	depth;          /* pixel depth of source image */
	uint16_t	clutID;         /* ID number of the color table for image */
} QT_SAMPLE_VIDEO;

typedef struct __attribute__((packed)){
	uint32_t	AtomSize;        /* Size of this atom in bytes */
	uint32_t	AtomType;        /* Type of atom ('mdhd') */
	uint32_t	Flags;           /* Atom version and flags */
	uint32_t	CreationTime;    /* Time/date atom was created */
	uint32_t	LastModifyTime;  /* Time/date atom was last modified */
	uint32_t	TimeScale;       /* Time scale used for this media */
	uint32_t	Duration;        /* Length of this media */
	uint16_t	Language;        /* Language code for this media */
	uint16_t	Quality;         /* Quality rating for this media */
} QT_MEDIAHEADERATOM;



typedef struct __attribute__((packed)){
	uint32_t	samplecount;
	uint32_t	sampleduration;
} QT_TIME_TO_SAMPLE_ENTRY;

/***************************************************************************
 * MPEG stuff and structures below					   *
 ***************************************************************************/

const double mpg_aspect_ratio[] = {
	0, 		//invalid
	(double)1.0,	// i.e. square
	(double)4/3,		//as the value says
	(double)16/9,		//as the value says
	(double)2.21,
	0,		//invalid
	0,		//invalid
	0,		//invalid
	0,		//invalid
	0,      	//invalid
	0,      	//invalid
	0,      	//invalid
	0,      	//invalid
	0,      	//invalid
	0,      	//invalid
};

const double mpg_frame_rate[] = {
	0,		//invalid
	(double)24000/1001,	//cinema?
	24,		//cinema
	25,		//PAL 
	(double)30000/1001,	//real NTSC
	30,		//NTSC
	50,		//PAL (interlaced?)
	(double)60000/1001,	//real NTSC (interlaced?)
	60,		//NTSC (interlaced?)
	0,		//invalid
	0,		//invalid
	0,		//invalid
	0,		//invalid
	0,		//invalid
	0,		//invalid
	0		//invalid
};
 
const uint16_t  mpg_audio_bitrate [][5]= {
// V1,L1	V1,L2	V1,L3	V2,L1	V2,L2 & L3	
	{0,		0,		0,		0,		0},
	{32,	32,		32,		32,		8},
	{64,	48,		40,		48,		16},
	{96,	56,		48,		56,		24},
	{128,	64,		56,		64,		32},
	{160,	80,		64,		80,		40},
	{192,	96,		80,		96,		48},
	{224,	112,	96,		112,	56},
	{256,	128,	112,	128,	64},
	{288,	160,	128,	144,	80},
	{320,	192,	160,	160,	96},
	{352,	224,	192,	176,	112},
	{384,	256,	224,	192,	128},
	{416,	320,	256,	224,	144},
	{448,	384,	320,	256,	160},
	{0,		0,		0,		0,		0}
};

const uint32_t mpg_audio_samplerate[][3]={
	{44100,	22050,	11025},
	{48000,	24000,	12000},
	{32000,	16000,	8000},
	{0,		0,		0}
};	
	
const uint16_t mpg_ac3audio_samplerate[]={// Hz
	48000,
	44100,
	32000,
	0
};
	
const uint16_t mpg_ac3audio_bitrate[]={// Kbps
	32,
	32,
	40,
	40,
	48,
	48,
	56,
	56,
	64,
	64,
	80,
	80,
	96,
	96,
	112,
	112,
	128,
	128,
	160,
	160,
	192,
	192,
	224,
	224,
	256,
	256,
	320,
	320,
	384,
	384,
	448,
	448,
	512,
	512,
	576,
	576,
	640,
	640
};

	

#endif /*MOVIE_INFO_H_*/
