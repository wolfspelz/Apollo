// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "dll.h"
#include "Local.h"
#include "DialogModule.h"

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
  "Dialog", // szServices
  "Dialog Module", // szLongName
  "1", // szVersion
  "Provides HTML based dialogs and windows.", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

DIALOG_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

DIALOG_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  DialogModuleInstance::Delete();
  if (DialogModuleInstance::Get() != 0) { ok = DialogModuleInstance::Get()->Init(); }

  return ok;
}

DIALOG_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (DialogModuleInstance::Get() != 0) {
    DialogModuleInstance::Get()->Exit();
    DialogModuleInstance::Delete();
  }

  return ok;
}
