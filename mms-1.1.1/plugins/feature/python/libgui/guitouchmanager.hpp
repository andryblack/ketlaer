#ifndef GUITOUCHMANAGER_HPP
#define GUITOUCHMANAGER_HPP

#include "singleton.hpp"
#include "guicontrol.hpp"

namespace pymms {

namespace gui {

class GUITouchManager
{
  public:
    static pthread_mutex_t singleton_mutex;

    GUITouchManager();
    void registerControl(GUIControl* pControl, int iParam1=0, int iParam2=0);
    bool runCallback();
    void clear();
    void onTouch(int iControl, int iParam1, int iParam2);
};

typedef Singleton<GUITouchManager> S_TouchManager;

}

}

#endif
