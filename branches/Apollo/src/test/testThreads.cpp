// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Threads)

// ----------------------------------------------------------------------------
// Test_Threads_ThreadPool

#include "ApLog.h"
#include "ThreadPool.h"
#include "MsgThreadPool.h"

static ApHandle g_Test_Threads_ThreadPool_hPoolHandle = Apollo::newHandle();
static int g_Test_Threads_ThreadPool_nConcurrency = 5;
static int g_Test_Threads_ThreadPool_nJobsPerThread = 5;
static Apollo::ThreadPool g_Test_Threads_ThreadPool_Pool = Apollo::ThreadPool(g_Test_Threads_ThreadPool_hPoolHandle
                                                                             ,"Test_Threads_ThreadPool_Pool"
                                                                             ,g_Test_Threads_ThreadPool_nConcurrency
                                                                             );
static List g_Test_Threads_ThreadPool_lJobs = List();
static String g_Test_Threads_ThreadPool_sError = "";


class Msg_ThreadPool_Test_Threads_JobFinished : public ApNotificationMessage
{
public:
  Msg_ThreadPool_Test_Threads_JobFinished() : ApNotificationMessage("ThreadPool_Test_Threads_JobFinished"), sName_("") {}
public:
  String sName_;
};

class Test_Threads_ThreadPool_Job : public Apollo::Task
{
public:
  Test_Threads_ThreadPool_Job(const char* szName) : Apollo::Task((char*)szName) {}
  virtual ~Test_Threads_ThreadPool_Job() {}
public:
  virtual void Execute()
  {
    Apollo::Sleep(10);
    
    apLog_Verbose((LOG_CHANNEL, "Task::Execute", "End: job=%s thread=0x%08x", StringType(getName()), Apollo::GetCurrentThreadId()));
    
    ApAsyncMessage<Msg_ThreadPool_Test_Threads_JobFinished> msg;
    msg->sName_ = getName();
    msg.Post();
  }
};

static void Test_Threads_ThreadPool_On_Job_Finished(Msg_ThreadPool_Test_Threads_JobFinished* pMsg)
{
  Elem* eJob = g_Test_Threads_ThreadPool_lJobs.FindByName(pMsg->sName_);
  if (eJob != 0) {
    eJob->setInt(eJob->getInt() + 1);
  }

  eJob = 0;
  int nPendingJobs = (int)g_Test_Threads_ThreadPool_lJobs.length();
  for (;(eJob = g_Test_Threads_ThreadPool_lJobs.Next(eJob));) {
    if (eJob->getInt() == 0) {
      // Not completed
    } else if (eJob->getInt() == 1) {
      nPendingJobs--;
    } else if (eJob->getInt() > 1) {
      if (g_Test_Threads_ThreadPool_sError.empty()) {
        g_Test_Threads_ThreadPool_sError.appendf("Multiple job result for job=%s", StringType(eJob->getName()));
      }
    }
  }

  if ((nPendingJobs == 0) || (! g_Test_Threads_ThreadPool_sError.empty())) {
    g_Test_Threads_ThreadPool_Pool.Stop();
  }
}

static void Test_Threads_ThreadPool_On_Pool_Finished(Msg_ThreadPool_Finished* pMsg)
{
  if (pMsg->hPool == g_Test_Threads_ThreadPool_hPoolHandle) {
    int nPendingJobs = (int)g_Test_Threads_ThreadPool_lJobs.length();
    for (Elem* eJob = 0; (eJob = g_Test_Threads_ThreadPool_lJobs.Next(eJob));) {
      if (eJob->getInt() > 0) {
        nPendingJobs--;
      }
    }
    if ((nPendingJobs != 0) && g_Test_Threads_ThreadPool_sError.empty()) {
      g_Test_Threads_ThreadPool_sError.appendf("%d Jobs not finished");
    }

    if (!g_Test_Threads_ThreadPool_sError.empty()) {
      AP_UNITTEST_FAILED(Test_Threads_ThreadPool, g_Test_Threads_ThreadPool_sError);
    } else {
      AP_UNITTEST_SUCCESS(Test_Threads_ThreadPool);
    }
    
    pMsg->Stop();
  }
}

void Test_Threads_ThreadPool_Begin()
{
  { Msg_ThreadPool_Finished msg; msg.Hook("testThreads", (ApCallback) Test_Threads_ThreadPool_On_Pool_Finished, 0, ApCallbackPosNormal); }
  { Msg_ThreadPool_Test_Threads_JobFinished msg; msg.Hook("testThreads", (ApCallback) Test_Threads_ThreadPool_On_Job_Finished, 0, ApCallbackPosNormal); }
  
  for (int i = 0; i < g_Test_Threads_ThreadPool_nConcurrency * g_Test_Threads_ThreadPool_nJobsPerThread; ++i) {
    String sJobName; sJobName.appendf("Job_%d", i);
    Test_Threads_ThreadPool_Job* pJob = new Test_Threads_ThreadPool_Job(sJobName);
    if (pJob != 0) {
      Elem* eJob = g_Test_Threads_ThreadPool_lJobs.AddLast(sJobName);
      if (eJob != 0) {
        eJob->setInt(0);
        g_Test_Threads_ThreadPool_Pool.AddJob(pJob);
      }
    }
  }
  
  g_Test_Threads_ThreadPool_Pool.Run();
}

void Test_Threads_ThreadPool_End()
{
  { Msg_ThreadPool_Test_Threads_JobFinished msg; msg.UnHook("testThreads", (ApCallback) Test_Threads_ThreadPool_On_Job_Finished, 0); }
  { Msg_ThreadPool_Finished msg; msg.UnHook("testThreads", (ApCallback) Test_Threads_ThreadPool_On_Pool_Finished, 0); }
  
  g_Test_Threads_ThreadPool_lJobs.Empty();

  g_Test_Threads_ThreadPool_Pool.Stop();
}

#endif // AP_TEST_Threads

// ----------------------------------------------------------------------------

void Test_Threads_Register()
{
#if defined(AP_TEST_Threads)
  AP_UNITTEST_REGISTER(Test_Threads_ThreadPool);
#endif
}

void Test_Threads_Begin()
{
#if defined(AP_TEST_Threads)
  Test_Threads_ThreadPool_Begin();
#endif
}

void Test_Threads_End()
{
#if defined(AP_TEST_Threads)
  Test_Threads_ThreadPool_End();
#endif
}

