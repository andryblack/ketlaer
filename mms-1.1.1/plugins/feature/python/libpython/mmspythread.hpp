#ifndef MMSPYTHREAD_HPP
#define MMSPYTHREAD_HPP

#include "Python.h"
#include <cc++/thread.h>
#include <string>
#include <vector>

namespace pymms {

class MMSPyThread : public ost::Thread
{
  public:
    MMSPyThread(void* pExecuter, PyThreadState* pMainThreadState, int iId);
    virtual ~MMSPyThread();
    int evalFile(const std::string& strFile);
	
  protected:
    PyThreadState* m_pThreadState;
    void* m_pExecuter;

    char m_cType;
    std::string m_strSource;
    int m_iId;

    void run();
    void final();
};

}

#endif
