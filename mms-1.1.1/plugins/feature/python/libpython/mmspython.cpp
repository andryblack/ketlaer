#include "mmspython.hpp"

using std::string;

pthread_mutex_t pymms::MMSPython::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

pymms::MMSPython::MMSPython() : m_critSection("mmspython")
{
  m_bInitialized = false;
  m_iNextId = 0;
  m_pMainThreadState = NULL;
}

void pymms::MMSPython::initialize()
{
  ost::MutexLock lock(m_critSection);
  if(!m_bInitialized)
  {
    Py_Initialize();
    PyEval_InitThreads();
    m_pMainThreadState = PyThreadState_Get();
			
    // release the lock
    PyEval_ReleaseLock();

    m_bInitialized = true;
  }
}

int pymms::MMSPython::evalFile(const string& strFile)
{
  initialize();
  ost::MutexLock lock(m_critSection);
  if (!m_bInitialized)
    return -1;


  MMSPyThread* pPyThread = new MMSPyThread(this, m_pMainThreadState, ++m_iNextId);
  pPyThread->evalFile(strFile);

  PyElem pyElem;
  pyElem.iId = m_iNextId;
  pyElem.bDone = false;
  pyElem.strFile = strFile;
  pyElem.pPyThread = pPyThread;

  m_vecPyList.push_back(pyElem);

  return m_iNextId;
}

void pymms::MMSPython::setDone(int iScriptId)
{
  ost::MutexLock lock(m_critSection);
  PyList::iterator it = m_vecPyList.begin();
  while (it != m_vecPyList.end())
  {
    if (it->iId == iScriptId)
    {
      m_vecPyList.erase(it);
      break;
    }
    ++it;
  }
}

PyThreadState* pymms::MMSPython::getMainThreadState()
{
  ost::MutexLock lock(m_critSection);
  return m_pMainThreadState;
}

int pymms::MMSPython::scriptsSize()
{
  ost::MutexLock lock(m_critSection);
  return m_vecPyList.size();
}

const char* pymms::MMSPython::getFileName(int iScriptId)
{
  ost::MutexLock lock(m_critSection);
  const char* cFileName = NULL;

  PyList::iterator it = m_vecPyList.begin();
  while (it != m_vecPyList.end())
  {
    if (it->iId == iScriptId)
      cFileName = it->strFile.c_str();

    ++it;
  } 

  return cFileName;
}

int pymms::MMSPython::getScriptId(const string& strFile)
{
  ost::MutexLock lock(m_critSection);
  int iId = -1;

  PyList::iterator it = m_vecPyList.begin();
  while (it != m_vecPyList.end())
  {
    if (it->strFile == strFile)
      iId = it->iId;
    
    ++it;
  }

  return iId;
}

bool pymms::MMSPython::isRunning(int iScriptId)
{
  ost::MutexLock lock(m_critSection);
  bool bRunning = false;

  PyList::iterator it = m_vecPyList.begin();
  while (it != m_vecPyList.end())
  {
    if (it->iId == iScriptId)
      bRunning = true;
    ++it;
  }

  return bRunning;
}

int pymms::MMSPython::getPythonScriptId(int iScriptPos)
{
  ost::MutexLock lock(m_critSection);
  return (int)m_vecPyList[iScriptPos].iId;
}
