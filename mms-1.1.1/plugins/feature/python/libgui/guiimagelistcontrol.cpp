#include "guiimagelistcontrol.hpp"
#include "guibuttoncontrol.hpp"
#include "guitouchmanager.hpp"

using std::string;
using std::pair;

pymms::gui::GUIImageListControl::GUIImageListControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer, 
			       const string& strFont, const string& strRgb, const string& strRgbFocus, 
			       const string& strTextAlignment, int iItemHeight, int iItemWidth, int iTextHeight, 
			       int iItemSpaceX, int iItemSpaceY, bool bListCenterX, bool bListCenterY):  
			       GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer),  
                               m_imgIcon(iPosX, iPosY, iItemWidth, iItemHeight, "", iLayer),
                               m_imgText(iPosX, iPosY, iItemWidth, iTextHeight, "", iLayer, strFont, 
					 strRgb, 0, strTextAlignment)
{
  m_strFont = strFont;
  m_strRgb = strRgb;
  m_strRgbFocus = strRgbFocus;
  m_iControlType = IMAGELISTCONTROL;
  m_strTextAlignment = strTextAlignment;
  m_iItemHeight = (iItemHeight>0)?iItemHeight:30;
  m_iItemWidth = (iItemWidth>0)?iItemWidth:30;
  m_iItemSpaceX = (iItemSpaceX>0)?iItemSpaceX:1;
  m_iItemSpaceY = (iItemSpaceY>0)?iItemSpaceY:1;
  m_iTextHeight = (iTextHeight>0)?iTextHeight:1;
  m_bListCenterX = bListCenterX;
  m_bListCenterY = bListCenterY;
  m_iPos = 0;
}

void pymms::gui::GUIImageListControl::setFont(const string& strFont)
{ 
  m_strFont = strFont;
  m_imgText.setFont(strFont);
}

void pymms::gui::GUIImageListControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
}

void pymms::gui::GUIImageListControl::setRgbFocus(const string& strRgbFocus)
{ 
  m_strRgbFocus = strRgbFocus;
}

bool pymms::gui::GUIImageListControl::addItem(GUIListItem* pListItem)
{ 
  bool bAddable = true;

  for (std::vector<GUIListItem*>::iterator i = m_vecItems.begin(),
      end = m_vecItems.end(); i != end; i++) {
    if((*i) == pListItem)
    {
      bAddable =  false;
      break;
    }
  }

  if(bAddable)
    m_vecItems.push_back(pListItem);

  return bAddable;
}

void pymms::gui::GUIImageListControl::clear()
{
   m_vecItems.clear();
   m_iPos = 0;
}

int pymms::gui::GUIImageListControl::size() const
{
   return m_vecItems.size();
}

void pymms::gui::GUIImageListControl::freeResources()
{
  for (std::vector<GUIListItem*>::iterator i = m_vecItems.begin(),
      end = m_vecItems.end(); i != end; i++) {
    delete (*i);
  }

  m_vecItems.clear();
  m_iPos = 0;
}

const pymms::gui::GUIListItem *pymms::gui::GUIImageListControl::getSelectedItem() const
{ 
  GUIListItem* pListItem = 0;

  if(!m_vecItems.empty())
    pListItem =  m_vecItems[m_iPos];

  return pListItem;
}

int pymms::gui::GUIImageListControl::getSelectedPosition() const
{ 
  return m_iPos;
}

string pymms::gui::GUIImageListControl::getTextAlignment() const
{ 
  return m_strTextAlignment;
}

void pymms::gui::GUIImageListControl::setTextAlignment(const string& strTextAlignment)
{ 
  m_strTextAlignment = strTextAlignment;
  m_imgText.setAlignment(m_strTextAlignment);
}

void pymms::gui::GUIImageListControl::setListCenterX(bool bListCenterX)
{
  m_bListCenterX = bListCenterX;
}

void pymms::gui::GUIImageListControl::setListCenterY(bool bListCenterY)
{
  m_bListCenterY = bListCenterY;
}

bool pymms::gui::GUIImageListControl::onAction(const string& strAction)
{
  if(!m_vecItems.empty())
  {
    if(strAction == "prev")
    {
      if(m_iPos > (m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX))
        m_iPos -= (m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX);
      else if(m_iPos == 0)
	m_iPos = m_vecItems.size() - 1;
      else
        m_iPos = 0;

      return true;
    }

    else if(strAction == "next")
    {
      if(m_iPos < ((signed int)m_vecItems.size() - (m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX)))
        m_iPos += (m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX);
      else if(m_iPos == m_vecItems.size() - 1)
	m_iPos = 0;
      else
        m_iPos = m_vecItems.size() - 1;

      return true;
    }

    else if(strAction == "right")
    {
      if( (((m_iPos + 1) % ((m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX))) == 0) || (m_iPos == m_vecItems.size() - 1) )
        onRight();
      else if( (m_iPos < m_vecItems.size() - 1) )
        m_iPos += 1;
      else
        m_iPos = 0;

      return true;
    }

    else if(strAction == "left")
    {
      if( (m_iPos % ((m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX))) == 0  )
	onLeft();
      else if( (m_iPos > 0) )
        m_iPos -= 1;
      else
        m_iPos = m_vecItems.size() - 1;

      return true;
    }
  }

  return GUIControl::onAction(strAction);
}

bool pymms::gui::GUIImageListControl::onMessage(GUIMessage& message)
{
  if (message.getControlId() == getId())
  {
     switch (message.getMessage())
     {
        case GUI_MSG_LABEL_ADD:
        {
           GUIListItem* pItem=(GUIListItem*)message.getVoid();
           if(pItem)
              addItem(pItem);

           return true;
     	}
     
	case GUI_MSG_LABEL_RESET:
     	{
           clear();
      
           return true;
     	}

	case GUI_MSG_ITEM_SELECTED:
	{
	   message.setParam1(m_iPos);

	   return true;
	}

	case GUI_MSG_TOUCH:
        {
           m_iPos = message.getParam1();

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

void pymms::gui::GUIImageListControl::setItemHeight(int iItemHeight)
{ 
  m_iItemHeight = (iItemHeight>0)?iItemHeight:m_iItemHeight;
  m_imgIcon.setHeight(m_iItemHeight);
}

void pymms::gui::GUIImageListControl::setItemWidth(int iItemWidth)
{
  m_iItemWidth = (iItemWidth>0)?iItemWidth:m_iItemWidth;

  m_imgText.setWidth(m_iItemWidth);
  m_imgIcon.setWidth(m_iItemWidth);
}

void pymms::gui::GUIImageListControl::setItemSpaceX(int iItemSpaceX)
{ 
  m_iItemSpaceX = (iItemSpaceX>0)?iItemSpaceX:m_iItemSpaceX;
}

void pymms::gui::GUIImageListControl::setItemSpaceY(int iItemSpaceY)
{ 
  m_iItemSpaceY = (iItemSpaceY>0)?iItemSpaceY:m_iItemSpaceY;
}

void pymms::gui::GUIImageListControl::setTextHeight(int iTextHeight)
{
  m_iTextHeight = (iTextHeight>0)?iTextHeight:m_iTextHeight;
  m_imgText.setHeight(m_iTextHeight);
}

void pymms::gui::GUIImageListControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth); 
  m_iHeight = int(m_iHeight * dScalHeight);

  setItemHeight(int(m_iItemHeight * dScalHeight));
  setItemWidth(int(m_iItemWidth * dScalWidth));
  setTextHeight(int(m_iTextHeight * dScalHeight));
  setItemSpaceX(int(m_iItemSpaceX * dScalWidth));
  setItemSpaceY(int(m_iItemSpaceY * dScalHeight));  
}

pair<string, string> pymms::gui::GUIImageListControl::splitText(const string& strText)
{
  int iPos = 0;
  pair<string, string> temp;

  while(iPos < strText.size() && temp.second.empty())
  {
    if(temp.first.empty())
      temp.first = string_format::get_line(strText, iPos, m_iItemWidth,
                                        m_strFont + "/" + conv::itos(m_iTextHeight/2));
    else
      temp.second = string_format::get_line(strText, iPos, m_iItemWidth,
                                        m_strFont + "/" + conv::itos(m_iTextHeight/2));

  }

  return temp;
}

void pymms::gui::GUIImageListControl::render(Overlay* pOverlay)
{
  if(!m_vecItems.empty() && m_bVisible)
  {
    if(m_iHeight < (m_iItemHeight + m_iItemSpaceY + 2*m_iTextHeight))
      m_iHeight = m_iItemHeight + m_iItemSpaceY + 2*m_iTextHeight;

    if(m_iWidth < (m_iItemWidth + m_iItemSpaceX))
      m_iWidth = m_iItemWidth + m_iItemSpaceX;

    int iVisibleItemsH = (m_iWidth + m_iItemSpaceX)/(m_iItemWidth + m_iItemSpaceX);
    int iVisibleItemsV = (m_iHeight + m_iItemSpaceY)/(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY);

    int iOffsetH = 0;
    if(m_bListCenterX)
       iOffsetH = (m_iWidth - ((m_iItemWidth * iVisibleItemsH) + (m_iItemSpaceX * (iVisibleItemsH - 1))))/2;
       
    int iOffsetV = 0;
    if(m_bListCenterY)
       iOffsetV= (m_iHeight - ((m_iItemHeight * iVisibleItemsV) + ((2*m_iTextHeight * iVisibleItemsV) + 
				m_iItemSpaceY * (iVisibleItemsV - 1))))/2;

    int iDiff = m_iPos/iVisibleItemsH + 1 - iVisibleItemsV;

    if(iDiff <= 0)
    {
      for(int i = 0; i < iVisibleItemsV; i++)
      {
	for(int j = i*iVisibleItemsH, k = 0; j < i*iVisibleItemsH+iVisibleItemsH; j++, k++)
	{
	  if(j < m_vecItems.size())
          {
	    if(!m_vecItems[j]->getIconImage().empty())
            {
	      m_imgIcon.setId(getId());
              m_imgIcon.setPosition(iOffsetH + m_iPosX + k*(m_iItemWidth + m_iItemSpaceX), 
				iOffsetV + m_iPosY + i*(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY));
              m_imgIcon.setPath(m_vecItems[j]->getIconImage());
              m_imgIcon.setLayer(m_iLayer + 1);

	      S_TouchManager::get_instance()->registerControl(&m_imgIcon, j);
              m_imgIcon.render(pOverlay);

	      pair<string, string> temp = splitText(m_vecItems[j]->getLabel());

	      m_imgText.setPosition(iOffsetH + m_iPosX + k*(m_iItemWidth + m_iItemSpaceX), 
			  iOffsetV + m_iPosY + i*(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY) + m_iItemHeight);
              m_imgText.setLabel(temp.first);
              m_imgText.setLayer(m_iLayer + 1);
              m_imgText.setRgb( ((j==m_iPos) && m_bFocus)?m_strRgbFocus:m_strRgb );
              m_imgText.render(pOverlay);

	      m_imgText.setPosition(iOffsetH + m_iPosX + k*(m_iItemWidth + m_iItemSpaceX),
                          iOffsetV + m_iPosY + i*(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY) + m_iItemHeight + m_iTextHeight);
              m_imgText.setLabel(temp.second);
              m_imgText.setLayer(m_iLayer + 1);
              m_imgText.setRgb( ((j==m_iPos) && m_bFocus)?m_strRgbFocus:m_strRgb );
              m_imgText.render(pOverlay);

            }
          }
        }
      }
    }
    else
    {
      for(int i = iDiff; i < (iDiff + iVisibleItemsV); i++)
      {
        for(int j = i*iVisibleItemsH, k = 0; j < i*iVisibleItemsH+iVisibleItemsH; j++, k++)
        {
          if(j < m_vecItems.size())
          {
            if(!m_vecItems[j]->getIconImage().empty())
            {
	      m_imgIcon.setId(getId());
              m_imgIcon.setPosition(iOffsetH + m_iPosX + k*(m_iItemWidth + m_iItemSpaceX), 
                                iOffsetV + m_iPosY + (i-iDiff)*(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY));
              m_imgIcon.setPath(m_vecItems[j]->getIconImage());
              m_imgIcon.setLayer(m_iLayer + 1);

	      S_TouchManager::get_instance()->registerControl(&m_imgIcon, j);
              m_imgIcon.render(pOverlay);

	      pair<string, string> temp = splitText(m_vecItems[j]->getLabel());

              m_imgText.setPosition(iOffsetH + m_iPosX + k*(m_iItemWidth + m_iItemSpaceX), 
                       iOffsetV + m_iPosY + (i-iDiff)*(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY) + m_iItemHeight);
              m_imgText.setLabel(temp.first);
              m_imgText.setLayer(m_iLayer + 1);
              m_imgText.setRgb( ((j==m_iPos) && m_bFocus)?m_strRgbFocus:m_strRgb );
              m_imgText.render(pOverlay);

	      m_imgText.setPosition(iOffsetH + m_iPosX + k*(m_iItemWidth + m_iItemSpaceX),
                       iOffsetV + m_iPosY + (i-iDiff)*(m_iItemHeight + m_iTextHeight*2 + m_iItemSpaceY) + m_iItemHeight + m_iTextHeight);
              m_imgText.setLabel(temp.second);
              m_imgText.setLayer(m_iLayer + 1);
              m_imgText.setRgb( ((j==m_iPos) && m_bFocus)?m_strRgbFocus:m_strRgb );
              m_imgText.render(pOverlay);
            }
          }
        }
      }
    }
  }
}  
