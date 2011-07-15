// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "dll.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "ArenaModule.h"

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
  "",
  "Web Display Module",
  "1",
  "Displays avatars on web pages using an HTML rendering engine",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

ARENA_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

ARENA_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  ArenaModuleInstance::Delete();
  if (ArenaModuleInstance::Get() != 0) { ok = ArenaModuleInstance::Get()->Init(); }

  return ok;
}

ARENA_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (ArenaModuleInstance::Get() != 0) {
    ArenaModuleInstance::Get()->Exit();
    ArenaModuleInstance::Delete();
  }

  return ok;
}
