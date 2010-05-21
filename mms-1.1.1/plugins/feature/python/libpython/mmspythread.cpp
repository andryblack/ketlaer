#include "mmspythread.hpp"
#include "mmspython.hpp"
#include <iostream>

using std::string;

pymms::MMSPyThread::MMSPyThread(void* pExecuter, PyThreadState* pMainThreadState, int iId)
{
  m_pExecuter = pExecuter;
  m_iId = iId;

  // get the global lock
  PyEval_AcquireLock();

  // get a reference to the PyInterpreterState
  PyInterpreterState* pMainInterpreterState = pMainThreadState->interp;

  // create a thread state object for this thread
  m_pThreadState = PyThreadState_New(pMainInterpreterState);

  // clear the thread state and free the lock
  PyThreadState_Swap(NULL);
  PyEval_ReleaseLock();
}

pymms::MMSPyThread::~MMSPyThread()
{
  terminate();
}

int pymms::MMSPyThread::evalFile(const string& strFile)
{
  m_cType = 'F';
  m_strSource = strFile;
  start();

  return 0;
}

void pymms::MMSPyThread::run()
{
  string strSourceDir;
  
  PyEval_AcquireLock();

  // swap in my thread state
  PyThreadState_Swap(m_pThreadState);

  strSourceDir = m_strSource.substr(0, m_strSource.rfind("/"));
  chdir(strSourceDir.c_str());

  if (m_cType == 'F') {
    // run script from file
    FILE* fp = fopen(m_strSource.c_str(), "r");

    if (fp)
    {
      if (PyRun_SimpleFile(fp, m_strSource.c_str()) == -1)
        std::cerr << "PyRun_SimpleFile return -1" << std::endl;
      

      fclose(fp);
    }
    else
     std::cerr << m_strSource << " not found!\n";
  }

  PyObject* pyErr = 0;

  if( (pyErr = PyErr_Occurred()) )
  {
    std::cerr << "Unhandled Exception pending:" << std::endl;
    PyErr_WriteUnraisable(pyErr);
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

void pymms::MMSPyThread::final()
{
  ((MMSPython*)m_pExecuter)->setDone(m_iId);

  delete this;
}
