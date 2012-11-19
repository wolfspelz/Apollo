// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgConfig.h"
#include "ServerModule.h"
#include "HttpServer.h"
#include "HttpConnection.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "WebSocketServer.h"
#include "WebSocketConnection.h"
#include "MsgSrpcGate.h"

int ServerModule::addHttpConnection(const ApHandle& hConnection, HttpConnection* pConnection)
{
  int ok = 1;

  ok = httpConnections_.Set(hConnection, pConnection);

  return ok;
}

int ServerModule::removeHttpConnection(const ApHandle& hConnection)
{
  int ok = 1;

  ok = httpConnections_.Unset(hConnection);
  // HttpConnection deletes itself externally

  return ok;
}

HttpConnection* ServerModule::findHttpConnection(const ApHandle& hConnection)
{
  HttpConnection* pConnection = 0;
  httpConnections_.Get(hConnection, pConnection);
  return pConnection;
}

//--------------------------

int ServerModule::addTcpConnection(const ApHandle& hConnection, TcpConnection* pConnection)
{
  int ok = 1;

  ok = tcpConnections_.Set(hConnection, pConnection);

  return ok;
}

int ServerModule::removeTcpConnection(const ApHandle& hConnection)
{
  int ok = 1;

  ok = tcpConnections_.Unset(hConnection);
  // TcpConnection deletes itself externally

  return ok;
}

TcpConnection* ServerModule::findTcpConnection(const ApHandle& hConnection)
{
  TcpConnection* pConnection = 0;
  tcpConnections_.Get(hConnection, pConnection);
  return pConnection;
}

//--------------------------

int ServerModule::addWebSocketConnection(const ApHandle& hConnection, WebSocketConnection* pConnection)
{
  int ok = 1;

  ok = websocketConnections_.Set(hConnection, pConnection);

  return ok;
}

int ServerModule::removeWebSocketConnection(const ApHandle& hConnection)
{
  int ok = 1;

  ok = websocketConnections_.Unset(hConnection);
  // WebSocketConnection deletes itself externally

  return ok;
}

WebSocketConnection* ServerModule::findWebSocketConnection(const ApHandle& hConnection)
{
  WebSocketConnection* pConnection = 0;
  websocketConnections_.Get(hConnection, pConnection);
  return pConnection;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ServerModule, Server_StartHttp)
{
  if (pHttpServer_ == 0) {
    pHttpServer_ = new HttpServer();
    if (!pHttpServer_) { throw ApException(LOG_CONTEXT, "new HttpServer failed"); }

    sHttpAddress_ = Apollo::getModuleConfig(MODULE_NAME, "HTTP/Address", "localhost");
    nHttpPort_ = Apollo::getModuleConfig(MODULE_NAME, "HTTP/Port", 23761);
    if (!pHttpServer_->Start(sHttpAddress_, nHttpPort_)) { throw ApException(LOG_CONTEXT, "pServer_->Start(%s, %d) failed", _sz(sHttpAddress_), nHttpPort_); }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_StopHttp)
{
  if (pHttpServer_ != 0) {
    pHttpServer_->Stop();
    delete pHttpServer_;
    pHttpServer_ = 0;
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, HttpServer_ReceiveRequest)
{
  Msg_HttpServer_SendResponse msg;
  msg.hConnection = pMsg->hConnection;
  msg.nStatus = 404;
  msg.sMessage = "Not Found";
  msg.sProtocol = pMsg->sProtocol;
  String sBody = "You have hit the default handler. No one else seems to care.";
  msg.sbBody.SetData(sBody);
  msg.kvHeader.add("Content-type", "text/plain");
  msg.kvHeader.add("Content-length", String::from(msg.sbBody.Length()));

  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_HttpServer_SendResponse failed conn=" ApHandleFormat "", ApHandlePrintf(pMsg->hConnection)));
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, HttpServer_SendResponse)
{
  HttpConnection* pConnection = findHttpConnection(pMsg->hConnection);
  if (pConnection == 0) { throw ApException(LOG_CONTEXT, "findHttpConnection(" ApHandleFormat ") failed", ApHandlePrintf(pMsg->hConnection)); }

  String sHeader;

  if (!pMsg->sProtocol) {
    pMsg->sProtocol = "HTTP/1.1";
  }

  if (pMsg->nStatus == 0) {
    pMsg->nStatus = 200;
  }

  if (!pMsg->sMessage) {
    if (pMsg->nStatus == 200) {
      pMsg->sMessage = "OK";
    } else {
      pMsg->sMessage = "No Message";
    }
  }

  sHeader.appendf("%s %d %s\r\n", _sz(pMsg->sProtocol), pMsg->nStatus, _sz(pMsg->sMessage));
  
  {
    for (Apollo::KeyValueElem* e = 0; (e = pMsg->kvHeader.nextElem(e)) != 0; ) {
      sHeader.appendf("%s: %s\r\n", _sz(e->getKey()), _sz(e->getString()));
    }
  }

  if (pMsg->sbBody.Length() > 0) {
    if (pMsg->kvHeader.find("content-length", Apollo::KeyValueList::IgnoreCase)) {
    } else {
      sHeader.appendf("Content-length: %d\r\n", pMsg->sbBody.Length());
    }
  } else {
    sHeader.appendf("Content-length: 0\r\n");
  }

  //sHeader += "Connection: close\r\n";
  sHeader += "\r\n";
  
  pConnection->DataOut((unsigned char*) sHeader.c_str(), sHeader.bytes());

  if (pMsg->sbBody.Length() > 0) {
    pConnection->DataOut(pMsg->sbBody.Data(), pMsg->sbBody.Length());
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_StartTCP)
{
  if (pTcpServer_ == 0) {
    pTcpServer_ = new TcpServer();
    if (!pTcpServer_) { throw ApException(LOG_CONTEXT, "new TcpServer failed"); }

    sTcpAddress_ = Apollo::getModuleConfig(MODULE_NAME, "TCP/Address", "localhost");
    nTcpPort_ = Apollo::getModuleConfig(MODULE_NAME, "TCP/Port", 23761);
    if (!pTcpServer_->Start(sTcpAddress_, nTcpPort_)) { throw ApException(LOG_CONTEXT, "pServer_->Start(%s, %d) failed", _sz(sTcpAddress_), nTcpPort_); }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_StopTCP)
{
  if (pTcpServer_ != 0) {
    pTcpServer_->Stop();
    delete pTcpServer_;
    pTcpServer_ = 0;
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, TcpServer_SrpcRequest)
{
  Msg_TcpServer_SendSrpc msgTSSS;

  String sType = pMsg->srpc.getString("ApType");
  ApSRPCMessage msg(sType);

  // Given message type -> send SRPC with custom type
  if (sType.empty()) {
    // Empty message type -> send SRPC via SRPCGATE
    msg.SetType(SRPCGATE_HANDLER_TYPE);
  }

  pMsg->srpc >> msg.srpc;
  (void) msg.Call();
  msg.response >> pMsg->response;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, TcpServer_SendSrpc)
{
  String sMsg = pMsg->srpc.toString();

  if (apLog_IsVerbose) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " send: %s", ApHandlePrintf(pMsg->hConnection), _sz(sMsg)));
  }

  sMsg += "\n";

  TcpConnection* pConnection = findTcpConnection(pMsg->hConnection);
  if (pConnection == 0) { throw ApException(LOG_CONTEXT, "findTcpConnection(" ApHandleFormat ") failed", ApHandlePrintf(pMsg->hConnection)); }
  if (! pConnection->DataOut((unsigned char*) sMsg.c_str(), sMsg.bytes()) ) { throw ApException(LOG_CONTEXT, "Connection " ApHandleFormat " DataOut() failed", ApHandlePrintf(pMsg->hConnection)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_StartWebSocket)
{
  if (pWebSocketServer_ == 0) {
    pWebSocketServer_ = new WebSocketServer();
    if (!pWebSocketServer_) { throw ApException(LOG_CONTEXT, "new WebSocketServer failed"); }

    sWebSocketAddress_ = Apollo::getModuleConfig(MODULE_NAME, "WebSocket/Address", "localhost");
    nWebSocketPort_ = Apollo::getModuleConfig(MODULE_NAME, "WebSocket/Port", 23765);
    if (!pWebSocketServer_->Start(sWebSocketAddress_, nWebSocketPort_)) { throw ApException(LOG_CONTEXT, "pServer_->Start(%s, %d) failed", _sz(sWebSocketAddress_), nWebSocketPort_); }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_StopWebSocket)
{
  if (pWebSocketServer_ != 0) {
    pWebSocketServer_->Stop();
    delete pWebSocketServer_;
    pWebSocketServer_ = 0;
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, WebSocketServer_ReceiveText)
{
  Msg_WebSocketServer_SrpcRequest msg;
  msg.hConnection = pMsg->hConnection;
  msg.srpc.fromString(pMsg->sData);

  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebSocketServer_SrpcRequest() failed conn=" ApHandleFormat "", ApHandlePrintf(msg.hConnection)));
  } else {

    // If a response was provided, then send it
    if (msg.response.length() > 0) {
      Msg_WebSocketServer_SendText msgWSST;
      msgWSST.sData = msg.response.toString();
      msgWSST.hConnection = pMsg->hConnection;
      if (!msgWSST.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebSocketServer_SendText() failed conn=", ApHandlePrintf(msgWSST.hConnection)));
      }
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, WebSocketServer_SendText)
{
  if (apLog_IsVerbose) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " send: %s", ApHandlePrintf(pMsg->hConnection), _sz(pMsg->sData)));
  }

  WebSocketConnection* pConnection = findWebSocketConnection(pMsg->hConnection);
  if (pConnection == 0) { throw ApException(LOG_CONTEXT, "findTcpConnection(" ApHandleFormat ") failed", ApHandlePrintf(pMsg->hConnection)); }
  //if (!pConnection->DataOut((unsigned char*) pMsg->sData.c_str(), pMsg->sData.bytes()) ) { throw ApException(LOG_CONTEXT, "Connection " ApHandleFormat " DataOut() failed", ApHandlePrintf(pMsg->hConnection)); }
  if (!pConnection->SendWebSocketMessage((unsigned char*) pMsg->sData.c_str(), pMsg->sData.bytes()) ) { throw ApException(LOG_CONTEXT, "Connection " ApHandleFormat " SendWebSocketMessage() failed", ApHandlePrintf(pMsg->hConnection)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, WebSocketServer_SrpcRequest)
{
  String sType = pMsg->srpc.getString("ApType");
  ApSRPCMessage msg(sType);

  // Given message type -> send SRPC with custom type
  if (sType.empty()) {
    // Empty message type -> send SRPC via SRPCGATE
    msg.SetType(SRPCGATE_HANDLER_TYPE);
  }

  pMsg->srpc >> msg.srpc;
  (void) msg.Call();
  msg.response >> pMsg->response;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {

    if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 0)) {
      Msg_Server_StartHttp msg;
      msg.Request();
    }

    if (Apollo::getModuleConfig(MODULE_NAME, "TCP/Enabled", 0)) {
      Msg_Server_StartTCP msg;
      msg.Request();
    }

    if (Apollo::getModuleConfig(MODULE_NAME, "WebSocket/Enabled", 0)) {
      Msg_Server_StartWebSocket msg;
      msg.Request();
    }

  } else if (pMsg->sLevel == Msg_System_RunLevel_Shutdown) {

    if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 0)) {
      Msg_Server_StopHttp msg;
      msg.Request();
    }

    if (Apollo::getModuleConfig(MODULE_NAME, "TCP/Enabled", 0)) {
      Msg_Server_StopTCP msg;
      msg.Request();
    }

    if (Apollo::getModuleConfig(MODULE_NAME, "WebSocket/Enabled", 0)) {
      Msg_Server_StopWebSocket msg;
      msg.Request();
    }

  }
}

//--------------------------

#if defined(AP_TEST)

#include "HttpParser.h"

AP_MSG_HANDLER_METHOD(ServerModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Server", 0)) {
    AP_UNITTEST_REGISTER(HttpParser::test);
    AP_UNITTEST_REGISTER(WebSocketConnection::test_KeyString2Buffer);
    AP_UNITTEST_REGISTER(WebSocketConnection::test_Create_ChromePreStandard_Response);
    AP_UNITTEST_REGISTER(WebSocketConnection::test_Create_Standard_Response);
  }
}

AP_MSG_HANDLER_METHOD(ServerModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Server", 0)) {
    AP_UNITTEST_EXECUTE(HttpParser::test);
    AP_UNITTEST_EXECUTE(WebSocketConnection::test_KeyString2Buffer);
    AP_UNITTEST_EXECUTE(WebSocketConnection::test_Create_ChromePreStandard_Response);
    AP_UNITTEST_EXECUTE(WebSocketConnection::test_Create_Standard_Response);
  }
}

AP_MSG_HANDLER_METHOD(ServerModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int ServerModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StartHttp, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StopHttp, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, HttpServer_ReceiveRequest, this, ApCallbackPosLate); // as a default hander
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, HttpServer_SendResponse, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StartTCP, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StopTCP, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, TcpServer_SrpcRequest, this, ApCallbackPosLate); // as default handler and SRPC gateway
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, TcpServer_SendSrpc, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StartWebSocket, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StopWebSocket, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, WebSocketServer_ReceiveText, this, ApCallbackPosLate); // as default handler, convert to SRPC
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, WebSocketServer_SendText, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, WebSocketServer_SrpcRequest, this, ApCallbackPosLate); // as default SRPC handler and SRPC gateway

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, System_RunLevel, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

void ServerModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
