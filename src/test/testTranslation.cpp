// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Translation)
#include "MsgTranslation.h"

String Test_Translation_Basic_1(const char* szLine, const char* szModule, const char* szContext, const char* szText, const char* szExpected)
{
  String s;

  String sExpected = szExpected;
  String sTranslated = Msg_Translation_Get::_(szModule, szContext, szText);
  if (sTranslated != sExpected) {
    s.appendf("%s: Got:%s expected:%s", _sz(szLine), _sz(sTranslated), _sz(sExpected));
  }

  return s;
}

String Test_Translation_Basic()
{
  String s;

  String sLanguage;
  { // Save language setting
    Msg_Translation_GetLanguage msg;
    if (msg.Request()) {
      sLanguage = msg.sLanguage;
    }
  }
  { Msg_Translation_Plane msg; msg.sPlane = "test"; msg.Request(); }

  if (!s) {
    Msg_Translation_Clear msg;
    if (!msg.Request()) {
      s = "Msg_Translation_Clear failed " + String::from(__LINE__);
    }
  }

  if (!s) {
    Msg_Translation_SetLanguage msg;
    msg.sLanguage = "de";
    if (!msg.Request()) {
      s = "Msg_Translation_SetLanguage de failed " + String::from(__LINE__);
    }
  }

  if (!s) {
    Msg_Translation_GetLanguage msg;
    if (!msg.Request()) {
      s = "Msg_Translation_GetLanguage de failed " + String::from(__LINE__);
    } else if (msg.sLanguage != "de") {
      s.appendf("Msg_Translation_GetLanguage got:%s expected:%s", _sz(msg.sLanguage), _sz("de"));
    }
  }

  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text1", "translated1"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text2withlf", "translated2\nwithlf\n"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context3", "text3", "context3translated3"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context4", "text4", "text4"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text5", "\xC3\xA4" "5" "\xC3\xB6"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text6", "translated6"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context7", "text7", "context7translated7"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context8", "text8", "\xC3\xA4" "context8translated8"); }

  if (!s) {
    Msg_Translation_SetLanguage msg;
    msg.sLanguage = "en";
    if (!msg.Request()) {
      s = "Msg_Translation_SetLanguage en failed " + String::from(__LINE__);
    }
  }

  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text1", "1"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text2withlf", "text2withlf"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context3", "text3", "text3"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context4", "text4", "text4"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text5", "text5"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text6", "text6"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context7", "text7", "text7"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context8", "text8", "text8"); }

  if (!s) {
    Msg_Translation_SetLanguage msg;
    msg.sLanguage = "de_DE";
    if (!msg.Request()) {
      s = "Msg_Translation_SetLanguage de_DE failed " + String::from(__LINE__);
    }
  }

  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text1", "translated1"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text2withlf", "translated2\nwithlf\n"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context3", "text3", "context3translated3"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context4", "text4", "text4"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text5", "\xC3\xA4" "5" "\xC3\xB6"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text6", "translated6"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context7", "text7", "context7translated7"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context8", "text8", "\xC3\xA4" "context8translated8"); }

  if (!s) {
    Msg_Translation_SetLanguage msg;
    msg.sLanguage = "zh";
    if (!msg.Request()) {
      s = "Msg_Translation_SetLanguage zh failed " + String::from(__LINE__);
    }
  }

  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text1", "\xE9\xA0\x81" "1"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text2withlf", "\xE9\xA0\x81" "\n" "\xE9\xA6\x96" "\n"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context3", "text3", "\xE9\xA0\x81" "3"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context4", "text4", "text4"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text5", "\xE9\xA0\x81"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "", "text6", "\xE9\xA0\x81" "\xE9\xA6\x96"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context7", "text7", "\xE9\xA0\x81" "\xE9\xA6\x96" "7"); }
  if (!s) { s = Test_Translation_Basic_1(String::from(__LINE__), "test", "context8", "text8", "8" "\xE9\xA0\x81" " " "\xE9\xA6\x96" "8"); }

  if (!s) {
    Msg_Translation_Clear msg;
    if (!msg.Request()) {
      s = "Msg_Translation_Clear failed " + String::from(__LINE__); 
    }
  }

  { // Restore language setting
    Msg_Translation_SetLanguage msg;
    msg.sLanguage = sLanguage;
    (void) msg.Request();
  }

  { Msg_Translation_Plane msg; msg.sPlane = "default"; msg.Request(); }

  return s;
}

#endif // AP_TEST_Translation

//----------------------------------------------------------

void Test_Translation_Register()
{
#if defined(AP_TEST_Translation)
  if (Apollo::isLoadedModule("Translation")) {
    AP_UNITTEST_REGISTER(Test_Translation_Basic);
  }
#endif // AP_TEST_Translation
}

void Test_Translation_Execute()
{
#if defined(AP_TEST_Translation)
  if (Apollo::isLoadedModule("Translation")) {
    AP_UNITTEST_EXECUTE(Test_Translation_Basic);
  }
#endif // AP_TEST_Translation
}
