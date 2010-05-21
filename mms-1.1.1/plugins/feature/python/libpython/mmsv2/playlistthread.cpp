#include "playlistthread.hpp"
#include "guiwindowmanager.hpp"
#include "pyutil.hpp"
#include <iostream>

using std::string;

pymms::PlayListThread::PlayListThread(PyObject* pObject)
{
  m_pObject = pObject;
  m_bIsRunning = false;

  // get a reference to the PyInterpreterState
  PyInterpreterState* pMainInterpreterState = PyThreadState_Get()->interp;

  // create a thread state object for this thread
  m_pThreadState = PyThreadState_New(pMainInterpreterState);
}

pymms::PlayListThread::~PlayListThread()
{
  terminate();
}

void pymms::PlayListThread::closeThread()
{
  m_bIsRunning = false;
  wait();
}

void pymms::PlayListThread::run()
{
  sleep(1000);
  m_bIsRunning = true;

  PyEval_AcquireLock();

  // swap in my thread state
  PyThreadState_Swap(m_pThreadState);
 
  while(m_bIsRunning)
  {
    if(! ((python::Player*)m_pObject)->pPlayer->isPlaying() )
    {
      PyPlayListAction* pAction = new PyPlayListAction;

      pAction->pCallback = m_pObject;

      Py_AddPendingCall(Py_MMS_CheckPlaylist, pAction);
      Py_PulseActionEvent();
    }

    Py_BEGIN_ALLOW_THREADS    
    sleep(1000);
    Py_END_ALLOW_THREADS
  }

  // clear the thread state
  PyThreadState_Swap(NULL);

  // clear out any cruft from thread state object
  PyThreadState_Clear(m_pThreadState);

  // delete my thread state object
  PyThreadState_Delete(m_pThreadState);

  // release the lock
  PyEval_ReleaseLock();  
}

int pymms::Py_MMS_CheckPlaylist(void* pArg)
{
  if (pArg != NULL)
  {
    PyPlayListAction* pAction = (PyPlayListAction*)pArg;

    PyObject* pRet = PyObject_CallMethod(pAction->pCallback, "_playListNext", NULL, NULL);

    pymms::gui::S_WindowManager::get_instance()->render();

    if(pRet)
      Py_DECREF(pRet);

    delete pAction;
  }

  return 0;
}
