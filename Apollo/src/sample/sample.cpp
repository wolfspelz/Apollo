// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "sample.h"
#include "MsgSample.h"
#include "MsgUnitTest.h"
#include "Local.h"
#include "SampleModule.h"

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
  "Sample",
  "Sample Module",
  "1",
  "Used as sample and for module load/unload unit tests.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

SAMPLE_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

SAMPLE_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  SampleModuleInstance::Delete();
  if (SampleModuleInstance::Get() != 0) { ok = SampleModuleInstance::Get()->init(); }

  return ok;
}

SAMPLE_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 1;

  if (SampleModuleInstance::Get() != 0) {
    SampleModuleInstance::Get()->exit();
    SampleModuleInstance::Delete();
  }

  return ok;
}
