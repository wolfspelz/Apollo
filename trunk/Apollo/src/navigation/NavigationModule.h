// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NavigationModule_H_INCLUDED)
#define NavigationModule_H_INCLUDED

#include "Apollo.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgNavigation.h"
#include "MsgBrowserInfo.h"
#include "MsgMainLoop.h"

class Connection;
class Context;
class SrpcServer;

typedef ApHandlePointerTree<Context*> ContextList;
typedef ApHandlePointerTreeNode<Context*> ContextNode;
typedef ApHandlePointerTreeIterator<Context*> ContextIterator;

typedef ApHandlePointerTree<Connection*> ConnectionList;
typedef ApHandlePointerTreeNode<Connection*> ConnectionNode;
typedef ApHandlePointerTreeIterator<Connection*> ConnectionIterator;

typedef ApHandleTree<ApHandle> ContextHandleList;
typedef ApHandleTreeNode<ApHandle> ContextHandleListNode;
typedef ApHandleTreeIterator<ApHandle> ContextHandleListIterator;

typedef ApHandleTree<ContextHandleList> ConnectionContextList;
typedef ApHandleTreeNode<ContextHandleList> ConnectionContextListNode;
typedef ApHandleTreeIterator<ContextHandleList> ConnectionContextListIterator;

#if defined(AP_TEST)
class NavigationModuleTester
{
public:
  static String associateContextWithConnection();
};
#endif

class NavigationModule
{
public:
  NavigationModule()
    :pServer_(0)
    {}

  int init();
  void exit();

  Context* findContext(const ApHandle& h);

  int addConnection(const ApHandle& hConnection, Connection* pConnection);
  int removeConnection(const ApHandle& hConnection);

  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);

  void On_Navigation_Receive(Msg_Navigation_Receive* pMsg);
  void On_Navigation_Send(Msg_Navigation_Send* pMsg);

  void On_BrowserInfo_Visibility(Msg_BrowserInfo_Visibility* pMsg);
  void On_BrowserInfo_Position(Msg_BrowserInfo_Position* pMsg);
  void On_BrowserInfo_Size(Msg_BrowserInfo_Size* pMsg);

  AP_UNITTEST_DECLAREHOOK();

protected:
  Context* createContext(const ApHandle& hContext);
  int destroyContext(const ApHandle& hContext);

  Connection* findConnection(const ApHandle& h);
  ApHandle findConnectionHandleByContextHandle(const ApHandle& hContext);
  void associateContextWithConnection(const ApHandle& hContext, const ApHandle& hConnection);
  void addContextToConnection(const ApHandle& hContext, const ApHandle& hConnection);
  void removeContextFromConnection(const ApHandle& hContext, const ApHandle& hConnection);

public:
  SrpcServer* pServer_;
  ConnectionList connections_;
  ContextList contexts_;
  ConnectionContextList connectionContexts_;

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class NavigationModuleTester;
#endif
};

typedef ApModuleSingleton<NavigationModule> NavigationModuleInstance;

#endif // NavigationModule_H_INCLUDED
