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
  "ChatWindow", // szServices
  "ChatWindow Module", // szLongName
  "1", // szVersion
  "Used as ChatWindow and for module load/unload unit tests.", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

CHAT_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

CHAT_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  ChatModuleInstance::Delete();
  if (ChatModuleInstance::Get() != 0) { ok = ChatModuleInstance::Get()->Init(); }

  return ok;
}

CHAT_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (ChatModuleInstance::Get() != 0) {
    ChatModuleInstance::Get()->Exit();
    ChatModuleInstance::Delete();
  }

  return ok;
}
