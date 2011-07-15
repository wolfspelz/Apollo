// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgUnitTest.h"
#include "gm.h"
#include "Local.h"
#include "GmModule.h"

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
  "Gm",
  "Gm Module",
  "1",
  "Game mechanics (Gm) and application controller. The client runs without, but his module cares for all you need if you want a quick start, low barrier, rich app",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

GM_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

GM_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  GmModuleInstance::Delete();
  if (GmModuleInstance::Get() != 0) { ok = GmModuleInstance::Get()->init(); }

  return ok;
}

GM_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (GmModuleInstance::Get() != 0) {
    GmModuleInstance::Get()->exit();
    GmModuleInstance::Delete();
  }

  return ok;
}
