// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SocketIO_h_INCLUDED)
#define SocketIO_h_INCLUDED

#include "Apollo.h"
#include "Socket.h"
#include "NetOS.h"

// -------------------------------------------------------------------
// SocketTaskQueue

class SocketTaskQueue: public List
{
public:
  SocketTaskQueue();
  ~SocketTaskQueue();

#if defined(WIN32)
  WSAEVENT GetWSAEvent() { return hQueueEvent_; }
#endif

  void Put(Apollo::Task* pTask);
  Apollo::Task* Get();

protected:
#if defined(WIN32)
  WSAEVENT hQueueEvent_;
#endif
  Apollo::Mutex oMutex_;
};

//----------------------------------------------------------

class SendDataTask: public Apollo::Task
{
public:
  SendDataTask(ApHandle h, const unsigned char* pData, size_t nLen)
    :hAp_(h)
  {
    sbData_.SetData(pData, nLen);
  }
  void Execute();
  ApHandle hAp_;
  Buffer sbData_;
};

//----------------------------------------------------------

class CloseSocketTask: public Apollo::Task
{
public:
  CloseSocketTask(ApHandle h)
    :hAp_(h)
  {}
  void Execute();
  ApHandle hAp_;
};

//----------------------------------------------------------

class DeleteSocketTask: public Apollo::Task
{
public:
  DeleteSocketTask(Socket* s)
    :s_(s)
  {}
  void Execute();
  Socket* s_;
};

//----------------------------------------------------------

class TCPConnectTask: public Apollo::Task
{
public:
  TCPConnectTask(ApHandle h, SocketAddress& saAddress)
    :hAp_(h)
    ,saAddress_(saAddress)
  {}
  void Execute();
  ApHandle hAp_;
  SocketAddress saAddress_;
};

//----------------------------------------------------------

class TCPListenTask: public Apollo::Task
{
public:
  TCPListenTask(ApHandle h, SocketAddress& saAddress)
    :hAp_(h)
    ,saAddress_(saAddress)
  {}
  void Execute();
  ApHandle hAp_;
  SocketAddress saAddress_;
};

//----------------------------------------------------------

class CloseAllSocketsTask: public Apollo::Task
{
public:
  CloseAllSocketsTask()
  {}
  void Execute();
};

//----------------------------------------------------------

class SocketIO: public Apollo::Thread
{
public:
  SocketIO()
    : Thread("SocketIO")
    , bFinished_(0)
  {};
  
  void Svc();
  void SvcEnd();

  void AddSocket(Socket* s)
  {
    if (!AssertThread()) { return; }
    lSockets_.Add(s);
  }

  void RemoveSocket(Socket* s)
  {
    if (!AssertThread()) { return; }
    lSockets_.Remove(s);
  }

  Socket* FindSocket(ApHandle h)
  {
    if (!AssertThread()) { return 0; }
    Socket* s = 0;
    while ((s = lSockets_.Next(s)) != 0) {
      if (s->hAp_ == h) {
        break;
      }
    }
    return s;
  }

  void AddTask(Apollo::Task* t)
  {
    oTasks_.Put(t);
  }

  void CloseAll();
  UINT ThreadId() { return nThreadId_; }
  int AssertThread();

public:
  int IsFinished() { return bFinished_; }
protected:
  int bFinished_;

protected:
  friend void NetOS::SocketIOThreadProc(UINT nThreadId);
  friend void NetOS_SocketTaskQueue_SigHandler(int nSigNum);
  void OnSocketTaskQueueEvent();

protected:
  friend class CloseAllSocketsTask;
  UINT nThreadId_;
  ListT<Socket, Elem> lSockets_;
  SocketTaskQueue oTasks_;
};

#endif // !defined(SocketIO_h_INCLUDED)
