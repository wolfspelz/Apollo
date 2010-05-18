// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApOS_h_INCLUDED)
#define ApOS_h_INCLUDED

#include "ApExports.h"
#include "ApTypes.h"

#if defined(WIN32)
  #include "Winsock2.h"
#elif defined(LINUX)
#elif defined(MAC)
#endif

#if defined(WIN32)
#elif defined(LINUX) || defined(MAC)
  #include <pthread.h>
  #include <sys/time.h>
#else
  #error threading not implemented for this os
#endif

#if defined(WIN32) && defined(_MSC_VER)
  typedef __int64 int64_t  ;
  #define	PLATFORM_S64(x)		x##i64
  #define PLATFORM_U64(x)   x##u64
#else
  #include <stdint.h>
  #define	PLATFORM_S64(x)		x##ll
  #define PLATFORM_U64(x)   x##ul
#endif

AP_NAMESPACE_BEGIN

// -------------------------------------------------------------------

APOLLO_API void* malloc(size_t n);
APOLLO_API void free(void* p);
APOLLO_API void* realloc(void* mem, size_t newLen);

// -------------------------------------------------------------------
// Mutex

class APOLLO_API Mutex
{
public:
  Mutex();
  ~Mutex();
  void Acquire();
  void Release();
  int TryAcquire();
protected:
#if defined(WIN32)
  CRITICAL_SECTION mutex_;
#elif defined(LINUX) || defined(MAC)
  pthread_mutex_t mutex_;
#endif
};

class APOLLO_API MutexGuard
{
public:
  MutexGuard(Mutex* pMutex);
  ~MutexGuard();
private:
  Mutex* pMutex_;
};


class APOLLO_API NullMutex
{
public:
  void Acquire() {}
  void Release()  {}
  int TryAcquire() { return 1; }
};

// -------------------------------------------------------------------
// Event

class APOLLO_API Event
{
public:
  Event();
  ~Event();
  void Set();
  void Wait();
protected:
#if defined(WIN32)
  HANDLE hEvent_;
#elif defined(LINUX) || defined(MAC)
  // the pthread way:
  pthread_mutex_t	mutex_;
  pthread_cond_t	condition_;
#else
  #error Not implemented for this OS
#endif
};

// -------------------------------------------------------------------
// TaskQueue

class APOLLO_API Task: public Elem
{
public:
  Task() {}
  Task(char* szType) :Elem(szType) {}
  virtual void Execute() {}
};

class APOLLO_API TaskQueue: public List
{
public:
  void Put(Task* pTask);
  Task* Get();
  void Delete(Task* pTask);
  void Unlock();

  Apollo::Mutex& Mutex() { return oMutex_; }

protected:
  Apollo::Mutex oMutex_;
  Apollo::Event oEvent_;
};

// -------------------------------------------------------------------
// Thread

APOLLO_API UINT GetCurrentThreadId();

class APOLLO_API Thread
{
public:
  Thread(const char* szName);
  virtual ~Thread();
  virtual void Run();
  virtual void Stop();

  void AddTask(Task* t) { lTasks_.Put(t); }

protected:
  virtual void Svc();
  virtual void SvcEnd();

  virtual int SvcEndCondition() { return bStop_; }
  virtual void SvcEndCondition(int b) { bStop_ = b; }

protected:
  TaskQueue lTasks_;

  int bStarted_;
  int bStop_;

#if defined(WIN32)
  HANDLE hThread_;
  static DWORD WINAPI ThreadJumpIn(LPVOID pThreadParam);
#elif defined(LINUX) || defined(MAC)
  pthread_t hThread_;
  static void* ThreadJumpIn(void* pThreadParam);
#else
  #error Not implemented for this OS
#endif

protected:
  String sName_;
  UINT nThreadId_;
};

// -------------------------------------------------------------------

void APOLLO_API Sleep(int nMillisecs);

// -------------------------------------------------------------------

AP_NAMESPACE_END

#endif // !defined(ApOS_h_INCLUDED)
