#include "guiwindow.hpp"
#include <iostream>
#include "guicontrolfactory.hpp"
#include "guiwindowmanager.hpp"
#include "guitouchmanager.hpp"
#include "libfspp.hpp"
#include "config.hpp"
#include <algorithm>
#include <functional>

using std::vector;
using std::string;
using std::for_each;
using std::find_if;
using std::bind2nd;
using std::mem_fun;
using std::ptr_fun;

bool pymms::gui::cmpControlId(GUIControl* pControl, int iControlId)
{
   if(pControl->getId() == iControlId)
      return true;
   else
      return false;
}

pymms::gui::GUIWindow::GUIWindow()
{
  m_iWindowId = 0;
  m_bUseOverlay = false;
  m_dScalWidth = m_dScalHeight = 1.0;
  m_iDefaultControl = -1;
  m_bActive = false;
}

pymms::gui::GUIWindow::~GUIWindow()
{
}

void pymms::gui::GUIWindow::render(Overlay* pOverlay)
{
  S_TouchManager::get_instance()->clear();

  for (GUIControlIterator i = m_vecControls.begin(),
          end = m_vecControls.end(); i != end; i++)
  {
    if( ((*i)->getType() != LISTCONTROL) and ((*i)->getType() != IMAGELISTCONTROL) )
      S_TouchManager::get_instance()->registerControl(*i);

    (*i)->render(pOverlay);
  }  
}

bool pymms::gui::GUIWindow::getOverlay() const
{
   return m_bUseOverlay;
}

void pymms::gui::GUIWindow::setOverlay(bool bUseOverlay)
{
   m_bUseOverlay = bUseOverlay;
}

bool pymms::gui::GUIWindow::addControl(GUIControl* pControl)
{
  if(pControl)
  {
     pControl->setScaling(m_dScalWidth, m_dScalHeight);
     pControl->setParentId(m_iWindowId);
     m_vecControls.push_back(pControl);

     return true;
  }

  return false;
}

pymms::gui::GUIControl* pymms::gui::GUIWindow::getControl(int iControlId)
{
   GUIControlIterator pos = find_if(m_vecControls.begin(), 
				    m_vecControls.end(), 
				    bind2nd(ptr_fun(cmpControlId), iControlId));

   if(pos != m_vecControls.end())
      return *pos;
   else
      return 0;
}

void pymms::gui::GUIWindow::setActive(bool bActive)
{
  if(bActive && !m_bActive)
  {
    GUIMessage focusMessage(GUI_MSG_SETFOCUS, getId(), m_iDefaultControl);
    onMessage(focusMessage);

    m_bActive = true;
  }

  else if(!bActive && m_bActive)
  {
    m_iDefaultControl = getFocus();

    GUIMessage focusMessage(GUI_MSG_LOSTFOCUS, getId(), m_iDefaultControl);
    onMessage(focusMessage);

    m_bActive = false;
  }
}

bool pymms::gui::GUIWindow::getActive() const
{
  return m_bActive;
}

void pymms::gui::GUIWindow::setFocus(int iControl)
{
  if(m_bActive)
  {
    GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, getId(), iControl);
    S_WindowManager::get_instance()->sendMessage(visibleMessage);

    if(visibleMessage.getParam1())
    {
      GUIMessage focusMessage(GUI_MSG_SETFOCUS, getId(), iControl);
      onMessage(focusMessage);
    }
  }

  else
  {
    m_iDefaultControl = iControl;
  }
}

bool pymms::gui::GUIWindow::onAction(const string& strAction)
{
   GUIControlIterator pos = find_if(m_vecControls.begin(), 
				    m_vecControls.end(), 
			 	    mem_fun(&GUIControl::getFocus));

   if(pos != m_vecControls.end())
      return (*pos)->onAction(strAction);
   else
      return false;
}

bool pymms::gui::GUIWindow::onMessage(GUIMessage& message)
{
  switch(message.getMessage())
  {
    case GUI_MSG_SETFOCUS:
    {
       if(m_bActive)
       {
         int iControl = getFocus();
     
         if(iControl != -1)
         {
           GUIMessage focusMessage(GUI_MSG_LOSTFOCUS, getId(), iControl);
           onMessage(focusMessage);
         }
       }

       break;
    }
  } 
  
  GUIControlIterator pos = find_if(m_vecControls.begin(), 
				   m_vecControls.end(), 
				   bind2nd(ptr_fun(cmpControlId), message.getControlId()));

  if(pos != m_vecControls.end())
     return (*pos)->onMessage(message);
  else
     return false;
}

int pymms::gui::GUIWindow::getFocus() const
{
   GUIControlConstIterator pos = find_if(m_vecControls.begin(), 
				         m_vecControls.end(), 
			 	         mem_fun(&GUIControl::getFocus));

   if(pos != m_vecControls.end())
      return (*pos)->getId();
   else
      return -1;
}

bool pymms::gui::GUIWindow::removeControl(int iControl)
{
   GUIControlIterator pos = find_if(m_vecControls.begin(), 
				    m_vecControls.end(), 
			 	    bind2nd(ptr_fun(cmpControlId), iControl));

   if(pos != m_vecControls.end())
   {
      m_vecControls.erase(pos);

      return true;
   }
   else
      return false;
}

int pymms::gui::GUIWindow::getWidth() const
{
  Config* pConfig = S_Config::get_instance();

  return pConfig->p_h_res();
}

int pymms::gui::GUIWindow::getHeight() const
{
  Config* pConfig = S_Config::get_instance();

  return pConfig->p_v_res();
}

void pymms::gui::GUIWindow::setId(int iWindowId)
{
  m_iWindowId = iWindowId;

  for_each(m_vecControls.begin(), m_vecControls.end(), 
	   bind2nd(mem_fun(&GUIControl::setParentId), iWindowId));
}

int pymms::gui::GUIWindow::getId() const
{
  return m_iWindowId;
}

void pymms::gui::GUIWindow::clear()
{
   for (GUIControlIterator i = m_vecControls.begin(),
          end = m_vecControls.end(); i != end; i++) 
   {
      (*i)->freeResources();
      delete (*i);
   }

   m_vecControls.clear();
}

void pymms::gui::GUIWindow::setScalWidth(double dScalWidth)
{
   m_dScalWidth = dScalWidth;
}
    
void pymms::gui::GUIWindow::setScalHeight(double dScalHeight)
{
   m_dScalHeight = dScalHeight;
}

double pymms::gui::GUIWindow::getScalWidth()
{
   return m_dScalWidth;
}

double pymms::gui::GUIWindow::getScalHeight()
{
   return m_dScalHeight;
}

void pymms::gui::GUIWindow::loadControl(const TiXmlNode* pControl)
{
   TiXmlNode* pNode = pControl->FirstChild("type");
   if (pNode)
   {
      GUIControlFactory controlFactory;
      GUIControl* pGUIControl = controlFactory.create(pControl);
      
      if(pGUIControl)
         addControl(pGUIControl);
   }
}

bool pymms::gui::GUIWindow::load(const string& strFileName)
{
   int iDefaultControl = -1;
   TiXmlDocument xmlDoc;
   Render* renderer = S_Render::get_instance();
   string strPath;

   // Find appropriate skin folder + resolution to load from
   if(file_exists(strFileName))
      strPath = strFileName;
   else if(strFileName.length() > 0)
       strPath = renderer->default_path + strFileName;

   if(!xmlDoc.LoadFile(strPath.c_str()))
   {
      return false;
   }

   TiXmlElement* pRootElement = xmlDoc.RootElement();
   string strValue = pRootElement->Value();
   if(strValue != "window")
   {
      return false;
   }

   const TiXmlNode* pChild = pRootElement->FirstChild();
   while (pChild)
   {
      string strValue = pChild->Value();
      if (strValue == "id")
      {
         setId(atoi(pChild->FirstChild()->Value()));
      }
      else if(strValue == "defaultcontrol")
      {
         iDefaultControl = atoi(pChild->FirstChild()->Value());
      }
      else if(strValue == "width")
      {
	 Config* pConfig = S_Config::get_instance();

         setScalWidth( ((double)pConfig->p_h_res()) / atoi(pChild->FirstChild()->Value()) );
      }
      else if(strValue == "height")
      {
         Config* pConfig = S_Config::get_instance();

         setScalHeight( ((double)pConfig->p_v_res()) / atoi(pChild->FirstChild()->Value()) );
      }
      else if (strValue == "controls")
      {
         const TiXmlNode* pControl = pChild->FirstChild("control");
         while (pControl)
         {
            loadControl(pControl);
            pControl=pControl->NextSibling("control");
         }
      }

      pChild=pChild->NextSibling();
   }

   setFocus(iDefaultControl);

   return true;
}
