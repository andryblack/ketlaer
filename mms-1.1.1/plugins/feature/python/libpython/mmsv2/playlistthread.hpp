#ifndef PLAYLISTTHREAD_HPP
#define PLAYLISTTHREAD_HPP

class PlayListThread;

#include "pyplayer.hpp"
#include <cc++/thread.h>
#include <string>

namespace pymms {

class PlayListThread : public ost::Thread
{
  public:
    PlayListThread(PyObject* pObject);
    virtual ~PlayListThread();
    void closeThread();
	
  protected:
    void run();
    
  private:
    bool m_bIsRunning;
    PyObject* m_pObject; 
    PyThreadState* m_pThreadState;
};

class PyPlayListAction
{
public:
  PyObject* pCallback;

  PyPlayListAction() { pCallback = 0;}
};

int Py_MMS_CheckPlaylist(void* pArg);

}

#endif
