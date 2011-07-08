// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLib.h"
#include "ApLog.h"
#include "STree.h"
#include "ThreadPool.h"
#include "MsgThreadPool.h"

// ----------------------------------------------------------------------------

AP_NAMESPACE_BEGIN

class WorkerThread : public Apollo::Thread
{
protected:
  friend class ThreadList;
public:
  WorkerThread(const char* szName, ThreadPool* pPoolContext);
  virtual ~WorkerThread();
  String& getName() { return sName_; }
public:
  void Synchronize();
protected:
  virtual void Svc();
protected:
  Apollo::Event syncEvent_;
  ThreadPool* pPoolContext_;
};

AP_NAMESPACE_END

Apollo::WorkerThread::WorkerThread(const char* szName, Apollo::ThreadPool* pPoolContext)
: Apollo::Thread(szName), pPoolContext_(pPoolContext)
{
}

Apollo::WorkerThread::~WorkerThread()
{
}

void Apollo::WorkerThread::Synchronize()
{
  if (bStarted_) {
    SvcEndCondition(1);
    lTasks_.Unlock();
    syncEvent_.Wait();
  }
}

void Apollo::WorkerThread::Svc()
{
  SvcEndCondition(0);
  nThreadId_ = Apollo::GetCurrentThreadId();

  while (!SvcEndCondition()) {
    pPoolContext_->PushIdleWorker(this);
    Apollo::Task* pTask = lTasks_.Get();
    if (pTask != 0) {
      pTask->Execute();
      lTasks_.Delete(pTask);
      pTask = NULL;
    }
  }
  
  lTasks_.Empty();
  syncEvent_.Set();
}

// ----------------------------------------------------------------------------

AP_NAMESPACE_BEGIN

class IdleWorkerTask : public Apollo::Task
{
public:
  IdleWorkerTask(ThreadPool* pPoolContext, Apollo::Task* pJob);
  virtual ~IdleWorkerTask();
public:
  virtual void Execute();
protected:
  Apollo::Task* pJob_;
  ThreadPool* pPoolContext_;
};

AP_NAMESPACE_END

Apollo::IdleWorkerTask::IdleWorkerTask(Apollo::ThreadPool* pPoolContext, Apollo::Task* pJob)
: Apollo::Task("IdleWorkerTask"), pJob_(pJob), pPoolContext_(pPoolContext)
{
}

Apollo::IdleWorkerTask::~IdleWorkerTask()
{
  if (pJob_ != 0) { delete pJob_; pJob_ = NULL; }
}

void Apollo::IdleWorkerTask::Execute()
{
  Apollo::WorkerThread* pIdleThread = NULL;
  {
    int nIterations = 0;
    while (!pPoolContext_->SvcEndCondition() && (pIdleThread == NULL)) {
      if (++nIterations > 2) {
        AP_DEBUG_BREAK();
        break;
      }
      pIdleThread =  pPoolContext_->PopIdleWorker();
    }
  }
  
  if (pIdleThread != 0) {
    pIdleThread->AddTask(pJob_);
    pJob_ = NULL; // Give up ownership to not destroy the job in our dtor
  } else {
    if (!pPoolContext_->SvcEndCondition()) {
      AP_DEBUG_BREAK();
    }
  }
}

// ----------------------------------------------------------------------------

AP_NAMESPACE_BEGIN

typedef StringTreeIterator<WorkerThread*> ThreadListIterator;
typedef StringTreeNode<WorkerThread*> ThreadListNode;

class ThreadList : public StringTree<WorkerThread*>
{
public:
  ThreadList() : StringTree<WorkerThread*>() {}
  virtual ~ThreadList() {}
public:
  Apollo::WorkerThread* Pop();
  void Push(Apollo::WorkerThread* pWorker);
  Apollo::WorkerThread* PopIdle();
  void PushIdle(Apollo::WorkerThread* pWorker);
protected:
  friend class ThreadPool;
  
  List lIdleThreads_;
  Apollo::Event idleListEvent_;
  Apollo::Mutex idleListMutex_;

  Apollo::Event listEvent_;
  Apollo::Mutex listMutex_;
};

AP_NAMESPACE_END

Apollo::WorkerThread* Apollo::ThreadList::Pop()
{
  Apollo::WorkerThread* pWorker = NULL;

  listMutex_.Acquire();
  ThreadListNode* pNode = Next(NULL);
  if (pNode != 0) {
    pWorker = pNode->Value();
    Unset(pNode->Key());
  }
  listMutex_.Release();

  if (pNode == NULL) {
    listEvent_.Wait();    
    
    listMutex_.Acquire();
    pNode = Next(NULL);
    if (pNode != 0) {
      pWorker = pNode->Value();
      Unset(pNode->Key());
    }
    listMutex_.Release();
  }

  if (pWorker != 0) {
    idleListMutex_.Acquire();
    Elem* eIdleThread = lIdleThreads_.FindByName(pWorker->getName());
    if (eIdleThread != 0) {
      lIdleThreads_.Remove(eIdleThread);
    }
    idleListMutex_.Release();
  }

  return pWorker;
}


void Apollo::ThreadList::Push(Apollo::WorkerThread* pWorker)
{
  if (pWorker != 0) {
    listMutex_.Acquire();  
    Set(pWorker->getName(), pWorker);
    listMutex_.Release();
    
    listEvent_.Set();
  }
}


Apollo::WorkerThread* Apollo::ThreadList::PopIdle()
{
  Apollo::WorkerThread* pWorker = NULL;
  
  idleListMutex_.Acquire();
  Elem* eIdleThread = lIdleThreads_.First();
  if (eIdleThread != 0) {
    lIdleThreads_.Remove(eIdleThread);
  }
  idleListMutex_.Release();

  if (eIdleThread == NULL) {

    idleListEvent_.Wait();

    idleListMutex_.Acquire();
    eIdleThread = lIdleThreads_.First();
    if (eIdleThread != 0) {
      lIdleThreads_.Remove(eIdleThread);
    }
    idleListMutex_.Release();
    
  }

  if (eIdleThread != 0) {
    listMutex_.Acquire();
    ThreadListNode* pNode = Find(eIdleThread->getName());
    if (pNode == NULL) {
      AP_DEBUG_BREAK();
    } else {
      pWorker = pNode->Value();
    }
    listMutex_.Release();

    delete eIdleThread; eIdleThread = NULL;
  }
  
  return pWorker;
}

void Apollo::ThreadList::PushIdle(Apollo::WorkerThread* pWorker)
{
  Apollo::MutexGuard lock(&listMutex_);

  ThreadListNode* pNode = NULL;
  if ((pNode = Find(pWorker->getName()))) {
    idleListMutex_.Acquire();
    lIdleThreads_.AddLast(pWorker->getName());
    idleListMutex_.Release();
    idleListEvent_.Set();
  } else {
    AP_DEBUG_BREAK();
  }
}

// ----------------------------------------------------------------------------

Apollo::ThreadPool::ThreadPool(const ApHandle& hAp, const char* szName, int nConcurrency)
: Apollo::Thread(szName), hAp_(hAp), nConcurrency_(nConcurrency)
{
}

Apollo::ThreadPool::~ThreadPool()
{
}

Apollo::ThreadList& Apollo::ThreadPool::threadList()
{
  static ThreadList lThreads;
  return lThreads;
}

void Apollo::ThreadPool::Run()
{
  for (int i = 0; i < nConcurrency_; ++i) {
    String sWorkerName; sWorkerName.appendf("%s-%d", _sz(sName_), i + 1);
    Apollo::WorkerThread* pWorker = new Apollo::WorkerThread((const char*)sWorkerName, this);
    if (pWorker != 0) { 
      threadList().Push(pWorker);
      pWorker->Run();
    }
  }
  Apollo::Thread::Run();
}

void Apollo::ThreadPool::AddJob(Apollo::Task* pJob)
{
  Apollo::IdleWorkerTask* pTask = new Apollo::IdleWorkerTask(this, pJob);
  if (pTask != 0) { AddTask(pTask); }
}

Apollo::WorkerThread* Apollo::ThreadPool::PopIdleWorker()
{
  return threadList().PopIdle();
}

void Apollo::ThreadPool::PushIdleWorker(Apollo::WorkerThread* pWorker)
{
  if (pWorker != 0) {
    threadList().PushIdle(pWorker);
  }
}

void Apollo::ThreadPool::Svc()
{
  Apollo::Thread::Svc();

  ThreadListNode* pNode = NULL;
  while((pNode = threadList().Next(NULL))) { 
    Apollo::WorkerThread* pWorker = pNode->Value();
    threadList().Unset(pNode->Key());
    pWorker->Synchronize(); 
    delete pWorker; pWorker = NULL;
  }
  
  ApAsyncMessage<Msg_ThreadPool_Finished> msg;
  msg->hPool = hAp_;
  msg.Post();
}

