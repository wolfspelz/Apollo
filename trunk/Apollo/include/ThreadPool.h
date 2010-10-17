// ============================================================================
//
// Apollo
//
// ============================================================================


#if !defined(Apollo_ThreadPool_h_INCLUDED)
#define Apollo_ThreadPool_h_INCLUDED

#include "ApCompiler.h"
#include "ApExports.h"
#include "ApTypes.h"
#include "ApOS.h"

AP_NAMESPACE_BEGIN

// ----------------------

class ThreadList;
class APOLLO_API ThreadPool : public Apollo::Thread
{
  friend class IdleWorkerTask;
  friend class WorkerThread;
public:
  ThreadPool(const ApHandle& hAp, const char* szName, int nConcurrency);
  virtual ~ThreadPool();
public:
  virtual void Run();
  void AddJob(Apollo::Task* pJob);
protected:
  virtual void Svc();
protected:
  WorkerThread* PopIdleWorker();
  void PushIdleWorker(WorkerThread* pWorker);
  virtual int SvcEndCondition() { return Thread::SvcEndCondition(); }
protected:
  ThreadList& threadList();
  ApHandle hAp_;
  int nConcurrency_;
};

AP_NAMESPACE_END


#endif // Apollo_ThreadPool_h_INCLUDED