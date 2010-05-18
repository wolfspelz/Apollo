// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "server.h"
#include "MsgServer.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "ServerModule.h"

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

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "Server",
  "Server Module",
  "1",
  "Provides access to internal data via localhost-HTTP and maybe more later.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

SERVER_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

SERVER_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  ServerModuleInstance::Delete();
  if (ServerModuleInstance::Get() != 0) { ok = ServerModuleInstance::Get()->init(); }

  return ok;
}

SERVER_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (ServerModuleInstance::Get() != 0) {
    ServerModuleInstance::Get()->exit();
    ServerModuleInstance::Delete();
  }

  return ok;
}
