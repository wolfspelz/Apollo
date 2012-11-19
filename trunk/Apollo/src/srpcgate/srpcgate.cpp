// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "srpcgate.h"
#include "MsgMainLoop.h"
#include "MsgSrpcGate.h"
#include "MsgHttpServer.h"
#include "SrpcGateHelper.h"

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

#define MODULE_NAME "SrpcGate"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "SrpcGate",
  "SrpcGate Module",
  "1",
  "Gateways from SRPC messages to native messages",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

SRPCGATE_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class Handler
{
public:
  Handler(const String& sCallName, ApSRPCCallback fnCallback, ApCallbackRef nRef)
    :sCallName_(sCallName)
    ,fnCallback_(fnCallback)
    ,nRef_(nRef)
  {}

  int handle(ApSRPCMessage* pMsg);

protected:
  String sCallName_;
  ApSRPCCallback fnCallback_;
  ApCallbackRef nRef_;
};

typedef StringTree<Handler> HandlerList;
typedef StringTreeNode<Handler> HandlerNode;
typedef StringTreeIterator<Handler> HandlerIterator;

class SrpcGateModule
{
public:
  SrpcGateModule() {}

  int Init();
  void Exit();

  void On_SrpcGate_Register(Msg_SrpcGate_Register* pMsg);
  void On_SrpcGate_Unregister(Msg_SrpcGate_Unregister* pMsg);
  void On_SrpcGate_Handler(ApSRPCMessage* pMsg);

  void On_HttpServer_ReceiveRequest(Msg_HttpServer_ReceiveRequest* pMsg);

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;

protected:
  HandlerList handlers_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<SrpcGateModule> SrpcGateModuleInstance;

//----------------------------------------------------------

int Handler::handle(ApSRPCMessage* pMsg)
{
  int ok = 0;

  if (fnCallback_ != 0) {
    pMsg->Ref(nRef_);
    fnCallback_(pMsg);
    ok = 1;
  }

  return ok;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(SrpcGateModule, SrpcGate_Register)
{
  int ok = 0;

  Handler handler(pMsg->sCallName, pMsg->fnHandler, pMsg->nRef);
  ok = handlers_.Set(pMsg->sCallName, handler);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(SrpcGateModule, SrpcGate_Unregister)
{
  int ok = 0;

  ok = handlers_.Unset(pMsg->sCallName);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSGCLASS_HANDLER_METHOD(SrpcGateModule, SrpcGate_Handler, ApSRPCMessage)
{
  int ok = 0;

  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);
  HandlerNode* pNode = handlers_.Find(sMethod);
  if (pNode == 0) { throw ApException(LOG_CONTEXT, "Method=%s not registered", _sz(sMethod)); }

  ok = pNode->Value().handle(pMsg);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(SrpcGateModule, HttpServer_ReceiveRequest)
{
  #define SrpcGateModule_HttpServer_Request_sUriPrefix "/" MODULE_NAME

  if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 1) && pMsg->sUri.startsWith(SrpcGateModule_HttpServer_Request_sUriPrefix)) {

    String sUriPrefix = SrpcGateModule_HttpServer_Request_sUriPrefix;
    try {
      Apollo::SrpcMessage request;

      // Params from query
      String sQuery = pMsg->sUri;
      String sBase; sQuery.nextToken("?", sBase);
      List lQuery;
      KeyValueBlob2List(sQuery, lQuery, "&", "=", "");
      for (Elem* e = 0; (e = lQuery.Next(e)) != 0; ) {
        String sKey = e->getName();
        String sValue = e->getString();
        sKey.unescape(String::EscapeURL);
        sValue.unescape(String::EscapeURL);
        request.set(sKey, sValue);
      }

      // Params from body
      String sBody;
      pMsg->sbBody.GetString(sBody);
      List lBody;
      KeyValueLfBlob2List(sBody, lBody);
      for (Elem* e = 0; (e = lBody.Next(e)) != 0; ) {
        String sKey = e->getName();
        String sValue = e->getString();
        sValue.unescape(String::EscapeCRLF);
        request.set(sKey, sValue);
      }

      // Do the call
      ApSRPCMessage msg(SRPCGATE_HANDLER_TYPE);
      request >> msg.srpc;
      (void) msg.Call();

      String sResponse;
      // Create response
      if (msg.apStatus == ApMessage::Error) {
        sResponse = msg.sComment;
      } else {
        sResponse = msg.response.toString();
      }

      Msg_HttpServer_SendResponse msgSHR;
      msgSHR.sbBody.SetData(sResponse);
      msgSHR.kvHeader.add("Content-type", "text/plain");

      msgSHR.hConnection = pMsg->hConnection;
      msgSHR.kvHeader.add("Pragma", "no-cache");
      msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
      msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
      if (!msgSHR.Request()) { throw ApException(LOG_CONTEXT, "Msg_HttpServer_SendResponse failed: conn=" ApHandleFormat "", ApHandlePrintf(msgSHR.hConnection)); }

      pMsg->Stop();
      pMsg->apStatus = ApMessage::Ok;

    } catch (ApException& ex) {

      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(ex.getText())));

      Msg_HttpServer_SendResponse msgSHR;
      msgSHR.hConnection = pMsg->hConnection;
      msgSHR.nStatus = 500;
      msgSHR.sMessage = "Internal Error";
      msgSHR.kvHeader.add("Content-type", "text/plain");
      msgSHR.kvHeader.add("Pragma", "no-cache");
      msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
      msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
      String sBody = ex.getText();
      msgSHR.sbBody.SetData(sBody);
      if (!msgSHR.Request()) {
        { throw ApException(LOG_CONTEXT, "Msg_HttpServer_SendResponse (for error message) failed: conn=" ApHandleFormat "", ApHandlePrintf(msgSHR.hConnection)); }
      } else {
        pMsg->Stop();
        pMsg->apStatus = ApMessage::Ok;
      }
    }

  } // sUriPrefix
}

//----------------------------------------------------------
#include "MsgMainLoop.h"

void SrpcGate_MainLoop_EndLoop(ApSRPCMessage* pMsg)
{
  Msg_MainLoop_EndLoop msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

//--------------------------
#include "MsgXmpp.h"

void SrpcGate_Xmpp_Connect(ApSRPCMessage* pMsg)
{
  Msg_Xmpp_Connect msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Xmpp_Disconnect(ApSRPCMessage* pMsg)
{
  Msg_Xmpp_Disconnect msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

//--------------------------
#include "MsgVp.h"

void SrpcGate_Vp_OpenContext(ApSRPCMessage* pMsg)
{
  Msg_Vp_OpenContext msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Vp_NavigateContext(ApSRPCMessage* pMsg)
{
  Msg_Vp_NavigateContext msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.sUrl = pMsg->srpc.getString("sUrl");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Vp_CloseContext(ApSRPCMessage* pMsg)
{
  Msg_Vp_CloseContext msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

//--------------------------
#include "MsgSystem.h"

void SrpcGate_System_GetTime(ApSRPCMessage* pMsg)
{
  Msg_System_GetTime msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("nSec", msg.nSec);
  pMsg->response.set("nMicroSec", msg.nMicroSec);
}

void SrpcGate_System_GetHandle(ApSRPCMessage* pMsg)
{
  Msg_System_GetHandle msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("hResult", msg.hResult);
}

void SrpcGate_System_Echo(ApSRPCMessage* pMsg)
{
  Msg_System_Echo msg;
  msg.nIn = pMsg->srpc.getInt("nIn");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("nOut", msg.nOut);
}

//--------------------------
#include "MsgConfig.h"

void SrpcGate_Config_SetValue(ApSRPCMessage* pMsg)
{
  Msg_Config_SetValue msg;
  msg.sPath = pMsg->srpc.getString("sPath");
  msg.sValue = pMsg->srpc.getString("sValue");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Config_GetValue(ApSRPCMessage* pMsg)
{
  Msg_Config_GetValue msg;
  msg.sPath = pMsg->srpc.getString("sPath");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("sValue", msg.sValue);
}

//----------------------------------------------------------

int SrpcGateModule::Init()
{
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SrpcGateModule, SrpcGate_Register, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SrpcGateModule, SrpcGate_Unregister, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SrpcGateModule, SrpcGate_Handler, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SrpcGateModule, HttpServer_ReceiveRequest, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("MainLoop_EndLoop", SrpcGate_MainLoop_EndLoop);
  srpcGateRegistry_.add("Xmpp_Connect", SrpcGate_Xmpp_Connect);
  srpcGateRegistry_.add("Xmpp_Disconnect", SrpcGate_Xmpp_Disconnect);
  srpcGateRegistry_.add("Vp_OpenContext", SrpcGate_Vp_OpenContext);
  srpcGateRegistry_.add("Vp_NavigateContext", SrpcGate_Vp_NavigateContext);
  srpcGateRegistry_.add("Vp_CloseContext", SrpcGate_Vp_CloseContext);
  srpcGateRegistry_.add("System_GetTime", SrpcGate_System_GetTime);
  srpcGateRegistry_.add("System_GetHandle", SrpcGate_System_GetHandle);
  srpcGateRegistry_.add("System_Echo", SrpcGate_System_Echo);
  srpcGateRegistry_.add("Config_SetValue", SrpcGate_Config_SetValue);
  srpcGateRegistry_.add("Config_GetValue", SrpcGate_Config_GetValue);

  return 1;
}

void SrpcGateModule::Exit()
{
  srpcGateRegistry_.finish();
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

SRPCGATE_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  SrpcGateModuleInstance::Delete();
  int ok = SrpcGateModuleInstance::Get()->Init();

  return ok;
}

SRPCGATE_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  SrpcGateModuleInstance::Get()->Exit();
  SrpcGateModuleInstance::Delete();

  return 1;
}
