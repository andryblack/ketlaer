#ifndef GUICONTROL_HPP
#define GUICONTROL_HPP

#include <string>
#include <cc++/thread.h>
#include "renderer.hpp"
#include "guimessage.hpp"

namespace pymms {

namespace gui {

enum CONTROLTYPE
{
   CONTROL,
   BUTTONCONTROL,
   IMAGECONTROL,
   LABELCONTROL,
   LISTCONTROL,   
   RECTANGLECONTROL,
   TEXTBOXCONTROL,
   TEXTFIELDCONTROL,
   PROGRESSCONTROL,
   IMAGELISTCONTROL
};

class GUIControl
{
  friend class GUIWindow;

  protected:
    int m_iPosX, m_iPosY, m_iWidth, m_iHeight;
    int m_iLayer, m_iControlId, m_iParentId;
    bool m_bFocus, m_bVisible;
    int m_iRight, m_iLeft, m_iNext, m_iPrev;
    int m_iControlType, m_iRouteControl;
    std::string m_strInputMap;
    void onRight() const;
    void onLeft() const;
    void onPrev() const;
    void onNext() const;
    void onClick() const;

    virtual void render(Overlay* pOverlay=0) = 0;

  public:
    GUIControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer, const std::string& strInputMap="python");
    virtual ~GUIControl() {}
    virtual bool onAction(const std::string& strAction);
    virtual void freeResources();
    virtual bool onMessage(GUIMessage& message);
    virtual void setScaling(double dScalWidth, double dScalHeight) = 0;
    void controlLeft(const GUIControl* pLeft);
    void controlRight(const GUIControl* pRight);
    void controlPrev(const GUIControl* pPrev);
    void controlNext(const GUIControl* pNext);
    void controlLeft(int iLeft);
    void controlRight(int iRight);
    void controlPrev(int iPrev);
    void controlNext(int iNext);
    void setFocus(bool bFocus);
    bool getFocus() const;
    void setVisible(bool bVisible);
    bool getVisible() const;
    void setHeight(int iHeight);
    int getHeight() const;
    void setWidth(int iWidth);
    int getWidth() const;
    void setRouteControl(int iRouteControl);
    int getRouteControl() const;
    void setLayer(int iLayer);
    int getLayer() const;
    void setId(int iControlId);
    int getId() const;
    void setParentId(int iParentId);
    int getParentId() const;
    void setPosition(int iPosX, int iPosY);
    void setPosX(int iPosX);
    void setPosY(int iPosY);
    int getPosX();
    int getPosY();
    int getType();
    void reset();
};
    
}

}

#endif
