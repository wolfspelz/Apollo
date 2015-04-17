// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "config.h"
#include "Local.h"
#include "ConfigModule.h"

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
  "Config",
  "Config Module",
  "1",
  "The settings database.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

CONFIG_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

CONFIG_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  ConfigModuleInstance::Delete();
  if (ConfigModuleInstance::Get() != NULL) { ok = ConfigModuleInstance::Get()->init(); }

  return ConfigModuleInstance::Get() != 0;
}

CONFIG_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  if (ConfigModuleInstance::Get() != NULL) {
    ConfigModuleInstance::Get()->exit();
    ConfigModuleInstance::Delete();
  }

  return 1;
}
