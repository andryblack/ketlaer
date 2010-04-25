/*system/Include/Platform_Lib/Graphics/DrawScale.h*/
class Scale {
 private:
  static int SRC_X;
  static int DST_X;
  static int SRC_Y;
  static int DST_Y;
 public:
  static void SetCoeff(int SrcX, int DstX, int SrcY, int DstY);
  static int GetSrcX();
  static int GetDstX();
  static int GetSrcY();
  static int GetDstY();
  static int AdjustX(int x);
  static int AdjustY(int y);
  static int AdjustW(int w);
  static int AdjustH(int h);
};

