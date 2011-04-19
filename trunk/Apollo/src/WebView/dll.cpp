// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "dll.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "WebViewModule.h"

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
  "WebView",
  "WebView Module",
  "1",
  "HTML based GUI",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

WEBVIEW_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

WEBVIEW_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  WebViewModuleInstance::Delete();
  if (WebViewModuleInstance::Get() != 0) { ok = WebViewModuleInstance::Get()->init(); }

  return ok;
}

WEBVIEW_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (WebViewModuleInstance::Get() != 0) {
    WebViewModuleInstance::Get()->exit();
    WebViewModuleInstance::Delete();
  }

  return ok;
}
