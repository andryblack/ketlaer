#include "guilabelcontrol.hpp"
#include "common.hpp"
#include "text_object.hpp"
#include "renderer.hpp"

using std::string;

pymms::gui::GUILabelControl::GUILabelControl(int iPosX, int iPosY, int iWidth, int iHeight, 
					const string& strText, int iLayer, const string& strFont, 
					const string& strRgb, int iTextOffset, const string& strAlignment) : 
					GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer)
{
  m_iFontSize = m_iRed = m_iGreen = m_iBlue = 0;
  m_iTextOffset = iTextOffset;
  m_iControlType = LABELCONTROL;
  m_strText = strText;
  m_strFont = strFont;
  m_strRgb = strRgb;
  m_strAlignment = strAlignment;
}

void pymms::gui::GUILabelControl::hexToRgb()
{
  int iHexRgb;

  sscanf(m_strRgb.c_str(), "%x", &iHexRgb);
  m_iRed = (iHexRgb & 0xff0000) >> 16;
  m_iGreen = (iHexRgb & 0x00ff00) >> 8;
  m_iBlue = iHexRgb & 0x0000ff;
}

void pymms::gui::GUILabelControl::calcFontSize() const
{
  m_iFontSize = m_iHeight/2;
  /*m_iFontSize = m_iHeight;

  while(m_iFontSize > 0)
  {
    m_realSize = string_format::calculate_string_size(m_strText, m_strFont + "/" + conv::itos(m_iFontSize));
    if(m_realSize.second <= m_iHeight)
      break;
    else
      m_iFontSize--;
  }*/
}

std::string pymms::gui::GUILabelControl::getLabel() const
{ 
  return m_strText;
}

void pymms::gui::GUILabelControl::setLabel(const string& strText)
{ 
  m_strText = strText;
}

void pymms::gui::GUILabelControl::setFont(const string& strFont)
{ 
  m_strFont = strFont;
}

void pymms::gui::GUILabelControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
}

int pymms::gui::GUILabelControl::getRealWidth() const
{
  calcFontSize();

  return m_realSize.first;
}

int pymms::gui::GUILabelControl::getRealHeight() const
{ 
  calcFontSize();

  return m_realSize.second;
}

void pymms::gui::GUILabelControl::setAlignment(const string& strAlignment)
{ 
  m_strAlignment = strAlignment;
}  

int pymms::gui::GUILabelControl::getTextOffset() const
{ 
  return m_iTextOffset;
}

void pymms::gui::GUILabelControl::setTextOffset(int iTextOffset)
{ 
  m_iTextOffset = iTextOffset;
}

bool pymms::gui::GUILabelControl::onMessage(GUIMessage& message)
{
  if (message.getControlId() == getId())
  {
     switch (message.getMessage())
     {
        case GUI_MSG_LABEL_SET:
        {
           setLabel(message.getLabel());

           return true;
     	}
     
	case GUI_MSG_LABEL_RESET:
     	{
           setLabel("");
      
           return true;
     	}
     }

     return GUIControl::onMessage(message);
  }

  return false;
}

void pymms::gui::GUILabelControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth); 
  m_iHeight = int(m_iHeight * dScalHeight);

  m_iTextOffset = int(m_iTextOffset * dScalWidth);
}

void pymms::gui::GUILabelControl::render(Overlay* pOverlay)
{
  if(m_bVisible)
  {
    Render* pRenderer = S_Render::get_instance();
    hexToRgb();
    calcFontSize();

    string strFontToUse = m_strFont + "/" + conv::itos(m_iFontSize);
    string strTempText = m_strText;

    string_format::format_to_size(strTempText, strFontToUse, (m_iWidth-m_iTextOffset)>0?(m_iWidth-m_iTextOffset):0, false);
    m_realSize = string_format::calculate_string_size(strTempText, strFontToUse);

    int iTempTextOffset = m_iTextOffset;
    if(m_strAlignment == "right")
      iTempTextOffset = m_iWidth - m_realSize.first;
    else if(m_strAlignment == "center")
      iTempTextOffset = (m_iWidth - m_realSize.first)/2;
    else
      iTempTextOffset = m_iTextOffset;

    if(pOverlay)
      pOverlay->add(new TObj(strTempText, strFontToUse, m_iPosX + iTempTextOffset, m_iPosY, m_iRed, m_iGreen, m_iBlue, m_iLayer + 1));
    else
      pRenderer->current.add(new TObj(strTempText, strFontToUse, m_iPosX + iTempTextOffset, m_iPosY, m_iRed, m_iGreen, m_iBlue, m_iLayer + 1));
  }
}  
