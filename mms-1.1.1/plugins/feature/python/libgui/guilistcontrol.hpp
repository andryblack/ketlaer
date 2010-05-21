#ifndef GUILISTCONTROL_HPP
#define GUILISTCONTROL_HPP

#include "guicontrol.hpp"
#include "guilistitem.hpp"
#include "guibuttoncontrol.hpp"
#include "guiimagecontrol.hpp"
#include "guilabelcontrol.hpp"
#include <string>
#include <vector>

namespace pymms {

namespace gui {

class GUIListControl : public GUIControl
{
  private:
    GUIButtonControl m_btnItem;
    GUIImageControl m_imgThumb;
    GUILabelControl m_lblSuffix;

  protected:
    std::string m_strFont, m_strRgb, m_strRgbFocus, m_strAlignment;
    std::string m_strTextureFocus, m_strTextureNoFocus;
    int m_iTextOffset, m_iItemHeight, m_iItemSpace, m_iPos;
    std::vector<GUIListItem*> m_vecItems; 

  public:
    int m_iPosExtra;
    GUIListControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer, 
		const std::string& strTextureFocus, const std::string& strTextureNoFocus, 
		const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
		const std::string& strRgbFocus="0xffffff", int iTextOffset=0, 
		const std::string& strAlignment="left", int iItemHeight=30, int iItemSpace=0);
    virtual ~GUIListControl() {}
    void setFont(const std::string& strFont);
    void setRgb(const std::string& strRgb);
    void setRgbFocus(const std::string& strRgbFocus);
    void setItemHeight(int iItemHeight);
    void setItemSpace(int iItemSpace);
    bool addItem(GUIListItem* pListItem);
    std::string getAlignment() const;
    void setAlignment(const std::string& strAlignment);
    void clear();
    int size() const;
    void freeResources();
    GUIListItem* getSelectedItem();
    int getSelectedPosition() const;
    GUIListItem* getItemAt(int iPos);
    bool onAction(const std::string& strAction);
    bool onMessage(GUIMessage& message);
    void setScaling(double dScalWidth, double dScalHeight);
    void render(Overlay* pOverlay=0);
};

}

}

#endif
