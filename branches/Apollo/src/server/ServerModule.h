// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ServerModule_H_INCLUDED)
#define ServerModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgSystem.h"
#include "MsgServer.h"

class HttpConnection;
class HttpServer;

typedef ApHandlePointerTree<HttpConnection*> HttpConnectionList;
typedef ApHandlePointerTreeNode<HttpConnection*> HttpConnectionNode;
typedef ApHandlePointerTreeIterator<HttpConnection*> HttpConnectionIterator;

class ServerModule
{
public:
  ServerModule()
    :pHttpServer_(0)
  {}

  int init();
  void exit();

  int addHttpConnection(const ApHandle& hHttpConnection, HttpConnection* pHttpConnection);
  int removeHttpConnection(const ApHandle& hHttpConnection);

  void On_Server_StartHTTP(Msg_Server_StartHTTP* pMsg);
  void On_Server_StopHTTP(Msg_Server_StopHTTP* pMsg);

  void On_Server_HttpRequest(Msg_Server_HttpRequest* pMsg);
  void On_Server_HttpResponse(Msg_Server_HttpResponse* pMsg);

  void On_System_RunLevel(Msg_System_RunLevel* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  HttpServer* pHttpServer_;
  HttpConnectionList httpConnections_;
  String sHttpAddress_;
  int nHttpPort_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ServerModule> ServerModuleInstance;

#endif // ServerModule_H_INCLUDED
