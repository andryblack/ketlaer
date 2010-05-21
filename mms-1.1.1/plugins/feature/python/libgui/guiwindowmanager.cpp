#include "guiwindowmanager.hpp"
#include "guiutil.hpp"
#include "guitouchmanager.hpp"

using std::string;

pthread_mutex_t pymms::gui::GUIWindowManager::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

pymms::gui::GUIWindowManager::GUIWindowManager() : m_windowOverlay(Overlay("WindowManager")), m_windowManagerLock("windowmanager")
{
  m_pInputMaster = S_InputMaster::get_instance();
}

void pymms::gui::GUIWindowManager::setInputMap(const string& strInputMap)
{
  ost::MutexLock lock(m_windowManagerLock);

  m_pInputMaster->set_map(strInputMap);
}

string pymms::gui::GUIWindowManager::getInputMap()
{
  ost::MutexLock lock(m_windowManagerLock);

  return m_pInputMaster->current_map();
}

string pymms::gui::GUIWindowManager::getInput()
{
  Input input = m_pInputMaster->get_input();

  if(getInputMap() == "search")
  {
    if( input.command.empty() || !GUIInputUtil::isSearchCommand(input.command) )
      input.command = input.key;
  }

  if(input.key == "touch_input")
  {
    S_TouchManager::get_instance()->runCallback();
    input.command = "action";
  }

  return input.command;
}

bool pymms::gui::GUIWindowManager::onAction(const string& strAction)
{
  if(!m_vecWindows.empty())
    return  m_vecWindows.front()->onAction(strAction);
  else
     return false;
}

void pymms::gui::GUIWindowManager::setDefaultActive(bool bActive)
{
  ost::MutexLock lock(m_windowManagerLock);

  GUIWindow* pFrontWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);
  if(pFrontWindow)
  {
    pFrontWindow->setActive(bActive);

    if(bActive)
      __render();
  }
}

void pymms::gui::GUIWindowManager::registerWindow(GUIWindow* pWindow)
{
  ost::MutexLock lock(m_windowManagerLock);

  for (std::vector<GUIWindow*>::iterator i = m_vecWindows.begin(),
       end = m_vecWindows.end(); i != end; i++) {
    if((*i) == pWindow)
    {
      return;
    }
  }

  m_vecWindows.push_back(pWindow);
}

void pymms::gui::GUIWindowManager::unregisterWindow(GUIWindow* pWindow)
{
  ost::MutexLock lock(m_windowManagerLock);

  Render* pRenderer = S_Render::get_instance();

  for (std::vector<GUIWindow*>::iterator i = m_vecWindows.begin(),
       end = m_vecWindows.end(); i != end; i++) {
    if((*i) == pWindow)
    {
      if(pWindow->getOverlay())
      {
          pRenderer->wait_and_aquire();
          m_windowOverlay.cleanup();
  	  pRenderer->image_mut.leaveMutex();
      }

      m_vecWindows.erase(i);
      pWindow->setActive(false);

      GUIWindow* pFrontWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);
      if(pFrontWindow)
        pFrontWindow->setActive(true);
    
      __render();

      return;
    }
  }
}

void pymms::gui::GUIWindowManager::setActiveWindow(GUIWindow* pWindow)
{
  ost::MutexLock lock(m_windowManagerLock);

  for (std::vector<GUIWindow*>::iterator i = m_vecWindows.begin(),
       end = m_vecWindows.end(); i != end; i++) {
    if((*i) == pWindow)
    {
      GUIWindow* pFrontWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);
      if((pFrontWindow != pWindow) && pFrontWindow)
        pFrontWindow->setActive(false);

      m_vecWindows.erase(i);
      m_vecWindows.insert(m_vecWindows.begin(), pWindow);
      pWindow->setActive(true);

      __render();

      return;
    }
  }
}

void pymms::gui::GUIWindowManager::setActiveWindow(int iWindowId)
{
  ost::MutexLock lock(m_windowManagerLock);

  for (std::vector<GUIWindow*>::iterator i = m_vecWindows.begin(),
       end = m_vecWindows.end(); i != end; i++) {
    if((*i)->getId() == iWindowId)
    {
      GUIWindow* pFrontWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);
      if((pFrontWindow != *i) && pFrontWindow)
        pFrontWindow->setActive(false);

      m_vecWindows.erase(i);
      m_vecWindows.insert(m_vecWindows.begin(), *i);
      (*i)->setActive(true);

      __render();

      return;
    }
  }
}

pymms::gui::GUIWindow* pymms::gui::GUIWindowManager::getActiveWindow()
{
  ost::MutexLock lock(m_windowManagerLock);

  if(!m_vecWindows.empty())
    return m_vecWindows.front();
  else
    return 0;
}

pymms::gui::GUIWindow* pymms::gui::GUIWindowManager::getWindow(int iWindowId)
{
  ost::MutexLock lock(m_windowManagerLock);

  for (std::vector<GUIWindow*>::iterator i = m_vecWindows.begin(),
       end = m_vecWindows.end(); i != end; i++) {
    if((*i)->getId() == iWindowId)
    {
      return (*i);
    }
  }

  return 0;
}

void pymms::gui::GUIWindowManager::render()
{
  ost::MutexLock lock(m_windowManagerLock);

  Render* pRenderer = S_Render::get_instance();
  GUIWindow* pWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);

  if(pWindow && pWindow->getActive())
  {
    if(!pWindow->getOverlay())
    {
      pRenderer->prepare_new_image();

      pWindow->render();

      pRenderer->draw_and_release("Window");
    }
    else
    {
     pRenderer->wait_and_aquire();

     if (m_windowOverlay.elements.size() > 0)
        m_windowOverlay.partial_cleanup();

     pWindow->render(&m_windowOverlay);

     pRenderer->draw_and_release("Window", true);
    }
  }
}

void pymms::gui::GUIWindowManager::__render()
{
  Render* pRenderer = S_Render::get_instance();
  GUIWindow* pWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);

  if(pWindow && pWindow->getActive())
  {
    if(!pWindow->getOverlay())
    {
      pRenderer->prepare_new_image();

      pWindow->render();

      pRenderer->draw_and_release("Window");
    }
    else
    {
     pRenderer->wait_and_aquire();

     if (m_windowOverlay.elements.size() > 0)
        m_windowOverlay.partial_cleanup();

     pWindow->render(&m_windowOverlay);

     pRenderer->draw_and_release("Window", true);
    }
  }
}

void pymms::gui::GUIWindowManager::sendMessage(GUIMessage& message)
{ 
  GUIWindow* pWindow = (!m_vecWindows.empty()?m_vecWindows.front():0);

  if(pWindow && pWindow->getActive())
     pWindow->onMessage(message);
}
