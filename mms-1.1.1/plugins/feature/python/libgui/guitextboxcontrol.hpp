#ifndef GUITEXTBOXCONTROL_HPP
#define GUITEXTBOXCONTROL_HPP

#include "guicontrol.hpp"
#include "guilabelcontrol.hpp"
#include <string>
#include <vector>

namespace pymms {

namespace gui {

class GUITextBoxControl : public GUIControl
{
  protected:
    std::string m_strFont, m_strRgb, m_strAlignment, m_strText;
    int m_iTextHeight, m_iTextSpace, m_iPos;
    mutable int m_iFontSize;
    std::vector<std::string> m_vecItems;
    GUILabelControl m_lblText;

    void calcFontSize() const;
    void splitText();

  public:
    GUITextBoxControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer, 
		      const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
		      int iTextHeight=30, int iTextSpace=0);
    virtual ~GUITextBoxControl();
    void setFont(const std::string& strFont);
    void setRgb(const std::string& strRgb);
    void setText(const std::string& strText);
    void setTextHeight(int iTextHeight);
    void setTextSpace(int iTextSpace);
    void clear();
    bool onAction(const std::string& strAction);
    bool onMessage(GUIMessage& message);
    void setScaling(double dScalWidth, double dScalHeight);
    void render(Overlay* pOverlay=0);
};

}

}

#endif
