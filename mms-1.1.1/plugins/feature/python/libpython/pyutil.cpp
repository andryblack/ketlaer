#include "pyutil.hpp"
#include <vector>
#include <iostream>

typedef std::pair<int(*)(void*), void*> Func;
typedef std::vector<Func> CallQueue;
static CallQueue g_callQueue;
static ost::Mutex g_critSectionPyCall("CritSectionPyCall");
static ost::Mutex g_critSectionPyReturn("CritSectionPyReturn");
static ost::Event g_actionEvent;
static ost::Event g_returnEvent;
static bool g_isWaiting = false;

using std::string;
//using std::wstring;

void _Py_AddPendingCall(int(*func)(void*), void *arg)
{
  g_critSectionPyCall.enterMutex();
  g_callQueue.push_back(Func(func, arg));
  g_critSectionPyCall.leaveMutex();
}

void _Py_MakePendingCalls()
{
  g_critSectionPyCall.enterMutex();

  CallQueue::iterator iter = g_callQueue.begin();
  while (iter != g_callQueue.end())
  {
    int(*f)(void*) = (*iter).first;
    void* arg = (*iter).second;
    g_callQueue.erase(iter);
    g_critSectionPyCall.leaveMutex();
    if (f)
      f(arg);
    //(*((*iter).first))((*iter).second);
    g_critSectionPyCall.enterMutex();
    iter = g_callQueue.begin();
  }
  g_critSectionPyCall.leaveMutex();  
}

void Py_WaitForActionEvent()
{
  g_actionEvent.wait();
  g_actionEvent.reset();
}

void Py_PulseActionEvent()
{
  g_actionEvent.signal();
}

void Py_SetWaiting()
{
  g_critSectionPyReturn.enterMutex();
  g_isWaiting = true;
  g_critSectionPyReturn.leaveMutex();
}

void Py_WaitForReturnEvent()
{
  g_returnEvent.wait();

  g_critSectionPyReturn.enterMutex();
  g_isWaiting = false;
  g_critSectionPyReturn.leaveMutex();

  g_returnEvent.reset();
}

void Py_PulseReturnEvent()
{
  g_critSectionPyReturn.enterMutex();
  if(g_isWaiting)
    g_returnEvent.signal();
  g_critSectionPyReturn.leaveMutex();
}

int Py_wCharToChar(std::string& s, PyObject* pObject)
{
#if 0 //ADI
  if(PyUnicode_Check(pObject)) {
    Py_ssize_t size = PyUnicode_GET_DATA_SIZE(pObject);
    char buf[size];

    if(wcstombs(buf, (wchar_t*)PyUnicode_AS_UNICODE(pObject), size) != -1)
      s = buf;

    return 1;
  }
  else
    if(PyString_Check(pObject)) {
      s = PyString_AsString(pObject);
 
      return 1;
    }
    else
      return 0;
#else
  s = PyString_AsString(pObject);
  return 1;
#endif
}
