/*system/Include/Application/AppClass/VoutUtil.h*/
class VoutUtil {
private:
  static VO_RECTANGLE m_VideoRect[2][8];
public:
  CLNT_STRUCT m_clnt;
  VO_STANDARD m_tv_standard;
  VoutUtil();
  ~VoutUtil();
  HRESULT SetVideoStandard(VO_STANDARD standard, u_char enProg, u_char enDIF, u_char enCompRGB, VO_PEDESTAL_TYPE pedType, u_char dataInt0, u_char dataInt1);
  void UpdateVoutRectangleSetup(VO_VIDEO_PLANE videoPlane, ENUM_VIDEO_SYSTEM videoSystem, VO_RECTANGLE rect);
  VO_RECTANGLE GetVoutRectangleSetup(VO_VIDEO_PLANE videoPlane, ENUM_VIDEO_SYSTEM videoSystem);
  HRESULT ApplyVoutDisplayWindowSetup(VO_COLOR borderColor, u_char enBorder);
  HRESULT ApplyVideoStandardSetup();
  HRESULT SetTVtype(VO_TV_TYPE tvType);
  HRESULT SetBackground(VO_COLOR bgColor, u_char bgEnable);
  HRESULT SetClosedCaption(u_char enCC_odd, u_char enCC_even);
  HRESULT SetAPS(u_char enExt, VO_VBI_APS APS);
  HRESULT SetCopyMode(u_char enExt, VO_VBI_COPY_MODE copyMode);
  HRESULT SetAspectRatio(u_char enExt, VO_VBI_ASPECT_RATIO aspectRatio);
  HRESULT ConfigureDisplayWindow(VO_VIDEO_PLANE videoPlane, VO_RECTANGLE videoWin, VO_RECTANGLE borderWin, VO_COLOR borderColor, u_char enBorder);
  HRESULT SetV2alpha(u_char v2Alpha);
  HRESULT SetRescaleMode(VO_VIDEO_PLANE videoPlane, VO_RESCALE_MODE rescaleMode, VO_RECTANGLE rescaleWindow);
  HRESULT SetDeintMode(VO_VIDEO_PLANE videoPlane, VO_DEINT_MODE deintMode);
  HRESULT Zoom(VO_VIDEO_PLANE videoPlane, VO_RECTANGLE zoomWin);
  HRESULT ConfigureOSD(VO_OSD_LPF_TYPE lpfType, short int * RGB2YUVcoeff);
  int CreateOSDwindow(VO_RECTANGLE winPos, VO_OSD_COLOR_FORMAT colorFmt, int colorKey, u_char alpha);
  HRESULT ModifyOSDwindow(u_char winID, u_char reqMask, VO_RECTANGLE winPos, VO_OSD_COLOR_FORMAT colorFmt, int colorKey, u_char alpha, u_short startX, u_short startY, u_short imgPitch, long int pImage, bool bBatchJob);
  HRESULT DeleteOSDwindow(u_char winID, bool bBatchJob);
  HRESULT DrawOSDwindow(u_char winID, u_short startX, u_short startY, u_short imgPitch, long int pImage, bool bBatchJob);
  HRESULT HideOSDwindow(u_char winID, bool bBatchJob);
  HRESULT ConfigOSDCanvas(VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, bool bBatchJob);
  HRESULT ConfigGraphicCanvas(VO_GRAPHIC_PLANE plane, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, bool bBatchJob);
  HRESULT ConfigureCursor(char alpha, char colorKey, VO_COLOR * colorMap, VO_OSD_LPF_TYPE lpfType, long int pCursorImg);
  HRESULT DrawCursor(u_short x, u_short y);
  HRESULT HideCursor();
  HRESULT SetPeakingStrength(u_char peekingStrength);
  HRESULT SetBrightness(u_char brightness);
  HRESULT SetContrast(u_char contrast);
  HRESULT SetHue(u_char hue);
  HRESULT SetSaturation(u_char saturation);
  HRESULT GetTvDim(VO_STANDARD standard, int * pwidth, int * pheight);
};


