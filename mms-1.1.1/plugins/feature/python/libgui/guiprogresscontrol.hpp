#ifndef GUIPROGRESSCONTROL_HPP
#define GUIPROGRESSCONTROL_HPP

#include "guicontrol.hpp"
#include "guilabelcontrol.hpp"
#include "guirectanglecontrol.hpp"
#include <string>

namespace pymms {

namespace gui {

class GUIProgressControl : public GUIControl
{
  private:
    GUILabelControl m_lblText;
    GUIRectangleControl m_recProgress;

    int getProgressWidth();

  protected:
    std::string m_strText, m_strFont, m_strRgb, m_strAlignment;
    std::string m_strRgbProgress;
    int m_iAlphaProgress;
    int m_iTextOffset;
    int m_iCutOff;
    int m_iMin, m_iMax, m_iCurrent;

  public:
    GUIProgressControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer, const std::string& strText="",
		     const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
		     const std::string& strAlignment="center", const std::string& strRgbProgress="0x00ff00", 
		     int iAlphaProgress=100, int iTextOffset=0, int iCutOff=0);
    void setBounds(int iMin, int iMax);
    void update(int iValue);
    int getValue();
    std::string getText() const;
    void setText(const std::string& strText);
    std::string getFont() const;
    void setFont(const std::string& strFont);
    std::string getRgb() const;
    void setRgb(const std::string& strRgb);
    std::string getRgbProgress() const;
    void setRgbProgress(const std::string& strRgbProgress);
    int getAlphaProgress() const;
    void setAlphaProgress(int iAlphaProgess);
    int getTextOffset() const;
    void setTextOffset(int iTextOffset);
    std::string getAlignment() const;
    void setAlignment(const std::string& strAlignment);
    int getCutOff() const;
    void setCutOff(int iCutOff);
    void render(Overlay* pOverlay=0);
    bool onMessage(GUIMessage& message);
    void setScaling(double dScalWidth, double dScalHeight);
};

}

}

#endif
