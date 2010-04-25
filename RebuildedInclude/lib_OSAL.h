/*common/IPC/include/xdr/OSALTypes.h*/

typedef u_int64_t uint64_t;

typedef u_int32_t uint32_t;

typedef u_int16_t uint16_t;

typedef u_int8_t uint8_t;

typedef long unsigned int ULONG;

typedef ULONG DWORD;

typedef short unsigned int WORD;

typedef unsigned char BYTE;

/*system/Include/OSAL/IO.cpp*/
int osal_Unlink(char const * pathname);

int osal_ExclusiveCreateFile(char const * pathname);


/*system/Include/OSAL/OSAL.cpp*/
pid_t myfork();

extern "C" int mysystem(char const * command);

extern long unsigned int g_userInputTotalCount;

extern long unsigned int g_userInputHandledCount;

extern short int g_userInputKeyType;

long int osal_isKeyPressed();

short int osal_getKeyType();

void * osal_OpenLibrary(char * path, int bLazy);

HRESULT osal_CloseLibrary(void * lib_handle);

void * osal_GetFuncAddress(void * lib_handle, char * func_name);


/*system/Include/OSAL/Threads.h*/
class tag_osal_thread_t {
public:
  unsigned char dummy[96];
};

typedef tag_osal_thread_t osal_thread_t;

class tag_osal_mutex_t {
public:
  unsigned char dummy[32];
};

typedef tag_osal_mutex_t osal_mutex_t;

class tag_osal_event_t {
public:
  unsigned char dummy[80];
};

typedef tag_osal_event_t osal_event_t;

class tag_osal_sem_t {
public:
  unsigned char dummy[88];
};

typedef tag_osal_sem_t osal_sem_t;


/*system/Include/OSAL/Threads.cpp*/
class tag_posix_thread_t {
public:
  int type;
  void (*start_address)(void * );
  void * arglist;
  pthread_t handle;
  osal_event_t end_event;
};

typedef tag_posix_thread_t posix_thread_t;

void * posix_start_address(void * arglist);

extern "C" HRESULT osal_ThreadBegin(void /*should not happen:void (*)(void * )*/ * start_address, void * arglist, long unsigned int stack_size, long int priority, osal_thread_t * pThreadID);

extern "C" HRESULT osal_ThreadEnd(osal_thread_t * pThreadID, long int millisecondsMaxWait);

extern "C" HRESULT osal_ThreadDetach(osal_thread_t * pThreadID);

extern "C" HRESULT osal_ThreadExit();

extern "C" void osal_Sleep(long int millisecondsSleep);

class tag_posix_mutex_t {
public:
  int type;
  pthread_mutex_t handle;
  int lockCount;
};

typedef tag_posix_mutex_t posix_mutex_t;

extern "C" HRESULT osal_MutexCreate(osal_mutex_t * pMutexID);

extern "C" HRESULT osal_MutexLock(osal_mutex_t * pMutexID);

extern "C" HRESULT osal_MutexTimedLock(osal_mutex_t * pMutexID, int nseconds);

extern "C" HRESULT osal_MutexLock_BoostPriority(osal_mutex_t * pMutexID, int priority);

extern "C" HRESULT osal_MutexTryLock(osal_mutex_t * pMutexID);

extern "C" HRESULT osal_MutexUnlock(osal_mutex_t * pMutexID);

extern "C" HRESULT osal_MutexDestroy(osal_mutex_t * pMutexID);

int posix_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex, long int millisecondsTimeout, timespec * abs_timeout);

class tag_posix_event_t {
public:
  int type;
  int state;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

typedef tag_posix_event_t posix_event_t;

extern "C" HRESULT osal_EventCreate(osal_event_t * pEventID);

extern "C" HRESULT osal_EventSet(osal_event_t * pEventID);

extern "C" HRESULT osal_EventReset(osal_event_t * pEventID);

extern "C" HRESULT osal_EventWait(osal_event_t * pEventID, long int millisecondsTimeout);

extern "C" HRESULT osal_EventDestroy(osal_event_t * pEventID);

class tag_posix_sem_t {
public:
  int type;
  unsigned int count;
  unsigned int max;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

typedef tag_posix_sem_t posix_sem_t;

extern "C" HRESULT osal_SemCreate(unsigned int maxCount, unsigned int initialCount, osal_sem_t * pSemID);

extern "C" HRESULT osal_SemGive(osal_sem_t * pSemID);

extern "C" HRESULT osal_SemWait(osal_sem_t * pSemID, long int millisecondsTimeout);

extern "C" HRESULT osal_SemSetCount(osal_sem_t * pSemID, unsigned int count);

extern "C" HRESULT osal_SemDestroy(osal_sem_t * pSemID);


/*system/Include/OSAL/debug_level_clnt.c*/
extern "C" int * set_debug_flag_0(int * argp, CLNT_STRUCT * clnt);


/*system/Include/OSAL/remote_malloc_svc.c*/
extern "C" REG_STRUCT * R_PROGRAM_0_register(REG_STRUCT * rnode);


/*system/Include/OSAL/remote_malloc_svr.c*/
extern "C" int * rmalloc_0_svc(int * argp, RPC_STRUCT * rpc, int * result);

extern "C" int * rfree_0_svc(int * argp, RPC_STRUCT * rpc, int * result);


/*system/Include/OSAL/udf_check.c*/
extern "C" int start_record_udf(int my_fd, char * map_addr);

extern "C" int end_record_udf();

extern "C" int check_udf();

/*system/Include/OSAL/PLI.h*/
class alloc_record {
public:
  int addr;
  int flag;
  int requested_size;
  int allocated_size;
  char mesg[10];
  alloc_record * next;
};

class watch_struct {
public:
  long unsigned int addr;
  long unsigned int prot;
};

typedef class {
  public:
    int fd;
    char name[36];
    long unsigned int addr;
  } dirty_page_struct;

class sched_log_struct {
public:
  long unsigned int addr;
  long unsigned int size;
};

class _tagTIME {
public:
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

typedef _tagTIME TIME;


/*system/Include/OSAL/PLI.cpp*/
class record_struct {
public:
  int name;
  int code;
};

extern char const * dvr_auth;

extern int fd;

extern int start_addr;

extern int asid;

extern sched_log_struct log_struct;

extern alloc_record * record_pointer;

extern pthread_mutex_t record_mutex;

extern int total_size;

extern int alloc_size;

extern int alloc_size_s;

extern int alloc_size_a;

extern int alloc_size_v;

extern osal_mutex_t g_HwMutex;

extern "C" int64_t pli_getSCR();

int pli_print_time(char * pTime, int size);

extern "C" int pli_getTime(TIME * time);

extern "C" int64_t pli_getMilliseconds();

extern "C" int pli_init();

extern "C" int pli_close();

extern "C" void pli_showInfo();

extern "C" void pli_freeAllMemory();

extern "C" void pli_clearCacheMemory();

extern "C" void pli_startDetectOccupy(int ticks);

extern "C" void pli_listAllMemory();

extern "C" void pli_disableInterrupt();

extern "C" void pli_enableInterrupt();

extern "C" int * pli_getIOAddress(int addr);

extern "C" int pli_setWatchPoint(long unsigned int addr, long unsigned int prot);

extern "C" int pli_clrWatchPoint();

extern "C" int pli_getWatchPoint(long unsigned int * addr, long unsigned int * prot);

extern "C" void pli_resetRPC();

extern "C" int pli_initLog(int bufsize);

extern "C" int pli_freeLog();

extern "C" int pli_startLog();

extern "C" int pli_logEvent(int event);

extern "C" int pli_stopLog(char * logfile);

extern "C" int pli_startRecordDirty(dirty_page_struct * dirty);

extern "C" int pli_endRecordDirty();

extern "C" int pli_syncMetaData(int fd_dev);

extern "C" int pli_setThreadName(char * str);

extern "C" int64_t pli_getPTS();

extern "C" int pli_getStartAddress();

extern "C" int pli_getGraphicEndAddress();

extern "C" void * pli_allocContinuousMemoryMesg(char * str, size_t size, BYTE * * nonCachedAddr, long unsigned int * phyAddr);

extern "C" void pli_flushMemory(void * ptr, long int size);

extern "C" void pli_prefetchCache(void * ptr, long int size);

extern "C" uint64_t pli_ddrCopy(void * dest, void * src, long int size);

extern "C" long int pli_checkTaskCompletion(uint64_t serialNumber);

int removeRecord(int address);

extern "C" void pli_freeContinuousMemoryMesg(char * str, void * ptr);

extern "C" void pli_IPCWriteULONG(BYTE * des, long unsigned int data);

extern "C" void pli_IPCWriteULONGLONG(BYTE * des, long long unsigned int data);

extern "C" long unsigned int pli_IPCReadULONG(BYTE * src);

extern "C" long long unsigned int pli_IPCReadULONGLONG(BYTE * src);

extern "C" void pli_IPCCopyMemory(BYTE * src, BYTE * des, long unsigned int len);

extern "C" void * pli_allocGraphicMemory(size_t size, long unsigned int * phyAddr);

extern "C" void pli_freeGraphicMemory(void * ptr);

extern "C" long int pli_lockHWSem();

extern "C" void pli_unlockHWSem();

extern "C" long int pli_initHWSem();

extern "C" long int pli_hwLock();

extern "C" void pli_hwUnlock();

extern "C" long int pli_hwInit();


/*system/Include/OSAL/SmartClass.h*/
class CCritSec {
private:
  osal_mutex_t m_mutex;
public:
  CCritSec();
  ~CCritSec();
  void Lock();
  void Unlock();
};

class CAutoLock {
private:
  CCritSec * m_pLock;
public:
  CAutoLock(CCritSec * pLock);
  ~CAutoLock();
};

class CSemaphore {
private:
  osal_sem_t m_sem;
public:
  CSemaphore();
  ~CSemaphore();
  void Init(char * name, long unsigned int initValue);
  void Reset(long unsigned int resetValue);
  void Wait();
  void Post();
  bool TimedWait(long unsigned int milliseconds);
};

class CPolling {
protected:
  long int m_milliSleepSlot;
  void * m_pTimeOut;
public:
  CPolling(long int TotalTime, long int SleepSlot);
  ~CPolling();
  void Sleep();
  bool IsTimeOut();
};

class CThread {
protected:
  void (*m_start_address)(void * );
  void * m_pThreadData;
  osal_thread_t m_thread;
  unsigned char m_Flag;
public:
  CThread(void /*should not happen:void (*)(void * )*/ * start_address, void * pThreadData);
  ~CThread();
  bool Run();
  bool IsRun();
  void Exit(bool bWaitForExit);
  bool IsAskToExit();
};


