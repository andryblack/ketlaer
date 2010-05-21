#ifndef GUIIMAGECONTROL_HPP
#define GUIIMAGECONTROL_HPP

#include "guicontrol.hpp"
#include <string>

namespace pymms {

namespace gui {

class GUIImageControl : public GUIControl
{
  private:
    std::string m_strPath;

  public:
    GUIImageControl(int iPosX, int iPosY, int iWidth, int iHeight, 
		    const std::string& strPath, int iLayer);
    std::string getPath() const;
    void setPath(const std::string& strPath);
    void setScaling(double dScalWidth, double dScalHeight);
    void render(Overlay* pOverlay=0);
};

}

}

#endif
