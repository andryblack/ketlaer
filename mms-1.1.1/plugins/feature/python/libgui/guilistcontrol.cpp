#include "guilistcontrol.hpp"
#include "guibuttoncontrol.hpp"
#include "guitouchmanager.hpp"

using std::string;

pymms::gui::GUIListControl::GUIListControl(int iPosX, int iPosY, int iWidth, int iHeight, int iLayer, 
			       const string& strTextureFocus, const string& strTextureNoFocus, 
			       const string& strFont, const string& strRgb, const string& strRgbFocus, 
			       int iTextOffset, const string& strAlignment, int iItemHeight, 
			       int iItemSpace) : GUIControl(iPosX, iPosY, iWidth, iHeight, iLayer),  
			       m_btnItem(iPosX, iPosY, iWidth, iItemHeight, "", iLayer, strTextureFocus, 
					 strTextureNoFocus),
                               m_imgThumb(iPosX, iPosY, iItemHeight, iItemHeight, "", iLayer),
                               m_lblSuffix(iPosX, iPosY, iWidth, iItemHeight, "", iLayer, strFont, 
				           strRgb, 0, "right")
{
  m_strFont = strFont;
  m_strRgb = strRgb;
  m_strRgbFocus = strRgbFocus;
  m_strTextureFocus = strTextureFocus;
  m_strTextureNoFocus = strTextureNoFocus;
  m_iTextOffset = iTextOffset;
  m_iControlType = LISTCONTROL;
  m_strAlignment = strAlignment;
  m_iItemHeight = (iItemHeight>0)?iItemHeight:30;
  m_iItemSpace = (iItemSpace>0)?iItemSpace:1;
  m_iPos = 0;
  m_iPosExtra = -1;
}

void pymms::gui::GUIListControl::setFont(const string& strFont)
{ 
  m_strFont = strFont;
  m_btnItem.setFont(strFont);
  m_lblSuffix.setFont(strFont);
}

void pymms::gui::GUIListControl::setRgb(const string& strRgb)
{ 
  m_strRgb = strRgb;
  m_btnItem.setRgb(strRgb);
}

void pymms::gui::GUIListControl::setRgbFocus(const string& strRgbFocus)
{ 
  m_strRgbFocus = strRgbFocus;
  m_btnItem.setRgbFocus(strRgbFocus);
}

bool pymms::gui::GUIListControl::addItem(GUIListItem* pListItem)
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

void pymms::gui::GUIListControl::clear()
{
   m_vecItems.clear();
   m_iPos = 0;
   m_iPosExtra = -1;
}

int pymms::gui::GUIListControl::size() const
{
   return m_vecItems.size();
}

void pymms::gui::GUIListControl::freeResources()
{
  for (std::vector<GUIListItem*>::iterator i = m_vecItems.begin(),
      end = m_vecItems.end(); i != end; i++) {
    delete (*i);
  }

  m_vecItems.clear();
  m_iPos = 0;
  m_iPosExtra = -1;
}

pymms::gui::GUIListItem *pymms::gui::GUIListControl::getSelectedItem()
{ 
  GUIListItem* pListItem = 0;

  if(!m_vecItems.empty())
    pListItem =  m_vecItems[m_iPos];

  return pListItem;
}

pymms::gui::GUIListItem *pymms::gui::GUIListControl::getItemAt(int iPos)
{
  GUIListItem* pListItem = 0;

  if(!m_vecItems.empty() && (iPos > -1) && (iPos < size()))
    pListItem =  m_vecItems[iPos];

  return pListItem;
}

int pymms::gui::GUIListControl::getSelectedPosition() const
{ 
  return m_iPos;
}

string pymms::gui::GUIListControl::getAlignment() const
{ 
  return m_strAlignment;
}

void pymms::gui::GUIListControl::setAlignment(const string& strAlignment)
{ 
  m_strAlignment = strAlignment;
  m_btnItem.setAlignment(strAlignment);
}

bool pymms::gui::GUIListControl::onAction(const string& strAction)
{
  if(!m_vecItems.empty())
  {
    if(strAction == "prev")
    {
      if(m_iPos > 0)
        m_iPos--;
      else
        m_iPos = m_vecItems.size() - 1;

      return true;
    }

    else if(strAction == "next")
    {
      if(m_iPos < (m_vecItems.size() - 1))
        m_iPos++;
      else
        m_iPos = 0;

      return true;
    }

    if(strAction == "page_up")
    {
      int iVisibleItems = m_iHeight/(m_iItemHeight + m_iItemSpace);

      if(m_iPos > iVisibleItems)
        m_iPos--;
      else
        m_iPos = 0;

      return true;
    }

    else if(strAction == "page_down")
    {
      int iVisibleItems = m_iHeight/(m_iItemHeight + m_iItemSpace);

      if( (m_iPos == 0) && (m_vecItems.size() > iVisibleItems) )
        m_iPos += iVisibleItems;
      else if( (m_iPos > 0) && (m_iPos < (m_vecItems.size() - 1)) )
        m_iPos++;

      return true;
    }
  }

  return GUIControl::onAction(strAction);
}

bool pymms::gui::GUIListControl::onMessage(GUIMessage& message)
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

void pymms::gui::GUIListControl::setItemHeight(int iItemHeight)
{ 
  m_iItemHeight = (iItemHeight>0)?iItemHeight:m_iItemHeight;

  m_btnItem.setHeight(m_iItemHeight);
  m_lblSuffix.setHeight(m_iItemHeight);
  m_imgThumb.setWidth(m_iItemHeight);
  m_imgThumb.setHeight(m_iItemHeight);
}

void pymms::gui::GUIListControl::setItemSpace(int iItemSpace)
{ 
  m_iItemSpace = (iItemSpace>0)?iItemSpace:m_iItemSpace;
}

void pymms::gui::GUIListControl::setScaling(double dScalWidth, double dScalHeight)
{
  m_iPosX = int(m_iPosX * dScalWidth);
  m_iPosY = int(m_iPosY * dScalHeight);
  m_iWidth = int(m_iWidth * dScalWidth); 
  m_iHeight = int(m_iHeight * dScalHeight);

  m_iTextOffset = int(m_iTextOffset * dScalWidth);
  setItemHeight(int(m_iItemHeight * dScalHeight));
  m_iItemSpace = int(m_iItemSpace * dScalHeight);  
}

void pymms::gui::GUIListControl::render(Overlay* pOverlay)
{
  if(!m_vecItems.empty() && m_bVisible)
  {
    int iCutOff = 0;
    bool bHasThumbImage = false;

    if(m_iHeight < (m_iItemHeight + m_iItemSpace))
      m_iHeight = m_iItemHeight + m_iItemSpace;

    int iVisibleItems = m_iHeight/(m_iItemHeight + m_iItemSpace);
    int iDiff = m_iPos + 1 - iVisibleItems;

    if(iDiff <= 0)
    {
      for(int i = 0; i < iVisibleItems; i++)
      {
        if(i<m_vecItems.size())
        {
	  if(m_vecItems[i]->getThumbImage().length() > 0)
          {
            m_imgThumb.setPosition(m_iPosX + m_iTextOffset, m_iPosY + i*(m_iItemHeight + m_iItemSpace));
            m_imgThumb.setPath(m_vecItems[i]->getThumbImage());
            m_imgThumb.setLayer(m_iLayer + 1);
            m_imgThumb.render(pOverlay);

	    bHasThumbImage = true;
	  }

  	  if(m_vecItems[i]->getLabel2().length() > 0)
          {
            m_lblSuffix.setPosition(m_iPosX, m_iPosY + i*(m_iItemHeight + m_iItemSpace));
            m_lblSuffix.setWidth(m_iWidth);
	    m_lblSuffix.setLabel( (i<m_vecItems.size())?m_vecItems[i]->getLabel2():"" );
	    m_lblSuffix.setLayer(m_iLayer + 1);
	    m_lblSuffix.setRgb( ((i==m_iPos) && m_bFocus)?m_strRgbFocus:m_strRgb );

	    m_lblSuffix.render(pOverlay);

	    iCutOff = m_lblSuffix.getRealWidth() + 5;
   	  }

	  m_btnItem.setId(getId());
	  m_btnItem.setPosition(m_iPosX, m_iPosY + i*(m_iItemHeight + m_iItemSpace));
	  m_btnItem.setWidth(m_iWidth);
	  m_btnItem.setLabel( (i<m_vecItems.size())?m_vecItems[i]->getLabel():"" );
	  m_btnItem.setLayer(m_iLayer);
          m_btnItem.setTextOffset(m_iTextOffset + ((bHasThumbImage)?(m_iItemHeight+6):0));
	  m_btnItem.setCutOff(iCutOff);
          m_btnItem.setFocus((i == m_iPos) && m_bFocus);
	  m_btnItem.setRgb( (i == m_iPosExtra)?m_vecItems[i]->getRgb():m_strRgb );
	  m_btnItem.setRgbFocus( (i == m_iPosExtra)?m_vecItems[i]->getRgb():m_strRgbFocus );

	  S_TouchManager::get_instance()->registerControl(&m_btnItem, i);
	  m_btnItem.render(pOverlay);

	  iCutOff = 0;
	  bHasThumbImage = false;
        }
      }
    }
    else
    {
      for(int i = iDiff; i < (iVisibleItems + iDiff); i++)
      {
 	if(m_vecItems[i]->getThumbImage().length() > 0)
        {
	  m_imgThumb.setPosition(m_iPosX + m_iTextOffset, m_iPosY + (i-iDiff)*(m_iItemHeight + m_iItemSpace));
          m_imgThumb.setPath(m_vecItems[i]->getThumbImage());
          m_imgThumb.setLayer(m_iLayer + 1);
          m_imgThumb.render(pOverlay);

          bHasThumbImage = true;
        }

  	if(m_vecItems[i]->getLabel2().length() > 0)
        {
	  m_lblSuffix.setPosition(m_iPosX, m_iPosY + (i-iDiff)*(m_iItemHeight + m_iItemSpace));
          m_lblSuffix.setWidth(m_iWidth);
          m_lblSuffix.setLabel( (i<m_vecItems.size())?m_vecItems[i]->getLabel2():"" );
          m_lblSuffix.setLayer(m_iLayer + 1);
          m_lblSuffix.setRgb( ((i==m_iPos) && m_bFocus)?m_strRgbFocus:m_strRgb );

          m_lblSuffix.render(pOverlay);

          iCutOff = m_lblSuffix.getRealWidth() + 5;
	}

	m_btnItem.setId(getId());
	m_btnItem.setPosition(m_iPosX, m_iPosY + (i-iDiff)*(m_iItemHeight + m_iItemSpace));
        m_btnItem.setWidth(m_iWidth);
        m_btnItem.setLabel( (i<m_vecItems.size())?m_vecItems[i]->getLabel():"" );
        m_btnItem.setLayer(m_iLayer);
	m_btnItem.setTextOffset(m_iTextOffset + ((bHasThumbImage)?(m_iItemHeight+6):0));
        m_btnItem.setCutOff(iCutOff);
        m_btnItem.setFocus((i == m_iPos) && m_bFocus);
	m_btnItem.setRgb( (i == m_iPosExtra)?m_vecItems[i]->getRgb():m_strRgb );
	m_btnItem.setRgbFocus( (i == m_iPosExtra)?m_vecItems[i]->getRgb():m_strRgbFocus );

	S_TouchManager::get_instance()->registerControl(&m_btnItem, i);
        m_btnItem.render(pOverlay);

        iCutOff = 0;
        bHasThumbImage = false;
      }
    }
  }
}  
