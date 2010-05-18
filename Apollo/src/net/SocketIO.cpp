// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApCompiler.h"
#include "ApOS.h"
#include "SocketIO.h"
#include "NetModule.h"

#if defined(LINUX) || defined(MAC)
  #include <signal.h>
#endif

// -------------------------------------------------------------------
// SocketTaskQueue

SocketTaskQueue::SocketTaskQueue()
{
#if defined(WIN32)
  hQueueEvent_ = ::WSACreateEvent();
#endif
}

SocketTaskQueue::~SocketTaskQueue()
{
#if defined(WIN32)
  ::WSACloseEvent(hQueueEvent_);
#endif
}

void SocketTaskQueue::Put(Apollo::Task* pTask)
{
  oMutex_.Acquire();
  AddLast(pTask);
#if defined(WIN32)
  ::WSASetEvent(hQueueEvent_);
#elif defined(LINUX) || defined(MAC)
  UINT nTid = NetModuleInstance::Get()->oSocketIO_.ThreadId();
  if (nTid <= 0) {
    AP_DEBUG_BREAK();
  }
  ::pthread_kill((pthread_t)nTid, SIGUSR1);
#else
  #error Not implemented for this OS
#endif
  oMutex_.Release();
}

Apollo::Task* SocketTaskQueue::Get()
{
  if (!NetModuleInstance::Get()->oSocketIO_.AssertThread()) { return 0; }

  oMutex_.Acquire();
  Apollo::Task* oResult = (Apollo::Task*) First();
  if (oResult != 0) {
    Remove(oResult);
  }
  oMutex_.Release();
  return oResult;
}

//----------------------------------------------------------

void SendDataTask::Execute()
{
  Socket* s = NetModuleInstance::Get()->oSocketIO_.FindSocket(hAp_);
  if (s != 0) {
    s->Send(sbData_.Data(), sbData_.Length());
  }
}

//----------------------------------------------------------

void CloseSocketTask::Execute()
{
  Socket* s = NetModuleInstance::Get()->oSocketIO_.FindSocket(hAp_);
  if (s != 0) {
    s->OnClose();
  }
}

//----------------------------------------------------------

void DeleteSocketTask::Execute()
{
  NetModuleInstance::Get()->oSocketIO_.RemoveSocket(s_);
  delete s_;
}

//----------------------------------------------------------

void TCPConnectTask::Execute()
{
  int ok = 0;
  Socket* s = new Socket(hAp_);
  if (s == 0) {
    apLog_Error((LOG_CHANNEL, "TCPConnectTask::Execute", "new Socket failed " ApHandleFormat "", ApHandleType(hAp_)));
  } else {
    ok = s->Open();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "TCPConnectTask::Execute", "Open failed " ApHandleFormat "", ApHandleType(hAp_)));
    } else {

      if (!NetModuleInstance::Get()->inShutdown()) {
        ok = s->Connect(saAddress_);
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "TCPConnectTask::Execute", "Connect failed " ApHandleFormat "", ApHandleType(hAp_)));
          s->Close();
        } else {
          NetModuleInstance::Get()->oSocketIO_.AddSocket(s);
        }
      }

    }
    if (!ok) {
      delete s;
    }
  }
}

//----------------------------------------------------------

void TCPListenTask::Execute()
{
  int ok = 0;
  String sError = "";
  Socket* s = new Socket(hAp_);
  if (s == 0) {
    apLog_Error((LOG_CHANNEL, "TCPListenTask::Execute", "new Socket failed " ApHandleFormat "", ApHandleType(hAp_)));
    sError = "new Socket failed";
  } else {
    ok = s->Open();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "TCPListenTask::Execute", "Open failed " ApHandleFormat "", ApHandleType(hAp_)));
      sError = "Socket::Open() failed";
    } else {

      if (!NetModuleInstance::Get()->inShutdown()) {
        ok = s->Listen(saAddress_);
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "TCPListenTask::Execute", "Listen failed " ApHandleFormat "", ApHandleType(hAp_)));
          s->Close();
          sError = "Socket::Listen() failed";
        } else {
          NetModuleInstance::Get()->oSocketIO_.AddSocket(s);
        }
      }
    }
    if (!ok) {
      delete s;
    }
  }

  ApAsyncMessage<Msg_Net_TCP_Listening> msg;
  msg->bSuccess = ok;
  msg->hServer = hAp_;
  msg->sComment = sError;
  msg.Post();
}

//----------------------------------------------------------

void CloseAllSocketsTask::Execute()
{
  for (Socket* s = 0; (s = NetModuleInstance::Get()->oSocketIO_.lSockets_.Next(s)) != 0; ) {
    s->OnClose();
  }
  
  NetModuleInstance::Get()->oSocketIO_.bFinished_ = 1;
}

//----------------------------------------------------------

void SocketIO::CloseAll()
{
  CloseAllSocketsTask* pTask = new CloseAllSocketsTask();
  if (pTask != 0) {
    AddTask(pTask);
  }
}

int SocketIO::AssertThread()
{
  if (Apollo::GetCurrentThreadId() != nThreadId_) {
  #if defined(_DEBUG) 
    AP_DEBUG_BREAK();
  #endif
    return 0;
  }
  return 1;
}

void SocketIO::SvcEnd()
{
  SvcEndCondition(1);

  Apollo::Task* pTask = new Apollo::Task();
  if (pTask != 0) {
    AddTask(pTask);
  }
}

void SocketIO::Svc()
{
  nThreadId_ = Apollo::GetCurrentThreadId();
  NetOS::SocketIOThreadProc(nThreadId_); // All processing happens here
}

void SocketIO::OnSocketTaskQueueEvent()
{
  Apollo::Task* pTask = 0;
  while ((pTask = oTasks_.Get()) != 0) {
    pTask->Execute();
    delete pTask;
    pTask = 0;
  }
}
