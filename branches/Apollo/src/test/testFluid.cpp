// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "ApLog.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Fluid)
#include "MsgFluid.h"
#include "MsgConfig.h"
#include "SrpcMessage.h"

static void Test_Fluid_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Fluid_UnitTest_TokenEnd", "Finished Test/Fluid"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

//----------------------------------------------------------

ApHandle hTest_Fluid_Basic_Display = ApNoHandle;
int bTest_Fluid_Basic_Called_Created = 0;
int bTest_Fluid_Basic_Called_Loaded = 0;
int bTest_Fluid_Basic_Called_BeforeUnload = 0;
int bTest_Fluid_Basic_Called_Unloaded = 0;
int bTest_Fluid_Basic_Called_Destroyed = 0;
int bTest_Fluid_Basic_Called_HostCall = 0;
String sTest_Fluid_Basic_Called_HostCall_Error;
typedef struct _Test_Fluid_Basic_Data {
  String sString;
  String sStringLF;
  String sStringUnicode;
  int nInt;
} Test_Fluid_Basic_Data;
Test_Fluid_Basic_Data oTest_Fluid_Basic_Data;
String sTest_Fluid_Basic_ConfigTest_Key;
String sTest_Fluid_Basic_ConfigTest_Value;

static void Test_Fluid_Basic_ConfigTest_Start()
{
  sTest_Fluid_Basic_ConfigTest_Key = "Fluid/ConfigTest";
  sTest_Fluid_Basic_ConfigTest_Value = Apollo::getModuleConfig(MODULE_NAME, sTest_Fluid_Basic_ConfigTest_Key, "");

  {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.srpc.setString("sModule", "test");
    msg.srpc.setString("sKey", sTest_Fluid_Basic_ConfigTest_Key);
    msg.srpc.setString("Method", "StartConfigTest");
    msg.Call();
  }
}

static void Test_Fluid_Basic_AIRInterface_Start()
{
  {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.srpc.setString("Method", "GetTotalMemory");
    msg.Call();
  }
}

static void Test_Fluid_Basic_IO_Host2Display_Start()
{
  {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.srpc.setString("Method", "SetValues");
    msg.srpc.setString("sString", "abc");
    msg.srpc.setString("sStringLF", "abc\ndef");
    msg.srpc.setString("sStringUnicode", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a");
    msg.srpc.setInt("nInt", 42);
    msg.srpc.setString("kvList", "a=111\nb=222\n");
    msg.Call();
  }

  {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.srpc.setString("Method", "GetValues");
    msg.Call();
  }
}

static void Test_Fluid_Basic_IO_Display2Host_Start()
{
  {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.srpc.setString("Method", "StartExchange");
    msg.Call();
  }
}

static void Test_Fluid_Basic_Finish()
{
  String s;

  if (!Apollo::getModuleConfig(MODULE_NAME, "Fluid/SkipClose", 0)) {
    Msg_Fluid_Destroy msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    if (!msg.Request()) {
      s = "Msg_Fluid_Destroy failed";
    }
  }

  AP_UNITTEST_RESULT(Test_Fluid_Basic_Finish, !s, s);
}

static void Test_Fluid_Basic_HostCall_Test_Fluid(Msg_Fluid_HostCall* pMsg)
{
  if (pMsg->hDisplay == hTest_Fluid_Basic_Display) {

    bTest_Fluid_Basic_Called_HostCall = 1;

    String sMethod = pMsg->srpc.getString("Method");
    if (0){
    } else if (sMethod == "GetValuesResponse") {
      String s;

      String sString = pMsg->srpc.getString("sString");
      String sStringLF = pMsg->srpc.getString("sStringLF");
      String sStringUnicode = pMsg->srpc.getString("sStringUnicode");
      int nInt = pMsg->srpc.getInt("nInt");
      String kvList = pMsg->srpc.getString("kvList");

      if (!s) { if (sString != "abc") { s = String::from(__LINE__); } }
      if (!s) { if (sStringLF != "abc\ndef") { s = String::from(__LINE__); } }
      if (!s) { if (sStringUnicode != "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a") { s = String::from(__LINE__); } }
      if (!s) { if (nInt != 42) { s = String::from(__LINE__); } }
      if (!s) { if (kvList != "a=111\nb=222\n") { s = String::from(__LINE__); } }
      AP_UNITTEST_RESULT(Test_Fluid_Basic_IO_Host2Display, !s, s);

      Test_Fluid_Basic_IO_Display2Host_Start();

    } else if (sMethod == "SetValues") {
      oTest_Fluid_Basic_Data.sString = pMsg->srpc.getString("sString");
      oTest_Fluid_Basic_Data.sStringLF = pMsg->srpc.getString("sStringLF");
      oTest_Fluid_Basic_Data.sStringUnicode = pMsg->srpc.getString("sStringUnicode");
      oTest_Fluid_Basic_Data.nInt = pMsg->srpc.getInt("nInt");

    } else if (sMethod == "GetValues") {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hTest_Fluid_Basic_Display;
      msg.srpc.setString("Method", "GetValuesResponse");
      msg.srpc.setString("sString", oTest_Fluid_Basic_Data.sString);
      msg.srpc.setString("sStringLF", oTest_Fluid_Basic_Data.sStringLF);
      msg.srpc.setString("sStringUnicode", oTest_Fluid_Basic_Data.sStringUnicode);
      msg.srpc.setInt("nInt", oTest_Fluid_Basic_Data.nInt);
      msg.Call();

    } else if (sMethod == "ExchangeResult") {
      String s = pMsg->srpc.getString("sResult");
      AP_UNITTEST_RESULT(Test_Fluid_Basic_IO_Display2Host, !s, s);

      Test_Fluid_Basic_AIRInterface_Start();

    } else if (sMethod == "GetTotalMemoryResponse") {
      String s = pMsg->srpc.getString("sResult");
      if (!s) {
        int nTotalMemory = pMsg->srpc.getInt("nTotalMemory");
        if (nTotalMemory <= 0) {
          s.appendf("nTotalMemory=%d", nTotalMemory);
        }
      }
      AP_UNITTEST_RESULT(Test_Fluid_Basic_AIRInterface, !s, s);

      Test_Fluid_Basic_ConfigTest_Start();

    } else if (sMethod == "ConfigTestResult") {
      String s;

      if (sTest_Fluid_Basic_ConfigTest_Value != pMsg->srpc.getString("sValue")) {
        s.appendf("Expected=%s got=%s", StringType(sTest_Fluid_Basic_ConfigTest_Value), StringType(pMsg->srpc.getString("sValue")));
      }

      AP_UNITTEST_RESULT(Test_Fluid_Basic_ConfigTest, !s, s);

      Test_Fluid_Basic_Finish();
    }
  }
}

static void Test_Fluid_Basic_Created(Msg_Fluid_Created* pMsg)
{
  if (pMsg->hDisplay == hTest_Fluid_Basic_Display) {
    bTest_Fluid_Basic_Called_Created = 1;
  }
}

static void Test_Fluid_Basic_Loaded(Msg_Fluid_Loaded* pMsg)
{
  if (pMsg->hDisplay == hTest_Fluid_Basic_Display) {
    bTest_Fluid_Basic_Called_Loaded = 1;

    Test_Fluid_Basic_IO_Host2Display_Start();
  }
}

static void Test_Fluid_Basic_BeforeUnload(Msg_Fluid_BeforeUnload* pMsg)
{
  if (pMsg->hDisplay == hTest_Fluid_Basic_Display) {
    bTest_Fluid_Basic_Called_BeforeUnload = 1;
  }
}

static void Test_Fluid_Basic_Unloaded(Msg_Fluid_Unloaded* pMsg)
{
  if (pMsg->hDisplay == hTest_Fluid_Basic_Display) {
    bTest_Fluid_Basic_Called_Unloaded = 1;
  }
}

static void Test_Fluid_Basic_Destroyed(Msg_Fluid_Destroyed* pMsg)
{
  if (pMsg->hDisplay == hTest_Fluid_Basic_Display) {
    bTest_Fluid_Basic_Called_Destroyed = 1;
    hTest_Fluid_Basic_Display = ApNoHandle;

    String s;
    if (!s) { if (!bTest_Fluid_Basic_Called_Created) { s = "!bTest_Fluid_Basic_Called_Created"; } }
    if (!s) { if (!bTest_Fluid_Basic_Called_Loaded) { s = "!bTest_Fluid_Basic_Called_Loaded"; } }
    //if (!s) { if (!bTest_Fluid_Basic_Called_BeforeUnload) { s = "!bTest_Fluid_Basic_Called_BeforeUnload"; } }
    if (!s) { if (!bTest_Fluid_Basic_Called_Unloaded) { s = "!bTest_Fluid_Basic_Called_Unloaded"; } }
    if (!s) { if (!bTest_Fluid_Basic_Called_Destroyed) { s = "!bTest_Fluid_Basic_Called_Destroyed"; } }
    if (!s) { if (!bTest_Fluid_Basic_Called_HostCall) { s = "!bTest_Fluid_Basic_Called_HostCall"; } }
    AP_UNITTEST_RESULT(Test_Fluid_Basic_Callbacks, !s, s);

    { Msg_Fluid_Created msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Created, 0); }
    { Msg_Fluid_Loaded msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Loaded, 0); }
    { Msg_Fluid_BeforeUnload msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_BeforeUnload, 0); }
    { Msg_Fluid_Unloaded msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Unloaded, 0); }
    { Msg_Fluid_Destroyed msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Destroyed, 0); }
    { Msg_Fluid_HostCall msg("Test_Fluid"); msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_HostCall_Test_Fluid, 0); }

    Test_Fluid_UnitTest_TokenEnd();
  }
}

static String Test_Fluid_Basic_Start()
{
  String s;

  { Msg_Fluid_Created msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Created, 0, ApCallbackPosNormal); }
  { Msg_Fluid_Loaded msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Loaded, 0, ApCallbackPosNormal); }
  { Msg_Fluid_BeforeUnload msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_BeforeUnload, 0, ApCallbackPosNormal); }
  { Msg_Fluid_Unloaded msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Unloaded, 0, ApCallbackPosNormal); }
  { Msg_Fluid_Destroyed msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_Destroyed, 0, ApCallbackPosNormal); }
  { Msg_Fluid_HostCall msg("Test_Fluid"); msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_Basic_HostCall_Test_Fluid, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }

  if (!s) {
    hTest_Fluid_Basic_Display = Apollo::newHandle();
    
    String sContentUrl;
    sContentUrl.appendf("file:%sfluid\\test.html", StringType(Apollo::getModuleResourcePath(MODULE_NAME)));
    sContentUrl = Apollo::getModuleConfig(MODULE_NAME, "Fluid/ContentURL", sContentUrl);

    Msg_Fluid_Create msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.sPathname = Apollo::getModuleConfig(MODULE_NAME, "Fluid/ExePath", "..\\..\\..\\fl\\ApolloLib\\SimpleHTML\\SimpleHTML.exe");
    msg.sArgs.appendf("-url \"%s\"", StringType(sContentUrl));
    msg.bVisible = Apollo::getModuleConfig(MODULE_NAME, "Fluid/InitiallyVisible", 1);
    if (!msg.Request()) {
      s = "Msg_Fluid_Create failed";
    }
  }

  return s;
}

static void Test_Fluid_Basic_Cleanup()
{
  if (ApIsHandle(hTest_Fluid_Basic_Display)) {
    Msg_Fluid_Destroy msg;
    msg.hDisplay = hTest_Fluid_Basic_Display;
    msg.Request();
  }
}

//----------------------------------------------------------

#endif // AP_TEST_Fluid

static void Test_Fluid_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Fluid_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Fluid_UnitTest_Token", "Starting Test/Fluid"));
  int bTokenEndNow = 1;

  bTokenEndNow = 0;
  AP_UNITTEST_EXECUTE(Test_Fluid_Basic_Start);

  if (bTokenEndNow) { Test_Fluid_UnitTest_TokenEnd(); }
}

void Test_Fluid_Register()
{
#if defined(AP_TEST_Fluid)
  if (Apollo::isLoadedModule("Fluid")) {
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_Start);
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_Callbacks);
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_IO_Host2Display);
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_IO_Display2Host);
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_AIRInterface);
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_ConfigTest);
    AP_UNITTEST_REGISTER(Test_Fluid_Basic_Finish);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Fluid_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Fluid
}

void Test_Fluid_Begin()
{
#if defined(AP_TEST_Fluid)
  if (Apollo::isLoadedModule("Fluid")) {
  }
#endif // AP_TEST_Fluid
}

void Test_Fluid_End()
{
#if defined(AP_TEST_Fluid)
  if (Apollo::isLoadedModule("Fluid")) {
    Test_Fluid_Basic_Cleanup();
  }
#endif // AP_TEST_Fluid
}
