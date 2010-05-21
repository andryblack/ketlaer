#include "common.hpp"
#include "guitextboxcontrol.hpp"
#include "guilabelcontrol.hpp"

using std::string;
using std::pair;

pymms::gui::GUITextBoxControl::GUITextBoxControl(int iPosX, int iPosY, int iWidth, 
				int iHeight, int iLayer, const string& strFont, 
				const string& strRgb, int iTextHeight, int iTextSpace) : 
				GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer),
				m_lblText(iPosX, iPosY, iWidth, iTextHeight, "", iLayer, strFont, strRgb) 
{
  m_strText = "";
  m_strFont = strFont;
  m_strRgb = strRgb;
  m_iTextHeight = (iTextHeight>0)?iTextHeight:30;
  m_iTextSpace = (iTextSpace>=0)?iTextSpace:0;
  m_iFontSize = m_iPos = 0;
  m_iControlType = TEXTBOXCONTROL;
}

pymms::gui::GUITextBoxControl::~GUITextBoxControl()
{
  m_vecItems.clear();
  m_iPos = 0;
}

void pymms::gui::GUITextBoxControl::setFont(const string& strFont)
{ 
  m_strFont = strFont;
  m_lblText.setFont(strFont);
}

void pymms::gui::GUITextBoxControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
  m_lblText.setRgb(strRgb);
}

void pymms::gui::GUITextBoxControl::setText(const string& strText)
{ 
  m_strText = strText;
  m_iPos = 0;

  int iPos = 0;
  while( (iPos = m_strText.find("\n", iPos)) != string::npos)
    m_strText.replace(iPos, 1, "");
}

void pymms::gui::GUITextBoxControl::clear()
{ 
  m_vecItems.clear();
  m_iPos = 0;
}

bool pymms::gui::GUITextBoxControl::onAction(const string& strAction)
{ 
  if(strAction == "prev")
  {
    int iVisibleItems = m_iHeight/(m_iTextHeight + m_iTextSpace);

    if(m_iPos > iVisibleItems)
      m_iPos--;
    else
      m_iPos = 0;

    return true;
  }

  else if(strAction == "next")
  {
    int iVisibleItems = m_iHeight/(m_iTextHeight + m_iTextSpace);

    if( (m_iPos == 0) && (m_vecItems.size() > iVisibleItems) )
      m_iPos += iVisibleItems;
    else if( (m_iPos > 0) && (m_iPos < (m_vecItems.size() - 1)) )
      m_iPos++;

    return true;
  }
  else if(strAction == "page_up")
  {
    int iVisibleItems = m_iHeight/(m_iTextHeight + m_iTextSpace);

    if(m_iPos > iVisibleItems)
      m_iPos--;
    else
      m_iPos = 0;

    return true;
  }

  else if(strAction == "page_down")
  {
    int iVisibleItems = m_iHeight/(m_iTextHeight + m_iTextSpace);

    if( (m_iPos == 0) && (m_vecItems.size() > iVisibleItems) )
      m_iPos += iVisibleItems;
    else if( (m_iPos > 0) && (m_iPos < (m_vecItems.size() - 1)) )
      m_iPos++;

    return true;
  }

  return GUIControl::onAction(strAction);
}

bool pymms::gui::GUITextBoxControl::onMessage(GUIMessage& message)
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
	case GUI_MSG_ACTION:
        {
          return onAction(message.getAction());
        }
     }

     return GUIControl::onMessage(message);
  }

  return false;
}

void pymms::gui::GUITextBoxControl::setTextHeight(int iTextHeight)
{ 
  m_iTextHeight = (iTextHeight>0)?iTextHeight:30;
  m_lblText.setHeight(m_iTextHeight);
}

void pymms::gui::GUITextBoxControl::setTextSpace(int iTextSpace)
{ 
  m_iTextSpace = (iTextSpace>=0)?iTextSpace:0;
}

void pymms::gui::GUITextBoxControl::calcFontSize() const
{
  m_iFontSize = m_iTextHeight/2;

  /*m_iFontSize = m_iHeight;

  while(m_iFontSize > 0)
  {
    pair<int, int> txtDimension = string_format::calculate_string_size("test", m_strFont + 
								    "/" + conv::itos(m_iFontSize));
    if(txtDimension.second <= m_iTextHeight)
      break;
    else
      m_iFontSize--;
  }*/
}

void pymms::gui::GUITextBoxControl::splitText()
{
  int iPos = 0;

  calcFontSize();

  m_vecItems.clear();
  while(iPos < m_strText.size())
  {
    m_vecItems.push_back(string_format::get_line(m_strText, iPos, m_iWidth, 
    					m_strFont + "/" + conv::itos(m_iFontSize)));

  }
}

void pymms::gui::GUITextBoxControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth); 
  m_iHeight = int(m_iHeight * dScalHeight);

  setTextHeight(int(m_iTextHeight * dScalHeight));
  m_iTextSpace = int(m_iTextSpace * dScalHeight);
}

void pymms::gui::GUITextBoxControl::render(Overlay* pOverlay)
{
  if(m_bVisible)
  {
    splitText();

    if(m_iHeight < (m_iTextHeight + m_iTextSpace))
      m_iHeight = m_iTextHeight + m_iTextSpace;

    int iVisibleItems = m_iHeight/(m_iTextHeight + m_iTextSpace);

    int iDiff = m_iPos + 1 - iVisibleItems;
    if(iDiff <= 0)
    {
      for(int i = 0; i < iVisibleItems; i++)
      {
	m_lblText.setPosition(m_iPosX, m_iPosY + i*(m_iTextHeight + m_iTextSpace));
	m_lblText.setWidth(m_iWidth);
	m_lblText.setLabel( (i<m_vecItems.size())?m_vecItems[i]:"" );
	m_lblText.setLayer(m_iLayer);
	m_lblText.render(pOverlay);
      }
    }
    else
    {
      for(int i = iDiff; i < (iVisibleItems + iDiff); i++)
      {
	m_lblText.setPosition(m_iPosX, m_iPosY + (i-iDiff)*(m_iTextHeight + m_iTextSpace));
        m_lblText.setWidth(m_iWidth);
        m_lblText.setLabel(m_vecItems[i]);
        m_lblText.setLayer(m_iLayer);
	m_lblText.render(pOverlay);
      }
    }
  }
}  
