// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "ApLog.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Vp)
#include "MsgVp.h"
#include "MsgNet.h"

static int g_bTest_Vp_Online = 0;

#if defined(AP_TEST_Net)

static String Test_Vp_NotYet()
{
  String s;

  return s;
}

static void Test_Vp_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Vp_UnitTest_TokenEnd", "Finished Test/Vp"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Vp_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vp_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Vp_UnitTest_Token", "Starting Test/Vp"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(Test_Vp_NotYet);

  if (bTokenEndNow) { Test_Vp_UnitTest_TokenEnd(); }
}

#endif // AP_TEST_Net

//----------------------------------------------------------

#endif // AP_TEST_Vp

void Test_Vp_Register()
{
#if defined(AP_TEST_Vp)
#if defined(AP_TEST_Net)
    if (Apollo::isLoadedModule("Net")) {
      Msg_Net_IsOnline msg;
      if (msg.Request()) {
        if (msg.bIsOnline) {
          g_bTest_Vp_Online = 1;
        }
      }
    }
  #endif // AP_TEST_Net_HTTP

  if (Apollo::isLoadedModule("Vp")) {
    AP_UNITTEST_REGISTER(Test_Vp_NotYet);
  
    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vp_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Vp
}

void Test_Vp_Begin()
{
#if defined(AP_TEST_Vp)
  if (Apollo::isLoadedModule("Vp") && g_bTest_Vp_Online) {
  }
#endif // AP_TEST_Vp
}

void Test_Vp_End()
{
#if defined(AP_TEST_Vp)
  if (Apollo::isLoadedModule("Vp")) {
  }
#endif // AP_TEST_Vp
}
