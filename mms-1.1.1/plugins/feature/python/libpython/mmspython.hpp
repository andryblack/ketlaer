#ifndef MMSPYTHON_HPP
#define MMSPYTHON_HPP

#include "mmspythread.hpp"
#include <cc++/thread.h>
#include "singleton.hpp"
#include <string>
#include <vector>

namespace pymms {

typedef struct {
	int iId;
	bool bDone;
	std::string strFile;
	MMSPyThread* pPyThread;
} PyElem;

typedef std::vector<PyElem> PyList;

class MMSPython
{
  public:

    static pthread_mutex_t singleton_mutex;

    MMSPython();
    void initialize();
    int scriptsSize(); 
    int getPythonScriptId(int iScriptPos); 
    int evalFile(const std::string& strFile);
    bool isRunning(int iScriptId);
    void setDone(int iScriptId);

    // returns NULL if script doesn't exist or if script doesn't have a filename
    const char* getFileName(int iScriptId);
    
    // returns -1 if no scripts exist with specified filename
    int getScriptId(const std::string& strFile);
    
    PyThreadState* getMainThreadState();

  private:
    int m_iNextId;
    PyThreadState* m_pMainThreadState;
    bool m_bInitialized;
    PyList m_vecPyList;
    ost::Mutex m_critSection;
};

typedef Singleton<MMSPython> S_MMSPython;

}

#endif
