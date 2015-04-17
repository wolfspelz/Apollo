// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "netapi.h"

#include "ApLog.h"

#include "NetAPIModule.h"

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

#define MODULE_NAME "NetAPI"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "NetAPI",
  "NetAPI Module",
  "1",
  "Provides an object oriented API for messages of the network module.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

NETAPI_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, System_BeforeUnloadModules)

// For TCPConnection
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_TCP_Connected)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_TCP_DataIn)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_TCP_Closed)

// For TCPServer
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_TCP_Listening)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_TCP_ConnectionAccepted)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_TCP_ListenStopped)

// For HTTPClient
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_HTTP_Connected)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_HTTP_Header)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_HTTP_DataIn)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_HTTP_Failed)
AP_REFINSTANCE_MSG_HANDLER(NetAPIModule, Net_HTTP_Closed)

//----------------------------------------------------------

NETAPI_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  int ok = 1;

  if (ok) {
    ok = NetAPIModuleInstance::Get()->Init();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "NetAPIModuleInstance::Get()->Init failed"));
    }
  }

  if (ok) {
    { Msg_System_BeforeUnloadModules msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, System_BeforeUnloadModules), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_TCP_Connected msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_Connected), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_DataIn msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_DataIn), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_Closed msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_Closed), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_TCP_Listening msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_Listening), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_ConnectionAccepted msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_ConnectionAccepted), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_ListenStopped msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_ListenStopped), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    
    { Msg_Net_HTTP_Connected msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Connected), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_HTTP_Header msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Header), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_HTTP_DataIn msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_DataIn), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_HTTP_Failed msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Failed), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_HTTP_Closed msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Closed), NetAPIModuleInstance::Get(), ApCallbackPosNormal); }
  }

  return ok;
}

NETAPI_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_System_BeforeUnloadModules msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, System_BeforeUnloadModules), NetAPIModuleInstance::Get()); }

  { Msg_Net_TCP_Listening msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_Listening), NetAPIModuleInstance::Get()); }
  { Msg_Net_TCP_ConnectionAccepted msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_ConnectionAccepted), NetAPIModuleInstance::Get()); }
  { Msg_Net_TCP_ListenStopped msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_ListenStopped), NetAPIModuleInstance::Get()); }

  { Msg_Net_TCP_Connected msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_Connected), NetAPIModuleInstance::Get()); }
  { Msg_Net_TCP_DataIn msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_DataIn), NetAPIModuleInstance::Get()); }
  { Msg_Net_TCP_Closed msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_TCP_Closed), NetAPIModuleInstance::Get()); }

  { Msg_Net_HTTP_Connected msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Connected), NetAPIModuleInstance::Get()); }
  { Msg_Net_HTTP_Header msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Header), NetAPIModuleInstance::Get()); }
  { Msg_Net_HTTP_DataIn msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_DataIn), NetAPIModuleInstance::Get()); }
  { Msg_Net_HTTP_Failed msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Failed), NetAPIModuleInstance::Get()); }
  { Msg_Net_HTTP_Closed msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetAPIModule, Net_HTTP_Closed), NetAPIModuleInstance::Get()); }

  NetAPIModuleInstance::Delete();

  return 1;
}
