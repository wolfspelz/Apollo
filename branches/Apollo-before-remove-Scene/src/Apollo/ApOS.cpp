// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApOS.h"
#include "ApLog.h"

#define LOG_CHANNEL "ApOS"

AP_NAMESPACE_BEGIN

// -------------------------------------------------------------------
// Utils

void* malloc(size_t n)
{
  return ::malloc(n);
}

void free(void* p)
{
  ::free(p);
}

void* realloc(void* mem, size_t newLen)
{
  return ::realloc(mem, newLen);
}

// -------------------------------------------------------------------
// Mutex

MutexGuard::MutexGuard(Mutex* pMutex)
:pMutex_(pMutex)
{
  if (pMutex_) {
    pMutex_->Acquire();
  }
}

MutexGuard::~MutexGuard()
{
  if (pMutex_) {
    pMutex_->Release();
  }
}

Mutex::Mutex()
{
#if defined(WIN32)
  ::InitializeCriticalSection(&mutex_);
#else
  // ru: we need a recursive pthread_mutex, else we deadlock in main thread upon first Apollo::callMsg()
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex_, &mutex_attr);
#endif
}

Mutex::~Mutex()
{
#if defined(WIN32)
  ::DeleteCriticalSection(&mutex_);
#else
  pthread_mutex_destroy(&mutex_);
#endif
}

void Mutex::Acquire()
{
#if defined(WIN32)
  ::EnterCriticalSection(&mutex_);
#else
  pthread_mutex_lock(&mutex_);
#endif
}

void Mutex::Release()
{
#if defined(WIN32)
  ::LeaveCriticalSection(&mutex_);
#else
  pthread_mutex_unlock(&mutex_);
#endif
}

int Mutex::TryAcquire()
{
  int ok = 0;
#if defined(WIN32)
  ok = (TRUE == ::TryEnterCriticalSection(&mutex_));
#else
  ok = (0 == pthread_mutex_trylock(&mutex_));
#endif
  return ok;
}

// -------------------------------------------------------------------
// Event

Event::Event()
{
#if defined(WIN32)
  hEvent_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
#elif defined(LINUX) || defined(MAC)
  pthread_mutex_init(&mutex_, NULL);
  pthread_cond_init(&condition_, NULL);
#else
  #error not implemented for this os
#endif
}
/*
  typedef struct {
    pthread_mutex_t	semCount_Mutex;
    int			        semCount;	
	  pthread_cond_t	condition;
  } event_t;
*/
Event::~Event()
{
#if defined(WIN32)
  ::CloseHandle(hEvent_);
#elif defined(LINUX) || defined(MAC)
  pthread_mutex_destroy(&mutex_);
  pthread_cond_destroy(&condition_);
#else
  #error not implemented for this os
#endif
}

void Event::Set()
{
#if defined(WIN32)
  ::SetEvent(hEvent_);
#elif defined(LINUX) || defined(MAC)
  pthread_mutex_lock(&mutex_);
  pthread_cond_broadcast(&condition_);
  pthread_mutex_unlock(&mutex_);
#else
  #error not implemented for this os
#endif
}

void Event::Wait()
{
#if defined(WIN32)
  ::WaitForSingleObject(hEvent_, INFINITE);
#elif defined(LINUX)  || defined(MAC)
  // pthread_cond_wait does unlock on mutex upon enter and lock upon leave
  pthread_mutex_lock(&mutex_);
  pthread_cond_wait(&condition_, &mutex_);
  pthread_mutex_unlock(&mutex_);
#else
  #error not implemented for this os
#endif
}

// -------------------------------------------------------------------
// TaskQueue

void TaskQueue::Put(Task* pTask)
{
  MutexGuard g(&oMutex_);

  AddLast(pTask);
  oEvent_.Set();
}

Task* TaskQueue::Get()
{
  Task* oResult = 0;
  {
    MutexGuard g(&oMutex_);

    oResult = (Task*) First();
    if (oResult != 0) {
      Remove(oResult);
    }
  }

  if (oResult == 0) {
    oEvent_.Wait();

    MutexGuard g(&oMutex_);

    oResult = (Task*) First();
    if (oResult != 0) {
      Remove(oResult);
    }
  }

  return oResult;
}

void TaskQueue::Delete(Task* pTask)
{
  MutexGuard g(&oMutex_);

  Remove(pTask);
  delete pTask;
}

void TaskQueue::Unlock()
{
  MutexGuard g(&oMutex_);

  Empty();
  Task* pTask = new Task();
  if (pTask != 0) {
    Put(pTask);
  }
}

// -------------------------------------------------------------------
// Thread

UINT GetCurrentThreadId()
{
#if defined(WIN32)
  return ::GetCurrentThreadId();
#elif defined(LINUX) || defined(MAC)
  return (UINT)::pthread_self();
#else
#error not implemented for this os
#endif
}

Thread::Thread(const char* szName)
:sName_(szName)
,bStarted_(0)
,bStop_(0)
,hThread_(0)
,nThreadId_(0)
{
}

Thread::~Thread(void)
{
}

void Thread::Run()
{
#if defined(WIN32)
  hThread_ = CreateThread(
                            0,        //LPSECURITY_ATTRIBUTES lpThreadAttributes
                            0,        //SIZE_T dwStackSize
                            ThreadJumpIn,  //LPTHREAD_START_ROUTINE lpStartAddress
                            this,      //LPVOID lpParameter
                            0,        //DWORD dwCreationFlags
                            0        //LPDWORD lpThreadId
                            );
  if (hThread_ == 0) {
    apLog_Error((LOG_CHANNEL, "Thread::Run", "CreateThread failed %d", GetLastError()));
  } else {
    bStarted_ = 1;
  }
#else
  int nErr = ::pthread_create(&hThread_,  0, ThreadJumpIn, this);
  if (nErr != 0) {
    apLog_Error((LOG_CHANNEL, "Thread::Run", "pthread_create failed %d", nErr));
  } else {
    bStarted_ = 1;
  }
#endif
}

#if defined(WIN32)
DWORD WINAPI Thread::ThreadJumpIn(LPVOID pThreadParam)
#else
void* Thread::ThreadJumpIn(void* pThreadParam)
#endif
{
  ((Thread *) pThreadParam)->Svc();
  return 0;
}

void Thread::Stop()
{
  if (bStarted_) {
    SvcEnd();

    #if defined(WIN32)
      WaitForSingleObject(hThread_, INFINITE);
      CloseHandle(hThread_);
    #else
      pthread_join(hThread_, 0);
    #endif
    hThread_ = 0;
  }
}

void Thread::SvcEnd()
{
  SvcEndCondition(1);
  lTasks_.Unlock();
}

void Thread::Svc()
{
  SvcEndCondition(0);
  nThreadId_ = Apollo::GetCurrentThreadId();
  while (!SvcEndCondition()) {
    Apollo::Task* pTask = lTasks_.Get();
    if (pTask != 0) {
      pTask->Execute();
      lTasks_.Delete(pTask);
      pTask = 0;
    }
  }
}

// -------------------------------------------------------------------

void APOLLO_API Sleep(int nMillisecs)
{
#if defined(WIN32)
  ::Sleep(nMillisecs);
#elif defined(LINUX) || defined(MAC)
  struct timespec ts_req, ts_rem;
  ts_req.tv_sec = 0; ts_req.tv_nsec = nMillisecs * 1000000;
  if (::nanosleep(&ts_req, &ts_rem) == -1 && errno == EINTR) {
    Apollo::Sleep(ts_rem.tv_nsec / 1000000);
  }
#else
  #error Not implemented for this OS
#endif
}

// -------------------------------------------------------------------

AP_NAMESPACE_END
