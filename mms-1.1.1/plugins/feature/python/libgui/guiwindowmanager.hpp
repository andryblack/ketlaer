#ifndef GUIWINDOWMANAGER_HPP
#define GUIWINDOWMANAGER_HPP

#include <cc++/thread.h>
#include "singleton.hpp"
#include "renderer.hpp"
#include "guiwindow.hpp"
#include "guimessage.hpp"
#include "input.hpp"
#include <vector>
#include <string>

namespace pymms {

namespace gui {

class GUIWindowManager
{
  private:
    std::vector<GUIWindow*> m_vecWindows;
    Overlay m_windowOverlay;
    InputMaster* m_pInputMaster;
    ost::Mutex m_windowManagerLock;

  
  public:

    static pthread_mutex_t singleton_mutex;

    GUIWindowManager();
    bool onAction(const std::string& strAction);
    void registerWindow(GUIWindow* pWindow);
    void unregisterWindow(GUIWindow* pWindow);
    void setActiveWindow(GUIWindow* pWindow);
    void setActiveWindow(int iWindowId);
    void setDefaultActive(bool bActive);
    GUIWindow* getActiveWindow();
    GUIWindow* getWindow(int iWindowId);
    void render();
    void __render();
    void sendMessage(GUIMessage& message);
    std::string getInput();
    void setInputMap(const std::string& strInputMap);
    std::string getInputMap();
};

typedef Singleton<GUIWindowManager> S_WindowManager;

}

}

#endif
