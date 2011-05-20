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
#include "MsgServer.h"
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

#define LOG_CHANNEL "SrpcGate"
#define MODULE_NAME "SrpcGate"

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

  void On_SrpcGate_Register(Msg_SrpcGate_Register* pMsg);
  void On_SrpcGate_Unregister(Msg_SrpcGate_Unregister* pMsg);
  void On_SrpcGate(ApSRPCMessage* pMsg);

  void On_Server_HttpRequest(Msg_Server_HttpRequest* pMsg);

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;

protected:
  HandlerList handlers_;
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

void SrpcGateModule::On_SrpcGate_Register(Msg_SrpcGate_Register* pMsg)
{
  int ok = 0;

  Handler handler(pMsg->sCallName, pMsg->fnHandler, pMsg->nRef);
  ok = handlers_.Set(pMsg->sCallName, handler);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void SrpcGateModule::On_SrpcGate_Unregister(Msg_SrpcGate_Unregister* pMsg)
{
  int ok = 0;

  ok = handlers_.Unset(pMsg->sCallName);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void SrpcGateModule::On_SrpcGate(ApSRPCMessage* pMsg)
{
  int ok = 0;

  String sMethod = pMsg->srpc.getString("Method");
  HandlerNode* pNode = handlers_.Find(sMethod);
  if (pNode != 0) {
    ok = pNode->Value().handle(pMsg);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void SrpcGateModule::On_Server_HttpRequest(Msg_Server_HttpRequest* pMsg)
{
  #define SrpcGateModule_Server_HttpRequest_sUriPrefix "/" MODULE_NAME

  if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 1) && pMsg->sUri.startsWith(SrpcGateModule_Server_HttpRequest_sUriPrefix)) {

    String sUriPrefix = SrpcGateModule_Server_HttpRequest_sUriPrefix;
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
      ApSRPCMessage msg("SrpcGate");
      request >> msg.srpc;
      (void) msg.Call();

      // Create response
      String sResponse = msg.response.toString();

      Msg_Server_HttpResponse msgSHR;
      msgSHR.sbBody.SetData(sResponse);
      msgSHR.kvHeader.add("Content-type", "text/plain");

      msgSHR.hConnection = pMsg->hConnection;
      msgSHR.kvHeader.add("Pragma", "no-cache");
      msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
      msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
      if (!msgSHR.Request()) { throw ApException("Msg_Server_HttpResponse failed: conn=" ApHandleFormat "", ApHandleType(msgSHR.hConnection)); }

      pMsg->Stop();
      pMsg->apStatus = ApMessage::Ok;

    } catch (ApException& ex) {

      apLog_Warning((LOG_CHANNEL, "SrpcGateModule::Server_HttpRequest", "%s", StringType(ex.getText())));

      Msg_Server_HttpResponse msgSHR;
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
        { throw ApException("Msg_Server_HttpResponse (for error message) failed: conn=" ApHandleFormat "", ApHandleType(msgSHR.hConnection)); }
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
  msg.hContext = Apollo::string2Handle(pMsg->srpc.getString("hContext"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Vp_NavigateContext(ApSRPCMessage* pMsg)
{
  Msg_Vp_NavigateContext msg;
  msg.hContext = Apollo::string2Handle(pMsg->srpc.getString("hContext"));
  msg.sUrl = pMsg->srpc.getString("sUrl");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Vp_CloseContext(ApSRPCMessage* pMsg)
{
  Msg_Vp_CloseContext msg;
  msg.hContext = Apollo::string2Handle(pMsg->srpc.getString("hContext"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

//--------------------------
#include "MsgSystem.h"

void SrpcGate_System_GetTime(ApSRPCMessage* pMsg)
{
  Msg_System_GetTime msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.createResponse(pMsg->srpc);
  pMsg->response.set("nSec", msg.nSec);
  pMsg->response.set("nMicroSec", msg.nMicroSec);
}

void SrpcGate_System_GetHandle(ApSRPCMessage* pMsg)
{
  Msg_System_GetHandle msg;
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("h", msg.h);
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

AP_REFINSTANCE_MSG_HANDLER(SrpcGateModule, SrpcGate_Register)
AP_REFINSTANCE_MSG_HANDLER(SrpcGateModule, SrpcGate_Unregister)
AP_REFINSTANCE_SRPC_HANDLER(SrpcGateModule, ApSRPCMessage, SrpcGate)
AP_REFINSTANCE_MSG_HANDLER(SrpcGateModule, Server_HttpRequest)

//----------------------------------------------------------

SRPCGATE_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_SrpcGate_Register msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(SrpcGateModule, SrpcGate_Register), SrpcGateModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_SrpcGate_Unregister msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(SrpcGateModule, SrpcGate_Unregister), SrpcGateModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { ApSRPCMessage msg("SrpcGate"); msg.Hook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(SrpcGateModule, SrpcGate), SrpcGateModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  { Msg_Server_HttpRequest msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(SrpcGateModule, Server_HttpRequest), SrpcGateModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  SrpcGateModule* pModule = SrpcGateModuleInstance::Get();
  if (pModule) {
    pModule->srpcGateRegistry_.add("MainLoop_EndLoop", SrpcGate_MainLoop_EndLoop);
    pModule->srpcGateRegistry_.add("Xmpp_Connect", SrpcGate_Xmpp_Connect);
    pModule->srpcGateRegistry_.add("Xmpp_Disconnect", SrpcGate_Xmpp_Disconnect);
    pModule->srpcGateRegistry_.add("Vp_OpenContext", SrpcGate_Vp_OpenContext);
    pModule->srpcGateRegistry_.add("Vp_NavigateContext", SrpcGate_Vp_NavigateContext);
    pModule->srpcGateRegistry_.add("Vp_CloseContext", SrpcGate_Vp_CloseContext);
    pModule->srpcGateRegistry_.add("System_GetTime", SrpcGate_System_GetTime);
    pModule->srpcGateRegistry_.add("System_GetHandle", SrpcGate_System_GetHandle);
    pModule->srpcGateRegistry_.add("System_Echo", SrpcGate_System_Echo);
    pModule->srpcGateRegistry_.add("Config_SetValue", SrpcGate_Config_SetValue);
    pModule->srpcGateRegistry_.add("Config_GetValue", SrpcGate_Config_GetValue);
  }

  return pModule != 0;
}

SRPCGATE_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  SrpcGateModule* pModule = SrpcGateModuleInstance::Get();
  if (pModule) {
    pModule->srpcGateRegistry_.finish();
  }

  { Msg_SrpcGate_Register msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(SrpcGateModule, SrpcGate_Register), SrpcGateModuleInstance::Get()); }
  { Msg_SrpcGate_Unregister msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(SrpcGateModule, SrpcGate_Unregister), SrpcGateModuleInstance::Get()); }
  { ApSRPCMessage msg("SrpcGate"); msg.Unhook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(SrpcGateModule, SrpcGate), SrpcGateModuleInstance::Get()); }

  { Msg_Server_HttpRequest msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(SrpcGateModule, Server_HttpRequest), SrpcGateModuleInstance::Get()); }

  SrpcGateModuleInstance::Delete();

  return 1;
}
