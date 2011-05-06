// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgUnitTest.h"
#include "dun.h"
#include "Local.h"
#include "DunModule.h"

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
  "Dun",
  "Dun Module",
  "1",
  "This module implements additional options.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

DUN_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

DUN_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  DunModuleInstance::Delete();
  if (DunModuleInstance::Get() != 0) { ok = DunModuleInstance::Get()->init(); }

  return ok;
}

DUN_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (DunModuleInstance::Get() != 0) {
    DunModuleInstance::Get()->exit();
    DunModuleInstance::Delete();
  }

  return ok;
}
