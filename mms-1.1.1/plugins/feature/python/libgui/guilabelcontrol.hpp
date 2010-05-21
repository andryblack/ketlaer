#ifndef GUILABELCONTROL_HPP
#define GUILABELCONTROL_HPP

#include "guicontrol.hpp"
#include <string>

namespace pymms {

namespace gui {

class GUILabelControl : public GUIControl
{
  protected:
    std::string m_strText, m_strFont, m_strRgb, m_strAlignment;
    int m_iRed, m_iGreen, m_iBlue, m_iTextOffset;
    mutable int m_iFontSize;
    mutable std::pair<int, int> m_realSize;

    void hexToRgb();
    void calcFontSize() const;

  public:
    GUILabelControl(int iPosX, int iPosY, int iWidth, int iHeight, const std::string& strText, int iLayer, 
		    const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
	            int iTextOffset=0, const std::string& strAlignment="left");
    std::string getLabel() const;
    void setLabel(const std::string& strText);
    void setFont(const std::string& strFont);
    void setRgb(const std::string& strRgb);
    void setAlignment(const std::string& strAlignment);
    int getRealWidth() const;
    int getRealHeight() const;
    int getTextOffset() const;
    void setTextOffset(int iTextOffset);
    void render(Overlay* pOverlay=0);
    bool onMessage(GUIMessage& message);
    void setScaling(double dScalWidth, double dScalHeight);
};

}

}

#endif
