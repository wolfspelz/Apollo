// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "dll.h"
#include "Local.h"

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
  MODULE_NAME, // szName
  "", // szServices
  "Setup Module", // szLongName
  "1", // szVersion
  "Perform post installer, pre normal run setup actions.", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

SETUP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

SETUP_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  SetupModuleInstance::Delete();
  if (SetupModuleInstance::Get() != 0) { ok = SetupModuleInstance::Get()->Init(); }

  return ok;
}

SETUP_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (SetupModuleInstance::Get() != 0) {
    SetupModuleInstance::Get()->Exit();
    SetupModuleInstance::Delete();
  }

  return ok;
}
