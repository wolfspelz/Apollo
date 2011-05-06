// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "navigation.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "NavigationModule.h"

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
  "Navigation",
  "Navigation Module",
  "1",
  "Collects browser tracking intelligence.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

NAVIGATION_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

NAVIGATION_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  NavigationModuleInstance::Delete();
  if (NavigationModuleInstance::Get() != 0) { ok = NavigationModuleInstance::Get()->init(); }

  return ok;
}

NAVIGATION_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (NavigationModuleInstance::Get() != 0) {
    NavigationModuleInstance::Get()->exit();
    NavigationModuleInstance::Delete();
  }

  return ok;
}
