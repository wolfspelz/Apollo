// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "MsgUnitTest.h"
#include "ApLog.h"

#if defined(WIN32)
HINSTANCE g_hDllInstance = NULL;

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
  "Test",
  "Test Module",
  "1",
  "Test Module.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

TEST_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

#if defined(AP_TEST)

int On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  
  if (Apollo::getConfig("Test/Builtin", 0)) { Test_Builtin_Register(); }
  if (Apollo::getConfig("Test/Core", 0)) { Test_Core_Register(); }
  if (Apollo::getConfig("Test/Translation", 0)) { Test_Translation_Register(); }
  if (Apollo::getConfig("Test/Net", 0)) { Test_Net_Register(); }
  if (Apollo::getConfig("Test/Vpi", 0)) { Test_Vpi_Register(); }
  if (Apollo::getConfig("Test/Vp", 0)) { Test_Vp_Register(); }
  if (Apollo::getConfig("Test/Threads", 0)) { Test_Threads_Register(); }
  if (Apollo::getConfig("Test/Fluid", 0)) { Test_Fluid_Register(); }
  if (Apollo::getConfig("Test/Arena", 0)) { Test_Arena_Register(); }

  return 1;
}

int On_UnitTest_Execute(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  
  int nMask = apLog_SetMask(apLog_MaskMaxDebug);

  if (Apollo::getConfig("Test/Builtin", 0)) { Test_Builtin_Execute(); }
  if (Apollo::getConfig("Test/Core", 0)) { Test_Core_Execute(); }
  if (Apollo::getConfig("Test/Translation", 0)) { Test_Translation_Execute(); }
  if (Apollo::getConfig("Test/Net", 0)) { Test_Net_Begin(); }
  if (Apollo::getConfig("Test/Vpi", 0)) { Test_Vpi_Begin(); }
  if (Apollo::getConfig("Test/Vp", 0)) { Test_Vp_Begin(); }
  if (Apollo::getConfig("Test/Threads", 0)) { Test_Threads_Begin(); }
  if (Apollo::getConfig("Test/Fluid", 0)) { Test_Fluid_Begin(); }
  if (Apollo::getConfig("Test/Arena", 0)) { Test_Arena_Begin(); }

  apLog_SetMask(nMask);

  //Msg_UnitTest_Complete msg; msg.bSuccess = 0; msg.sName = "I-do-not-exist"; msg.sMessage = "yy"; msg.Send();

  return 1;
}

int On_UnitTest_End(Msg_UnitTest_End* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  
  if (Apollo::getConfig("Test/Net", 0)) { Test_Net_End(); }
  if (Apollo::getConfig("Test/Vpi", 0)) { Test_Vpi_End(); }
  if (Apollo::getConfig("Test/Vp", 0)) { Test_Vp_End(); }
  if (Apollo::getConfig("Test/Threads", 0)) { Test_Threads_End(); }
  if (Apollo::getConfig("Test/Fluid", 0)) { Test_Fluid_End(); }
  if (Apollo::getConfig("Test/Arena", 0)) { Test_Arena_End(); }

  return 1;
}

#endif // defined(AP_TEST)

//----------------------------------------------------------

TEST_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, (ApCallback) On_UnitTest_Begin, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, (ApCallback) On_UnitTest_Execute, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, (ApCallback) On_UnitTest_End, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
#endif // defined(AP_TEST)

  return 1;
}

TEST_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.UnHook(MODULE_NAME, (ApCallback) On_UnitTest_Begin, 0); }
  { Msg_UnitTest_Execute msg; msg.UnHook(MODULE_NAME, (ApCallback) On_UnitTest_Execute, 0); }
  { Msg_UnitTest_End msg; msg.UnHook(MODULE_NAME, (ApCallback) On_UnitTest_End, 0); }
#endif // defined(AP_TEST)

  return 1;
}
