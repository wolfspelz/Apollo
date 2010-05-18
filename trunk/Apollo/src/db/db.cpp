// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "db.h"
#include "Local.h"
#include "DbModule.h"

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
  "Db",
  "DB Module",
  "1",
  "The database module.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

DB_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

DB_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  DbModuleInstance::Delete();
  if (DbModuleInstance::Get() != NULL) { ok = DbModuleInstance::Get()->init(); }

  return DbModuleInstance::Get() != 0;
}

DB_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  if (DbModuleInstance::Get() != NULL) {
    DbModuleInstance::Get()->exit();
    DbModuleInstance::Delete();
  }

  return 1;
}
