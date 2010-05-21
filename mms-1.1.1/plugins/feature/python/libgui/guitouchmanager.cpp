#include "guitouchmanager.hpp"
#include "guiwindowmanager.hpp"
#include "guimessage.hpp"
#include "touch.hpp"

pthread_mutex_t pymms::gui::GUITouchManager::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

pymms::gui::GUITouchManager::GUITouchManager()
{
}

void pymms::gui::GUITouchManager::registerControl(GUIControl* pControl, int iParam1, int iParam2)
{
  if(pControl->getVisible())
    S_Touch::get_instance()->register_area(TouchArea(rect((pControl)->getPosX(), (pControl)->getPosY(),
                                (pControl)->getWidth(), (pControl)->getHeight()),(pControl)->getLayer(), 
				boost::bind(&GUITouchManager::onTouch, this, (pControl)->getId(), iParam1, iParam2)));
}
    
bool pymms::gui::GUITouchManager::runCallback()
{
  S_Touch::get_instance()->run_callback();
}

void pymms::gui::GUITouchManager::clear()
{
  S_Touch::get_instance()->clear();
}

void pymms::gui::GUITouchManager::onTouch(int iControl, int iParam1, int iParam2)
{
  GUIMessage visibleMessage(GUI_MSG_ISVISIBLE, 0, iControl);
  S_WindowManager::get_instance()->sendMessage(visibleMessage);

  if(visibleMessage.getParam1())
  {
    GUIMessage focusMessage(GUI_MSG_SETFOCUS, 0, iControl);
    S_WindowManager::get_instance()->sendMessage(focusMessage);

    GUIMessage touchMessage(GUI_MSG_TOUCH, 0, iControl, iParam1, iParam2);
    S_WindowManager::get_instance()->sendMessage(touchMessage);
  }
}
