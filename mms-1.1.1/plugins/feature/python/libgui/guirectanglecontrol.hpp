#ifndef GUIRECTANGLECONTROL_HPP
#define GUIRECTANGLECONTROL_HPP

#include "guicontrol.hpp"
#include <string>

namespace pymms {

namespace gui {

class GUIRectangleControl : public GUIControl
{
  protected:
    std::string m_strRgb;
    int m_iRed, m_iGreen, m_iBlue, m_iAlpha;

    void hexToRgb();

  public:
    GUIRectangleControl(int iPosX, int iPosY, int iWidth, int iHeight, 
		int iLayer, int iAlpha, const std::string& strRgb="0xffffff");
    void setRgb(const std::string& strRgb);
    void setAlpha(int iAlpha);
    void setScaling(double dScalWidth, double dScalHeight);
    void render(Overlay* pOverlay=0);
};

}

}

#endif
