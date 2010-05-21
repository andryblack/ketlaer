#include "guicontrol.hpp"
#include "guiwindowmanager.hpp"

using std::string;

pymms::gui::GUIControl::GUIControl(int iPosX, int iPosY, int iWidth, 
				   int iHeight, int iLayer, const string& strInputMap)
{
  m_iPosX = (iPosX>-1)?iPosX:0;
  m_iPosY = (iPosY>-1)?iPosY:0;
  m_iWidth = (iWidth>-1)?iWidth:0;
  m_iHeight = (iHeight>-1)?iHeight:0;
  m_iLayer = (iLayer>-1)?iLayer:0;
  m_bFocus = false;
  m_bVisible = true;
  m_iRight = m_iLeft = m_iPrev = m_iNext = -1;
  m_iControlId = 0;
  m_iParentId = 0;
  m_iControlType = CONTROL;
  m_strInputMap = strInputMap;
  m_iRouteControl = -1;
}

void pymms::gui::GUIControl::reset()
{ 
  m_bFocus = false;
  m_bVisible = true;
  m_iRight = m_iLeft = m_iPrev = m_iNext = -1;
  m_iControlId = 0;
  m_iParentId = 0;
}

void pymms::gui::GUIControl::controlLeft(const GUIControl* pLeft)
{ 
  m_iLeft = pLeft->getId();
}

void pymms::gui::GUIControl::controlRight(const GUIControl* pRight)
{ 
  m_iRight = pRight->getId();
}

void pymms::gui::GUIControl::controlPrev(const GUIControl* pPrev)
{ 
  m_iPrev = pPrev->getId();
}

void pymms::gui::GUIControl::controlNext(const GUIControl* pNext)
{ 
  m_iNext = pNext->getId();
}

void pymms::gui::GUIControl::controlLeft(int iLeft)
{ 
  m_iLeft = iLeft;
}

void pymms::gui::GUIControl::controlRight(int iRight)
{ 
  m_iRight = iRight;
}

void pymms::gui::GUIControl::controlPrev(int iPrev)
{ 
  m_iPrev = iPrev;
}

void pymms::gui::GUIControl::controlNext(int iNext)
{ 
  m_iNext = iNext;
}

void pymms::gui::GUIControl::setFocus(bool bFocus)
{ 
  m_bFocus = bFocus;
}

bool pymms::gui::GUIControl::getFocus() const
{ 
  return m_bFocus;
}

void pymms::gui::GUIControl::setVisible(bool bVisible)
{ 
  m_bVisible = bVisible;
}

bool pymms::gui::GUIControl::getVisible() const
{ 
  return m_bVisible;
}

void pymms::gui::GUIControl::setHeight(int iHeight)
{ 
  m_iHeight = (iHeight>-1)?iHeight:m_iHeight;
}

int pymms::gui::GUIControl::getHeight() const
{ 
  return m_iHeight;
}

void pymms::gui::GUIControl::setWidth(int iWidth)
{ 
  m_iWidth = (iWidth>-1)?iWidth:m_iWidth;
}

int pymms::gui::GUIControl::getWidth() const
{ 
  return m_iWidth;
}

void pymms::gui::GUIControl::setRouteControl(int iRouteControl)
{
  m_iRouteControl = iRouteControl;
}

int pymms::gui::GUIControl::getRouteControl() const
{
  return m_iRouteControl;
}

void pymms::gui::GUIControl::setLayer(int iLayer)
{ 
  m_iLayer = (iLayer>-1)?iLayer:m_iLayer;
}

int pymms::gui::GUIControl::getLayer() const
{ 
  return m_iLayer;
}

void pymms::gui::GUIControl::setId(int iControlId)
{ 
  m_iControlId = iControlId;
}

int pymms::gui::GUIControl::getId() const
{ 
  return m_iControlId;
}

void pymms::gui::GUIControl::setParentId(int iParentId)
{ 
  m_iParentId = iParentId;
}

int pymms::gui::GUIControl::getParentId() const
{ 
  return m_iParentId;
}

int pymms::gui::GUIControl::getType()
{
  return m_iControlType;
}

void pymms::gui::GUIControl::setPosition(int iPosX, int iPosY)
{ 
  m_iPosX = (iPosX>-1)?iPosX:m_iPosX;
  m_iPosY = (iPosY>-1)?iPosY:m_iPosY;
}

void pymms::gui::GUIControl::setPosX(int iPosX)
{
  m_iPosX = (iPosX>-1)?iPosX:m_iPosX;
}

void pymms::gui::GUIControl::setPosY(int iPosY)
{
  m_iPosY = (iPosY>-1)?iPosY:m_iPosY;
}

int pymms::gui::GUIControl::getPosX()
{
  return m_iPosX;
}

int pymms::gui::GUIControl::getPosY()
{
  return m_iPosY;
}

void pymms::gui::GUIControl::onRight() const
{
  if(m_iRight != -1)
  {
     GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, getId(), m_iRight);
     S_WindowManager::get_instance()->sendMessage(visibleMessage);

     if(visibleMessage.getParam1())
     {
       GUIMessage focusMessage(GUI_MSG_SETFOCUS, getId(), m_iRight);
       S_WindowManager::get_instance()->sendMessage(focusMessage);
     }
  }
}

void pymms::gui::GUIControl::onLeft() const
{
  if(m_iLeft != -1)
  {
     GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, getId(), m_iLeft);
     S_WindowManager::get_instance()->sendMessage(visibleMessage);

     if(visibleMessage.getParam1())
     {
       GUIMessage focusMessage(GUI_MSG_SETFOCUS, getId(), m_iLeft);
       S_WindowManager::get_instance()->sendMessage(focusMessage);
     }
  }
}

void pymms::gui::GUIControl::onPrev() const
{
  if(m_iPrev != -1)
  {
     GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, getId(), m_iPrev);
     S_WindowManager::get_instance()->sendMessage(visibleMessage);

     if(visibleMessage.getParam1())
     {
       GUIMessage focusMessage(GUI_MSG_SETFOCUS, getId(), m_iPrev);
       S_WindowManager::get_instance()->sendMessage(focusMessage);
     }
  }
}

void pymms::gui::GUIControl::onNext() const
{
  if(m_iNext != -1)
  {
     GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, getId(), m_iNext);
     S_WindowManager::get_instance()->sendMessage(visibleMessage);

     if(visibleMessage.getParam1())
     {
       GUIMessage focusMessage(GUI_MSG_SETFOCUS, getId(), m_iNext);
       S_WindowManager::get_instance()->sendMessage(focusMessage);
     }
  }
}

void pymms::gui::GUIControl::onClick() const
{
  GUIMessage clickMessage(GUI_MSG_CLICKED, getId(), getId());
  S_WindowManager::get_instance()->sendMessage(clickMessage);
}

bool pymms::gui::GUIControl::onAction(const string& strAction)
{
  if(m_iRouteControl != -1)
  {
    GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, 0, m_iRouteControl);
    S_WindowManager::get_instance()->sendMessage(visibleMessage);

    if(visibleMessage.getParam1())
    {
      if(strAction != "action")
      {
        GUIMessage actionMessage(GUI_MSG_ACTION, 0, m_iRouteControl);
	actionMessage.setAction(strAction);

        S_WindowManager::get_instance()->sendMessage(actionMessage);
      }
    }

    return true;
  }

  if(strAction == "right")
  {
    onRight();
    
    return true;
  }

  else if(strAction == "left")
  {
    onLeft();

    return true;
  }

  else if(strAction == "prev")
  {
    onPrev();

    return true;
  }

  else if(strAction == "next")
  {
    onNext();

    return true;
  }

  else if(strAction == "action")
  {
    onClick();

    return true;
  }

  return false;
}

bool pymms::gui::GUIControl::onMessage(GUIMessage& message)
{
  if (message.getControlId() == getId())
  {
     switch (message.getMessage())
     {
	case GUI_MSG_SETFOCUS:
	{
	   if(m_strInputMap != S_WindowManager::get_instance()->getInputMap())
             S_WindowManager::get_instance()->setInputMap(m_strInputMap);

           setFocus(true);

	   return true;
	}

	case GUI_MSG_LOSTFOCUS:
	{
	   setFocus(false);

	   return true;
	}
	
	case GUI_MSG_VISIBLE:
	{
	   setVisible(true);

	   return true;
	}

	case GUI_MSG_HIDDEN:
	{
	   setVisible(false);

	   return true;
	}

	case GUI_MSG_ISVISIBLE:
	{
	   if(getVisible())
	     message.setParam1(1);
	   else
	     message.setParam1(0);

	   return true;
	}
     }
  }

  return false;
}

void pymms::gui::GUIControl::freeResources()
{
   reset();
}
