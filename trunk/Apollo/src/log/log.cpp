// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "log.h"
#include "logModule.h"

// ----------------------------------

#if defined(WIN32)
  static HINSTANCE g_hDllInstance = NULL;

  BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
  {
    AP_MEMORY_CHECK(dwReason);
    g_hDllInstance = (HINSTANCE) hModule;
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
  "log",
  "logging module",
  "1",
  "The module implements additional logging functionality (file/console/etc.)",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

LOG_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

LOG_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);  
  LogModuleInstance::Delete();
  int ok = 1;
  if (ok) { ok = LogModuleInstance::Get() != 0; }
  if (ok) { LogModuleInstance::Get()->Init(); }
  return (ok);
}

LOG_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  if (LogModuleInstance::Get() != 0) {
    LogModuleInstance::Get()->Exit();
    LogModuleInstance::Delete();
  }
  return (int)true;
}
