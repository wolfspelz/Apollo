// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ExDisplay.h"
#include "ExDisplayModule.h"

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
  "ExDisplay",
  "ExDisplay Module",
  "1",
  "Controls external displays over TCP/SRPC",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

EXDISPLAY_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//---------------------------

EXDISPLAY_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  ExDisplayModuleInstance::Delete();
  if (ExDisplayModuleInstance::Get() != 0) { ok = ExDisplayModuleInstance::Get()->init(); }

  return ok;
}

EXDISPLAY_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (ExDisplayModuleInstance::Get() != 0) {
    ExDisplayModuleInstance::Get()->exit();
    ExDisplayModuleInstance::Delete();
  }

  return ok;
}
