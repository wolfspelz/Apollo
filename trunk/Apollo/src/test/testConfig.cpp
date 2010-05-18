// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Config)
#include "MsgConfig.h"

int Test_Config_SetGet()
{
  int ok = 1;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }

  Apollo::setConfig("Test_Config_SetGet/Int1", 42);
  int n1 = Apollo::getConfig("Test_Config_SetGet/Int1", 43);
  if (n1 != 42) ok = 0;
  int n2 = Apollo::getConfig("Test_Config_SetGet/Int2", 44);
  if (n2 != 44) ok = 0;

  Apollo::setConfig("Test_Config_SetGet/String1", "A");
  String s1 = Apollo::getConfig("Test_Config_SetGet/String1", "B");
  if (s1 != "A") ok = 0;
  String s2 = Apollo::getConfig("Test_Config_SetGet/String2", "C");
  if (s2 != "C") ok = 0;

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
  return ok;
}

int Test_Config_Load()
{
  int ok = 1;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getModuleResourcePath("Test");
    msg.sName = "testConfig";
    msg.Request();
  }

  { int n = Apollo::getConfig("Test_Config_Load/not-available", 42); if (n != 42) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/String", 43); if (n != 0) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/EmptyString", 44); if (n != 44) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/Int", 45); if (n != 5) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/Int0", 46); if (n != 0) ok = 0; }

  { String s = Apollo::getConfig("Test_Config_Load/not-available", "s42"); if (s != "s42") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/String", "s43"); if (s != "abc") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/EmptyString", "s44"); if (s != "") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/Int", "s45"); if (s != "5") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/Int0", "s46"); if (s != "0") ok = 0; }

  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List/A/a", vlKeys); if (vlKeys.length() != 0) { ok = 0; } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List/A", vlKeys); if (vlKeys.length() != 1) { ok = 0; } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List", vlKeys); if (vlKeys.length() != 3) { ok = 0; } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List/C/c", vlKeys); if (vlKeys.length() != 0) { ok = 0; } }

  {
    Apollo::ValueList vlKeys;
    Apollo::getConfigKeys("Test/List", vlKeys);
    Apollo::ValueElem* e = 0;
    e = vlKeys.nextElem(e); if (e->getString() != "A") { ok = 0; }
    e = vlKeys.nextElem(e); if (e->getString() != "B") { ok = 0; }
    e = vlKeys.nextElem(e); if (e->getString() != "C") { ok = 0; }
  }

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
  return ok;
}

int Test_Config_Delete()
{
  int ok = 1;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }

  Apollo::setConfig("Test_Config_Delete/root", 42);
  Apollo::setConfig("Test_Config_Delete/keep", 43);
  Apollo::setConfig("Test_Config_Delete/root/delete/this", 44);
  Apollo::setConfig("Test_Config_Delete/root/delete/me", 45);
  Apollo::setConfig("Test_Config_Delete/root/and_me", 46);

  Apollo::deleteConfig("Test_Config_Delete/root");

  {
    Apollo::KeyValueList kvKeys;
    Apollo::getConfigKeys("Test_Config_Delete", kvKeys);
    if (ok) { ok = (kvKeys.findString("keep") != NULL); }
    if (ok) { ok = (kvKeys.findString("root") == NULL); }
    if (ok) { ok = (kvKeys.length() == 1); }
  }

  {
    Apollo::KeyValueList kvKeys;
    Apollo::getConfigKeys("Test_Config_Delete/root", kvKeys);
    if (ok) { ok = (kvKeys.findString("delete") == NULL); }
    if (ok) { ok = (kvKeys.findString("and_me") == NULL); }
    if (ok) { ok = (kvKeys.length() == 0); }
  }

  {
    Apollo::KeyValueList kvKeys;
    Apollo::getConfigKeys("Test_Config_Delete/root/delete", kvKeys);
    if (ok) { ok = (kvKeys.findString("this") == NULL); }
    if (ok) { ok = (kvKeys.findString("me") == NULL); }
    if (ok) { ok = (kvKeys.length() == 0); } 
  }

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }

  return ok;
}

#endif // AP_TEST_Config

//----------------------------------------------------------

void Test_Config_Register()
{
#if defined(AP_TEST_Config)
  AP_UNITTEST_REGISTER(Test_Config_SetGet);
  AP_UNITTEST_REGISTER(Test_Config_Load);
  AP_UNITTEST_REGISTER(Test_Config_Delete);
#endif // AP_TEST_Config
}

void Test_Config_Execute()
{
#if defined(AP_TEST_Config)
  AP_UNITTEST_EXECUTE1(Test_Config_SetGet);
  AP_UNITTEST_EXECUTE1(Test_Config_Load);
  AP_UNITTEST_EXECUTE1(Test_Config_Delete);
#endif // AP_TEST_Config
}
