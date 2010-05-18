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

  Context* findContext(ApHandle h);

  int addConnection(ApHandle hConnection, Connection* pConnection);
  int removeConnection(ApHandle hConnection);

  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);

  void On_Navigation_Receive(Msg_Navigation_Receive* pMsg);
  void On_Navigation_Send(Msg_Navigation_Send* pMsg);

  void On_BrowserInfo_Visibility(Msg_BrowserInfo_Visibility* pMsg);
  void On_BrowserInfo_Position(Msg_BrowserInfo_Position* pMsg);
  void On_BrowserInfo_Size(Msg_BrowserInfo_Size* pMsg);

  AP_UNITTEST_DECLAREHOOK();

protected:
  Context* createContext(ApHandle hContext);
  int destroyContext(ApHandle hContext);

  Connection* findConnection(ApHandle h);
  ApHandle findConnectionHandleByContextHandle(ApHandle hContext);
  void associateContextWithConnection(ApHandle hContext, ApHandle hConnection);
  void addContextToConnection(ApHandle hContext, ApHandle hConnection);
  void removeContextFromConnection(ApHandle hContext, ApHandle hConnection);

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
