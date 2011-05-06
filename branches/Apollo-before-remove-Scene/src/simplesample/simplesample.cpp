// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "simplesample.h"
#include "MsgSample.h"

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
  "SimpleSample", // szName
  "Sample", // szServices
  "SimpleSample Module", // szLongName
  "1", // szVersion
  "Used as simple sample for module implementation.", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

SIMPLESAMPLE_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

static void Handle_Sample_Get(Msg_Sample_Get* pMsg)
{
  pMsg->nValue = 42;
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

SIMPLESAMPLE_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  Apollo::hookMsg("Sample_Get", "SimpleSample", (ApCallback) Handle_Sample_Get, 0, 0);

  return 1;
}

SIMPLESAMPLE_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  Apollo::unhookMsg("Sample_Get", "SimpleSample", (ApCallback) Handle_Sample_Get, 0);

  return 1;
}
