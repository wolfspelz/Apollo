// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ScWidget.h"
#include "MsgScWidget.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "ScWidgetModule.h"

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
  "ScWidget",
  "ScWidget Module",
  "1",
  "Provides widgets based on the 2d scene graph.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

SCWIDGET_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

SCWIDGET_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  ScWidgetModuleInstance::Delete();
  if (ScWidgetModuleInstance::Get() != 0) { ok = ScWidgetModuleInstance::Get()->Init(); }

  return ok;
}

SCWIDGET_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (ScWidgetModuleInstance::Get() != 0) {
    ScWidgetModuleInstance::Get()->Exit();
    ScWidgetModuleInstance::Delete();
  }

  return ok;
}
