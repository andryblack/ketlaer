/*system/Include/Platform_Lib/Graphics/StreamingEngine/se_export.h*/
class se_cmd_counter_t {
public:
  u_int32_t low;
  u_int32_t high;
};

typedef se_cmd_counter_t se_cmd_counter;

class ioc_dcu_info_t {
public:
  u_int32_t index;
  u_int32_t addr;
  u_int32_t pitch;
};

typedef ioc_dcu_info_t ioc_dcu_info;

class vsync_queue_param {
public:
  uint32_t u32_max_queue_num;
  uint32_t vsync_queue_phy;
};

typedef vsync_queue_param vsync_queue_param_t;


/*system/Include/Platform_Lib/Graphics/se.h*/
typedef uint64_t SE_CMD_HANDLE;


/*system/Include/Platform_Lib/Graphics/StreamingEngine/SeLib.h*/
typedef int SEINFO_CMDQUEUE;

enum _SEINFO_DMAINFO {SEINFO_DMA_CHANNEL0, SEINFO_DMA_CHANNEL1, SEINFO_DMA_CHANNEL2, SEINFO_DMA_CHANNEL3};

typedef _SEINFO_DMAINFO SEINFO_DMAINFO;

enum _SEINFO_CLIPPINGINFO {SEINFO_CLIPPING_CHANNEL0, SEINFO_CLIPPING_CHANNEL1, SEINFO_CLIPPING_CHANNEL2, SEINFO_CLIPPING_CHANNEL3};

typedef _SEINFO_CLIPPINGINFO SEINFO_CLIPPINGINFO;

enum _SEINFO_REGMODE {SEINFO_REGMODE_REGISTER, SEINFO_REGMODE_CLUT};

typedef _SEINFO_REGMODE SEINFO_REGMODE;

enum _SEINFO_WRITEREG {SEINFO_REG_COLOR_FORMAT = 64, SEINFO_REG_COLOR_KEY = 76, SEINFO_REG_SRC_COLOR = 88, SEINFO_REG_SRC_COLOR2 = 100, SEINFO_REG_SRC_ALPHA = 112, SEINFO_REG_SRC_ALPHA2 = 1232, SEINFO_REG_DEST_ALPHA = 124, SEINFO_REG_RESULT_ALPHA = 136, SEINFO_REG_BASE_ADDR0 = 148, SEINFO_REG_BASE_ADDR1 = 152, SEINFO_REG_BASE_ADDR2 = 156, SEINFO_REG_BASE_ADDR3 = 160, SEINFO_REG_PITCH0 = 164, SEINFO_REG_PITCH1 = 168, SEINFO_REG_PITCH2 = 172, SEINFO_REG_PITCH3 = 176, SEINFO_REG_FELICS_ADDR0 = 244, SEINFO_REG_FELICS_ADDR1 = 248, SEINFO_REG_FELICS_ADDR2 = 252, SEINFO_REG_FELICS_ADDR3 = 256, SEINFO_REG_CLIP_X0 = 296, SEINFO_REG_CLIP_X1 = 300, SEINFO_REG_CLIP_X2 = 304, SEINFO_REG_CLIP_X3 = 308, SEINFO_REG_CLIP_Y0 = 312, SEINFO_REG_CLIP_Y1 = 316, SEINFO_REG_CLIP_Y2 = 320, SEINFO_REG_CLIP_Y3 = 324, SEINFO_REG_CONV_SCALING = 392, SEINFO_REG_CONV_COEF1 = 404, SEINFO_REG_CONV_COEF2 = 416, SEINFO_REG_CONV_COEF3 = 428, SEINFO_REG_CONV_COEF4 = 440, SEINFO_REG_CONV_COEF5 = 452, SEINFO_REG_CONV_COEF6 = 464, SEINFO_REG_STRETCH = 476, SEINFO_REG_HDHS = 488, SEINFO_REG_VDHS = 500, SEINFO_REG_HCOEF0 = 512, SEINFO_REG_HCOEF1 = 516, SEINFO_REG_HCOEF2 = 520, SEINFO_REG_HCOEF3 = 524, SEINFO_REG_HCOEF4 = 528, SEINFO_REG_HCOEF5 = 532, SEINFO_REG_HCOEF6 = 536, SEINFO_REG_HCOEF7 = 540, SEINFO_REG_HCOEF8 = 544, SEINFO_REG_HCOEF9 = 548, SEINFO_REG_HCOEF10 = 552, SEINFO_REG_HCOEF11 = 556, SEINFO_REG_HCOEF12 = 560, SEINFO_REG_HCOEF13 = 564, SEINFO_REG_HCOEF14 = 568, SEINFO_REG_HCOEF15 = 572, SEINFO_REG_HCOEF16 = 576, SEINFO_REG_HCOEF17 = 580, SEINFO_REG_HCOEF18 = 584, SEINFO_REG_HCOEF19 = 588, SEINFO_REG_HCOEF20 = 592, SEINFO_REG_HCOEF21 = 596, SEINFO_REG_HCOEF22 = 600, SEINFO_REG_HCOEF23 = 604, SEINFO_REG_HCOEF24 = 608, SEINFO_REG_HCOEF25 = 612, SEINFO_REG_HCOEF26 = 616, SEINFO_REG_HCOEF27 = 620, SEINFO_REG_HCOEF28 = 624, SEINFO_REG_HCOEF29 = 628, SEINFO_REG_HCOEF30 = 632, SEINFO_REG_HCOEF31 = 636, SEINFO_REG_VCOEF0 = 640, SEINFO_REG_VCOEF1 = 644, SEINFO_REG_VCOEF2 = 648, SEINFO_REG_VCOEF3 = 652, SEINFO_REG_VCOEF4 = 656, SEINFO_REG_VCOEF5 = 660, SEINFO_REG_VCOEF6 = 664, SEINFO_REG_VCOEF7 = 668, SEINFO_REG_VCOEF8 = 672, SEINFO_REG_VCOEF9 = 676, SEINFO_REG_VCOEF10 = 680, SEINFO_REG_VCOEF11 = 684, SEINFO_REG_VCOEF12 = 688, SEINFO_REG_VCOEF13 = 692, SEINFO_REG_VCOEF14 = 696, SEINFO_REG_VCOEF15 = 700, SEINFO_REG_2TO4_MAP = 1088, SEINFO_REG_2TO8_MAP = 1092, SEINFO_REG_4TO8_MAP0 = 1096, SEINFO_REG_4TO8_MAP1 = 1100, SEINFO_REG_4TO8_MAP2 = 1104, SEINFO_REG_4TO8_MAP3 = 1108, SEINFO_REG_FELICS_ABORT = 1100, SEINFO_REG_INT_STATUS = 1140};

typedef _SEINFO_WRITEREG SEINFO_WRITEREG;

enum _SEINFO_ROPCODE {SEINFO_ROPCODE_AND, SEINFO_ROPCODE_OR, SEINFO_ROPCODE_XOR, SEINFO_ROPCODE_COPY, SEINFO_ROPCODE_NONE};

typedef _SEINFO_ROPCODE SEINFO_ROPCODE;

enum _SEINFO_MIRRORMODE {SEINFO_MIRROR_X = 1, SEINFO_MIRROR_Y, SEINFO_MIRROR_XY};

typedef _SEINFO_MIRRORMODE SEINFO_MIRRORMODE;

enum _SEINFO_CLUT {SEINFO_FETCH_CLUT_4ARGB, SEINFO_FETCH_CLUT_16ARGB, SEINFO_FETCH_CLUT_256ARGB};

typedef _SEINFO_CLUT SEINFO_CLUT;

enum _SEINFO_SETUPMODE {SEINFO_BITBLT_SETUP_CONST, SEINFO_BITBLT_SETUP_MANUAL};

typedef _SEINFO_SETUPMODE SEINFO_SETUPMODE;

enum _SEINFO_ALPHAPROC_EN {SEINFO_ALPHAPROC_ENABLE = 1, SEINFO_ALPHAPROC_DISABLE = 0};

typedef _SEINFO_ALPHAPROC_EN SEINFO_ALPHAPROC_EN;

enum _SEINFO_CLIPPING_EN {SEINFO_CLIPPING_ENABLE = 1, SEINFO_CLIPPING_DISABLE = 0};

typedef _SEINFO_CLIPPING_EN SEINFO_CLIPPING_EN;

enum _SEINFO_CONVERT_YUV_EN {SEINFO_CONVERT_YUV_ENABLE = 1, SEINFO_CONVERT_YUV_DISABLE = 0};

typedef _SEINFO_CONVERT_YUV_EN SEINFO_CONVERT_YUV_EN;

enum _SEINFO_FELICS_ALPHA_EN {SEINFO_FELICS_ALPHA_ENABLE, SEINFO_FELICS_ALPHA_DISABLE};

typedef _SEINFO_FELICS_ALPHA_EN SEINFO_FELICS_ALPHA_EN;

enum _SEINFO_COMPRESSMODE {SEINFO_DECOMPRESS, SEINFO_COMPRESS};

typedef _SEINFO_COMPRESSMODE SEINFO_COMPRESSMODE;

enum _SEINFO_BITMAP_SRC {SEINFO_BITMAP_DRAM, SEINFO_BITMAP_FLASH};

typedef _SEINFO_BITMAP_SRC SEINFO_BITMAP_SRC;

enum _SEINFO_BITMAP_DIR {SEINFO_BITMAP_DIR_INC, SEINFO_BITMAP_DIR_DEC};

typedef _SEINFO_BITMAP_DIR SEINFO_BITMAP_DIR;

enum _SEINFO_BGCOLOR_EN {SEINFO_BGCOLOR_ENABLE = 1, SEINFO_BGCOLOR_DISABLE = 0};

typedef _SEINFO_BGCOLOR_EN SEINFO_BGCOLOR_EN;

enum _SEINFO_FGCOLOR_EN {SEINFO_FGCOLOR_ENABLE = 1, SEINFO_FGCOLOR_DISABLE = 0};

typedef _SEINFO_FGCOLOR_EN SEINFO_FGCOLOR_EN;

enum _SEINFO_BITMAP_32BIT_EN {SEINFO_BITMAP_32BIT_ENABLE = 1, SEINFO_BITMAP_32BIT_DISABLE = 0};

typedef _SEINFO_BITMAP_32BIT_EN SEINFO_BITMAP_32BIT_EN;

enum _SEINFO_NO_COLORMODIFY_EN {SEINFO_NO_COLORMODIFY_ENABLE = 1, SEINFO_NO_COLORMODIFY_DISABLE = 0};

typedef _SEINFO_NO_COLORMODIFY_EN SEINFO_NO_COLORMODIFY_EN;

enum _SEINFO_BITMAP_ENDIAN {SEINFO_BITMAP_BIG_ENDIAN, SEINFO_BITMAP_LITTLE_ENDIAN};

typedef _SEINFO_BITMAP_ENDIAN SEINFO_BITMAP_ENDIAN;

enum _SEINFO_CORNER {SEINFO_BITBLT_CORNER_TOP_LEFT, SEINFO_BITBLT_CORNER_TOP_RIGHT, SEINFO_BITBLT_CORNER_BOTTOM_LEFT, SEINFO_BITBLT_CORNER_BOTTOM_RIGHT, SEINFO_BITBLT_CORNER_NONE};

typedef _SEINFO_CORNER SEINFO_CORNER;

enum _SEINFO_ALPHAMODE {SEINFO_BITBLT_ALPHA_CONST, SEINFO_BITBLT_ALPHA_1_CONST, SEINFO_BITBLT_ALPHA_DEST, SEINFO_BITBLT_ALPHA_1_DEST, SEINFO_BITBLT_ALPHA_SRC, SEINFO_BITBLT_ALPHA_1_SRC, SEINFO_BITBLT_ALPHA_DISABLE};

typedef _SEINFO_ALPHAMODE SEINFO_ALPHAMODE;

enum _SEINFO_OUTPUT_ALPHAMODE {SEINFO_OUTPUT_ALPHA_NORMAL, SEINFO_OUTPUT_ALPHA_CONST};

typedef _SEINFO_OUTPUT_ALPHAMODE SEINFO_OUTPUT_ALPHAMODE;

enum _SEINFO_STRETCH {SEINFO_STRETCH_FIR = 1, SEINFO_STRETCH_REPEAT_DROP, SEINFO_STRETCH_DIRECTIONAL};

typedef _SEINFO_STRETCH SEINFO_STRETCH;

enum _SEINFO_SACLING_TYPE {SEINFO_SCALING_UP, SEINFO_SCALING_DOWN, SEINFO_SCALING_DISABLE};

typedef _SEINFO_SACLING_TYPE SEINFO_SACLING_TYPE;

enum _SEINFO_STRETCH_HTAP {SEINFO_HSCALING_2TAP, SEINFO_HSCALING_4TAP, SEINFO_HSCALING_8TAP};

typedef _SEINFO_STRETCH_HTAP SEINFO_STRETCH_HTAP;

enum _SEINFO_STRETCH_VTAP {SEINFO_VSCALING_2TAP, SEINFO_VSCALING_4TAP};

typedef _SEINFO_STRETCH_VTAP SEINFO_STRETCH_VTAP;

enum _SEINFO_COLORKEY {SEINFO_COLORKEY_NONE, SEINFO_COLORKEY_SOURCE, SEINFO_COLORKEY_DESTINATION};

typedef _SEINFO_COLORKEY SEINFO_COLORKEY;

enum _SEINFO_CONVERSION {SEINFO_CONVERSION_INDEX_ARGB, SEINFO_CONVERSION_ARGB_ARGB, SEINFO_CONVERSION_YUV_ARGB, SEINFO_CONVERSION_ARGB_YUV};

typedef _SEINFO_CONVERSION SEINFO_CONVERSION;

enum _SEINFO_COLOR_FORMAT {SEINFO_COLOR_FORMAT_INDEX2, SEINFO_COLOR_FORMAT_INDEX4, SEINFO_COLOR_FORMAT_INDEX8, SEINFO_COLOR_FORMAT_RGB332, SEINFO_COLOR_FORMAT_RGB565, SEINFO_COLOR_FORMAT_ARGB1555, SEINFO_COLOR_FORMAT_ARGB4444, SEINFO_COLOR_FORMAT_ARGB8888, SEINFO_COLOR_FORMAT_Y, SEINFO_COLOR_FORMAT_UV, SEINFO_COLOR_FORMAT_YUV420, SEINFO_COLOR_FORMAT_YUV422, SEINFO_COLOR_FORMAT_YUV444};

typedef _SEINFO_COLOR_FORMAT SEINFO_COLOR_FORMAT;

enum _SEINFO_ARGB_ALPHA_LOCATION {SEINFO_ARGB_ALPHA_LEFTMOST, SEINFO_ARGB_ALPHA_RIGHTMOST, SEINFO_ARGB_ALPHA_NONE};

typedef _SEINFO_ARGB_ALPHA_LOCATION SEINFO_ARGB_ALPHA_LOCATION;

enum _SEINFO_RGB_LOCATION {SEINFO_RGB_LOCATION_LEFTMOST, SEINFO_RGB_LOCATION_MIDDLE, SEINFO_RGB_LOCATION_RIGHTMOST, SEINFO_RGB_LOCATION_NONE};

typedef _SEINFO_RGB_LOCATION SEINFO_RGB_LOCATION;

enum _SEINFO_SRC_FORMAT {SEINFO_SRC_FORMAT_INDEX2, SEINFO_SRC_FORMAT_INDEX4, SEINFO_SRC_FORMAT_INDEX8, SEINFO_SRC_FORMAT_RGB332 = 0, SEINFO_SRC_FORMAT_RGB565, SEINFO_SRC_FORMAT_ARGB1555, SEINFO_SRC_FORMAT_ARGB4444, SEINFO_SRC_FORMAT_ARGB8888, SEINFO_SRC_FORMAT_YUV420 = 0, SEINFO_SRC_FORMAT_YUV422, SEINFO_SRC_FORMAT_YUV444};

typedef _SEINFO_SRC_FORMAT SEINFO_SRC_FORMAT;

enum _SEINFO_DEST_FORMAT {SEINFO_DEST_FORMAT_RGB332, SEINFO_DEST_FORMAT_RGB565, SEINFO_DEST_FORMAT_ARGB1555, SEINFO_DEST_FORMAT_ARGB4444, SEINFO_DEST_FORMAT_ARGB8888, SEINFO_DEST_FORMAT_YUV420 = 0, SEINFO_DEST_FORMAT_YUV422, SEINFO_DEST_FORMAT_YUV444};

typedef _SEINFO_DEST_FORMAT SEINFO_DEST_FORMAT;

enum _SEINFO_GRADIENT {SEINFO_GRADIENT_LEFT_TO_RIGHT, SEINFO_GRADIENT_TOP_TO_BOTTOM, SEINFO_GRADIENT_TOP_LEFT_TO_BOTTOM_RIGHT, SEINFO_GRADIENT_TOP_RIGHT_TO_BOTTOM_LEFT, SEINFO_GRADIENT_DISABLE};

typedef _SEINFO_GRADIENT SEINFO_GRADIENT;

enum _SEINFO_INTERLEAVING_MODE {SEINFO_INTERLEAVING_MODE_ENABLE = 1, SEINFO_INTERLEAVING_MODE_DISABLE = 0};

typedef _SEINFO_INTERLEAVING_MODE SEINFO_INTERLEAVING_MODE;

class _SE_FELICS_HEADER {
public:
  uint32_t Word1;
  union {
    uint32_t Value;
      class {
    public:
      uint32_t BitStreamLength;
      uint32_t Abort;
      uint32_t Reserved2;
    } Fields;
  } Word2;
};

typedef _SE_FELICS_HEADER SE_FELICS_HEADER;

class _SE_PROCINFO {
public:
  uint16_t wDataX;
  uint16_t wDataY;
  uint16_t wDataHeight;
  uint16_t wDataWidth;
  uint8_t byCommandQueueSelect;
  bool bInterruptService;
  uint8_t byBaseAddressIndex;
  uint8_t byChronmaAddressIndex;
  bool bInterleavingModeEnable;
};

typedef _SE_PROCINFO SE_PROCINFO;


/*system/Include/Platform_Lib/Graphics/StreamingEngine/SeReg.h*/
union _SEREG_CMDBASE {
  uint32_t Value;
  class {
  public:
    uint32_t Reserved0;
    uint32_t base;
    uint32_t Reserved1;
  } Fields;
};

typedef _SEREG_CMDBASE SEREG_CMDBASE;

union _SEREG_CMDLMT {
  uint32_t Value;
  class {
  public:
    uint32_t Reserved0;
    uint32_t limit;
    uint32_t Reserved1;
  } Fields;
};

typedef _SEREG_CMDLMT SEREG_CMDLMT;

union _SEREG_CMDRPTR {
  uint32_t Value;
  class {
  public:
    uint32_t Reserved0;
    uint32_t rptr;
    uint32_t Reserved1;
  } Fields;
};

typedef _SEREG_CMDRPTR SEREG_CMDRPTR;

union _SEREG_CMDWPTR {
  uint32_t Value;
  class {
  public:
    uint32_t Reserved0;
    uint32_t wptr;
    uint32_t Reserved1;
  } Fields;
};

typedef _SEREG_CMDWPTR SEREG_CMDWPTR;

union _SEREG_SRWORDCNT {
  uint32_t Value;
  class {
  public:
    uint32_t srwordcnt;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_SRWORDCNT SEREG_SRWORDCNT;

union _SEREG_Q_PRIORITY {
  uint32_t Value;
  class {
  public:
    uint32_t pri1;
    uint32_t pri2;
    uint32_t pri3;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_Q_PRIORITY SEREG_Q_PRIORITY;

union _SEREG_CLR_FMT {
  uint32_t Value;
  class {
  public:
    uint32_t Format;
    uint32_t write_enable1;
    uint32_t alpha_loc;
    uint32_t write_enable2;
    uint32_t clr_loc2;
    uint32_t write_enable3;
    uint32_t clr_loc1;
    uint32_t write_enable4;
    uint32_t alpha_loc2;
    uint32_t write_enable5;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CLR_FMT SEREG_CLR_FMT;

union _SEREG_COLOR_KEY {
  uint32_t Value;
  class {
  public:
    uint32_t color_key;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_COLOR_KEY SEREG_COLOR_KEY;

union _SEREG_SRC_COLOR {
  uint32_t Value;
  class {
  public:
    uint32_t src_color;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_SRC_COLOR SEREG_SRC_COLOR;

union _SEREG_SRC_COLOR2 {
  uint32_t Value;
  class {
  public:
    uint32_t src_color2;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_SRC_COLOR2 SEREG_SRC_COLOR2;

union _SEREG_SRC_ALPHA {
  uint32_t Value;
  class {
  public:
    uint32_t src_alpha;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_SRC_ALPHA SEREG_SRC_ALPHA;

union _SEREG_SRC_ALPHA2 {
  uint32_t Value;
  class {
  public:
    uint32_t src_alpha2;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_SRC_ALPHA2 SEREG_SRC_ALPHA2;

union _SEREG_DEST_ALPHA {
  uint32_t Value;
  class {
  public:
    uint32_t dest_alpha;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_DEST_ALPHA SEREG_DEST_ALPHA;

union _SEREG_RSLT_ALPHA {
  uint32_t Value;
  class {
  public:
    uint32_t rslt_alpha;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_RSLT_ALPHA SEREG_RSLT_ALPHA;

union _SEREG_BADDR {
  uint32_t Value;
  class {
  public:
    uint32_t baddr;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_BADDR SEREG_BADDR;

union _SEREG_PITCH {
  uint32_t Value;
  class {
  public:
    uint32_t pitch;
    uint32_t interleave;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_PITCH SEREG_PITCH;

class _SEREG_DMAINFO {
public:
  SEREG_BADDR SeRegBaseAddress[4];
  SEREG_PITCH SeRegPitch[4];
};

typedef _SEREG_DMAINFO SEREG_DMAINFO;

union _SEREG_FADDR {
  uint32_t Value;
  class {
  public:
    uint32_t faddr;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_FADDR SEREG_FADDR;

union _SEREG_BSTLEN {
  uint32_t Value;
  class {
  public:
    uint32_t bstlen;
    uint32_t pkgen;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_BSTLEN SEREG_BSTLEN;

union _SEREG_CLIP_XCOOR {
  uint32_t Value;
  class {
  public:
    uint32_t xmax;
    uint32_t xmin;
    uint32_t clipen;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CLIP_XCOOR SEREG_CLIP_XCOOR;

union _SEREG_CLIP_YCOOR {
  uint32_t Value;
  class {
  public:
    uint32_t ymax;
    uint32_t ymin;
    uint32_t clipen;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CLIP_YCOOR SEREG_CLIP_YCOOR;

class _SEREG_CLIP_INFO {
public:
  SEREG_CLIP_XCOOR SeRegClipXCoor[4];
  SEREG_CLIP_YCOOR SeRegClipYCoor[4];
};

typedef _SEREG_CLIP_INFO SEREG_CLIP_INFO;

union _SEREG_CONVSCALING {
  uint32_t Value;
  class {
  public:
    uint32_t vup;
    uint32_t hup;
    uint32_t vdown;
    uint32_t hdown;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVSCALING SEREG_CONVSCALING;

union _SEREG_CONVCOEF1 {
  uint32_t Value;
  class {
  public:
    uint32_t m00;
    uint32_t m01;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVCOEF1 SEREG_CONVCOEF1;

union _SEREG_CONVCOEF2 {
  uint32_t Value;
  class {
  public:
    uint32_t m02;
    uint32_t m10;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVCOEF2 SEREG_CONVCOEF2;

union _SEREG_CONVCOEF3 {
  uint32_t Value;
  class {
  public:
    uint32_t m11;
    uint32_t m12;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVCOEF3 SEREG_CONVCOEF3;

union _SEREG_CONVCOEF4 {
  uint32_t Value;
  class {
  public:
    uint32_t m20;
    uint32_t m21;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVCOEF4 SEREG_CONVCOEF4;

union _SEREG_CONVCOEF5 {
  uint32_t Value;
  class {
  public:
    uint32_t m22;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVCOEF5 SEREG_CONVCOEF5;

union _SEREG_CONVCOEF6 {
  uint32_t Value;
  class {
  public:
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CONVCOEF6 SEREG_CONVCOEF6;

union _SEREG_STRETCH {
  uint32_t Value;
  class {
  public:
    uint32_t vodd;
    uint32_t hodd;
    uint32_t vtype;
    uint32_t htype;
    uint32_t ref_alpha;
    uint32_t ds_thx;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_STRETCH SEREG_STRETCH;

union _SEREG_HDHS {
  uint32_t Value;
  class {
  public:
    uint32_t H_scaling_dph_lsb;
    uint32_t H_scaling_dph_msb;
    uint32_t H_scaling_iniph;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_HDHS SEREG_HDHS;

union _SEREG_VDHS {
  uint32_t Value;
  class {
  public:
    uint32_t V_scaling_dph_lsb;
    uint32_t V_scaling_dph_msb;
    uint32_t V_scaling_iniph;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_VDHS SEREG_VDHS;

union _SEREG_HCOEF {
  uint32_t Value;
  class {
  public:
    uint32_t hcoef;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_HCOEF SEREG_HCOEF;

union _SEREG_VCOEF {
  uint32_t Value;
  class {
  public:
    uint32_t vcoef;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_VCOEF SEREG_VCOEF;

class _SEREG_COEFINFO {
public:
  SEREG_HCOEF SeRegHCoef[32];
  SEREG_VCOEF SeRegVCoef[16];
};

typedef _SEREG_COEFINFO SEREG_COEFINFO;

union _SEREG_2TO4_MAP {
  uint32_t Value;
  class {
  public:
    uint32_t map;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_2TO4_MAP SEREG_2TO4_MAP;

union _SEREG_2TO8_MAP {
  uint32_t Value;
  class {
  public:
    uint32_t map;
  } Fields;
};

typedef _SEREG_2TO8_MAP SEREG_2TO8_MAP;

union _SEREG_4TO8_MAP {
  uint32_t Value;
  class {
  public:
    uint32_t map;
  } Fields;
};

typedef _SEREG_4TO8_MAP SEREG_4TO8_MAP;

union _SEREG_FELICS_ABORT {
  uint32_t Value;
  class {
  public:
    uint32_t threshold;
    uint32_t Enable;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_FELICS_ABORT SEREG_FELICS_ABORT;

union _SEREG_CTRL {
  uint32_t Value;
  class {
  public:
    uint32_t write_data;
    uint32_t go;
    uint32_t endiswap;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CTRL SEREG_CTRL;

union _SEREG_IDLE {
  uint32_t Value;
  class {
  public:
    uint32_t idle;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_IDLE SEREG_IDLE;

union _SEREG_INTSEL {
  uint32_t Value;
  class {
  public:
    uint32_t Sel1;
    uint32_t Sel2;
    uint32_t Sel3;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_INTSEL SEREG_INTSEL;

union _SEREG_INTS {
  uint32_t Value;
  class {
  public:
    uint32_t write_data;
    uint32_t sync;
    uint32_t com_err;
    uint32_t com_empty;
    uint32_t dvb_err_t;
    uint32_t dvb_err_s;
    uint32_t dvb_err_m;
    uint32_t dvb_err_e;
    uint32_t bdrle_rlerr;
    uint32_t bdrle_long_h;
    uint32_t bdrle_long_w;
    uint32_t bdrle_isbl;
    uint32_t Felics_err;
    uint32_t Felics_long;
    uint32_t Felics_short;
    uint32_t Felics_range;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_INTS SEREG_INTS;

union _SEREG_INTE {
  uint32_t Value;
  class {
  public:
    uint32_t write_data;
    uint32_t sync;
    uint32_t com_err;
    uint32_t com_empty;
    uint32_t dvb_err_t;
    uint32_t dvb_err_s;
    uint32_t dvb_err_m;
    uint32_t dvb_err_e;
    uint32_t bdrle_rlerr;
    uint32_t bdrle_long_h;
    uint32_t bdrle_long_w;
    uint32_t bdrle_isbl;
    uint32_t Felics_err;
    uint32_t Felics_long;
    uint32_t Felics_short;
    uint32_t Felics_range;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_INTE SEREG_INTE;

union _SEREG_INSTCNT {
  uint32_t Value;
  class {
  public:
    uint32_t instcnt;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_INSTCNT SEREG_INSTCNT;

union _SEREG_CLUT_RADR {
  uint32_t Value;
  class {
  public:
    uint32_t radr;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_CLUT_RADR SEREG_CLUT_RADR;

union _SEREG_CLUT_VALUE {
  uint32_t Value;
  class {
  public:
    uint32_t value;
  } Fields;
};

typedef _SEREG_CLUT_VALUE SEREG_CLUT_VALUE;

union _SEREG_DBG {
  uint32_t Value;
  class {
  public:
    uint32_t dbg_en;
    uint32_t dbg_sel0;
    uint32_t dbg_sel1;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_DBG SEREG_DBG;

union _SEREG_SCTCH {
  uint32_t Value;
  class {
  public:
    uint32_t Reg0;
    uint32_t Reg1;
  } Fields;
};

typedef _SEREG_SCTCH SEREG_SCTCH;

union _SEREG_GCRA_CTRL {
  uint32_t Value;
  class {
  public:
    uint32_t Gcra_en;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_GCRA_CTRL SEREG_GCRA_CTRL;

union _SEREG_GCRA_REQ_CNT {
  uint32_t Value;
  class {
  public:
    uint32_t Gcra_req_num;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_GCRA_REQ_CNT SEREG_GCRA_REQ_CNT;

union _SEREG_GCRA_TIM_CNT {
  uint32_t Value;
  class {
  public:
    uint32_t Gcra_tim_cnt;
  } Fields;
};

typedef _SEREG_GCRA_TIM_CNT SEREG_GCRA_TIM_CNT;

union _SEREG_GCRA_TIM_THR {
  uint32_t Value;
  class {
  public:
    uint32_t Gcra_tim_thr;
  } Fields;
};

typedef _SEREG_GCRA_TIM_THR SEREG_GCRA_TIM_THR;

union _SEREG_GCRA_REQ_THR {
  uint32_t Value;
  class {
  public:
    uint32_t Gcra_req_thr;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_GCRA_REQ_THR SEREG_GCRA_REQ_THR;

union _SEREG_DC1SPACE {
  uint32_t Value;
  class {
  public:
    uint32_t dc1_space_ctrl;
    uint32_t Reserved;
  } Fields;
};

typedef _SEREG_DC1SPACE SEREG_DC1SPACE;

class _SEREG_INFO {
public:
  SEREG_CMDBASE SeCmdBase[3];
  SEREG_CMDLMT SeCmdLimit[3];
  SEREG_CMDRPTR SeCmdReadPtr[3];
  SEREG_CMDWPTR SeCmdWritePtr[3];
  SEREG_SRWORDCNT SeSramWordCount[3];
  SEREG_Q_PRIORITY SeQueuePriority;
  SEREG_CLR_FMT SeColorFormat[3];
  SEREG_COLOR_KEY SeColorKey[3];
  SEREG_SRC_COLOR SeSrcColor[3];
  SEREG_SRC_COLOR2 SeSrcColor2[3];
  SEREG_SRC_ALPHA SeSrcAlpha[3];
  SEREG_DEST_ALPHA SeDestAlpha[3];
  SEREG_RSLT_ALPHA SeResultAlpha[3];
  SEREG_DMAINFO SeDmaInfo[3];
  SEREG_FADDR SeFelicsAddr[3][4];
  SEREG_BSTLEN SeDmaBurstLength;
  SEREG_CLIP_INFO SeClipInfo[3];
  SEREG_CONVSCALING SeConvScaling[3];
  SEREG_CONVCOEF1 SeConvCoef1[3];
  SEREG_CONVCOEF2 SeConvCoef2[3];
  SEREG_CONVCOEF3 SeConvCoef3[3];
  SEREG_CONVCOEF4 SeConvCoef4[3];
  SEREG_CONVCOEF5 SeConvCoef5[3];
  SEREG_CONVCOEF6 SeConvCoef6[3];
  SEREG_STRETCH SeStretch[3];
  SEREG_HDHS SeHdhs[3];
  SEREG_VDHS SeVdhs[3];
  SEREG_COEFINFO SeCoefInfo[3];
  SEREG_2TO4_MAP Se2To4Map;
  SEREG_2TO8_MAP Se2To8Map;
  uint32_t Reserved;
  SEREG_FELICS_ABORT SeFelicsAbort[3];
  SEREG_CTRL SeCtrl[3];
  SEREG_IDLE SeIdle[3];
  SEREG_INTSEL SeIntSel;
  SEREG_INTS SeInts[3];
  SEREG_INTE SeInte[3];
  SEREG_INSTCNT SeInstCnt[3];
  SEREG_CLUT_RADR SeClutReadAddress;
  SEREG_CLUT_VALUE SeClutValue;
  SEREG_DBG SeDebug;
  SEREG_SCTCH SeSctch;
  SEREG_GCRA_CTRL SeGcraCtrl;
  SEREG_GCRA_REQ_CNT SeGcraReqCnt;
  SEREG_GCRA_TIM_CNT SeGcraTimCnt;
  SEREG_GCRA_TIM_THR SeGcraTimThr;
  SEREG_GCRA_REQ_THR SeGcraReqThr;
  SEREG_DC1SPACE SeDc1Space;
  SEREG_4TO8_MAP Se4To8Map[4];
  SEREG_SRC_ALPHA2 SeSrcAlpha2[3];
};

typedef _SEREG_INFO SEREG_INFO;


/*system/Include/Graphics/StreamingEngine2SeDrv.c*/
extern SEREG_INFO * SeRegInfo;

class vsync_queue {
public:
  uint32_t u32Occupied;
  uint32_t u32Size;
  uint32_t u32RdPtr;
  uint32_t u32WrPtr;
  uint32_t go;
  uint8_t u8Buf[32748];
};

typedef vsync_queue vsync_queue_t;

extern vsync_queue_t * p_vsync_queue;

extern uint32_t vsync_queue_cached;

extern uint32_t vsync_queue_phy;

extern uint32_t vsync_enable_addr;

extern uint32_t vsync_queue_allocated;

class se_dev {
public:
  void * pCommandBuffer;
  void * pCacheCommandBuffer;
  void * pCommandQueueBaseAddress;
  void * pCommandQueueLimitAddress;
  int32_t lCommandQueueWritePointer;
  int32_t lRemappingOffsetAddress;
  int32_t lCommandQueueSize;
  se_cmd_counter aSoftwareCounter[3];
  uint64_t u64InstCnt[3];
  uint32_t u32QueueCounter[32];
  pthread_mutex_t aLockMutex;
};

extern uint8_t * pSeDeviceInfoNonCached;

extern long unsigned int pSeDeviceInfoizPhy;

extern "C" HRESULT se_open();

extern "C" HRESULT se_close();

void UpdateInstCount(uint32_t queue);

void QueueCmd(uint32_t hQueue, se_dev * pSeDeviceInfo, uint8_t * pbyCommandBuffer, int32_t lCommandLength, int inst_count);

void WriteCmd(se_dev * pSeDeviceInfo, uint8_t * pbyCommandBuffer, int32_t lCommandLength, int inst_count);

void se_lock();

void se_unlock();

HANDLE SE_RequestQueue();

HRESULT SE_ReleaseQueue(HANDLE hQueue);

size_t se_write(uint32_t hQueue, int8_t const * pbyCommandBuffer, size_t dwCommandLength, int inst_count);

extern "C" HRESULT se_ioctl(uint32_t dwCommandCode, void * arg);

extern "C" bool se_checkfinish(SE_CMD_HANDLE hCommandHandle);

extern "C" SE_CMD_HANDLE se_memcpy(void * lpDst, void * lpSrc, int len, bool forward);

extern "C" SE_CMD_HANDLE se_memset(void * lpDst, uint8_t data, int len);

size_t se_write_ex(uint8_t byCommandQueue, int8_t const * pbyCommandBuffer, size_t dwCommandLength);

void SeWriteCommand(uint8_t byCommandQueue, se_dev * pSeDeviceInfo, uint8_t * pbyCommandBuffer, uint32_t dwCommandLength);


/*system/Include/Graphics/StreamingEngine2SeLib.c*/
HRESULT SE_WriteRegister(SEINFO_CMDQUEUE byCommandQueue, SEINFO_REGMODE bRegisterModeSelect, SEINFO_WRITEREG wRegisterAddress, uint32_t dwRegisterValue);

HRESULT SE_BitBltEx(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hDestinationHandle, SE_PROCINFO * pSeDestinationProcInfo, SURFACEHANDLE hSourceHandle, SE_PROCINFO * pSeSourceProcInfo, SEINFO_ALPHAMODE bySrcAlphaModeSelect, uint8_t bySrcAplhaValue, SEINFO_ALPHAMODE byDestAlphaModeSelect, uint8_t byDestAplhaValue, SEINFO_OUTPUT_ALPHAMODE byOutputAlphaModeSelect, uint8_t byOutputAlphaValue, SEINFO_SETUPMODE bSetupModeSelect, SEINFO_CORNER byStartingCorner, SEINFO_GRADIENT byGradientModeSelect, SEINFO_COLORKEY byColorKeySelect, SEINFO_ROPCODE byRopCode);

HRESULT SE_Stretch(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hDestinationHandle, SE_PROCINFO * pSeDestinationProcInfo, SURFACEHANDLE hSourceHandle, SE_PROCINFO * pSeSourceProcInfo, SEINFO_SACLING_TYPE byVerticalScalingType, uint32_t dwVerticalScalingRatio, SEINFO_STRETCH_VTAP byVerticalTapNumber, SEINFO_SACLING_TYPE byHorizontalScalingType, uint32_t dwHorizontalScalingRatio, SEINFO_STRETCH_HTAP byHorizontalTapNumber, SEINFO_STRETCH byScalingAlgorithmSelect, SEINFO_COLORKEY byColorKeySelect, SEINFO_ALPHAPROC_EN bAlphaEnable, uint8_t iVerticalScalingOffset);

HRESULT SE_FetchClut(SEINFO_CMDQUEUE byCommandQueue, SEINFO_CLUT byFetchModeSelect, uint32_t dwStartingAddress);

HRESULT SE_FormatConversion(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hDestinationHandle, SE_PROCINFO * pSeDestinationProcInfo, SURFACEHANDLE hSourceHandle, SE_PROCINFO * pSeSourceProcInfo, SEINFO_CONVERSION byConversionModeSelect, SEINFO_DEST_FORMAT byDestinationFormatSelect, SEINFO_SRC_FORMAT bySourceFormatSelect, SEINFO_COLORKEY byColorKeySelect, SEINFO_OUTPUT_ALPHAMODE byOutputAlphaModeSelect, uint8_t byOutputAlphaValue);

HRESULT SE_Felics(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hSourceHandle, SE_PROCINFO * pSeSourceProcInfo, SEINFO_COMPRESSMODE bCompressMode, SEINFO_CONVERT_YUV_EN bConvertYuvEnable, SEINFO_FELICS_ALPHA_EN bAlphaEnable, uint32_t dwBitstreamBufferAddress0, uint32_t dwBitstreamLength0, uint32_t dwBitstreamBufferAddress1, uint32_t dwBitstreamLength1, uint32_t dwBitstreamBufferAddress2, uint32_t dwBitstreamLength2, uint32_t dwBitstreamBufferAddress3, uint32_t dwBitstreamLength3);

HRESULT SE_Mirror(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hDestinationHandle, SE_PROCINFO * pSeDestinationProcInfo, SURFACEHANDLE hSourceHandle, SE_PROCINFO * pSeSourceProcInfo, SEINFO_MIRRORMODE byMirrorDirection);

HRESULT SE_Bitmap(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hDestinationHandle, SE_PROCINFO * pSeDestinationProcInfo, SEINFO_BITMAP_SRC bBitmapLoadingLocation, SEINFO_BITMAP_DIR bFlashAddressDirection, SEINFO_BITMAP_ENDIAN bPixelOrder, uint32_t dwBitmapStartingAddress, SEINFO_BGCOLOR_EN bBackgroundColorEnable, uint32_t dwBackgroundColor, SEINFO_FGCOLOR_EN bForegroundColorEnable, uint32_t dwForegroundColor, SEINFO_BITMAP_32BIT_EN bUse32BitColorFormat);

HRESULT SE_DvbSubtitleDecode(SEINFO_CMDQUEUE byCommandQueue, SURFACEHANDLE hDestinationHandle, SE_PROCINFO * pSeDestinationProcInfo, uint32_t dwPixelDataSubBlockAddress, uint16_t wPixelDataSubBlockLength, SEINFO_NO_COLORMODIFY_EN bNoColorModification);

HRESULT SE_ExecCmd(SEINFO_CMDQUEUE byCommandQueue);

HRESULT SE_WaitCmdDone(SEINFO_CMDQUEUE byCommandQueue);

HRESULT SE_SetDmaInfo(SEINFO_CMDQUEUE byCommandQueue, SEINFO_DMAINFO byDmaChannel, uint32_t dwBaseAddress, uint32_t dwPitch, SEINFO_INTERLEAVING_MODE bInterleavingModeEnable);

HRESULT SE_SetFelicsDmaInfo(SEINFO_CMDQUEUE byCommandQueue, SEINFO_DMAINFO byDmaChannel, uint32_t dwBaseAddress);

HRESULT SE_SetClippingInfo(SEINFO_CMDQUEUE byCommandQueue, SEINFO_CLIPPING_EN bClippingEnable, SEINFO_CLIPPINGINFO byClippingChannel, uint16_t wXmin, uint16_t wYmin, uint16_t wXmax, uint16_t wYmax);

HRESULT SE_SetColorFormat(SEINFO_CMDQUEUE byCommandQueue, SEINFO_COLOR_FORMAT byColorFormat, SEINFO_ARGB_ALPHA_LOCATION byArgbAlphaLocation, SEINFO_ARGB_ALPHA_LOCATION byOutputArgbAlphaLocation, SEINFO_RGB_LOCATION byRgbLocation, SEINFO_RGB_LOCATION byOutputRgbLocation);

HRESULT SE_SetColorKey(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwColorKey);

HRESULT SE_SetSourceColor1(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwSourceColor);

HRESULT SE_SetSourceColor2(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwSourceColor);

HRESULT SE_SetSourceAlpha1(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwSourceAlpha);

HRESULT SE_SetSourceAlpha2(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwSourceAlpha);

HRESULT SE_SetDestAlpha(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwDestAlpha);

HRESULT SE_SetConstAlpha(SEINFO_CMDQUEUE byCommandQueue, uint32_t dwConstAlpha);

size_t SE_GetBitStreamLength(uint32_t dwBitStreamBufferAddress);

HRESULT SE_WaitVSync(HANDLE hQueue, int count);

HRESULT SE_SetConvCoeff(SEINFO_CMDQUEUE byCommandQueue);

/*system/Include/Platform_Lib/Graphics/StreamingEngine/SeCmd.h*/
class _SECMD_WRITE_REGISTER {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t RegisterAddress;
    uint32_t Clut;
    uint32_t Reserved;
  } Word1;
  class {
  public:
    uint32_t RegisterValue;
  } Word2;
  class {
  public:
    uint32_t Reserved;
  } Word3;
  class {
  public:
    uint32_t Reserved;
  } Word4;
};

typedef _SECMD_WRITE_REGISTER SECMD_WRITE_REGISTER;

class _SECMD_FETCH_CLUT {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t FetchEntry;
    uint32_t Reserved;
  } Word1;
  class {
  public:
    uint32_t StartingAddress;
    uint32_t Reserved;
  } Word2;
  class {
  public:
    uint32_t Reserved;
  } Word3;
  class {
  public:
    uint32_t Reserved;
  } Word4;
};

typedef _SECMD_FETCH_CLUT SECMD_FETCH_CLUT;

class _SECMD_FORMAT_CONVERSION {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t Width;
    uint32_t Height;
    uint32_t SelectOutputAlpha;
    uint32_t ColorKey;
    uint32_t ConversionMode;
  } Word1;
  class {
  public:
    uint32_t Reserved;
    uint32_t InputX;
    uint32_t InputY;
    uint32_t SourceFormat;
  } Word2;
  class {
  public:
    uint32_t Reserved;
    uint32_t OutputX;
    uint32_t OutputY;
    uint32_t DestinationFormat;
  } Word3;
  class {
  public:
    uint32_t Reserved;
    uint32_t ChromaAddressIndex;
    uint32_t DestAddressIndex;
    uint32_t SrcAddressIndex;
  } Word4;
};

typedef _SECMD_FORMAT_CONVERSION SECMD_FORMAT_CONVERSION;

class _SECMD_BITBLT {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t Width;
    uint32_t Height;
    uint32_t InputSelect;
    uint32_t SelectDestAlpha;
  } Word1;
  class {
  public:
    uint32_t OutputX;
    uint32_t OutputY;
    uint32_t GradientDirection;
    uint32_t RopCode;
    uint32_t SelectOutputAlpha;
    uint32_t GradientEnable;
    uint32_t DestAddressIndex;
  } Word2;
  class {
  public:
    uint32_t InputX;
    uint32_t InputY;
    uint32_t RopMode;
    uint32_t ColorKey;
    uint32_t SelectSrcAlpha;
    uint32_t SrcAddressIndex;
  } Word3;
  class {
  public:
    uint32_t InvDy;
  } Word4;
  class {
  public:
    uint32_t InvDx;
  } Word5;
  class {
  public:
    uint32_t Reserved;
  } Word6;
  class {
  public:
    uint32_t Reserved;
  } Word7;
  class {
  public:
    uint32_t Reserved;
  } Word8;
};

typedef _SECMD_BITBLT SECMD_BITBLT;

class _SECMD_STRETCH {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t OutputX;
    uint32_t OutputY;
    uint32_t ColorKey;
    uint32_t HScalingEnable;
    uint32_t VScalingEnable;
    uint32_t SrcInterleaving;
  } Word1;
  class {
  public:
    uint32_t OutputWidth;
    uint32_t OutputHeight;
    uint32_t Reserved;
    uint32_t ScalingAlgorithm;
  } Word2;
  class {
  public:
    uint32_t InputX;
    uint32_t InputY;
    uint32_t DestInterleaving;
    uint32_t SrcAddressIndex;
  } Word3;
  class {
  public:
    uint32_t InputWidth;
    uint32_t InputHeight;
    uint32_t DestAddressIndex;
  } Word4;
};

typedef _SECMD_STRETCH SECMD_STRETCH;

class _SECMD_MIRROR {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t OutputX;
    uint32_t OutputY;
    uint32_t MirrorDirection;
    uint32_t DestAddressIndex;
  } Word1;
  class {
  public:
    uint32_t Width;
    uint32_t Height;
    uint32_t Reserved;
  } Word2;
  class {
  public:
    uint32_t Reserved1;
    uint32_t InputX;
    uint32_t InputY;
    uint32_t Reserved2;
    uint32_t SrcAddressIndex;
  } Word3;
  class {
  public:
    uint32_t Reserved;
  } Word4;
};

typedef _SECMD_MIRROR SECMD_MIRROR;

class _SECMD_DECODE_DVB {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t Reserved1;
    uint32_t OutputX;
    uint32_t OutputY;
    uint32_t Reserved2;
    uint32_t DestAddressIndex;
  } Word1;
  class {
  public:
    uint32_t PxdAddress;
    uint32_t Reserved;
  } Word2;
  class {
  public:
    uint32_t PxdLength;
    uint32_t NonModifyingClorFlag;
    uint32_t Reserved;
  } Word3;
  class {
  public:
    uint32_t Reserved;
  } Word4;
};

typedef _SECMD_DECODE_DVB SECMD_DECODE_DVB;

class _SECMD_BITMAP {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t ClrFmt32b;
    uint32_t OutputX;
    uint32_t OutputY;
    uint32_t Reserved2;
    uint32_t DestAddressIndex;
  } Word1;
  class {
  public:
    uint32_t BitmapStartingAddress;
    uint32_t Reserved;
    uint32_t FlashDirection;
    uint32_t UseFlash;
  } Word2;
  class {
  public:
    uint32_t Reserved;
    uint32_t PixelOrder;
    uint32_t UseForegroundColor;
    uint32_t UseBackgroundColor;
    uint32_t Height;
    uint32_t Width;
  } Word3;
  union {
    uint32_t BackgroundColor;
      class {
    public:
      uint32_t ForegroundColor;
      uint32_t BackgroundColor;
    } Fields;
  } Word4;
  class {
  public:
    uint32_t ForegroundColor;
  } Word5;
  class {
  public:
    uint32_t Reserved;
  } Word6;
  class {
  public:
    uint32_t Reserved;
  } Word7;
  class {
  public:
    uint32_t Reserved;
  } Word8;
};

typedef _SECMD_BITMAP SECMD_BITMAP;

class _SECMD_FELICS {
public:
  class {
  public:
    uint32_t OpCode;
    uint32_t Width;
    uint32_t Reserved1;
    uint32_t Height;
    uint32_t Reserved2;
    uint32_t Rgb2Yuv;
    uint32_t Compression;
  } Word1;
  class {
  public:
    uint32_t Reserved1;
    uint32_t InputX;
    uint32_t InputY;
    uint32_t Reserved2;
  } Word2;
  class {
  public:
    uint32_t Reserved1;
    uint32_t ConstAlpha;
    uint32_t Interleaving;
    uint32_t ChromaAddressIndex;
    uint32_t BaseAddressIndex;
    uint32_t Reserved2;
  } Word3;
  class {
  public:
    uint32_t BitstreamLength;
  } Word4;
  class {
  public:
    uint32_t BitstreamLength;
  } Word5;
  class {
  public:
    uint32_t BitstreamLength;
  } Word6;
  class {
  public:
    uint32_t BitstreamLength;
  } Word7;
  class {
  public:
    uint32_t Reserved;
  } Word8;
};

typedef _SECMD_FELICS SECMD_FELICS;


