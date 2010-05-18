// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "fluid.h"
#include "Local.h"
#include "FluidModule.h"

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
  "Fluid",
  "Flash UI Driver",
  "1",
  "This module provides a Flash(tm) based user interface driver.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

FLUID_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

FLUID_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  int ok = 1;

  if (ok) {
    ok = FluidModuleInstance::Get()->init();
  }

  return FluidModuleInstance::Get() != 0;
}

FLUID_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  FluidModuleInstance::Get()->exit();
  FluidModuleInstance::Delete();

  return 1;
}
