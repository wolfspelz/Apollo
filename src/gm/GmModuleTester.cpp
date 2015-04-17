// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "GmModule.h"
#include "GmModuleTester.h"

#if defined(AP_TEST)

static int g_bTest_Online = 0;

static void Test_Gm_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Finished Test/Gm"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Gm_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Gm_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Starting Test/Gm"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(GmModuleTester::test_Encryption);

  //bTokenEndNow = 0;
  //AP_UNITTEST_EXECUTE(GmModuleTester::test_Xx);

  if (bTokenEndNow) { Test_Gm_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#define Test_Gm_Encryption_Text1 "Hello World"

String GmModuleTester::test_Encryption()
{
  String s;

  if (!s) {
    String sEnc = GmModuleInstance::Get()->encrypt(Test_Gm_Encryption_Text1);
    String sDec = GmModuleInstance::Get()->decrypt(sEnc);
    if (!s) { if (sDec != Test_Gm_Encryption_Text1) { s = String::from(__LINE__); } }
  }

  return s;
}

//----------------------

//----------------------------------------------------------

void GmModuleTester::begin()
{
  if (Apollo::getConfig("Test/Gm", 0)) {
    AP_UNITTEST_REGISTER(GmModuleTester::test_Encryption);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Gm_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
}

void GmModuleTester::execute()
{
  if (Apollo::getConfig("Test/Gm", 0)) {
  }
}

void GmModuleTester::end()
{
}

#endif
