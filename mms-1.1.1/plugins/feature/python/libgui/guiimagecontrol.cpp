#include "guiimagecontrol.hpp"
#include "renderer.hpp"
#include "pic_object.hpp"
#include "libfspp.hpp"

using std::string;

pymms::gui::GUIImageControl::GUIImageControl(int iPosX, int iPosY, int iWidth, 
			     int iHeight, const string& strPath, int iLayer) : 
			     GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer) 
{
  m_strPath = strPath;
  m_iControlType = IMAGECONTROL;
}

string pymms::gui::GUIImageControl::getPath() const
{ 
  return m_strPath;
}

void pymms::gui::GUIImageControl::setPath(const string& strPath)
{ 
  m_strPath = strPath;
}

void pymms::gui::GUIImageControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth); 
  m_iHeight = int(m_iHeight * dScalHeight);
}

void pymms::gui::GUIImageControl::render(Overlay* pOverlay)
{
  if(m_bVisible)
  {
    Render* pRenderer = S_Render::get_instance();
    string strPath;

    if(file_exists(m_strPath))
      strPath = m_strPath;
    else if(m_strPath.length() > 0)
      strPath = pRenderer->default_path + m_strPath;

    if(file_exists(strPath))
    {
      if(pOverlay)
         pOverlay->add(new PObj(strPath, m_iPosX, m_iPosY, m_iWidth, m_iHeight, false, m_iLayer));
      else
         pRenderer->current.add(new PObj(strPath, m_iPosX, m_iPosY, m_iWidth, m_iHeight, false, m_iLayer));
    }
  }
}  
