#include "guibuttoncontrol.hpp"

using std::string;

pymms::gui::GUIButtonControl::GUIButtonControl(int iPosX, int iPosY, int iWidth, int iHeight, 
		     const std::string& strText,int iLayer, const std::string& strTextureFocus, 
		     const std::string& strTextureNoFocus, const std::string& strFont, 
		     const std::string& strRgb, const std::string& strRgbFocus, int iTextOffset, 
		     const std::string& strAlignment, int iCutOff) : 
		     GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer),
		     m_lblText(iPosX, iPosY, iWidth - iCutOff, iHeight, strText, iLayer, strFont, 
			       strRgb, iTextOffset, strAlignment),
		     m_imgTexture(iPosX, iPosY, iWidth, iHeight, "", iLayer)
{
  m_strText = strText;
  m_strFont = strFont;
  m_strRgb = strRgb;
  m_strRgbFocus = strRgbFocus;
  m_strTextureFocus = strTextureFocus;
  m_strTextureNoFocus = strTextureNoFocus;
  m_iTextOffset = iTextOffset;
  m_iControlType = BUTTONCONTROL;
  m_strAlignment = strAlignment;
  m_iCutOff = iCutOff;
}

string pymms::gui::GUIButtonControl::getLabel() const
{ 
  return m_strText;
}

void pymms::gui::GUIButtonControl::setLabel(const string& strText)
{ 
  m_strText = strText;
  m_lblText.setLabel(strText);
}

string pymms::gui::GUIButtonControl::getFont() const
{ 
  return m_strFont;
}

void pymms::gui::GUIButtonControl::setFont(const string& strFont)
{ 
  m_strFont = strFont;
  m_lblText.setFont(strFont);
}

string pymms::gui::GUIButtonControl::getRgb() const
{ 
  return m_strRgb;
}

void pymms::gui::GUIButtonControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
}

string pymms::gui::GUIButtonControl::getRgbFocus() const
{ 
  return m_strRgbFocus;
}

void pymms::gui::GUIButtonControl::setRgbFocus(const string& strRgbFocus)
{ 
  m_strRgbFocus = strRgbFocus;
}

int pymms::gui::GUIButtonControl::getTextOffset() const
{ 
  return m_iTextOffset;
}

void pymms::gui::GUIButtonControl::setTextOffset(int iTextOffset)
{ 
  m_iTextOffset = iTextOffset;
  m_lblText.setTextOffset(iTextOffset);
}

string pymms::gui::GUIButtonControl::getAlignment() const
{ 
  return m_strAlignment;
}

void pymms::gui::GUIButtonControl::setAlignment(const string& strAlignment)
{ 
  m_strAlignment = strAlignment;
  m_lblText.setAlignment(strAlignment);
}

int pymms::gui::GUIButtonControl::getCutOff() const
{ 
  return m_iCutOff;
}

void pymms::gui::GUIButtonControl::setCutOff(int iCutOff)
{ 
  m_iCutOff = iCutOff;
}

bool pymms::gui::GUIButtonControl::onMessage(GUIMessage& message)
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

void pymms::gui::GUIButtonControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth);
  m_iHeight = int(m_iHeight * dScalHeight);

  setTextOffset(int(m_iTextOffset * dScalWidth));
  m_iCutOff = int(m_iCutOff * dScalWidth);
}

void pymms::gui::GUIButtonControl::render(Overlay* pOverlay)
{
  if(m_bVisible)
  {
    m_lblText.setPosition(m_iPosX, m_iPosY);
    m_lblText.setWidth(m_iWidth - m_iCutOff);
    m_lblText.setHeight(m_iHeight);
    m_lblText.setLayer(m_iLayer + 1);
    m_lblText.setRgb(m_bFocus?m_strRgbFocus:m_strRgb);
    m_lblText.render(pOverlay);

    m_imgTexture.setPosition(m_iPosX, m_iPosY);
    m_imgTexture.setWidth(m_iWidth);
    m_imgTexture.setHeight(m_iHeight);
    m_imgTexture.setLayer(m_iLayer);
    m_imgTexture.setPath(m_bFocus?m_strTextureFocus:m_strTextureNoFocus);
    m_imgTexture.render(pOverlay);
  }
}  
