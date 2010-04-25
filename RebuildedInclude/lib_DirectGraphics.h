/*system/Include/Platform_Lib/Graphics/DirectGraphics.h*/

/*rop codes, to be verified*/
#define BLACKNESS   1
#define DSTINVERT   2
#define MERGECOPY   3
#define MERGEPAINT  4
#define NOTSRCCOPY  5
#define NOTSRCERASE 6
#define PATCOPY     7
#define PATINVERT   8
#define PATPAINT    9
#define SRCAND      10
#define SRCCOPY     11 	//ok
#define SRCERASE    12
#define SRCINVERT   13
#define SRCPAINT    14
#define WHITENESS   15


typedef void * SURFACEHANDLE;

typedef void * HANDLE;

class MYSHIFT {
public:
  int32_t w;
  int32_t h;
};

class MYRECT {
public:
  int32_t x;
  int32_t y;
  int32_t w;
  int32_t h;
};

union CONFIG_PARAM {
  int clipOn;
  int shiftOn;
  MYRECT clipRect;
  MYSHIFT shift;
};

enum ENUM_CONFIG_FLAG {FLAG_CONFIG_SET_CLIP_ON_OFF, FLAG_CONFIG_GET_CLIP_ON_OFF, FLAG_CONFIG_SET_SHIFT_ON_OFF, FLAG_CONFIG_GET_SHIFT_ON_OFF, FLAG_CONFIG_SET_CLIP_RECT, FLAG_CONFIG_GET_CLIP_RECT, FLAG_CONFIG_SET_SHIFT_OFFSET, FLAG_CONFIG_GET_SHIFT_OFFSET};

enum ENUM_CONFIG_TARGET {CONFIG_SOURCE, CONFIG_DESTINATION};

enum _STRETCH_COEFF_MODE {SCALING_MODE_AUTO, SCALING_MODE_BY_USER};

typedef _STRETCH_COEFF_MODE STRETCH_COEFF_MODE;

enum _PIXELFORMAT {Format_8, Format_16, Format_32, Format_32_888A, Format_2_IDX, Format_4_IDX, Format_8_IDX, Format_1555, Format_4444_ARGB, Format_4444_YUVA, Format_8888_YUVA, Format_5551, Format_4444_RGBA, Format_YUV420, Format_YUV422, Format_YUV444, NO_OF_PIXEL_FORMAT};

typedef _PIXELFORMAT PIXEL_FORMAT;

enum VOUT_GRAPHIC_PLANE {VOUT_GRAPHIC_OSD, VOUT_GRAPHIC_SUB1, VOUT_GRAPHIC_SUB2};

typedef short unsigned int COLORDEF;

enum _ALPHAMODE {Alpha_None, Alpha_Constant, Alpha_SrcMajor, Alpha_DecMajor};

typedef _ALPHAMODE ALPHA_MODE;

enum _COLORKEYMODE {ColorKey_Src, ColorKey_Des, ColorKey_None};

typedef _COLORKEYMODE COLORKEY_MODE;

enum _START_CONER {CONER_TOP_LEFT, CONER_TOP_RIGHT, CONER_BOTTOM_LEFT, CONER_BOTTOM_RIGHT};

typedef _START_CONER START_CORNER;

class _SURFACEDESC {
public:
  uint32_t dwSize;
  uint32_t dwFlags;
  uint32_t dwHeight;
  uint32_t dwWidth;
  uint32_t lPitch;
  uint32_t dwColorKey;
  uint32_t dwBackBufferCount;
  uint32_t dwRefreshRate;
  uint32_t dwAlphaBitDepth;
  uint32_t PhyAddr;
  void * lpSurface;
  PIXEL_FORMAT pixelFormat;
  int storageMode;
  MYRECT srcClipRect;
  MYRECT dstClipRect;
  MYSHIFT srcShift;
  MYSHIFT dstShift;
  int srcClipOn;
  int dstClipOn;
  int srcShiftOn;
  int dstShiftOn;
  uint32_t dwCompressedBitstreamBufAddr[4];
  uint32_t dwCompressedBitstreamLen[4];
};

typedef _SURFACEDESC SURFACEDESC;

class PALENTRY {
public:
  uint32_t c1;
  uint32_t c2;
  uint32_t c3;
  uint32_t a;
};

class EFFECTDRAWHDR {
public:
  bool use_header_wh;
  int dst_height;
  int dst_width;
  int effect_type;
};

enum ENUM_IMG_TYPE {IMG_TYPE_BMP, IMG_TYPE_PNG};

class DGIMAGEHDR {
public:
  int width;
  int height;
  int bpp;
  int BPP;
  int pitch;
  int fd;
  int imagebits_pos[4];
  char * filename;
  int ori_width;
  int ori_height;
  SURFACEHANDLE hSurface;
  EFFECTDRAWHDR effect_draw;
  int isRtkProprietary;
  ENUM_IMG_TYPE img_type;
};

typedef DGIMAGEHDR * PDGIMAGEHDR;

class ui_animation_path {
public:
  int x;
  int y;
  int w;
  int h;
  uint8_t alpha;
  uint32_t delay;
};

enum _animation_ty {TYPE_ANI_UNIT_SPEED, TYPE_ANI_TYPE_NUM};

typedef _animation_ty ENUM_ANIMATION_TYPE;


/*../include/../DG/DG_private.h*/
class surface_info_t {
public:
  surface_info_t * pNext;
  surface_info_t * pPrev;
  SURFACEHANDLE handle;
  uint64_t sw_se_cmd_counter[3];
  int32_t AddrMapIdx;
  SURFACEDESC surfdesc;
  pthread_mutex_t lock_mutex;
  int32_t s32OSDDisplaying;
  int32_t lPitch;
  int serial_number;
  bool bLocked;
  uint32_t u32Size;
  uint32_t u32BytesPerPixel;
  void * lpSurfaceFreePtr;
};

typedef surface_info_t * PSURFACEINFO;


/*system/Include/Platform_Lib/Graphics/include/dcu_mem.h*/
class dcu_mem_t {
public:
  DWORD addr;
  DWORD pitch;
  DWORD dcu_idx;
};

typedef dcu_mem_t DCU_MEM;


/*system/Include/Platform_Lib/Graphics/DG/DG_private.h*/
extern PIXEL_FORMAT compatible_pix_fmt[13];


/*system/Include/Platform_Lib/Graphics/DG/DirectGraphicsOnBoard2.cpp*/
extern PSURFACEINFO g_pSurfInfoHead;

extern PSURFACEINFO g_pSurfInfoTail;

extern int32_t g_pixelFormat;

typedef int DGBOOL;

extern DGBOOL g_graphic_dcu_avail[4];

extern sem_t g_graphic_dcu_sem;

extern DCU_MEM DCU_Mem[4];

extern pthread_mutex_t g_sb2_mutex;

extern pthread_mutex_t g_dgf_mutex;

extern "C" void DG_Init();

void SetColorFormat(HANDLE hQueue, PIXEL_FORMAT format);

extern "C" void DG_UnInit();

extern "C" HRESULT SetDcuMem(uint32_t idx, uint32_t addr, uint32_t pitch);

extern "C" HRESULT GetDcuMem(uint32_t idx, DCU_MEM * dcumem);

HRESULT AddrMapIdx_alloc(uint32_t * pidx);

HRESULT AddrMapIdx_free(uint32_t idx);

class mem_entry_t {
public:
  uint32_t mem_addr;
  uint32_t in_use;
  uint32_t size;
};

typedef mem_entry_t MEM_ENTRY;

extern MEM_ENTRY * g_pMem_entryGraphic;

extern uint32_t g_PhyAddrStart;

extern uint32_t g_VirtAddrStart;

extern uint32_t g_VirtAddrStartFreePtr;

extern uint32_t g_VirtToPhysOffset;

extern "C" int DG_GetGraphicCacheMemoryMaxSize();

void * DG_allocGraphicMemory(size_t size, BYTE * * nonCachedAddr, long unsigned int * pPhyAddr);

void DG_freeGraphicMemory(void * mem_addr);

extern "C" SURFACEHANDLE DG_ModifySurface(SURFACEHANDLE hSurface, SURFACEDESC * pDesc);

extern "C" SURFACEHANDLE DG_CreateSurface(SURFACEDESC * pDesc);

extern "C" SURFACEHANDLE DG_CreateSurfaceEx(SURFACEDESC * pDesc, bool bForceUseCache, bool bAllocMem);

extern "C" SURFACEHANDLE DG_CreateSurfaceForDFB(SURFACEDESC * pDesc);

extern "C" HRESULT DG_CloseSurface(SURFACEHANDLE handleDesc);

extern "C" HRESULT DG_GetSurfaceDesc(SURFACEHANDLE handle, SURFACEDESC * pDesc);

extern "C" void * DG_GetSurfaceRowDataPointer(SURFACEHANDLE handle);

extern "C" HRESULT DG_GetSurfaceRowDataSize(SURFACEHANDLE handle, int * iRet);

extern "C" HRESULT DG_ConfigSurface(SURFACEHANDLE handle, ENUM_CONFIG_TARGET target, ENUM_CONFIG_FLAG flag, CONFIG_PARAM * pConfigParam);

extern "C" HRESULT DG_AlphaBlt(SURFACEHANDLE handleDes, int nXDest, int nYDest, int nWidth, int nHeight, SURFACEHANDLE handleSrc, int srcColor, int nXSrc, int nYSrc, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t desA, int isForceDestAlpha, uint8_t forcedDesA, COLORKEY_MODE colorKeyMode, long unsigned int colorKey, HANDLE hQueue, START_CORNER corner);

extern "C" HRESULT DG_BitBlt(SURFACEHANDLE handleDes, int nXDest, int nYDest, int nWidth, int nHeight, SURFACEHANDLE handleSrc, int srcColor, int nXSrc, int nYSrc, int ropCode, HANDLE hQueue, START_CORNER corner);

extern "C" HRESULT DG_FormatConv(SURFACEHANDLE handleDes, int nXDest, int nYDest, int nWidth, int nHeight, SURFACEHANDLE handleSrc, int nXSrc, int nYSrc, bool bIsSrcColorKey, bool bIsConstAlpha, uint8_t alpha, HANDLE hQueue);

extern "C" int CarryToIntByRatioDevide(int SrcInt, float ratio);

extern "C" HRESULT DG_Stretch(SURFACEHANDLE hSurfaceDes, int iXDes, int iYDes, int iWDes, int iHDes, SURFACEHANDLE hSurfaceSrc, int iXSrc, int iYSrc, int iWSrc, int iHSrc, COLORKEY_MODE byColorKeySelect, long unsigned int iColorKey, bool bAlphaEnable, HANDLE hQueue, STRETCH_COEFF_MODE StretchMode, uint8_t iVerticalScalingOffset, int iVerticalTapNumber);

extern "C" HRESULT DG_MakeAnimationArray(int s_x, int s_y, int s_w, int s_h, uint8_t s_A, int e_x, int e_y, int e_w, int e_h, uint8_t e_A, ENUM_ANIMATION_TYPE ani_type, int step, ui_animation_path * pPath, int delay);

extern "C" HRESULT DG_AlphaStretch(SURFACEHANDLE hSurface, int dstX, int dstY, int dstW, int dstH, SURFACEHANDLE hImgSurface, int srcX, int srcY, int srcW, int srcH, SURFACEHANDLE hAlphaStretchSurface, int stretchX, int stretchY, int stretchW, int stretchH, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t desA, HANDLE hQueue);

extern "C" HRESULT DG_MovingBlt(uint32_t behavior, SURFACEHANDLE hSurface, SURFACEHANDLE hBGSurface, int off_x, int off_y, SURFACEHANDLE hObjSurface, int obj_x, int obj_y, int obj_w, int obj_h, SURFACEHANDLE hAlphaStretchSurface, int stretchX, int stretchY, int stretchW, int stretchH, int minX, int minY, int maxX, int maxY, int step, ui_animation_path * pPath, HANDLE hQueue);

extern "C" HRESULT DG_WaitComplete(SURFACEHANDLE handleDes);

extern "C" HRESULT DG_Lock(SURFACEHANDLE handleDes, int nXDest, int nYDest, int nWidth, int nHeight, int dwFlags);

extern "C" HRESULT DG_Unlock(SURFACEHANDLE handleDes);

extern "C" HRESULT DG_DrawIntersectRectangle(SURFACEHANDLE handleDes, MYRECT * p_src_rect, MYRECT * p_clip_rect, int srcColor);

extern "C" HRESULT DG_DrawRectangle(SURFACEHANDLE handleDes, int nXDest, int nYDest, int nWidth, int nHeight, int srcColor, HANDLE hQueue);

extern "C" void DrawHollowRectangle(SURFACEHANDLE hSurface, int x, int y, int w, int h, int thick, COLORDEF color);

extern "C" HRESULT DG_SetColorKey(SURFACEHANDLE handle, long unsigned int dwColorKey);

extern "C" HRESULT DG_GetColorKey(SURFACEHANDLE handle, long unsigned int * lpColorKey);

extern "C" int DG_GetSerialNumber(SURFACEHANDLE handle);

class disp_info {
public:
  int nNumPaletteHandleUsed;
  class {
  public:
    int InUse;
  } palette[4];
  class {
  public:
    int nNumDisplayHandleUsed;
      class {
    public:
      int InUse;
      int winID;
      int hSurface;
      int hPalette;
      int storageMode;
    } disp_area[16];
  } graphic[3];
};

typedef disp_info DISP_INFO;

extern DISP_INFO g_disp_info;

extern "C" HANDLE DG_GetGraphicDisplayHandle(VOUT_GRAPHIC_PLANE plane);

extern "C" HANDLE DG_RequestQueue();

extern "C" HRESULT DG_ReleaseQueue(HANDLE hQueue);

extern "C" HRESULT DG_WaitVSync(HANDLE hQueue, int count);

extern "C" HANDLE DG_GetDisplayHandle();

extern "C" HANDLE DG_GetPaletteHandle();

extern "C" HRESULT DG_SetPaletteContent(HANDLE hPalette, char * pal_table, int entry_num);

extern "C" HRESULT DG_FreePaletteHandle(HANDLE hPalette);

extern "C" HRESULT DG_BindPalette(HANDLE hDisplay, HANDLE hPalette);

extern "C" HANDLE DG_GetDisplaySurfaceInfo(HANDLE handleDisplay);

HRESULT ReleaseDisplayHandle(HANDLE displayHandle, int bBatchJob);

extern "C" HRESULT DG_ReleaseDisplayHandle(HANDLE displayHandle);

extern "C" HRESULT DG_ReleaseDisplayHandleBatch(HANDLE displayHandle);

extern "C" HRESULT DG_WaitCmdDone(HANDLE hQueue);

HRESULT DisplayArea(HANDLE handleDisplay, int nXDest, int nYDest, int nWidth, int nHeight, SURFACEHANDLE handle, int nXSrc, int nYSrc, ALPHA_MODE alphaMode, uint8_t srcA, COLORKEY_MODE colorKeyMode, long unsigned int colorKey, int bBatchJob);

extern "C" HRESULT DG_DisplayArea(HANDLE handleDisplay, int nXDest, int nYDest, int nWidth, int nHeight, SURFACEHANDLE handle, int nXSrc, int nYSrc, ALPHA_MODE alphaMode, uint8_t srcA, COLORKEY_MODE colorKeyMode, long unsigned int colorKey);

extern "C" HRESULT DG_DisplayAreaBatch(HANDLE handleDisplay, int nXDest, int nYDest, int nWidth, int nHeight, SURFACEHANDLE handle, int nXSrc, int nYSrc, ALPHA_MODE alphaMode, uint8_t srcA, COLORKEY_MODE colorKeyMode, long unsigned int colorKey);

extern "C" MYRECT GetIntersectRect(MYRECT * pOriRect, MYRECT * pClipRect);

extern "C" int DG_PixelConvert(PIXEL_FORMAT srcFormat, int srcColor, PIXEL_FORMAT dstFormat);

extern "C" HRESULT DG_CopyBlt(SURFACEHANDLE handleDest, uint32_t nXDest, uint32_t nYDest, uint32_t nWidth, uint32_t nHeight, SURFACEHANDLE handleSrc, uint32_t nXSrc, uint32_t nYSrc);

extern "C" HRESULT DG_SurfaceFelics(SURFACEHANDLE cmpHandle, int nWidth, int nHeight, SURFACEHANDLE uncmpHandle, int nXuncmp, int nYuncmp, bool mode, int threshold);


/*system/Include/Platform_Lib/Graphics/DG/DrawBitmap.cpp*/
void ComputePitch(int bpp, int width, int * pitch, int * BPP);

class BMPFILEHEAD {
public:
  BYTE BM[2];
  DWORD size;
  WORD rsvd1;
  WORD rsvd2;
  DWORD offset;
};

class BMPINFOHEAD {
public:
  DWORD size;
  DWORD width;
  DWORD height;
  WORD planes;
  WORD bpp;
  DWORD compress;
  DWORD imageSize;
  DWORD ppmX;
  DWORD ppmY;
  DWORD palsize;
  DWORD clrImp;
};

extern "C" HRESULT DG_GetBmpPalette(char * filename, PALENTRY * palette);

extern "C" HRESULT DG_GetBmpHeader(char * filename, PDGIMAGEHDR * ppimage);

extern "C" HRESULT DG_FreeBmpHeader(PDGIMAGEHDR pimage);

extern "C" HRESULT DG_GetPngHeader(char * filename, PDGIMAGEHDR * ppimage);

extern "C" HRESULT DG_GetPngHeader_int(char * filename, PDGIMAGEHDR * ppimage, bool * pbInterlaced);

extern "C" HRESULT DG_FreePngHeader(PDGIMAGEHDR pimage);

extern "C" HRESULT DG_DrawPng(SURFACEHANDLE handleDes, uint32_t x, uint32_t y, PDGIMAGEHDR pimage, bool * pbIsCancel);

extern "C" HRESULT DG_DrawPngEx(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, COLORKEY_MODE colorKeyMode, uint32_t colorKey, bool * pbIsCancel);

extern "C" HRESULT DG_DrawPngFile(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, char * filename, bool * pbIsCancel);

extern "C" HRESULT DG_DrawPngFileEx1(SURFACEHANDLE handleDes, char * filename, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool bCalNewSize, uint32_t sx_per, uint32_t sy_per, uint32_t sw_per, uint32_t sh_per, COLORKEY_MODE colorKeyMode, uint32_t colorKey, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t dstA, bool * pbIsCancel);

extern "C" HRESULT DG_DrawPngFileEx1_resol(SURFACEHANDLE handleDes, char * filename, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool bCalNewSize, uint32_t sx_per, uint32_t sy_per, uint32_t sw_per, uint32_t sh_per, COLORKEY_MODE colorKeyMode, uint32_t colorKey, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t dstA, int * pImg_width, int * pImg_height, bool * pbIsCancel);

extern "C" HRESULT DG_DrawAlphaPng(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t dstA, bool * pbIsCancel);

extern "C" HRESULT DG_DrawPngScale(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, COLORKEY_MODE colorKeyMode, uint32_t colorKey, uint32_t dstW, uint32_t dstH, bool * pbIsCancel);

extern "C" HRESULT DG_DrawPngEx1(SURFACEHANDLE handleDes, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, uint32_t sx, uint32_t sy, uint32_t sw, uint32_t sh, COLORKEY_MODE colorKeyMode, uint32_t colorKey, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t dstA, bool * pbIsCancel);

extern "C" HRESULT DG_DrawBitmap(SURFACEHANDLE handleDes, uint32_t x, uint32_t y, PDGIMAGEHDR pimage);

extern "C" HRESULT DG_DrawBitmapEx(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, COLORKEY_MODE colorKeyMode, uint32_t colorKey);

extern "C" HRESULT DG_DrawAlphaBitmap(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t dstA);

extern "C" HRESULT DG_DrawBitmapScale(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, COLORKEY_MODE colorKeyMode, uint32_t colorKey, uint32_t dstW, uint32_t dstH);

extern "C" HRESULT DG_DrawBitmapEx1(SURFACEHANDLE handleDes, uint32_t x, uint32_t y, uint32_t w, uint32_t h, PDGIMAGEHDR pImageHdr, uint32_t sx, uint32_t sy, uint32_t sw, uint32_t sh, COLORKEY_MODE colorKeyMode, uint32_t colorKey, ALPHA_MODE alphaMode, uint8_t srcA, uint8_t desA);

extern "C" HRESULT DG_DrawBitmapFile(SURFACEHANDLE hSurface, uint32_t x, uint32_t y, char * filename);

extern "C" HRESULT DG_DrawBitmapWithEffect(SURFACEHANDLE hSurface, PDGIMAGEHDR pImageHdr, COLORKEY_MODE colorKeyMode, uint32_t colorKey, int effectType, int dst_w, int dst_h, int bgColor);


