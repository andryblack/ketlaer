#ifndef GUIBUTTONCONTROL_HPP
#define GUIBUTTONCONTROL_HPP

#include "guicontrol.hpp"
#include "guilabelcontrol.hpp"
#include "guiimagecontrol.hpp"
#include <string>

namespace pymms {

namespace gui {

class GUIButtonControl : public GUIControl
{
  private:
    GUILabelControl m_lblText;
    GUIImageControl m_imgTexture;

  protected:
    std::string m_strText, m_strFont, m_strRgb, m_strRgbFocus, m_strAlignment;
    std::string m_strTextureFocus, m_strTextureNoFocus;
    int m_iTextOffset;
    int m_iCutOff;

  public:
    GUIButtonControl(int iPosX, int iPosY, int iWidth, int iHeight, const std::string& strText, 
		     int iLayer, const std::string& strTextureFocus, const std::string& strTextureNoFocus, 
		     const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
		     const std::string& strRgbFocus="0xffffff", int iTextOffset=0, 
		     const std::string& strAlignment="left", int iCutOff=0);
    std::string getLabel() const;
    void setLabel(const std::string& strText);
    std::string getFont() const;
    void setFont(const std::string& strFont);
    std::string getRgb() const;
    void setRgb(const std::string& strRgb);
    std::string getRgbFocus() const;
    void setRgbFocus(const std::string& strRgbFocus);
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
