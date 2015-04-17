// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NavigationModule_H_INCLUDED)
#define NavigationModule_H_INCLUDED

#include "Apollo.h"
#include "ApContainer.h"
#include "Local.h"

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
    {}

  int init();
  void exit();

  Context* findContext(const ApHandle& h);

  void On_TcpServer_SrpcRequest(Msg_TcpServer_SrpcRequest* pMsg);
  void On_TcpServer_Disconnected(Msg_TcpServer_Disconnected* pMsg);
  void On_WebSocketServer_SrpcRequest(Msg_WebSocketServer_SrpcRequest* pMsg);
  void On_WebSocketServer_Disconnected(Msg_WebSocketServer_Disconnected* pMsg);

  void On_Navigation_NavigatorHello(Msg_Navigation_NavigatorHello* pMsg);
  void On_Navigation_NavigatorBye(Msg_Navigation_NavigatorBye* pMsg);
  void On_Navigation_ContextOpen(Msg_Navigation_ContextOpen* pMsg);
  void On_Navigation_ContextNavigate(Msg_Navigation_ContextNavigate* pMsg);
  void On_Navigation_ContextClose(Msg_Navigation_ContextClose* pMsg);
  void On_Navigation_ContextShow(Msg_Navigation_ContextShow* pMsg);
  void On_Navigation_ContextHide(Msg_Navigation_ContextHide* pMsg);
  void On_Navigation_ContextPosition(Msg_Navigation_ContextPosition* pMsg);
  void On_Navigation_ContextSize(Msg_Navigation_ContextSize* pMsg);
  void On_Navigation_ContextNativeWindow(Msg_Navigation_ContextNativeWindow* pMsg);

  void On_BrowserInfo_Visibility(Msg_BrowserInfo_Visibility* pMsg);
  void On_BrowserInfo_Position(Msg_BrowserInfo_Position* pMsg);
  void On_BrowserInfo_Size(Msg_BrowserInfo_Size* pMsg);

  AP_UNITTEST_DECLAREHOOK();

protected:
  Context* createContext(const ApHandle& hContext);
  int destroyContext(const ApHandle& hContext);

  ApHandle findConnectionHandleByContextHandle(const ApHandle& hContext);
  void associateContextWithConnection(const ApHandle& hContext, const ApHandle& hConnection);
  void addContextToConnection(const ApHandle& hContext, const ApHandle& hConnection);
  void removeContextFromConnection(const ApHandle& hContext, const ApHandle& hConnection);
  void cleanupConnectionAssociatedContexts(const ApHandle& hConnection);

public:
  ContextList contexts_;
  ConnectionContextList connectionContexts_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class NavigationModuleTester;
#endif
};

typedef ApModuleSingleton<NavigationModule> NavigationModuleInstance;

#endif // NavigationModule_H_INCLUDED
