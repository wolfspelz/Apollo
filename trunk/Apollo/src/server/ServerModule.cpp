// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "ServerModule.h"
#include "HttpServer.h"
#include "HttpConnection.h"

int ServerModule::addHttpConnection(ApHandle hHttpConnection, HttpConnection* pHttpConnection)
{
  int ok = 1;

  ok = httpConnections_.Set(hHttpConnection, pHttpConnection);

  return ok;
}

int ServerModule::removeHttpConnection(ApHandle hHttpConnection)
{
  int ok = 1;

  ok = httpConnections_.Unset(hHttpConnection);
  // HttpConnection deletes itself

  return ok;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ServerModule, Server_StartHTTP)
{
  if (pHttpServer_ == 0) {
    pHttpServer_ = new HttpServer();
    if (!pHttpServer_) { throw ApException("new HttpServer failed"); }

    sHttpAddress_ = Apollo::getModuleConfig(MODULE_NAME, "HTTP/Address", "localhost");
    nHttpPort_ = Apollo::getModuleConfig(MODULE_NAME, "HTTP/Port", 23761);
    if (!pHttpServer_->Start(sHttpAddress_, nHttpPort_)) { throw ApException("pServer_->Start(%s, %d) failed", StringType(sHttpAddress_), nHttpPort_); }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_StopHTTP)
{
  if (pHttpServer_ != 0) {
    pHttpServer_->Stop();
    delete pHttpServer_;
    pHttpServer_ = 0;
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_HttpRequest)
{
  Msg_Server_HttpResponse msg;
  msg.hConnection = pMsg->hConnection;
  msg.nStatus = 404;
  msg.sMessage = "Not Found";
  msg.sProtocol = pMsg->sProtocol;
  String sBody = "You have hit the default handler. No one else seems to care.";
  msg.sbBody.SetData(sBody);
  msg.kvHeader.add("Content-type", "text/plain");
  msg.kvHeader.add("Content-length", String::from(msg.sbBody.Length()));

  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "ServerModule::Server_HttpRequest", "Msg_Server_HttpResponse failed conn=" ApHandleFormat "", ApHandleType(pMsg->hConnection)));
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ServerModule, Server_HttpResponse)
{
  HttpConnection* pConnection = 0;
  if (! httpConnections_.Get(pMsg->hConnection, pConnection)) { throw ApException("httpConnections_.Get(" ApHandleFormat ") failed", ApHandleType(pMsg->hConnection)); }

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

  sHeader.appendf("%s %d %s\r\n", StringType(pMsg->sProtocol), pMsg->nStatus, StringType(pMsg->sMessage));
  
  {
    for (Apollo::KeyValueElem* e = 0; (e = pMsg->kvHeader.nextElem(e)) != 0; ) {
      sHeader.appendf("%s: %s\r\n", StringType(e->getKey()), StringType(e->getString()));
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

AP_MSG_HANDLER_METHOD(ServerModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    Msg_Server_StartHTTP msg;
    msg.Request();

  } else if (pMsg->sLevel == Msg_System_RunLevel_Shutdown) {
    Msg_Server_StopHTTP msg;
    msg.Request();

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
  }
}

AP_MSG_HANDLER_METHOD(ServerModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Server", 0)) {
    AP_UNITTEST_EXECUTE(HttpParser::test);
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

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StartHTTP, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_StopHTTP, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_HttpRequest, this, ApCallbackPosLate); // as a default
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ServerModule, Server_HttpResponse, this, ApCallbackPosNormal);

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
