#include "guiprogresscontrol.hpp"

using std::string;

pymms::gui::GUIProgressControl::GUIProgressControl(int iPosX, int iPosY, int iWidth, int iHeight, 
		     int iLayer, const std::string& strText, const std::string& strFont, 
		     const std::string& strRgb, const std::string& strAlignment, 
		     const std::string& strRgbProgress, int iAlphaProgress, int iTextOffset, int iCutOff) : 
		     GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer),
		     m_lblText(iPosX, iPosY, iWidth - iCutOff, iHeight, strText, iLayer, strFont, 
			       strRgb, iTextOffset, strAlignment),
		     m_recProgress(iPosX, iPosY, iWidth, iHeight, iLayer, iAlphaProgress, strRgbProgress)
{
  m_strText = strText;
  m_strFont = strFont;
  m_strRgb = strRgb;
  m_strRgbProgress = strRgbProgress;
  m_iAlphaProgress = iAlphaProgress;
  m_iTextOffset = iTextOffset;
  m_iControlType = PROGRESSCONTROL;
  m_strAlignment = strAlignment;
  m_iCutOff = iCutOff;
  m_iMin = m_iCurrent = 0;
  m_iMax = 100;
}

void pymms::gui::GUIProgressControl::setBounds(int iMin, int iMax)
{
  m_iMin = m_iCurrent = ((iMin>=0)?iMin:0);
  m_iMax = ((iMax>0)?iMax:100);

  if(m_iMin >= m_iMax)
  {
    m_iMin = m_iCurrent = 0;
    m_iMax = 100;
  }
}

void pymms::gui::GUIProgressControl::update(int iValue)
{
  if(iValue <= m_iMin)
    m_iCurrent = m_iMin;
  else if(iValue >= m_iMax)
    m_iCurrent = m_iMax;
  else
    m_iCurrent = iValue; 
}

int pymms::gui::GUIProgressControl::getProgressWidth()
{
  return int(float(m_iCurrent-m_iMin)/float(m_iMax-m_iMin) * float(m_iWidth));
}

string pymms::gui::GUIProgressControl::getText() const
{ 
  return m_strText;
}

void pymms::gui::GUIProgressControl::setText(const string& strText)
{ 
  m_strText = strText;
  m_lblText.setLabel(strText);
}

string pymms::gui::GUIProgressControl::getFont() const
{ 
  return m_strFont;
}

void pymms::gui::GUIProgressControl::setFont(const string& strFont)
{ 
  m_strFont = strFont;
  m_lblText.setFont(strFont);
}

string pymms::gui::GUIProgressControl::getRgb() const
{ 
  return m_strRgb;
}

void pymms::gui::GUIProgressControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
  m_lblText.setRgb(strRgb);
}

string pymms::gui::GUIProgressControl::getRgbProgress() const
{
  return m_strRgbProgress;
}

void pymms::gui::GUIProgressControl::setRgbProgress(const string& strRgbProgress)
{
  m_strRgbProgress = strRgbProgress;
  m_recProgress.setRgb(strRgbProgress);
}

int pymms::gui::GUIProgressControl::getAlphaProgress() const
{
  return m_iAlphaProgress;
}

void pymms::gui::GUIProgressControl::setAlphaProgress(int iAlphaProgress)
{
  m_iAlphaProgress = iAlphaProgress;
  m_recProgress.setAlpha(iAlphaProgress);
}

int pymms::gui::GUIProgressControl::getTextOffset() const
{ 
  return m_iTextOffset;
}

void pymms::gui::GUIProgressControl::setTextOffset(int iTextOffset)
{ 
  m_iTextOffset = iTextOffset;
  m_lblText.setTextOffset(iTextOffset);
}

string pymms::gui::GUIProgressControl::getAlignment() const
{ 
  return m_strAlignment;
}

void pymms::gui::GUIProgressControl::setAlignment(const string& strAlignment)
{ 
  m_strAlignment = strAlignment;
  m_lblText.setAlignment(strAlignment);
}

int pymms::gui::GUIProgressControl::getCutOff() const
{ 
  return m_iCutOff;
}

void pymms::gui::GUIProgressControl::setCutOff(int iCutOff)
{ 
  m_iCutOff = iCutOff;
}

bool pymms::gui::GUIProgressControl::onMessage(GUIMessage& message)
{
  if (message.getControlId() == getId())
  {
     switch (message.getMessage())
     {
        case GUI_MSG_LABEL_SET:
        {
           setText(message.getLabel());

           return true;
     	}
     
	case GUI_MSG_LABEL_RESET:
     	{
           setText("");
      
           return true;
     	}
     }

     return GUIControl::onMessage(message);
  }

  return false;
}

void pymms::gui::GUIProgressControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth);
  m_iHeight = int(m_iHeight * dScalHeight);

  setTextOffset(int(m_iTextOffset * dScalWidth));
  m_iCutOff = int(m_iCutOff * dScalWidth);
}

void pymms::gui::GUIProgressControl::render(Overlay* pOverlay)
{
  if(m_bVisible)
  {
    if(m_iCurrent > m_iMin)
    {
      m_lblText.setPosition(m_iPosX, m_iPosY);
      m_lblText.setWidth(m_iWidth - m_iCutOff);
      m_lblText.setHeight(m_iHeight);
      m_lblText.setLayer(m_iLayer + 1);
      m_lblText.render(pOverlay);
    }

    m_recProgress.setPosition(m_iPosX, m_iPosY);
    m_recProgress.setWidth(getProgressWidth());
    m_recProgress.setHeight(m_iHeight);
    m_recProgress.setLayer(m_iLayer);
    m_recProgress.render(pOverlay);
  }
}  
