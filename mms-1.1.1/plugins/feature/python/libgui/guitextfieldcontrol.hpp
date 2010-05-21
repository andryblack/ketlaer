#ifndef GUITEXTFIELDCONTROL_HPP
#define GUITEXTFIELDCONTROL_HPP

#include "guicontrol.hpp"
#include "guiutil.hpp"
#include <string>
#include <vector>

namespace pymms {

namespace gui {

class GUITextFieldControl : public GUIControl
{
  protected:
    std::string m_strText, m_strFont, m_strRgb, m_strAlignment;
    int m_iRed, m_iGreen, m_iBlue, m_iTextOffset;
    mutable int m_iFontSize;
    mutable std::pair<int, int> m_realSize;
    bool m_bEditable;
    GUIInputUtil m_guiInputUtil;

    void hexToRgb();
    void calcFontSize() const;

  public:
    GUITextFieldControl(int iPosX, int iPosY, int iWidth, int iHeight, const std::string& strText, int iLayer, 
		    const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
	            int iTextOffset=0, const std::string& strAlignment="left");
    void setText(const std::string& strText);
    std::string getText() const;
    void setFont(const std::string& strFont);
    void setRgb(const std::string& strRgb);
    void setAlignment(const std::string& strAlignment);
    int getRealWidth() const;
    int getRealHeight() const;
    int getTextOffset() const;
    void setEditable(bool bEditable);
    bool getEditable();
    void setTextOffset(int iTextOffset);
    void render(Overlay* pOverlay=0);
    bool onMessage(GUIMessage& message);
    bool onAction(const std::string& strAction);
    void setScaling(double dScalWidth, double dScalHeight);
};

}

}

#endif
