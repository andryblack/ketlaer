#include "guirectanglecontrol.hpp"
#include "rectangle_object.hpp"
#include "renderer.hpp"

using std::string;

pymms::gui::GUIRectangleControl::GUIRectangleControl(int iPosX, int iPosY, int iWidth, 
				int iHeight, int iLayer, int iAlpha, 
				const string& strRgb) : GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer)
{
  m_iRed = m_iGreen = m_iBlue = 0;
  m_iAlpha = (iAlpha>=0)?iAlpha:0;
  m_iControlType = RECTANGLECONTROL;
  m_strRgb = strRgb;
}

void pymms::gui::GUIRectangleControl::hexToRgb()
{
  int iHexRgb;

  sscanf(m_strRgb.c_str(), "%x", &iHexRgb);
  m_iRed = (iHexRgb & 0xff0000) >> 16;
  m_iGreen = (iHexRgb & 0x00ff00) >> 8;
  m_iBlue = iHexRgb & 0x0000ff;
}

void pymms::gui::GUIRectangleControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
}

void pymms::gui::GUIRectangleControl::setAlpha(int iAlpha)
{ 
  m_iAlpha = (iAlpha>=0)?iAlpha:0;
}

void pymms::gui::GUIRectangleControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth); 
  m_iHeight = int(m_iHeight * dScalHeight);
}

void pymms::gui::GUIRectangleControl::render(Overlay* pOverlay)
{
  if(m_bVisible)
  {
    Render* pRenderer = S_Render::get_instance();

    hexToRgb();

    if(pOverlay)
      pOverlay->add(new RObj(m_iPosX, m_iPosY, m_iWidth, m_iHeight, m_iRed, 
		  m_iGreen, m_iBlue, m_iAlpha, m_iLayer));
    else
      pRenderer->current.add(new RObj(m_iPosX, m_iPosY, m_iWidth, m_iHeight, 
			   m_iRed, m_iGreen, m_iBlue, m_iAlpha, m_iLayer));
  }
}  
