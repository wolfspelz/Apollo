// ============================================================================
//
// Apollo
//
// ============================================================================

#include "net.h"

#include "Apollo.h"
#include "Local.h"
#include "NetModule.h"

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

static AP_MODULE_INFO AP_UNUSED_VARIABLE g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "HTTP,TCP",
  "Network Module",
  "1",
  "The module does all network I/O.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

NET_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_IsOnline)

AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_DNS_Resolve)
AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_DNS_Resolved)

AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_TCP_Connect)
AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_TCP_DataOut)
AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_TCP_Close)

AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_TCP_Listen)
AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_TCP_ListenStop)

AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_HTTP_Request)
AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_HTTP_Cancel)
AP_REFINSTANCE_MSG_HANDLER(NetModule, Net_HTTP_CancelAll)

AP_REFINSTANCE_MSG_HANDLER(NetModule, MainLoop_EventLoopDelayEnd)
AP_REFINSTANCE_MSG_HANDLER(NetModule, System_SecTimer)
AP_REFINSTANCE_MSG_HANDLER(NetModule, System_3SecTimer)
AP_REFINSTANCE_MSG_HANDLER(NetModule, System_BeforeUnloadModules)
#if defined(AP_TEST)
AP_REFINSTANCE_MSG_HANDLER(NetModule, UnitTest_Begin)
AP_REFINSTANCE_MSG_HANDLER(NetModule, UnitTest_Execute)
AP_REFINSTANCE_MSG_HANDLER(NetModule, UnitTest_End)
#endif

//----------------------------------------------------------

NET_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  int ok = 1;

  if (ok) {
    ok = NetModuleInstance::Get()->Init();
  }

  if (ok) {
    { Msg_MainLoop_EventLoopDelayEnd msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, MainLoop_EventLoopDelayEnd), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_System_SecTimer msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, System_SecTimer), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_System_3SecTimer msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, System_3SecTimer), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_System_BeforeUnloadModules msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, System_BeforeUnloadModules), NetModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_IsOnline msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_IsOnline), NetModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_DNS_Resolve msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_DNS_Resolve), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_DNS_Resolved msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_DNS_Resolved), NetModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_TCP_Connect msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_Connect), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_DataOut msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_DataOut), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_Close msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_Close), NetModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_TCP_Listen msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_Listen), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_TCP_ListenStop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_ListenStop), NetModuleInstance::Get(), ApCallbackPosNormal); }

    { Msg_Net_HTTP_Request msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_HTTP_Request), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_HTTP_Cancel msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_HTTP_Cancel), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_Net_HTTP_CancelAll msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_HTTP_CancelAll), NetModuleInstance::Get(), ApCallbackPosNormal); }

#if defined(AP_TEST)
    { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, UnitTest_Begin), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, UnitTest_Execute), NetModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, UnitTest_End), NetModuleInstance::Get(), ApCallbackPosNormal); }
#endif
  }

  return ok;
}

NET_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_Net_IsOnline msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_IsOnline), NetModuleInstance::Get()); }

  { Msg_Net_DNS_Resolve msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_DNS_Resolve), NetModuleInstance::Get()); }
  { Msg_Net_DNS_Resolved msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_DNS_Resolved), NetModuleInstance::Get()); }

  { Msg_Net_TCP_Close msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_Close), NetModuleInstance::Get()); }
  { Msg_Net_TCP_DataOut msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_DataOut), NetModuleInstance::Get()); }
  { Msg_Net_TCP_Connect msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_Connect), NetModuleInstance::Get()); }

  { Msg_Net_TCP_Listen msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_Listen), NetModuleInstance::Get()); }
  { Msg_Net_TCP_ListenStop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_TCP_ListenStop), NetModuleInstance::Get()); }

  { Msg_Net_HTTP_Request msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_HTTP_Request), NetModuleInstance::Get()); }
  { Msg_Net_HTTP_Cancel msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_HTTP_Cancel), NetModuleInstance::Get()); }
  { Msg_Net_HTTP_CancelAll msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, Net_HTTP_CancelAll), NetModuleInstance::Get()); }

  { Msg_MainLoop_EventLoopDelayEnd msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, MainLoop_EventLoopDelayEnd), NetModuleInstance::Get()); }
  { Msg_System_SecTimer msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, System_SecTimer), NetModuleInstance::Get()); }
  { Msg_System_3SecTimer msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, System_3SecTimer), NetModuleInstance::Get()); }
  { Msg_System_BeforeUnloadModules msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, System_BeforeUnloadModules), NetModuleInstance::Get()); }

#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, UnitTest_Begin), NetModuleInstance::Get()); }
  { Msg_UnitTest_Execute msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, UnitTest_Execute), NetModuleInstance::Get()); }
  { Msg_UnitTest_End msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(NetModule, UnitTest_End), NetModuleInstance::Get()); }
#endif

  NetModuleInstance::Delete();

  return 1;
}
