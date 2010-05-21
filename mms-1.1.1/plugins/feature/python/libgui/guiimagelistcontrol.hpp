#ifndef GUIIMAGELISTCONTROL_HPP
#define GUIIMAGELISTCONTROL_HPP

#include "guicontrol.hpp"
#include "guilistitem.hpp"
#include "guiimagecontrol.hpp"
#include "guilabelcontrol.hpp"
#include <string>
#include <vector>

namespace pymms {

namespace gui {

class GUIImageListControl : public GUIControl
{
  private:
    GUIImageControl m_imgIcon;
    GUILabelControl m_imgText;

  protected:
    std::string m_strFont, m_strRgb, m_strRgbFocus, m_strTextAlignment;
    int m_iItemHeight, m_iItemWidth, m_iTextHeight, m_iItemSpaceX, m_iItemSpaceY, m_iPos;
    bool m_bListCenterX, m_bListCenterY;
    std::vector<GUIListItem*> m_vecItems; 

    std::pair<std::string, std::string> splitText(const std::string& strText);

  public:
    GUIImageListControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer,  
		const std::string& strFont="Vera", const std::string& strRgb="0xffffff", 
		const std::string& strRgbFocus="0xffffff", const std::string& strTextAlignment="left", 
		int iItemHeight=30, int iItemWidth = 30, int iTextHeight = 20, int iItemSpaceX=10, 
		int iItemSpaceY=5, bool bListCenterX=false, bool bListCenterY=false);
    virtual ~GUIImageListControl() {}
    void setFont(const std::string& strFont);
    void setRgb(const std::string& strRgb);
    void setRgbFocus(const std::string& strRgbFocus);
    void setItemHeight(int iItemHeight);
    void setItemWidth(int iItemWidth);
    void setItemSpaceX(int iItemSpaceX);
    void setItemSpaceY(int iItemSpaceY);
    void setTextHeight(int iTextHeight);
    void setListCenterX(bool bListCenterX);
    void setListCenterY(bool bListCenterY);
    bool addItem(GUIListItem* pListItem);
    std::string getTextAlignment() const;
    void setTextAlignment(const std::string& strTextAlignment);
    void clear();
    int size() const;
    void freeResources();
    const GUIListItem* getSelectedItem() const;
    int getSelectedPosition() const;
    bool onAction(const std::string& strAction);
    bool onMessage(GUIMessage& message);
    void setScaling(double dScalWidth, double dScalHeight);
    void render(Overlay* pOverlay=0);
};

}

}

#endif
