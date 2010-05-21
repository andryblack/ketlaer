#ifndef GUIWINDOW_HPP
#define GUIWINDOW_HPP

#include <vector>
#include <list>
#include <string>
#include <cc++/thread.h>
#include "renderer.hpp"
#include "guicontrol.hpp"
#include "guimessage.hpp"
#include "tinyxml.h"

namespace pymms {

namespace gui {

bool cmpControlId(GUIControl* pControl, int iControlId);

typedef std::vector<GUIControl*>::iterator GUIControlIterator;
typedef std::vector<GUIControl*>::const_iterator GUIControlConstIterator;

class GUIWindow
{
  friend class GUIWindowManager;

  private:
    void setActive(bool bActive);
    void render(Overlay* pOverlay=0);
    void loadControl(const TiXmlNode* pControl);

  protected:
    std::vector<GUIControl*> m_vecControls;
    int m_iWindowId;
    bool m_bUseOverlay;
    double m_dScalWidth, m_dScalHeight;
    int m_iDefaultControl;
    bool m_bActive;

    virtual bool onAction(const std::string& strAction);
    virtual bool onMessage(GUIMessage& message);

  public:
    GUIWindow();
    virtual ~GUIWindow();
    virtual bool load(const std::string& strFileName);
    bool getOverlay() const;
    void setOverlay(bool bUseOverlay);
    bool addControl(GUIControl* pControl);
    GUIControl* getControl(int iControlId);
    int getId() const;
    void setId(int iWindowId);
    int getFocus() const;
    void setFocus(int iControl);
    int getWidth() const;
    int getHeight() const;
    bool getActive() const;
    void setScalWidth(double dScalWidth);
    void setScalHeight(double dScalHeight);
    double getScalWidth();
    double getScalHeight();
    bool removeControl(int iControlId);
    virtual void clear();
};

}

}

#endif
