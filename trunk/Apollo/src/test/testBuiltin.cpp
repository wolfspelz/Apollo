
// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Builtin)
  #define AP_TEST_String
  #define AP_TEST_ApHandle
  #define AP_TEST_SrpcMessage
  #define AP_TEST_SrpcStreamParser
  #define AP_TEST_List
  #define AP_TEST_KeyValueList
  #define AP_TEST_Tree
  #define AP_TEST_Buffer
  #define AP_TEST_RegEx
  #define AP_TEST_Xml
  #define AP_TEST_Base64
  #define AP_TEST_Crypto
  #define AP_TEST_Random
  #define AP_TEST_Url
  #define AP_TEST_AutoPtr
#endif

#if defined(AP_TEST_String)

String Test_Check(String& s, size_t nBytes, size_t nChars, size_t nAllocated, const unsigned char* pBytes = 0)
{
  String err;

  if (!err) {
    if (s.bytes() != nBytes || s.chars() != nChars || (nAllocated != 0 && s.allocated() != nAllocated)) { err.appendf("%s: bytes()=%d != %d || chars()=%d != %d || allocated()=%d != %d", StringType(s), s.bytes(), nBytes, s.chars(), nChars, s.allocated(), nAllocated); }
  }
  
  if (!err && pBytes != 0) {
    String sDiff;
    const unsigned char* p = pBytes;
    const unsigned char* q = (unsigned char*) (const char*) s.c_str();
    for (; *p != '\0' && *q != '\0'; p++, q++) {
      if (*p != *q) {
        sDiff.appendf(" %02x/%02x", *p, *q);
      }
    }
    if (!sDiff.empty()) {
      err.appendf("bytes differ: %s", StringType(sDiff));
    }
  }

  return err;
}

String Test_Check(String& s, const char* sz)
{
  String err;

  if (!err) {
    if (s != sz) { err.appendf("%s != %s", StringType(s), sz); }
  }
  
  return err;
}

String Test_String_AsConst(const String& s)
{
  String err;

  if (!err) {
    if (s) { err = String::from(__LINE__); }
  }
  
  return err;
}

String Test_String_SimpleASCII()
{
  String err;

  if (!err) { String s; if (!s.empty()) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s(""); if (!s.empty()) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s = ""; if (!s.empty()) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s; s = ""; if (!s.empty()) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s; if (s != "") { err.appendf("%d: %s != \"\"", __LINE__, StringType(s)); } }

  if (!err) { String s; if (s) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s(""); if (s) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s = ""; if (s) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }
  if (!err) { String s; s = ""; if (s) { err.appendf("%d: %s ! empty()", __LINE__, StringType(s)); } }

  if (!err) { String s("xx"); if (!s) { err.appendf("%d: %s is empty()", __LINE__, StringType(s)); } }

  if (!err) { String t; String s = t.c_str(); if (s != "") { err.appendf("%d: %s != \"\"", __LINE__, StringType(s)); } }
  if (!err) { String t; String s = (const char*) t; if (s != "") { err.appendf("%d: %s != \"\"", __LINE__, StringType(s)); } }
  if (!err) { String s; err = Test_String_AsConst(s); }
  if (!err) { err = Test_String_AsConst(""); }

  if (!err) {
    String s("abc");
    s.append("de");
    if (s != "abcde") { err.appendf("5: %s != %s", StringType(s), "abcde"); }
  }
  if (!err) {
    String s("abc");
    s += "de";
    if (s != "abcde") { err.appendf("6: %s != %s", StringType(s), "abcde"); }
  }
  if (!err) {
    String s("abc");
    String s2("de");
    s += s2;
    if (s != "abcde") { err.appendf("7: %s != %s", StringType(s), "abcde"); }
  }
  if (!err) {
    String s("ab");
    String s2("de");
    s += "c" + s2;
    if (s != "abcde") { err.appendf("8: %s != %s", StringType(s), "abcde"); }
  }

  if (!err) {
    String s("abc");
    s.appendf("d%s", "e");
    if (s != "abcde") { err.appendf("10: %s != %s", StringType(s), "abcde"); }
  }
  if (!err) {
    String s("123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ");
    s.appendf("%s", "1");
    err = Test_Check(s, 101, 101, 0);
  }
  if (!err) {
    String s("123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ");
    s.appendf("%s", "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ");
    err = Test_Check(s, 200, 200, 0);
  }

  return err;
}

String Test_String_Allocate()
{
  String err;

  if (!err) { String s = "123456789 123456789 "; err = Test_Check(s, 20, 20, 64); }
  if (!err) { String s = "123456789 123456789 "; s += s; err = Test_Check(s, 40, 40, 64); }
  if (!err) { String s = "123456789 123456789 "; s += "123456789 123456789 123456789 123456789 123"; err = Test_Check(s, 63, 63, 64); }
  if (!err) { String s = "123456789 123456789 "; s += "123456789 123456789 123456789 123456789 1234"; err = Test_Check(s, 64, 64, 128); }
  if (!err) { String s = "123456789 123456789 123456789 12"; err = Test_Check(s, 32, 32, 64); }
  if (!err) { String s = "123456789 123456789 123456789 123456789 123456789 123456789 123"; err = Test_Check(s, 63, 63, 126); }
  if (!err) { String s = "123456789 123456789 123456789 123456789 123456789 123456789 123"; s += "4"; err = Test_Check(s, 64, 64, 126); }
  if (!err) { String s = "123456789 123456789 123456789 123456789 123456789 123456789 1234"; err = Test_Check(s, 64, 64, 128); }
  if (!err) { String s = "123456789 123456789 123456789 123456789 123456789 123456789 1234"; s += "5"; err = Test_Check(s, 65, 65, 128); }
  if (!err) { String s = "123456789 123456789 123456789 123456789 123456789 123456789 1234"; s += s; err = Test_Check(s, 128, 128, 256); }
  if (!err) { String s = "123456789 123456789 123456789 12"; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; s += s; err = Test_Check(s, 524288, 524288, 1048576); }

  return err;
}

String Test_String_SimpleUmlauts()
{
  String err;

  if (!err) {
    String s = "\xC3\xA4";
    err = Test_Check(s, 2, 1, 0);
  }
  if (!err) {
    String s = "\xC3\xA4";
    s += "\xC3\xB6";
    err = Test_Check(s, 4, 2, 0);
  }

  return err;
}

String Test_String_CopeWithLatin1()
{
  String err;

  if (!err) {
    String s = "\xC3\xA4";
    s += "\xFC"; // u-uml
    // We can not completely fix it
    // Just tested, that it does not crash with an illegal UTF-8 first byte
    //err = Test_Check(s, 4, 2, 0);
  }

  if (!err) {
    String s = "\xC3\xA4";
    s += "\xE4"; // a-uml
    s += "\xC3\xB6";
    // We can not completely fix it
    // Just tested, that it does not crash with an illegal UTF-8 first byte
    //err = Test_Check(s, 6, 3, 0);
  }

  return err;
}

String Test_String_SimpleChinese()
{
  String err;

  if (!err) {
    String s = "\xE9\xA6\x96" "\xE9\xA0\x81";
#if defined(WIN32) && defined(UNICODE)
    WCHAR* wz = s;
    if (wz[0] != 0x9996 || wz[1] != 0x9801) {
      err = "w_str() failed";
    }
#endif
    //::MessageBox(NULL, (PTSTR) s, _T("Test"), MB_OK);
    err = Test_Check(s, 6, 2, 0);
  }

  if (!err) {
    String s = "\xE9\xA6\x96" "\xE9\xA0\x81";
    s += "\xC3\xA4" "\xC3\xB6";
    s += "a";
    unsigned char pCheck[] = {0xE9, 0xA6, 0x96, 0xE9, 0xA0, 0x81, 0xC3, 0xA4, 0xC3, 0xB6, 0x61, 0x00};
    err = Test_Check(s, 11, 5, 0, pCheck);
    //::MessageBox(NULL, (PTSTR) s, _T("Test"), MB_OK);
  }

  // Assert, that = with WCHAR does not append as before
  if (!err) {
    String s = L"abc";
    s = L"def";
    unsigned char pCheck[] = "def";
    err = Test_Check(s, 3, 3, 0, pCheck);
    //::MessageBox(NULL, (PTSTR) s, _T("Test"), MB_OK);
  }

  return err;
}

String Test_String_operators()
{
  String err;

  if (!err) { String s1 = "b"; String s2 = "c"; if (s1 > s2) { err.appendf("%s > %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s1 = "bbbbbbbb"; String s2 = "bbbbbbbc"; if (s1 > s2) { err.appendf("%s > %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s1 = "\xE9\xA0\x81"; String s2 = "\xE9\xA6\x96"; if (s1 > s2) { err.appendf("%s > %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s1 = "\xC3\xA4" "\xC3\xA4" "\xC3\xA4" "\xE9\xA0\x81"; String s2 = "\xC3\xA4" "\xC3\xA4" "\xC3\xA4" "\xE9\xA6\x96"; if (s1 > s2) { err.appendf("%s > %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s1 = "\xC3\xA4" "\xC3\xA4" "\xC3\xA4" "\xE9\xA6\x96"; String s2 = "\xC3\xA4" "\xC3\xA4" "\xE9\xA0\x81" "\xC3\xA4"; if (s1 > s2) { err.appendf("%s > %s", StringType(s1), StringType(s2)); }; }

  if (!err) { String s2 = "b"; String s1 = "c"; if (s1 < s2) { err.appendf("%s < %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s2 = "bbbbbbbb"; String s1 = "bbbbbbbc"; if (s1 < s2) { err.appendf("%s < %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s2 = "\xE9\xA0\x81"; String s1 = "\xE9\xA6\x96"; if (s1 < s2) { err.appendf("%s < %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s2 = "\xC3\xA4" "\xC3\xA4" "\xC3\xA4" "\xE9\xA0\x81"; String s1 = "\xC3\xA4" "\xC3\xA4" "\xC3\xA4" "\xE9\xA6\x96"; if (s1 < s2) { err.appendf("%s < %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s2 = "\xC3\xA4" "\xC3\xA4" "\xC3\xA4" "\xE9\xA6\x96"; String s1 = "\xC3\xA4" "\xC3\xA4" "\xE9\xA0\x81" "\xC3\xA4"; if (s1 < s2) { err.appendf("%s < %s", StringType(s1), StringType(s2)); }; }

  if (!err) { String s1 = "b"; String s2 = "b"; if (s1 != s2) { err.appendf("%s != %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s1 = "b"; if (s1 != "b") { err.appendf("%s != %s", StringType(s1), StringType("b")); }; }
  if (!err) { String s1 = "b"; String s2 = "c"; if (s1 == s2) { err.appendf("%s == %s", StringType(s1), StringType(s2)); }; }
  if (!err) { String s1 = "b"; if (s1 == "c") { err.appendf("%s == %s", StringType(s1), StringType("c")); }; }

  return err;
}

String Test_String_substr()
{
  String err;

  if (!err) {
    String s = "abcdefghij";
    String sub = s.subString(0);
    err = Test_Check(sub, "abcdefghij");
  }
  if (!err) {
    String s = "abcdefghij";
    String sub = s.subString(1);
    err = Test_Check(sub, "bcdefghij");
  }
  if (!err) {
    String s = "abcdefghij";
    String sub = s.subString(1, 1);
    err = Test_Check(sub, "b");
  }
  if (!err) {
    String s = "abcdefghij";
    String sub = s.subString(9, 1);
    err = Test_Check(sub, "j");
  }
  if (!err) {
    String s = "abcdefghij";
    String sub = s.subString(9);
    err = Test_Check(sub, "j");
  }
  if (!err) {
    String s = "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a";
    String sub = s.subString(0);
    unsigned char pCheck[] = {0xE9, 0xA6, 0x96, 0xE9, 0xA0, 0x81, 0xC3, 0xA4, 0xC3, 0xB6, 0x61, 0x00};
    err = Test_Check(sub, 11, 5, 0, pCheck);
  }
  if (!err) {
    String s = "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a";
    String sub = s.subString(1);
    unsigned char pCheck[] = {0xE9, 0xA0, 0x81, 0xC3, 0xA4, 0xC3, 0xB6, 0x61, 0x00};
    err = Test_Check(sub, 8, 4, 0, pCheck);
  }
  if (!err) {
    String s = "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a";
    String sub = s.subString(1, 2);
    unsigned char pCheck[] = {0xE9, 0xA0, 0x81, 0xC3, 0xA4, 0x00};
    err = Test_Check(sub, 5, 2, 0, pCheck);
  }
  if (!err) {
    String s = "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a";
    String sub = s.subString(s.chars(), 2);
    err = Test_Check(sub, 0, 0, 0);
  }

  return err;
}


String Test_String_StartsWith_One(const char* szText, const char* szHaystack, const char* szNeedle, int bResult)
{
  String err;

  String s = szHaystack;
  if (bResult != s.startsWith(szNeedle)) {
    err.appendf("%s: %s %s %s", szText, StringType(s), bResult?"does not start with":"starts with", szNeedle);
  }

  return err;
}

String Test_String_StartsWith()
{
  String err;

  if (!err) { err = Test_String_StartsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" ,1); }
  if (!err) { err = Test_String_StartsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" "\xE9\xA0\x81", 1); }
  if (!err) { err = Test_String_StartsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4", 1); }
  if (!err) { err = Test_String_StartsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xB6", 0); }
  if (!err) { err = Test_String_StartsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "a", 0); }
  if (!err) { err = Test_String_StartsWith_One(String::from(__LINE__), "http://www.bluehands.de/", "http://", 1); }

  return err;
}

String Test_String_EndsWith_One(const char* szText, const char* szHaystack, const char* szNeedle, int bResult)
{
  String err;

  String s = szHaystack;
  if (bResult != s.endsWith(szNeedle)) {
    err.appendf("%s: %s %s %s", szText, StringType(s), bResult?"does not end with":"ends with", szNeedle);
  }

  return err;
}

String Test_String_EndsWith()
{
  String err;

  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xC3\xB6" "a" ,1); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6", "\xC3\xA4" "\xC3\xB6", 1); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "a" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "\xE9\xA6\x96", "\xC3\xA4" "\xC3\xB6", 0); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a" "\xE9\xA6\x96", "a" "\xE9\xA6\x96", 1); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6", 0); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "a" "\xC3\xB6", "a", 0); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "a" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6", "a", 0); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "a", 1); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "http://www.bluehands.de/", "http://", 0); }
  if (!err) { err = Test_String_EndsWith_One(String::from(__LINE__), "http://www.bluehands.de/", "/", 1); }

  return err;
}

String Test_String_Contains_One(const char* szText, const char* szHaystack, const char* szNeedle, int bResult)
{
  String err;

  String s = szHaystack;
  if (bResult != s.contains(szNeedle)) {
    err.appendf("%s: %s %s %s", szText, StringType(s), bResult?"does not contain":"contains", szNeedle);
  }

  return err;
}

String Test_String_Contains()
{
  String err;

  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" "\xE9\xA0\x81",1); }
  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA0\x81" "\xC3\xA4", 1); }
  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA0\x81" "\xC3\xB6", 0); }
  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "b", 0); }
  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xB6", 1); }
  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xC3\xB6" "a", 1); }
  if (!err) { err = Test_String_Contains_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "a", 1); }

  return err;
}

String Test_String_Char_One(const char* szText, const char* szChar, unsigned int nChar)
{
  String err;

  unsigned int n = String::UTF8_Char(szChar);
  if (n != nChar) {
    err.appendf("%s: %s=0x%x != 0x%x", szText, StringType(szChar), n, nChar);
  }

  return err;
}

String Test_String_Char()
{
  String err;

  if (!err) { err = Test_String_Char_One(String::from(__LINE__), "\xE9\xA6\x96", 0xE9A696); }
  if (!err) { err = Test_String_Char_One(String::from(__LINE__), "\xE9\xA0\x81", 0xE9A081); }
  if (!err) { err = Test_String_Char_One(String::from(__LINE__), "a", 'a'); }
  if (!err) { err = Test_String_Char_One(String::from(__LINE__), "\xC3\xA4", 0xC3A4); }
  if (!err) { err = Test_String_Char_One(String::from(__LINE__), "\xC3\xB6", 0xC3B6); }

  return err;
}

String Test_String_FindCharSingle_One(const char* szText, const char* szHaystack, String::UTF8Char nNeedle, int nPos)
{
  String err;

  String s = szHaystack;
  const char* pPos = s.findChar(nNeedle);
  if (nPos < 0) {
    if (pPos != 0) {
      err.appendf("%s: 0x%x found in %s", szText, nNeedle, StringType(s));
    }
  } else {
    if (nPos != String::UTF8_CharLen(s.c_str(), pPos - s.c_str())) {
      err.appendf("%s: 0x%x not found @%d in %s", szText, nNeedle, nPos, StringType(s));
    }
  }

  return err;
}

String Test_String_FindCharSingle()
{
  String err;

  if (!err) { err = Test_String_FindCharSingle_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", String::UTF8_Char("a"), 4); }
  if (!err) { err = Test_String_FindCharSingle_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", String::UTF8_Char("\xC3\xB6"), 3); }
  if (!err) { err = Test_String_FindCharSingle_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", String::UTF8_Char("\xE9\xA6\x96"), 0); }
  if (!err) { err = Test_String_FindCharSingle_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", String::UTF8_Char(" "), -1); }
  if (!err) { err = Test_String_FindCharSingle_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", String::UTF8_Char("Ã"), -1); }

  return err;
}

String Test_String_FindCharMultiple_One(const char* szText, const char* szHaystack, const char* szNeedle, int nPos)
{
  String err;

  String s = szHaystack;
  const char* pPos = s.findChar(szNeedle);
  if (nPos < 0) {
    if (pPos != 0) {
      err.appendf("%s: one of %s found in %s", szText, szNeedle, StringType(s));
    }
  } else {
    if (pPos != s.c_str() + String::UTF8_ByteLen(s.c_str(), nPos)) {
      err.appendf("%s: none of %s found @%d in %s", szText, szNeedle, nPos, StringType(s));
    }
  }
  
  return err;
}
  
String Test_String_FindCharMultiple()
{
  String err;

  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", ".", 3); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "ab", 4); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 2); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA0\x81" "\xC3\xA4", 1); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA0\x81", 1); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", 0); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "a" "\xC3\xB6" "\xC3\xA4" "\xE9\xA0\x81" "\xE9\xA6\x96", 0); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "\xC3\xA4" "a", 2); }
  if (!err) { err = Test_String_FindCharMultiple_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xC3\xB6" "a", "b", -1); }

  return err;
}

String Test_String_Token_One(const char* szText, const char* szHaystack, const char* szSep, int nPos, const char* szToken)
{
  String err;

  String sToken;
  String s = szHaystack;
  for (int i = 0; i < nPos; i++) {
    if (!s.nextToken(szSep, sToken)) {
      sToken = "";
    }
  }
  if (sToken != szToken) {
    err.appendf("%s: '%s' not at pos %d in '%s'", szText, szToken, nPos, StringType(s));
  }
  
  return err;
}

String Test_String_ReverseToken_One(const char* szText, const char* szHaystack, const char* szSep, int nPos, const char* szToken)
{
  String err;

  String sToken;
  String s = szHaystack;
  for (int i = 0; i < nPos; i++) {
    if (!s.reverseToken(szSep, sToken)) {
      sToken = "";
    }
  }
  if (sToken != szToken) {
    err.appendf("%s: '%s' not at pos %d in '%s'", szText, szToken, nPos, StringType(s));
  }
  
  return err;
}

String Test_String_Token()
{
  String err;

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a.b.c.d", ".", 1, "a"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a.b.c.d", ".", 2, "b"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a.b.c.d", ".", 3, "c"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a.b.c.d", ".", 4, "d"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a.b.c.d", ".", 5, ""); }

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "", ".", 1, ""); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), ".b.c.d", ".", 2, "b"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), ".b.c.d", ".", 3, "c"); }

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a..b...c.d", ".", 1, "a"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a..b...c.d", ".", 2, "b"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a..b...c.d", ".", 3, "c"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a..b...c.d", ".", 4, "d"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "a..b...c.d", ".", 5, ""); }

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "aXYZbXXXcXYYd", "XYZ", 1, "a"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "aXYZbXXXcXYYd", "XYZ", 2, "b"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "aXYZbXXXcXYYd", "XYZ", 3, "c"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "aXYZbXXXcXYYd", "XYZ", 4, "d"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "aXYZbXXXcXYYd", "XYZ", 5, ""); }

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "XYZbXXXcXYYd", "XYZ", 1, ""); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "XYZbXXXcXYYd", "XYZ", 2, "b"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "XYZbXXXcXYYd", "XYZ", 3, "c"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "XYZbXXXcXYYd", "XYZ", 4, "d"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "XYZbXXXcXYYd", "XYZ", 5, ""); }

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xE9\xA0\x81", 1, "\xE9\xA6\x96"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xE9\xA0\x81", 2, "\xC3\xA4.\xC3\xB6" "a"); }

  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 1, "\xE9\xA6\x96\xE9\xA0\x81"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 2, "."); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 3, "a"); }
  if (!err) { err = Test_String_Token_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 4, ""); }

  // reverse
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a.b.c.d", ".", 1, "d"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a.b.c.d", ".", 2, "c"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a.b.c.d", ".", 3, "b"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a.b.c.d", ".", 4, "a"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a.b.c.d", ".", 5, ""); }

  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "", ".", 1, ""); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "b.c.d.", ".", 2, "d"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "b.c.d.", ".", 3, "c"); }

  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a..b...c.d", ".", 1, "d"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a..b...c.d", ".", 2, "c"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a..b...c.d", ".", 3, "b"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a..b...c.d", ".", 4, "a"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "a..b...c.d", ".", 5, ""); }

  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "aXYZbXXXcXYYd", "XYZ", 1, "d"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "aXYZbXXXcXYYd", "XYZ", 2, "c"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "aXYZbXXXcXYYd", "XYZ", 3, "b"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "aXYZbXXXcXYYd", "XYZ", 4, "a"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "aXYZbXXXcXYYd", "XYZ", 5, ""); }

  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "bXXXcXYYdXYZ", "XYZ", 1, ""); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "bXXXcXYYdXYZ", "XYZ", 2, "d"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "bXXXcXYYdXYZ", "XYZ", 3, "c"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "bXXXcXYYdXYZ", "XYZ", 4, "b"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "bXXXcXYYdXYZ", "XYZ", 5, ""); }

  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xE9\xA0\x81", 1, "\xC3\xA4" "." "\xC3\xB6" "a"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xE9\xA0\x81", 2, "\xE9\xA6\x96"); }

  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 1, "a"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 2, "."); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 3, "\xE9\xA6\x96" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_ReverseToken_One("reverseToken", "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "." "\xC3\xB6" "a", "\xC3\xA4" "\xC3\xB6", 4, ""); }

  return err;
}

// ------------------------------------

String Test_String_ReplaceSingle_One(const char* szText, const char* szSubject, const char* szSearch
                                     , const char* szReplace, const char* szResult)
{
  String err;
  String sSubject = szSubject; sSubject.replace(szSearch, szReplace);
  if (sSubject != szResult) { err.appendf("%s: %s != %s", szText, StringType(sSubject), szResult); }
  return err;
}

String Test_String_ReplaceSingle()
{
  String err;

  if (!err) { err = Test_String_ReplaceSingle_One(String::from(__LINE__), "[_Replace_] equals replace", "[_Replace_]", "replace", "replace equals replace"); }
  if (!err) { err = Test_String_ReplaceSingle_One(String::from(__LINE__), "replace equals [_Replace_]", "[_Replace_]", "replace", "replace equals replace"); }
  if (!err) { err = Test_String_ReplaceSingle_One(String::from(__LINE__), "replace equals [_Replace_] ...", "[_Replace_]", "replace", "replace equals replace ..."); }
  if (!err) { err = Test_String_ReplaceSingle_One(String::from(__LINE__), "\xE9\xA6\x96" " [" "\xC3\xA4" "] ...", "[" "\xC3\xA4" "]", "_" "\xE9\xA0\x81" "/", "\xE9\xA6\x96 " "_" "\xE9\xA0\x81" "/" " ..."); }
  
  return err;
}

// ------------------------------------

String Test_String_ReplaceList_One(const char* szText, const char* szSubject, List& lSearchReplace, const char* szResult)
{
  String err;
  String sSubject = szSubject; sSubject.replace(lSearchReplace);
  if (sSubject != szResult) { err.appendf("%s: %s != %s", szText, StringType(sSubject), szResult); }
  return err;
}

String Test_String_ReplaceList()
{
  String err;

  if (!err) { 
    List lSR;
    lSR.AddLast("[_Replace_]", "replace");
    lSR.AddLast("[replace]", "replace");
    err = Test_String_ReplaceList_One(String::from(__LINE__), "[_Replace_] equals [replace]", lSR, "replace equals replace");
  }

  if (!err) {
    List lSR;
    lSR.AddLast("\xC3\xA4", "\xE9\xA6\x96");
    lSR.AddLast("[[" "\xC3\xB6" "]]", "_" "\xE9\xA0\x81" "/");
    err = Test_String_ReplaceList_One(String::from(__LINE__), "abc" "\xC3\xA4"     " " "[[" "\xC3\xB6" "]]"   " ...", lSR, "abc" "\xE9\xA6\x96" " " "_" "\xE9\xA0\x81" "/" " ...");
  }

  return err;
}

// ------------------------------------

String Test_String_Trim_One(const char* szText, const char* szString, const char* szChars, const char* szResult)
{
  String err;

  String s = szString;
  s.trim(szChars);
  if (s != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(s), szResult);
  }
  
  return err;
}

String Test_String_Trim()
{
  String err;

  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " a ", "\r\n\t ", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " a", "\r\n\t ", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "a", "\r\n\t ", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "a ", "\r\n\t ", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " abc ", "\r\n\t ", "abc"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " ab  c ", "\r\n\t ", "ab  c"); }

  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " \na \n", "\r\n\t ", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " \na \t", "\r\n\t ", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " \na\r\n\t ", "\r\n\t ", "a"); }

  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\xE9\xA6\x96", "\r\n\t ", "\xE9\xA6\x96"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\r\n\xE9\xA6\x96", "\r\n\t ", "\xE9\xA6\x96"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\r\n\xE9\xA6\x96" "\xE9\xA0\x81" "\r\n\t ", "\r\n\t ", "\xE9\xA6\x96" "\xE9\xA0\x81" ); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\r\n\xE9\xA6\x96" " " "\xE9\xA0\x81" "\r\n\t ", "\r\n\t ", "\xE9\xA6\x96" " " "\xE9\xA0\x81" ); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), " " "\xC3\xA4" " ", "\r\n\t ", "\xC3\xA4"); }

  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\xE9\xA6\x96" "a" "\xE9\xA6\x96", "\xE9\xA6\x96", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA0\x81" "a" "\xE9\xA0\x81" "\xE9\xA6\x96", "\xE9\xA6\x96" "\xE9\xA0\x81", "a"); }
  if (!err) { err = Test_String_Trim_One(String::from(__LINE__), "\xE9\xA6\x96" "\xC3\xA4" "\xE9\xA0\x81" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xC3\xA4", "\xE9\xA6\x96" "\xC3\xA4", "\xE9\xA0\x81" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xC3\xA4" "\xE9\xA0\x81"); }

  return err;
}

String Test_String_Reverse_One(const char* szText, const char* szString, const char* szResult)
{
  String err;

  String sReverse = String::reverse(szString);
  if (sReverse != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(sReverse), szResult);
  }
  
  return err;
}

String Test_String_Reverse()
{
  String err;

  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "a", "a"); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "abc", "cba"); }

  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "a" "\xC3\xA4", "\xC3\xA4" "a"); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "\xC3\xA4" "a", "a" "\xC3\xA4"); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "\xC3\xA4" "a" "\xE9\xA6\x96" "\xE9\xA0\x81", "\xE9\xA0\x81" "\xE9\xA6\x96" "a" "\xC3\xA4"); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "\xC3\xA4" "a" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "a" "\xC3\xA4" , "\xC3\xA4" "a" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "a" "\xC3\xA4"); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), " a", "a "); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "", ""); }
  if (!err) { err = Test_String_Reverse_One(String::from(__LINE__), "\xE9\xA6\x96", "\xE9\xA6\x96"); }

  return err;
}

String Test_String_Escape_One(const char* szText, const char* szString, String::Escape_Type nType, const char* szResult)
{
  String err;

  String s = szString;
  s.escape(nType);
  if (s != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(s), szResult);
  }
  
  return err;
}

String Test_String_Escape()
{
  String err;

  if (!err) { err = Test_String_Escape_One(String::from(__LINE__), "a-b-ab-ba-aab-c-ddd-cd-dc-dddc-ddddc", String::EscapeTest, "A-B-AB-BA-AAB-CCC-D-CCCd-DC1-DCCC-DDC1"); }
  if (!err) { err = Test_String_Escape_One(String::from(__LINE__), 
    "\xE9\xA6\x96" "-" "\xE9\xA0\x81" "-" "\xE9\xA6\x96" "\xE9\xA0\x81" "-" "\xE9\xA0\x81" "\xE9\xA6\x96" "-" "\xE9\xA6\x96" "\xE9\xA6\x96" "\xE9\xA0\x81" "-" "\xC3\xA4" "-" "\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "-" "\xC3\xA4" "\xC3\xB6" "-" "\xC3\xB6" "\xC3\xA4" "-" "\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "\xC3\xA4" "-" "\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "\xC3\xA4", 
    String::EscapeTest, 
    "A-B-AB-BA-AAB-CCC-D-CCC" "\xC3\xB6" "-DC1-DCCC-DDC1"
    );
  }
  if (!err) { err = Test_String_Escape_One(String::from(__LINE__), "abx" "y" "\xC3\xA4" "c", String::EscapeTest, "AB" "\xE9\xA6\x96" "\xE9\xA0\x81" "CCC"); }
  if (!err) { err = Test_String_Escape_One(String::from(__LINE__), "a<b>c&d'e\"f", String::EscapeXML, "a&lt;b&gt;c&amp;d&apos;e&quot;f"); }
  if (!err) { err = Test_String_Escape_One(String::from(__LINE__), "ab'c'", String::EscapeDoubleQuotes, "ab''c''"); }

  if (!err) { err = Test_String_Escape_One(String::from(__LINE__), "http://www.heise.de/?a=b&c=d e", String::EscapeURL, "http%3A%2F%2Fwww.heise.de%2F%3Fa%3Db%26c%3Dd+e"); }

  return err;
}

String Test_String_UnEscape_One(const char* szText, const char* szString, String::Escape_Type nType, const char* szResult)
{
  String err;

  String s = szString;
  s.unescape(nType);
  if (s != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(s), szResult);
  }
  
  return err;
}

String Test_String_UnEscape()
{
  String err;

  if (!err) { err = Test_String_UnEscape_One(String::from(__LINE__), "abc\\nde", String::EscapeCRLF, "abc\nde"); }

  return err;
}

String Test_String_FilenameExtension_One(const char* szText, const char* szString, const char* szResult)
{
  String err;

  String s = szString;
  String sExt = String::filenameExtension(s);
  if (sExt != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(sExt), szResult);
  }
  
  return err;
}

String Test_String_FilenameBasePath_One(const char* szText, const char* szString, const char* szResult)
{
  String err;

  String s = szString;
  String sExt = String::filenameBasePath(s);
  if (sExt != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(sExt), szResult);
  }
  
  return err;
}

String Test_String_FilenameFile_One(const char* szText, const char* szString, const char* szResult)
{
  String err;

  String s = szString;
  String sFile = String::filenameFile(s);
  if (sFile != szResult) {
    err.appendf("%s: got %s instead of %s", szText, StringType(sFile), szResult);
  }
  
  return err;
}

String Test_String_Filename()
{
  String err;

  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "abc.", ""); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "abc.xyz", ""); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "/abc.xyz", "/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "dir/abc.xyz", "dir/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3\\abc.xyz", 
#if defined(WIN32)
  "C:\\dir1\\dir2/dir3\\"
#else
  "C:\\dir1\\dir2/"
#endif
  ); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3/abc.xyz", "C:\\dir1\\dir2/dir3/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "http://www.bluehands.de/dir1/dir3/abc.gif", "http://www.bluehands.de/dir1/dir3/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", ""); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "dir/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "dir/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3\\" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", 
#if defined(WIN32)
  "C:\\dir1\\dir2/dir3\\"
#else
  "C:\\dir1\\dir2/"
#endif
  ); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "C:\\dir1\\dir2/dir3/"); }
  if (!err) { err = Test_String_FilenameBasePath_One (String::from(__LINE__), "http://www.bluehands.de/dir1/dir3/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "g" "i" "f", "http://www.bluehands.de/dir1/dir3/"); }

//----

  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "abc.", "abc."); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "abc.xyz", "abc.xyz"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "/abc.xyz", "abc.xyz"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "dir/abc.xyz", "abc.xyz"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3\\abc.xyz", "abc.xyz"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3/abc.xyz", "abc.xyz"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "http://www.bluehands.de/dir1/dir3/abc.gif", "abc.gif"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "dir/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3\\" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "C:\\dir1\\dir2/dir3/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameFile_One (String::from(__LINE__), "http://www.bluehands.de/dir1/dir3/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "g" "i" "f", "\xE9\xA6\x96" "b" "\xC3\xA4" "." "g" "i" "f"); }

//----

  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "abc", ""); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "abc.", ""); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "abc.x", "x"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "abc.xyz", "xyz"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "/abc.xyz", "xyz"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "dir/abc.xyz", "xyz"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "C:\\dir1\\dir2/dir3\\abc.xyz", "xyz"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "http://www.bluehands.de/dir1/dir3/abc.gif", "gif"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "\xE9\xA6\x96" "b" "\xC3\xA4", ""); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "\xE9\xA6\x96" "b" "\xC3\xA4" ".", ""); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6", "\xC3\xB6"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "dir/" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "C:\\dir1\\dir2/dir3\\" "\xE9\xA6\x96" "b" "\xC3\xA4" "." "\xC3\xB6" "y" "\xE9\xA0\x81", "\xC3\xB6" "y" "\xE9\xA0\x81"); }
  if (!err) { err = Test_String_FilenameExtension_One(String::from(__LINE__), "http://www.bluehands.de/dir1/dir3/" "\xE9\xA6\x96" "b" "\xC3\xA4" ".gif", "gif"); }

  return err;
}

String Test_String_Truncate_One(const char* szText, const char* szString, int nLen, const char* szResult)
{
  String err;

  String s = String::truncate(szString, nLen);
  if (s != szResult) {
    err.appendf("%s: got %s instead of %s", StringType(szText), StringType(szString), StringType(szResult));
  }
  
  return err;
}

String Test_String_Truncate()
{
  String err;

  if (!err) { err = Test_String_Truncate_One(String::from(__LINE__), "abcdefg", 6, "abc..."); }
  if (!err) { err = Test_String_Truncate_One(String::from(__LINE__), "abcd", 6, "abc..."); }
  if (!err) { err = Test_String_Truncate_One(String::from(__LINE__), "abcdefg", 6, "abc..."); }
  if (!err) { err = Test_String_Truncate_One(String::from(__LINE__), "ab" "\xC3\xA4" "\xC3\xB6" "efg", 6, "ab" "\xC3\xA4" "..."); }

  return err;
}

String Test_String_ToLower_One(const char* szText, const char* szString, const char* szResult)
{
  String err;

  String s = String::toLower(szString);
  if (s != szResult) {
    err.appendf("%s: got %s instead of %s", StringType(szText), StringType(s), StringType(szResult));
  }
  
  return err;
}

String Test_String_ToLower()
{
  String err;

  if (!err) { err = Test_String_ToLower_One(String::from(__LINE__), "abcd", "abcd"); }
  if (!err) { err = Test_String_ToLower_One(String::from(__LINE__), "AbCd", "abcd"); }
  if (!err) { err = Test_String_ToLower_One(String::from(__LINE__), "aBcD", "abcd"); }
  if (!err) { err = Test_String_ToLower_One(String::from(__LINE__), "A" "\xC3\xA4" "C" "\xE9\xA6\x96", "a" "\xC3\xA4" "c" "\xE9\xA6\x96"); }

  return err;
}

String Test_String_isDigit_1(const char* szText, const char* szString, int bResult)
{
  String err;

  int is = String::isDigit(szString);
  if (is != bResult) {
    err.appendf("%s: isDigit returned %d (expected %d)", StringType(szText), is, bResult);
  }
  
  return err;
}

String Test_String_isPunct_1(const char* szText, const char* szString, int bResult)
{
  String err;

  int is = String::isPunct(szString);
  if (is != bResult) {
    err.appendf("%s: isPunct returned %d (expected %d)", StringType(szText), is, bResult);
  }
  
  return err;
}

String Test_String_ctype()
{
  String err;

  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "a", 0); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "0", 1); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "5", 1); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "", 0); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "abcd", 0); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "1bcd", 1); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "\xC3\xA4", 0); }
  if (!err) { err = Test_String_isDigit_1(String::from(__LINE__), "\xE9\xA6\x96", 0); }

  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), "a", 0); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), ".", 1); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), "", 0); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), "abcd", 0); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), ".bcd", 1); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), ":bcd", 1); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), "\xC3\xA4", 0); }
  if (!err) { err = Test_String_isPunct_1(String::from(__LINE__), "\xE9\xA6\x96", 0); }
  
  return err;
}

#if defined(WIN32) && defined(UNICODE)
String Test_String_WinUnicode()
{
  String err;
  
#if defined(_MSC_VER)
  WCHAR* wzStr = L"\x9996" L"\x9801" L"\x00E4" L"\x00F6" L"a";
  unsigned char pCheck[] = {0xE9, 0xA6, 0x96, 0xE9, 0xA0, 0x81, 0xC3, 0xA4, 0xC3, 0xB6, 0x61, 0x00};

  if (!err) {
    String s(wzStr);
    err = Test_Check(s, 11, 5, 0, pCheck);
  }
  if (!err) {
    String s;
    s.append(wzStr, 5);
    err = Test_Check(s, 11, 5, 0, pCheck);
  }
  if (!err) {
    String s;
    s.set(wzStr, 5);
    err = Test_Check(s, 11, 5, 0, pCheck);
  }
  if (!err) {
    String s = wzStr;
    err = Test_Check(s, 11, 5, 0, pCheck);
  }
#endif

  return err;
}
#endif

String Test_String_operator_equals_returns_ref()
{
  String err;

  {
    String s;
    if ( (s = "1") == "1") {
      // ok
    } else {
      err.appendf("3: result of operator= is <%s> should be <1>", StringType(s));
    }
  }

  {
    String s = "1";
    if ( (s = "2") == "2") {
      // ok
    } else {
      err.appendf("3: result of operator= is <%s> should be <2>", StringType(s));
    }
  }

  {
    String s;
    String s2 = "1";
    if ( (s = s2) == "1") {
      // ok
    } else {
      err.appendf("3: result of operator= is <%s> should be <1>", StringType(s));
    }
  }

  return err;
}


#endif // AP_TEST_String

//----------------------------------------------------------
// ApHandle

#if defined(AP_TEST_ApHandle)

String Test_ApHandle()
{
  String s;

  // Basic
  if (!s) {
    ApHandle h; h.fromString("[123]");
    if (h.nLo_ == 123 && h.nHi_ == 0) {
      // ok
    } else {
      s = "expected lo=123 and hi=0";
    }
  }

  // ApNoHandle
  if (!s) {
    ApHandle h = ApNoHandle;
    if (h.nHi_ != 0 || h.nLo_ != 0) {
      s = "not ApNoHandle";
    }
  }
  if (!s) {
    ApHandle h = ApNoHandle;
    if (!ApIsHandle(h)) {
      //ok
    } else {
      s = "expected h == ApNoHandle";
    }
  }

  // Wrap
  if (!s) {
    ApHandle h; h.fromString("[999999999]");
    ++h;
    if (h.nHi_ != 1 || h.nLo_ != 0) {
      s = "wrap failed (1)";
    }
  }
  if (!s) {
    ApHandle h; h.fromString("[123456789999999997]");
    ++h; ++h; ++h; ++h; 
    if (h.nHi_ != 123456790 || h.nLo_ != 1) {
      s = "wrap failed (2)";
    }
  }

  // LongLong and toString
  if (!s) {
    ApHandle h1;
    h1.nLo_ = 123;
    h1.nHi_ = 3;
    String sString = h1.toString();
    #define Test_ApHandle_LongLongString_1 "[3000000123]"
    if (!s) {
      if (sString != Test_ApHandle_LongLongString_1) {
        s = "expected " Test_ApHandle_LongLongString_1;
      }
    }
    if (!s) {
      ApHandle h2; h2.fromString(sString);
      if (h1 != h2) {
        s = "h1 != h2 (1)";
      }
    }
  }
  if (!s) {
    ApHandle h1;
    h1.nLo_ = 123;
    h1.nHi_ = 456;
    String sString = h1.toString();
    #define Test_ApHandle_LongLongString_2 "[456000000123]"
    if (!s) {
      if (sString != Test_ApHandle_LongLongString_2) {
        s = "expected " Test_ApHandle_LongLongString_2;
      }
    }
    if (!s) {
      ApHandle h2; h2.fromString(sString);
      if (h1 != h2) {
        s = "h1 != h2 (2)";
      }
    }
  }
  if (!s) {
    ApHandle h1;
    h1.nLo_ = 123;
    h1.nHi_ = 0;
    String sString = h1.toString();
    #define Test_ApHandle_LongLongString_3 "[123]"
    if (!s) {
      if (sString != Test_ApHandle_LongLongString_3) {
        s = "expected " Test_ApHandle_LongLongString_3;
      }
    }
    if (!s) {
      ApHandle h2; h2.fromString(sString);
      if (h1 != h2) {
        s = "h1 != h2 (3)";
      }
    }
  }

  // ==
  if (!s) {
    ApHandle h1; h1.nLo_ = 123; h1.nHi_ = 3;
    ApHandle h2; h2.nLo_ = 123; h2.nHi_ = 4;
    if (h1 == h2) {
      s = "h1 == h2";
    }
  }
  if (!s) {
    ApHandle h1; h1.nLo_ = 123; h1.nHi_ = 3;
    ApHandle h2; h2.nLo_ = 124; h2.nHi_ = 3;
    if (h1 == h2) {
      s = "h1 == h2";
    }
  }

  // ApHandleFormat/ApHandleType
  if (!s) {
    String sString;
    ApHandle h = Apollo::newHandle();
    sString.appendf("xx," ApHandleFormat ",yy", ApHandleType(h));

    String sToken;
    if (!s) { sString.nextToken(",", sToken); if (sToken != "xx") { s = "expected=xx"; }}
    if (!s) {
      sString.nextToken(",", sToken); 
      ApHandle hDecoded = Apollo::string2Handle(sToken);
      if (hDecoded != h) { 
        s = "expected handle"; 
      }
    }
    if (!s) { sString.nextToken(",", sToken); if (sToken != "yy") { s = "expected=yy"; }}
  }

  return s;
}

#endif // AP_TEST_ApHandle

//----------------------------------------------------------
// SrpcMessage

#if defined(AP_TEST_SrpcMessage)

String Test_SrpcMessage()
{
  String s;

  if (!s) {
    Apollo::SrpcMessage m;
    Apollo::ValueList vl; vl.add(111); vl.add(222);
    m.setList("vlList", vl);
    List lExpected; lExpected.AddLast("vlList", "111 222");
    s = Apollo::Test_CompareLists("Test_SrpcMessage int", m, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    Apollo::KeyValueList kv; kv.add("a", 111); kv.add("b", 222);
    m.setList("kvList", kv);
    List lExpected; lExpected.AddLast("kvList", "a=111\\nb=222\\n");
    s = Apollo::Test_CompareLists("Test_SrpcMessage Key=int", m, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    Apollo::ValueList vl; vl.add("111"); vl.add("222");
    m.setList("vlList", vl);
    List lExpected; lExpected.AddLast("vlList", "111 222");
    s = Apollo::Test_CompareLists("Test_SrpcMessage String", m, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    Apollo::KeyValueList kv; kv.add("a", "111"); kv.add("b", "222"); kv.add("c", "33 3");
    m.setList("kvList", kv);
    List lExpected; lExpected.AddLast("kvList", "a=111\\nb=222\\nc=33 3\\n");
    s = Apollo::Test_CompareLists("Test_SrpcMessage Key=String", m, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    Apollo::ValueList vl; vl.add(ApHandle(0, 111)); vl.add(ApHandle(0, 222));
    m.setList("vlList", vl);
    List lExpected; lExpected.AddLast("vlList", "[111] [222]");
    s = Apollo::Test_CompareLists("Test_SrpcMessage ApHandle", m, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    Apollo::KeyValueList kv; kv.add("a", ApHandle(0, 111)); kv.add("b", ApHandle(0, 222));
    m.setList("kvList", kv);
    List lExpected; lExpected.AddLast("kvList", "a=[111]\\nb=[222]\\n");
    s = Apollo::Test_CompareLists("Test_SrpcMessage Key=ApHandle", m, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    m.setString("vlList", "a b c");
    Apollo::ValueList vl;
    m.getValueList("vlList", vl);
    List lExpected; lExpected.AddLast("", "a"); lExpected.AddLast("", "b"); lExpected.AddLast("", "c");
    List lResult; vl.toList(lResult);
    s = Apollo::Test_CompareLists("Test_SrpcMessage getList strings", lResult, lExpected);
  }

  if (!s) {
    Apollo::SrpcMessage m;
    m.setString("vlList", "11 22 33");
    Apollo::ValueList vl;
    m.getValueList("vlList", vl);
    Apollo::ValueElem* e = 0;
    if (!s) { e = vl.nextElem(e); if (e == 0) { s = "missing 11"; } else { if (e->getInt() != 11) { s = "expected 11"; } } }
    if (!s) { e = vl.nextElem(e); if (e == 0) { s = "missing 22"; } else { if (e->getInt() != 22) { s = "expected 22"; } } }
    if (!s) { e = vl.nextElem(e); if (e == 0) { s = "missing 33"; } else { if (e->getInt() != 33) { s = "expected 33"; } } }
  }

  if (!s) {
    Apollo::SrpcMessage m;
    m.setString("vlList", "[11] [22] [33]");
    Apollo::ValueList vl;
    m.getValueList("vlList", vl);
    Apollo::ValueElem* e = 0;
    if (!s) { e = vl.nextElem(e); if (e == 0) { s = "missing [11]"; } else { if (e->getHandle() != ApHandle(0, 11)) { s = "expected [11]"; } } }
    if (!s) { e = vl.nextElem(e); if (e == 0) { s = "missing [22]"; } else { if (e->getHandle() != ApHandle(0, 22)) { s = "expected [22]"; } } }
    if (!s) { e = vl.nextElem(e); if (e == 0) { s = "missing [33]"; } else { if (e->getHandle() != ApHandle(0, 33)) { s = "expected [33]"; } } }
  }

  return s;
}

#endif // AP_TEST_SrpcMessage

//----------------------------------------------------------
// SrpcStreamParser

#if defined(AP_TEST_SrpcStreamParser)

#include "SrpcStreamParser.h"

String Test_SrpcStreamParser1(const char* szText, const char* szData, List& lExpected)
{
  String s;

  Apollo::SrpcStreamParser sp;

  sp.parse(szData);
  Apollo::SrpcMessage* pM = sp.getNextMessage();
  if (pM == 0) {
    s.appendf("%s: No message available", StringType(szText));
  } else {
    List lParams = *pM;
    delete pM;
    s = Apollo::Test_CompareLists(szText, lParams, lExpected);
  }

  return s;
}

String Test_SrpcStreamParser_CheckNextMessage(const char* szText, Apollo::SrpcStreamParser& parser, List& lExpected)
{
  String s;

  Apollo::SrpcMessage* pMessage = parser.getNextMessage();
  if (pMessage == 0) {
    s.appendf("%s: No message available", StringType(szText));
  } else {
    s = Apollo::Test_CompareLists(szText, *pMessage, lExpected);
    delete pMessage;
  }

  return s;
}

String Test_SrpcStreamParser()
{
  String s;

  if (!s) { List l; l.AddLast("Method", "Test"); l.AddLast("a", "b"); s = Test_SrpcStreamParser1("basic", "Method=Test\na=b\n\n", l); }
  if (!s) { List l; l.AddLast("a", "b"); s = Test_SrpcStreamParser1("just one", "a=b\n\n", l); }
  if (!s) { List l; l.AddLast("Method", "Test"); l.AddLast("a", ""); l.AddLast("c", "d"); s = Test_SrpcStreamParser1("no value 1", "Method=Test\na=\nc=d\n\n", l); }
  if (!s) { List l; l.AddLast("Method", "Test"); l.AddLast("a", ""); l.AddLast("c", "d"); s = Test_SrpcStreamParser1("no value 2", "Method=Test\na\nc=d\n\n", l); }
  if (!s) { List l; l.AddLast("Method", "Test"); l.AddLast("a", "b=c\\nc"); l.AddLast("d", "e"); s = Test_SrpcStreamParser1("basic", "Method=Test\na=b=c\\nc\nd=e\n\n", l); }

  // Several messages
  if (!s) { 
    Apollo::SrpcStreamParser sp;

    List l1; l1.AddLast("Method1", "Test1"); l1.AddLast("Key11", "Value11"); l1.AddLast("Key12", "Value12"); 
    List l2; l2.AddLast("Method2", "Test2"); l2.AddLast("Key21", "Value21"); l2.AddLast("Key22", "Value22"); 
    List l3; l3.AddLast("Method3", "Test3"); l3.AddLast("Key31", "Value31"); l3.AddLast("Key32", "Value32"); 

    sp.parse("Method1=Test1\nKey11=Value11\nKey12=Value12\n\n");
    sp.parse("Method2=Test2\nKey21=Value21\nKey22=Value22\n\n");
    sp.parse("Method3=Test3\nKey31=Value31\nKey32=Value32\n\n");
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("multi1", sp, l1); }
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("multi2", sp, l2); }
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("multi3", sp, l3); }
  }

  // Several messages with weird fragmenting
  if (!s) { 
    Apollo::SrpcStreamParser sp;

    List l1; l1.AddLast("Method1", "Test1"); l1.AddLast("Key11", "Value11"); l1.AddLast("Key12", "Value12"); 
    List l2; l2.AddLast("Method2", "Test2"); l2.AddLast("Key21", "Value21"); l2.AddLast("Key22", "Value22"); 
    List l3; l3.AddLast("Method3", "Test3"); l3.AddLast("Key31", "Value31"); l3.AddLast("Key32", "Value32"); 

    sp.parse("Method1=Test1\nKey11=Va");
    sp.parse("l");
    sp.parse("ue11");
    sp.parse("\n");
    sp.parse("Key12=Value12\n");
    sp.parse("\nMethod2=Test2\nKey21=");
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("fragmented1", sp, l1); }
    sp.parse("Value21\nKey22=Value22\n\nMethod3=Test3\nKey31=Value31\nKey32=Val");
    sp.parse("ue32\n\na=b\n");
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("fragmented2", sp, l2); }
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("fragmented3", sp, l3); }
  }

  // Several messages with weird fragmenting and CRLF endings
  if (!s) { 
    Apollo::SrpcStreamParser sp;

    List l1; l1.AddLast("Method1", "Test1"); l1.AddLast("Key11", "Value11"); l1.AddLast("Key12", "Value12"); 
    List l2; l2.AddLast("Method2", "Test2"); l2.AddLast("Key21", "Value21"); l2.AddLast("Key22", "Value22"); 
    List l3; l3.AddLast("Method3", "Test3"); l3.AddLast("Key31", "Value31"); l3.AddLast("Key32", "Value32"); 

    sp.parse("Method1=Test1\r\nKey11=Va");
    sp.parse("l");
    sp.parse("ue11");
    sp.parse("\r\n");
    sp.parse("Key12=Value12\r\n");
    sp.parse("\r\nMethod2=Test2\r\nKey21=");
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("fragmented1", sp, l1); }
    sp.parse("Value21\r\nKey22=Value22\r\n\r\nMethod3=Test3\r\nKey31=Value31\r\nKey32=Val");
    sp.parse("ue32\r\n\r\na=b\r\n");
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("fragmented2", sp, l2); }
    if (!s) { s = Test_SrpcStreamParser_CheckNextMessage("fragmented3", sp, l3); }
  }

  return s;
}
#endif // AP_TEST_SrpcStreamParser

//----------------------------------------------------------
// List

#if defined(AP_TEST_List)

String Test_List()
{
  String s;

  return s;
}

String Test_SafeList()
{
  String s;

  if (s.empty()) {
    SafeListT<Elem> l; Elem AP_UNUSED_VARIABLE* e1 = l.AddLast("1"); Elem AP_UNUSED_VARIABLE* e2 = l.AddLast("2"); Elem AP_UNUSED_VARIABLE* e3 = l.AddLast("3");
    SafeListIteratorT<Elem> i(l);
    Elem* eA = 0;
    Elem* eB = 0;
    Elem* eC = 0;

    if (s.empty()) {
      eA = l.First();
      eB = i.Next();
      eC = i.Current();
      if (! (eA == eB && eB == eC) ) { s = "1st elem failed"; }
    }

    if (s.empty()) {
      eA = l.Next(eA);
      eB = i.Next();
      eC = i.Current();
      if (! (eA == eB && eB == eC) ) { s = "2nd elem failed"; }
    }

    if (s.empty()) {
      eA = l.Next(eA);
      eB = i.Next();
      eC = i.Current();
      if (! (eA == eB && eB == eC) ) { s = "3rd elem failed"; }
    }

    if (s.empty()) {
      eA = l.Next(eA);
      eB = i.Next();
      eC = i.Current();
      if (! (eA == 0 && eB == 0 && eC == 0) ) { s = "end failed"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem* e1 = l.AddLast("1");
    {
      SafeListIteratorT<Elem> i(l);
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      l.Remove(e1); delete e1;
      Elem* eB = i.Next();
      if (! (eB == 0) ) { s = "safe remove 1/1 failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 1/1"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem* e1 = l.AddLast("1");; Elem* e2 = l.AddLast("2");
    {
      SafeListIteratorT<Elem> i(l);
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      l.Remove(e1); delete e1;
      Elem* eB = i.Next();
      if (! (eB == e2) ) { s = "safe remove 1/2 failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 1/2"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem AP_UNUSED_VARIABLE* e1 = l.AddLast("1");; Elem* e2 = l.AddLast("2");
    {
      SafeListIteratorT<Elem> i(l);
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      l.Remove(e2); delete e2;
      Elem* eB = i.Next();
      if (! (eB == 0) ) { s = "safe remove 2/2 failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 2/2"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem AP_UNUSED_VARIABLE* e1 = l.AddLast("1");; Elem* e2 = l.AddLast("2");
    {
      SafeListIteratorT<Elem> i(l);
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      Elem AP_UNUSED_VARIABLE* eB = i.Next();
      l.Remove(e2); delete e2;
      Elem* eC = i.Next();
      if (! (eC == 0) ) { s = "safe remove 2/2b failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 2/2b"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem* e1 = l.AddLast("1"); Elem* e2 = l.AddLast("2"); Elem* e3 = l.AddLast("3");
    {
      SafeListIteratorT<Elem> i(l);
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      l.Remove(e1); delete e1;
      Elem* eB = i.Next();
      Elem* eC = i.Next();
      Elem* eD = i.Next();
      if (! (eB == e2 && eC == e3 && eD == 0) ) { s = "safe remove 1/3 failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 1/3"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem AP_UNUSED_VARIABLE* e1 = l.AddLast("1"); Elem* e2 = l.AddLast("2"); Elem AP_UNUSED_VARIABLE* e3 = l.AddLast("3");
    {
      SafeListIteratorT<Elem> i(l);
      (void) i.Next();
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      Elem AP_UNUSED_VARIABLE* eB = i.Next();
      l.Remove(e2); delete e2;
      Elem* eC = i.Next();
      if (! (eC ==  0) ) { s = "safe remove 2/3 failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 2/3"; }
    }
  }

  if (s.empty()) {
    SafeListT<Elem> l; Elem AP_UNUSED_VARIABLE* e1 = l.AddLast("1"); Elem AP_UNUSED_VARIABLE* e2 = l.AddLast("2"); Elem* e3 = l.AddLast("3");
    {
      SafeListIteratorT<Elem> i(l);
      (void) i.Next();
      (void) i.Next();
      Elem AP_UNUSED_VARIABLE* eA = i.Next();
      Elem AP_UNUSED_VARIABLE* eB = i.Next();
      Elem AP_UNUSED_VARIABLE* eC = i.Next();
      l.Remove(e3); delete e3;
      Elem* eD = i.Next();
      if (! (eD == 0) ) { s = "safe remove 3/3 failed"; }
    }
    if (s.empty()) {
      SafeListIteratorT<Elem>* pI = l.SetIterator(0);
      if (pI != 0) { s = "iterator not removed form list 3/3"; }
    }
  }

  return s;
}

String Test_StringList()
{
  String s;

  if (!s) {
    Apollo::StringList l("a b c");
    if (! l.IsSet("a")) { s = "missing a"; }
    if (! l.IsSet("b")) { s = "missing b"; }
    if (! l.IsSet("c")) { s = "missing c"; }
  }

  if (!s) {
    Apollo::StringList l("a b=1 c=2");
    if (l["a"].getString() != "") { s = "missing a=0"; }
    if (l["b"].getString() != "1") { s = "missing b=1"; }
    if (l["c"].getString() != "2") { s = "missing c=2"; }
  }

  //if (!s) {
  //  Apollo::StringList l("a b=x\\ y c=2");
  //  if (l["a"] != "") { s = "missing a=0"; }
  //  if (l["b"] != " ") { s = "missing b=WSP"; }
  //  if (l["c"] != "2") { s = "missing c=2"; }
  //}

  return s;
}

String Test_KeyValueBlob2List1(const char* szText, const char* szData, const char* szFieldSep, const char* szLineSep, List& lExpected)
{
  String err;

  List lData;
  KeyValueBlob2List(szData, lData, szLineSep, szFieldSep, "");

  err = Apollo::Test_CompareLists(szText, lData, lExpected);

  return err;
}

String Test_KeyValueBlob2List()
{
  String err;

  // Basics
  if (!err) { List l; l.AddLast("abc", "def"); l.AddLast("ghi", "jkl"); l.AddLast("mno", "pqr stu"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "abc:def\nghi=jkl\r\nmno= pqr stu", ":= ", "\r\n", l); }
  if (!err) { List l; l.AddLast("a", "b"); l.AddLast("c", "d"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "a=b\nc=d", "=", "\n", l); }
  if (!err) { List l; l.AddLast("a", ""); l.AddLast("c", "d"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "a=\nc=d", "=", "\n", l); }
  if (!err) { List l; l.AddLast("a", ""); l.AddLast("c", "d"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "a\nc=d", "=", "\n", l); }
  if (!err) { List l; l.AddLast("c", "d"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "\nc=d", "=", "\n", l); }
  if (!err) { List l; l.AddLast("c", "d"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "c=d\n", "=", "\n", l); }

  //if (!err) { List l; l.AddLast("a", "\xC3\xB6"); l.AddLast("c", "d"); err = Test_KeyValueBlob2List1(String::from(__LINE__), "a=" "\xC3\xB6" "\xC3\xA4" "c=d", "=", "\xC3\xA4", l); }

  // "\xE9\xA6\x96" "\xE9\xA0\x81"

  return err;
}

#endif // AP_TEST_List

//----------------------------------------------------------
// KeyValueList

#if defined(AP_TEST_KeyValueList)

String Test_ValueElem()
{
  String s;

  if (!s) {
    Apollo::ValueElem e;
    e.setInt(111);
    if (!s) { if (e.getType() != Apollo::ValueElem::TypeInt) { s = String::from(__LINE__); } }
    if (!s) { if (e.getInt() != 111) { s = String::from(__LINE__); } }
    if (!s) { if (e.getString() != "111") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::ValueElem e;
    e.setString("111");
    if (!s) { if (e.getType() != Apollo::ValueElem::TypeString) { s = String::from(__LINE__); } }
    if (!s) { if (e.getString() != "111") { s = String::from(__LINE__); } }
    if (!s) { if (e.getInt() != 111) { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::ValueElem e;
    e.setHandle(ApHandle(0, 111));
    if (!s) { if (e.getType() != Apollo::ValueElem::TypeHandle) { s = String::from(__LINE__); } }
    if (!s) { if (e.getHandle() != ApHandle(0, 111)) { s = String::from(__LINE__); } }
    if (!s) { if (e.getString() != "[111]") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::ValueElem e;
    e.setString("[111]");
    if (!s) { if (e.getHandle() != ApHandle(0, 111)) { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::ValueElem e;
    e.setHandle(ApHandle(0, 111));
    if (!s) { if (e.getString() != "[111]") { s = String::from(__LINE__); } }
  }

  return s;
}


String Test_KeyValueList()
{
  String s;

  if (!s) {
    Apollo::ValueList vl; vl.add(111); vl.add(222);
    if (!s) { if (vl.toString() != "111 222") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::KeyValueList kv; kv.add("1", 111); kv.add("2", 222);
    if (!s) { if (kv.toString() != "1=111\n2=222\n") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::ValueList vl; vl.add("111"); vl.add("222");
    if (!s) { if (vl.toString() != "111 222") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::KeyValueList kv; kv.add("1", "111"); kv.add("2", "222");
    if (!s) { if (kv.toString() != "1=111\n2=222\n") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::ValueList vl; vl.add(ApHandle(0, 111)); vl.add(ApHandle(0, 222));
    if (!s) { if (vl.toString() != "[111] [222]") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::KeyValueList kv; kv.add("1", ApHandle(0, 111)); kv.add("2", ApHandle(0, 222));
    if (!s) { if (kv.toString() != "1=[111]\n2=[222]\n") { s = String::from(__LINE__); } }
  }

  if (!s) {
    Apollo::KeyValueList kv; kv.add("1", 111); kv.add("2", 222);
    Apollo::KeyValueList kv2; kv2.add("3", 333); kv2.add("4", 444);
    kv2 = kv;
    if (!s) { if (kv2.toString() != "1=111\n2=222\n") { s = String::from(__LINE__); } }
  }

  return s;
}


#endif // AP_TEST_KeyValueList

//----------------------------------------------------------
// Tree

#if defined(AP_TEST_Tree)

#include "STree.h"

class Test_Tree_Data
{
public:
  Test_Tree_Data(int n)
    :n_(n)
  {}
  ~Test_Tree_Data() {}

  int Get() { return n_; }

protected:
  int n_;
};

typedef StringTree<Test_Tree_Data> Test_Tree_Data_Map;

String Test_Tree()
{
  String s;

  // Add items
  if (s.empty()) {
    Test_Tree_Data_Map st;
    Test_Tree_Data d1(1); st.Set("1", d1);
    Test_Tree_Data d3(3); st.Set("3", d3);
    Test_Tree_Data d2(2); st.Set("2", d2);

    int cnt = 0;
    StringTreeNode<Test_Tree_Data>* node = 0;
    for (StringTreeIterator<Test_Tree_Data> iter(st); (node = iter.Next()) && s.empty(); ) {
      cnt++;
      int n = node->Value().Get();
      if (cnt != n) {
        s.appendf("%d != %d", cnt, n);
      }
    }
  }

  // Add pointer of item
  if (s.empty()) {
    List lData;
    lData.Add("1", 1);
    lData.Add("3", 3);
    lData.Add("2", 2);

    StringTree<Test_Tree_Data*> st;
    for (Elem* e = 0; (e = lData.Next(e)); ) {
      Test_Tree_Data* p = new Test_Tree_Data(e->getInt());
      if (p != 0) {
        e->setPtr(p);
        st.Set(e->getName(), p);
      }
    }

    if (s.empty()) {
      int cnt = 0;
      StringTreeNode<Test_Tree_Data*>* node = 0;
      for (StringTreeIterator<Test_Tree_Data*> iter(st); (node = iter.Next()) && s.empty(); ) {
        cnt++;
        Test_Tree_Data* p = node->Value();
        int n = p->Get();
        if (cnt != n) {
          s.appendf("%d != %d", cnt, n);
        }
      }
    }

    if (s.empty()) {
      for (Elem* e = 0; (e = lData.Next(e)); ) {
        String sKey(e->getName());
        Test_Tree_Data* pReal = (Test_Tree_Data*) e->getPtr();
        StringTreeNode<Test_Tree_Data*>* node = st.Find(sKey);
        if (node == 0) {
          s.appendf("Node not found: %s", StringType(sKey));
        } else {
          Test_Tree_Data* pStored = node->Value(); 
          if (pStored != pReal) {
            s.appendf("Node wrong value: %s", StringType(sKey));
          } else {
            st.Unset(sKey);
            delete pStored;
            pStored = 0;
          }
        }
      }
    }

  }

  return s;
}

#endif // AP_TEST_Tree

//----------------------------------------------------------
// Tree

#if defined(AP_TEST_Buffer)

String Test_Buffer()
{
  String s;

  const unsigned char pData[] = { 0x00, 0x08, 0x0b, 0x0f, 0x90, 0x98, 0x9b, 0x9f, 0xa0, 0xa8, 0xab, 0xaf, 0xf0, 0xf8, 0xfb, 0xff };

  if (!s) {
    Buffer b;
    b.SetData(pData, 16);
    String sBinhex;
    b.encodeBinhex(sBinhex);
    if (sBinhex != "00080b0f90989b9fa0a8abaff0f8fbff") { s = String::from(__LINE__); }
    Buffer b2;
    b2.decodeBinhex(sBinhex);
    for (int i = 0; i < 16; i++) {
      if (pData[i] != b2.Data()[i]) { s = String::from(__LINE__); break; }
    }
  }

  if (!s) {
    String sBinhex = "00080B0f90989b9fA0a8abAFF0f8FBFF";
    Buffer b2;
    b2.decodeBinhex(sBinhex);
    for (int i = 0; i < 16; i++) {
      if (pData[i] != b2.Data()[i]) { s = String::from(__LINE__); break; }
    }
  }

  return s;
}

#endif // AP_TEST_Buffer

//----------------------------------------------------------
// RegEx

#if defined(AP_TEST_RegEx)

#include "RegEx.h"

int Test_RegEx_One(const char* szExpression, const char* szInput, const char* szReplace, const char* szResult)
{
  int ok = 0;

  Apollo::RegEx re;
  if (re.Compile(szExpression)) {
    int nCount = re.Match(szInput);
    if (nCount > 0) {
      String sReplaced = re.Replace(szReplace);
      ok = (sReplaced == szResult);
    }
  }

  return ok;
}

int Test_Regex()
{
  int ok = 1;

  //if (ok) ok = Test_RegEx_One(_T("((\\x{100}a)b)"), _T("\\x{100}ab"), _T("x-\\1-\\2-y"), _T("x-\\x{100}ab-\\x{100}a-y"));

  if (ok) ok = Test_RegEx_One("ab(c(d)e)f", "xabcdefy", "x-\\1-\\2-y", "x-cde-d-y");
  if (ok) ok = Test_RegEx_One("ab(c(d)e)f", "xabcdefy", "x-\\1-\\z-y", "x-cde-\\z-y");
  if (ok) ok = Test_RegEx_One("ab(c(d)e)f", "xabcdefy", "x-\\1-\\\\-y", "x-cde-\\\\-y");
  if (ok) ok = Test_RegEx_One("ab(c(d)e)f", "\xC3\xA4" "abcdefy", "x-\\1-\\2-y", "x-cde-d-y");
  if (ok) ok = Test_RegEx_One("ab(" "\xC3\xA4" "(d)e)f", "xab" "\xC3\xA4" "defy", "x-\\1-\\2-y", "x-" "\xC3\xA4" "de-d-y");
  if (ok) ok = Test_RegEx_One("ab(c(d)e)f", "xabcdefy", "\xC3\xA4" "-\\1-\\2-y", "\xC3\xA4" "-cde-d-y");  
  if (ok) ok = Test_RegEx_One("\xC3\xA4" "b(c(d)e)f", "x" "\xC3\xA4" "bcdefy", "x-\\1-\\2-y", "x-cde-d-y");
  if (ok) ok = Test_RegEx_One("ab(" "\xC3\xA4" "(d)e)f", "xab" "\xC3\xA4" "defy", "x-\\1-\\2-y", "x-" "\xC3\xA4" "de-d-y");
  if (ok) ok = Test_RegEx_One("ab(" "\xC3\xA4" "(" "\xC3\xB6" ")e)f", "xab" "\xC3\xA4" "\xC3\xB6" "efy", "x-\\1-\\2-y", "x-" "\xC3\xA4" "\xC3\xB6" "e-" "\xC3\xB6" "-y");

  if (ok) ok = Test_RegEx_One("^http://(www([0-9]*)\\.)?([^/]+\\.de)($|/.*$)", "http://www6.gmx.de/page.html", "\\3", "gmx.de");
  if (ok) ok = Test_RegEx_One(".*", "abc", "\\0", "abc");
  if (ok) ok = Test_RegEx_One("abc", "abc", "\\0", "abc");
  if (ok) ok = Test_RegEx_One(".*", "http://www6.gmx.de/page.html", "\\0", "http://www6.gmx.de/page.html");
  if (ok) ok = Test_RegEx_One("(a)(b)(c)(d)(e)(f)(g)(h)(i)", "abcdefghi", "\\0\\1\\2\\3\\4\\5\\6\\7\\8\\9", "abcdefghiabcdefghi");
  if (ok) ok = Test_RegEx_One("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)", "abcdefghij", "\\0\\1\\2\\3\\4\\5\\6\\7\\8\\9\\10", "abcdefghijabcdefghia0");
  if (ok) ok = Test_RegEx_One("(((((((((.*)))))))))", "a", "\\0\\1\\2\\3\\4\\5\\6\\7\\8\\9", "aaaaaaaaaa");

  return ok;
}

#endif // AP_TEST_RegEx

//----------------------------------------------------------
// XMLProcessor

#if defined(AP_TEST_Xml)

#include "XMLProcessor.h"

int Test_XML_ASCII_Simple()
{
  int ok = 0;

  const char *text =
      "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
      "<a>\n"                                  // Unix end-of-line
      "  <b ba='ab' bc='cb' bd=\"db\">\r\n"    // Windows end-of-line
      "    <c/>\r"                             // Mac OS end-of-line
      "  </b>\n"
      "  <d>\n"
      "    <e/>\n"
      "  </d>\n"
      "</a>";

  Apollo::XMLProcessor xml;
  int bFinal = 1;
  ok = xml.XmlText(text, ::strlen(text), bFinal);
  if (!ok) {
    String err = xml.GetErrorString();
  } else {
    Apollo::XMLNode* a = 0; String an;
    Apollo::XMLNode* b = 0; String bn;
    Apollo::XMLNode* c = 0; String cn;
    Apollo::XMLNode* d = 0; String dn;
    Apollo::XMLNode* e = 0; String en;
    String ba, bc, bd;
    String acdata, bcdata;

    a = xml.Root();
    if (a) {
      an = a->getName();
      acdata = a->getCData();
      b = a->getChild("b");
      if (b) {
        bcdata = b->getCData();
        bn = b->getName();
        ba = b->getAttribute("ba").getValue();
        bc = b->getAttribute("bc").getValue();
        bd = b->getAttribute("bd").getValue();
        c = b->getChild("c");
        if (c) {
          cn = c->getName();
        }
      }
      d = a->nextChild(b, "d");
      if (d) {
        dn = d->getName();
        e = d->getChild("e");
        if (e) {
          en = e->getName();
        }
      }

      if (!
          ( a && b && c && d && e &&
          an == "a" && bn == "b" && cn == "c" && dn == "d" && en == "e" &&
          ba == "ab" && bc == "cb" && bd == "db" &&
          acdata == "\n  \n  \n" && bcdata == "\n    \n  " )
          ) {
        ok = 0;
      }
    }
  }

  return ok;
}

int Test_XML_ASCII_CData()
{
  int ok = 0;

  const char *text =
      "<a>\n"
      " 1\n"
      " 2 \n"
      " 3\n"
      "</a>";

  Apollo::XMLProcessor xml;
  int bFinal = 1;
  ok = xml.XmlText(text, ::strlen(text), bFinal);
  if (ok) {
    Apollo::XMLNode* a = 0;
    a = xml.Root();
    if (a) {
      String c = a->getCData();
      if (c == "\n 1\n 2 \n 3\n") {
        ok = 1;
      }
    }

  }

  return ok;
}

int Test_XML_Umlauts()
{
  int ok = 0;

  const char *text = "<a>" "\xC3\xA4" "</a>";

  Apollo::XMLProcessor xml;
  int bFinal = 1;
  ok = xml.XmlText(text, ::strlen(text), bFinal);
  if (ok) {
    Apollo::XMLNode* a = 0;
    a = xml.Root();
    if (a) {
      String c = a->getCData();
      if (c == "\xC3\xA4") {
        ok = 1;
      }
    }

  }

  return ok;
}

int Test_XML_Umlauts_Latin1()
{
  int ok = 0;

  const char *text = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
               "<a>ä</a>";

  Apollo::XMLProcessor xml;
  int bFinal = 1;
  ok = xml.XmlText(text, ::strlen(text), bFinal);
  if (ok) {
    Apollo::XMLNode* a = 0;
    a = xml.Root();
    if (a) {
      String c = a->getCData();
      if (c == "\xC3\xA4") {
        ok = 1;
      }
    }

  }

  return ok;
}

int Test_XML_UTF8_French()
{
  int ok = 0;

  const char *text =
      "<?xml version='1.0' encoding='utf-8'?>\n"
      "<doc>\xC3\xA9</doc>";
  Apollo::XMLProcessor xml;
  int bFinal = 1;
  ok = xml.XmlText(text, ::strlen(text), bFinal);
  if (ok) {
    Apollo::XMLNode* doc = 0;
    String doc_cdata;

    doc = xml.Root();
    if (doc) {
      doc_cdata = doc->getCData();
    }
    if (doc_cdata != "\xC3\xA9") {
      ok = 0;
    }
  }

  return ok;
}

int Test_XML_UTF8_Chinese()
{
#if defined(WIN32) && defined(_UNICODE)
  int ok = 0;

  const char *text =
      "<?xml version='1.0' encoding='utf-8'?>\n"
      "<\xE7\x88\xB1\xE6\xAF\x94\xE6\xAD\xBB\xE5\x86\xB7 \xE7\x88\xB1\xE6\xAF\x94='\xE6\xAD\xBB\xE5\x86\xB7'>"
      "\xE7\x88\xB1\xE6\xAF\x94\xE6\xAD\xBB\xE5\x86\xB7"
      "</\xE7\x88\xB1\xE6\xAF\x94\xE6\xAD\xBB\xE5\x86\xB7>";
  Apollo::XMLProcessor xml;
  int bFinal = 1;
  ok = xml.XmlText(text, ::strlen(text), bFinal);
  if (ok) {
    Apollo::XMLNode* root = 0;
    String cdata, name, key, value, value2;

    root = xml.Root();
    if (root) {
      name = root->getName();
      cdata = root->getCData();
      key = root->getAttributes().First()->getName();
      value = root->getAttributes().First()->getString();
      value2 = root->getAttribute("\xE7\x88\xB1\xE6\xAF\x94").getValue();
    }
    String all;
    all.appendf("name=%s\ncdata=%s\nkey=%s\nvalue=%s", StringType(name), StringType(cdata), StringType(key), StringType(value));
    //::MessageBox(NULL, all, _T(""), MB_OK);

    const unsigned short* pname = (const unsigned short*)name.w_str();
    const unsigned short* pcdata = (const unsigned short*)cdata.w_str();
    const unsigned short* pkey = (const unsigned short*)key.w_str();
    const unsigned short* pvalue = (const unsigned short*)value.w_str();
    const unsigned short* pvalue2 = (const unsigned short*)value2.w_str();

    if (!
      ( pname[0] == 0x7231 && pname[1] == 0x6BD4 && pname[2] == 0x6B7B && pname[3] == 0x51B7 &&
        pcdata[0] == 0x7231 && pcdata[1] == 0x6BD4 && pcdata[2] == 0x6B7B && pcdata[3] == 0x51B7 &&
        pkey[0] == 0x7231 && pkey[1] == 0x6BD4 &&
        pvalue[0] == 0x6B7B && pvalue[1] == 0x51B7 &&
        pvalue2[0] == 0x6B7B && pvalue2[1] == 0x51B7 )
       ) {
      ok = 0;
    }
  }

  return ok;
#else
  return 1;
#endif
}

#endif // AP_TEST_Xml

//----------------------------------------------------------
// base64

#if defined(AP_TEST_Base64)

#include "MessageDigest.h"

int Test_base64_base(const char* szData, const char* szCheck, const char* szSHA1)
{
  int ok = 1;

  String sData = szData;
  String sCheck = szCheck;
  String sSHA1 = szSHA1;
  
  Buffer sbEncoded;
  sbEncoded.SetData(sData.c_str(), sData.bytes());

  String sEncoded;
  sbEncoded.encodeBase64(sEncoded);

  if (!sCheck.empty()) {
    ok = (sEncoded == szCheck);
  }
  if (!sSHA1.empty()) {
    Apollo::MessageDigest sbHashed((unsigned char*) sEncoded.c_str(), sEncoded.bytes());
    String sHashed = sbHashed.getSHA1Hex();
    ok = (sHashed == sSHA1);
  }

  return ok;
}

int Test_base64()
{
  int ok = 1;

  if (ok) { ok = Test_base64_base("Hello World", "SGVsbG8gV29ybGQ=", ""); }
  if (ok) { ok = Test_base64_base("Hello Worldx", "SGVsbG8gV29ybGR4", ""); }
  if (ok) { ok = Test_base64_base("Hello Worldxx", "SGVsbG8gV29ybGR4eA==", ""); }
  if (ok) { ok = Test_base64_base("Hello W\xF6rld", "SGVsbG8gV/ZybGQ=", ""); }
  if (ok) { ok = Test_base64_base("Hello W" "\xC3\xB6" "rld", "SGVsbG8gV8O2cmxk", ""); }

  return ok;
}

int Test_base64_sha1()
{
  int ok = 1;

  if (ok) {
    #define Test_base64_SizePlusOne 12289
    Flexbuf<char, Test_base64_SizePlusOne> fbLarge(Test_base64_SizePlusOne);
    char* pLarge = fbLarge;
    const char* szBytes = "0123456789";
    for (int i = 0; i < Test_base64_SizePlusOne; i++) {
      pLarge[i] = 
        szBytes[i % 10];
    }
    pLarge[Test_base64_SizePlusOne - 1] = '\0';
    ok = Test_base64_base(pLarge, "", "4ec85361df8a5ac72142839f640dee521a781fc2");
  }

  return ok;
}
#endif // AP_TEST_Base64

//----------------------------------------------------------
// Crypto functions

#if defined(AP_TEST_Crypto)

#include "Crypto.h"

String Test_Crypto_Blowfish()
{
  String s;

  #define Test_Crypto_Blowfish_Text "Hello World"
  #define Test_Crypto_Blowfish_Key "Key"

  String sIn = Test_Crypto_Blowfish_Text;

  Apollo::Crypto data;
  data.SetData(sIn);

  Buffer sbCrypted;
  String sKey = Test_Crypto_Blowfish_Key;
  data.encryptBlowfish(sKey, sbCrypted);

  String sBase64Crypted;
  sbCrypted.encodeBase64(sBase64Crypted);

  Buffer sbDecodedBase64Crypted;
  sbDecodedBase64Crypted.decodeBase64(sBase64Crypted);

  Apollo::Crypto decryptedDecodedBase64Crypted;
  decryptedDecodedBase64Crypted.decryptBlowfish(sKey, sbDecodedBase64Crypted);
  
  String sOut;
  decryptedDecodedBase64Crypted.GetString(sOut);

  if (!s) { if (sIn != sOut) { s = String::from(__LINE__); }}
  if (!s) { if (sBase64Crypted != "mkLKKlQhxAroLq4=") { s = String::from(__LINE__); }}

  return s;
}

String Test_Crypto_NativeWithLoginCredentials()
{
  String s;
  int ok = 1;

  #define Test_Crypto_NativeWithLoginCredentials_Text "Hello World"

  String sIn = Test_Crypto_NativeWithLoginCredentials_Text;

  Apollo::Crypto data;
  data.SetData(sIn);

  Buffer sbCrypted;
  if (ok) { ok = data.encryptWithLoginCredentials(sbCrypted); }
  if (!ok) { s = String::from(__LINE__); }

  Apollo::Crypto sbDecrypted;
  if (ok) { ok = sbDecrypted.decryptWithLoginCredentials(sbCrypted); }
  if (!ok) { s = String::from(__LINE__); }
  
  String sOut;
  sbDecrypted.GetString(sOut);

  if (!s) { if (sIn != sOut) { s = String::from(__LINE__); }}

  return s;
}

#endif // AP_TEST_Crypto

//----------------------------------------------------------
// Random functions

#if defined(AP_TEST_Random)

String Test_Random()
{
  String s;

  if (!s) { String sRandomString = Apollo::getRandomString(97); if (sRandomString.chars() != 97) { s = String::from(__LINE__); } }
  if (!s) { String sRandomString = Apollo::getRandomString(12); if (sRandomString.chars() != 12) { s = String::from(__LINE__); } }
  if (!s) { String sRandomString = Apollo::getRandomString(13); if (sRandomString.chars() != 13) { s = String::from(__LINE__); } }
  if (!s) { String sRandomString = Apollo::getRandomString(1);  if (sRandomString.chars() != 1) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(100); if (nRandomInt < 0 || nRandomInt >= 100) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }
  if (!s) { int nRandomInt = Apollo::getRandom(3);   if (nRandomInt < 0 || nRandomInt >= 3) { s = String::from(__LINE__); } }

  if (!s) {
    String id1 = Apollo::getUniqueId();
    String id2 = Apollo::getUniqueId();
    if (id1 == id2) { s = String::from(__LINE__); }
  }

  return s;
}

#endif // AP_TEST_Random

//----------------------------------------------------------
// Url functions

#if defined(AP_TEST_Url)

#include "URL.h"

String Test_UriBuilder()
{
  String s;

  // basic + inherit
  if (!s) { 
    Apollo::UriBuilder uri;
    uri.setPath("/path/");
    uri.setFile("file");
    uri.setQueryParam("a", "b");
    uri.setQueryParam("c", " ");
    if (uri() != "/path/file?a=b&c=+") { s = String::from(__LINE__); }

    Apollo::UriBuilder derivedUri = uri;
    derivedUri.setPath("/");
    derivedUri.setQueryParam("c", "xx");
    if (derivedUri() != "/file?a=b&c=xx") { s = String::from(__LINE__); }
  }

  // auto / path
  if (!s) { 
    Apollo::UriBuilder uri;
    uri.setFile("file");
    if (uri() != "/file") { s = String::from(__LINE__); }
  }

  // set full query and a param
  if (!s) { 
    Apollo::UriBuilder uri;
    uri.setQuery("this is the query");
    uri.setQueryParam("an", "additional param");
    if (uri() != "/?this+is+the+query&an=additional+param") { s = String::from(__LINE__); }
  }

  return s;
}

String Test_UrlBuilder()
{
  String s;

  // basic
  if (!s) { 
    Apollo::UrlBuilder url;
    url.setHost("host.com");
    url.setPath("/path/");
    url.setFile("file");
    url.setQueryParam("a", "b");
    url.setQueryParam("c", " ");
    if (url() != "http://host.com/path/file?a=b&c=+") { s = String::from(__LINE__); }
  }

  // port 80 default
  if (!s) { 
    Apollo::UrlBuilder url;
    url.setHost("host.com");
    url.setPort(123);
    if (url() != "http://host.com:123/") { s = String::from(__LINE__); }

    url.setPort(80);
    if (url() != "http://host.com/") { s = String::from(__LINE__); }
  }

  // full base url overrides host, port, path, file
  if (!s) { 
    Apollo::UrlBuilder url = "http://host.com/path/file";
    url.setHost("host.com2");
    url.setPath("/path2/");
    url.setFile("file2");
    url.setQueryParam("a", "b");
    url.setQueryParam("c", " ");
    if (url() != "http://host.com/path/file?a=b&c=+") { s = String::from(__LINE__); }
  }

  // query
  {
    Apollo::UrlBuilder url;
    url.setHost("host.com");
    url.setPath("/path/");
    url.setFile("file");
    url.setQueryParam("a", "b");
    url.setQueryParam("c", " ");
    if (url() != "http://host.com/path/file?a=b&c=+") { s = String::from(__LINE__); }
  }

  return s;
}

#endif // AP_TEST_Url

//----------------------------------------------------------
// auto ptr

#if defined(AP_TEST_AutoPtr)

#include "SAutoPtr.h"

class SAutoPtrTest
{
public:
  SAutoPtrTest(int nValue)
    :nValue_(nValue)
  {}
  ~SAutoPtrTest();
  int nValue_;
};

int bSAutoPtrTest_DestructCalled = 0;
SAutoPtrTest::~SAutoPtrTest()
{
  bSAutoPtrTest_DestructCalled = 1;
}

String Test_AutoPtr()
{
  String s;

  if (s.empty()) {
    AutoPtr<SAutoPtrTest> p(new SAutoPtrTest(42));
    if (p.get()) {
      p->nValue_ = 123;
    }
    if (p->nValue_ != 123) {
      s = "Wrong value";
    }
  }
  if (s.empty()) {
    if (!bSAutoPtrTest_DestructCalled) {
      s = "Destructor not called";
    }
  }

  if (s.empty()) {
    SAutoPtrTest* q = new SAutoPtrTest(42);
    AutoPtr<SAutoPtrTest> p(q);
    if (s.empty()) {
      if (p == 0) {
        s = "p == 0";
      }
    }
    if (s.empty()) {
      if (p != q) {
        s = "Pointers differ";
      }
    }
  }

  return s;
}

#endif // AP_TEST_AutoPtr

//----------------------------------------------------------

void Test_Builtin_Register()
{
#if defined(AP_TEST_String)
  AP_UNITTEST_REGISTER(Test_String_SimpleASCII);
  AP_UNITTEST_REGISTER(Test_String_Allocate);
  AP_UNITTEST_REGISTER(Test_String_SimpleUmlauts);
  AP_UNITTEST_REGISTER(Test_String_CopeWithLatin1);
  AP_UNITTEST_REGISTER(Test_String_SimpleChinese);
  AP_UNITTEST_REGISTER(Test_String_operators);
  AP_UNITTEST_REGISTER(Test_String_substr);
  AP_UNITTEST_REGISTER(Test_String_StartsWith);
  AP_UNITTEST_REGISTER(Test_String_EndsWith);
  AP_UNITTEST_REGISTER(Test_String_Contains);
  AP_UNITTEST_REGISTER(Test_String_Char);
  AP_UNITTEST_REGISTER(Test_String_FindCharSingle);
  AP_UNITTEST_REGISTER(Test_String_FindCharMultiple);
  AP_UNITTEST_REGISTER(Test_String_Token);
  AP_UNITTEST_REGISTER(Test_String_ReplaceSingle);
  AP_UNITTEST_REGISTER(Test_String_ReplaceList);
  AP_UNITTEST_REGISTER(Test_String_Trim);
  AP_UNITTEST_REGISTER(Test_String_Reverse);
  AP_UNITTEST_REGISTER(Test_String_Escape);
  AP_UNITTEST_REGISTER(Test_String_UnEscape);
  AP_UNITTEST_REGISTER(Test_String_Filename);
  AP_UNITTEST_REGISTER(Test_String_Truncate);
  AP_UNITTEST_REGISTER(Test_String_ToLower);
  AP_UNITTEST_REGISTER(Test_String_ctype);
  AP_UNITTEST_REGISTER(Test_String_operator_equals_returns_ref);
  #if defined(WIN32) && defined(UNICODE)
    AP_UNITTEST_REGISTER(Test_String_WinUnicode);
  #endif
#endif

#if defined(AP_TEST_ApHandle)
  AP_UNITTEST_REGISTER(Test_ApHandle);
#endif

#if defined(AP_TEST_SrpcMessage)
  AP_UNITTEST_REGISTER(Test_SrpcMessage);
#endif

#if defined(AP_TEST_SrpcStreamParser)
    AP_UNITTEST_REGISTER(Test_SrpcStreamParser);
#endif

#if defined(AP_TEST_List)
  AP_UNITTEST_REGISTER(Test_List);
  AP_UNITTEST_REGISTER(Test_SafeList);
  AP_UNITTEST_REGISTER(Test_KeyValueBlob2List);
  AP_UNITTEST_REGISTER(Test_StringList);
#endif

#if defined(AP_TEST_Tree)
  AP_UNITTEST_REGISTER(Test_Tree);
#endif

#if defined(AP_TEST_Buffer)
  AP_UNITTEST_REGISTER(Test_Buffer);
#endif

#if defined(AP_TEST_KeyValueList)
  AP_UNITTEST_REGISTER(Test_ValueElem);
  AP_UNITTEST_REGISTER(Test_KeyValueList);
#endif

#if defined(AP_TEST_AutoPtr)
  AP_UNITTEST_REGISTER(Test_AutoPtr);
#endif

#if defined(AP_TEST_RegEx)
  AP_UNITTEST_REGISTER(Test_Regex);
#endif

#if defined(AP_TEST_Xml)
  AP_UNITTEST_REGISTER(Test_XML_ASCII_Simple);
  AP_UNITTEST_REGISTER(Test_XML_ASCII_CData);
  AP_UNITTEST_REGISTER(Test_XML_Umlauts);
  AP_UNITTEST_REGISTER(Test_XML_Umlauts_Latin1);
  AP_UNITTEST_REGISTER(Test_XML_UTF8_French);
  AP_UNITTEST_REGISTER(Test_XML_UTF8_Chinese);
#endif

#if defined(AP_TEST_Base64)
  AP_UNITTEST_REGISTER(Test_base64);
  AP_UNITTEST_REGISTER(Test_base64_sha1);
#endif // AP_TEST_Base64

#if defined(AP_TEST_Crypto)
  AP_UNITTEST_REGISTER(Test_Crypto_Blowfish);
  AP_UNITTEST_REGISTER(Test_Crypto_NativeWithLoginCredentials);
#endif // AP_TEST_Crypto

#if defined(AP_TEST_Random)
  AP_UNITTEST_REGISTER(Test_Random);
#endif // AP_TEST_Random

#if defined(AP_TEST_Url)
  AP_UNITTEST_REGISTER(Test_UriBuilder);
  AP_UNITTEST_REGISTER(Test_UrlBuilder);
#endif // AP_TEST_Url
}



void Test_Builtin_Execute()
{
#if defined(AP_TEST_String)
  AP_UNITTEST_EXECUTE(Test_String_SimpleASCII);
  AP_UNITTEST_EXECUTE(Test_String_Allocate);
  AP_UNITTEST_EXECUTE(Test_String_SimpleUmlauts);
  AP_UNITTEST_EXECUTE(Test_String_CopeWithLatin1);
  AP_UNITTEST_EXECUTE(Test_String_SimpleChinese);
  AP_UNITTEST_EXECUTE(Test_String_operators);
  AP_UNITTEST_EXECUTE(Test_String_substr);
  AP_UNITTEST_EXECUTE(Test_String_StartsWith);
  AP_UNITTEST_EXECUTE(Test_String_EndsWith);
  AP_UNITTEST_EXECUTE(Test_String_Contains);
  AP_UNITTEST_EXECUTE(Test_String_Char);
  AP_UNITTEST_EXECUTE(Test_String_FindCharSingle);
  AP_UNITTEST_EXECUTE(Test_String_FindCharMultiple);
  AP_UNITTEST_EXECUTE(Test_String_Token);
  AP_UNITTEST_EXECUTE(Test_String_ReplaceSingle);
  AP_UNITTEST_EXECUTE(Test_String_ReplaceList);
  AP_UNITTEST_EXECUTE(Test_String_Trim);
  AP_UNITTEST_EXECUTE(Test_String_Reverse);
  AP_UNITTEST_EXECUTE(Test_String_Escape);
  AP_UNITTEST_EXECUTE(Test_String_UnEscape);
  AP_UNITTEST_EXECUTE(Test_String_Filename);
  AP_UNITTEST_EXECUTE(Test_String_Truncate);
  AP_UNITTEST_EXECUTE(Test_String_ToLower);
  AP_UNITTEST_EXECUTE(Test_String_ctype);
  AP_UNITTEST_EXECUTE(Test_String_operator_equals_returns_ref);
  #if defined(WIN32) && defined(UNICODE)
    AP_UNITTEST_EXECUTE(Test_String_WinUnicode);
  #endif
#endif

#if defined(AP_TEST_ApHandle)
  AP_UNITTEST_EXECUTE(Test_ApHandle);
#endif

#if defined(AP_TEST_SrpcMessage)
  AP_UNITTEST_EXECUTE(Test_SrpcMessage);
#endif

#if defined(AP_TEST_SrpcStreamParser)
    AP_UNITTEST_EXECUTE(Test_SrpcStreamParser);
#endif

#if defined(AP_TEST_List)
  AP_UNITTEST_EXECUTE(Test_List);
  AP_UNITTEST_EXECUTE(Test_SafeList);
  AP_UNITTEST_EXECUTE(Test_KeyValueBlob2List);
  AP_UNITTEST_EXECUTE(Test_StringList);
#endif

#if defined(AP_TEST_Tree)
  AP_UNITTEST_EXECUTE(Test_Tree);
#endif

#if defined(AP_TEST_Buffer)
  AP_UNITTEST_EXECUTE(Test_Buffer);
#endif

#if defined(AP_TEST_KeyValueList)
  AP_UNITTEST_EXECUTE(Test_ValueElem);
  AP_UNITTEST_EXECUTE(Test_KeyValueList);
#endif

#if defined(AP_TEST_AutoPtr)
  AP_UNITTEST_EXECUTE(Test_AutoPtr);
#endif

#if defined(AP_TEST_RegEx)
  AP_UNITTEST_EXECUTE1(Test_Regex);
#endif

#if defined(AP_TEST_Xml)
  AP_UNITTEST_EXECUTE1(Test_XML_ASCII_Simple);
  AP_UNITTEST_EXECUTE1(Test_XML_ASCII_CData);
  AP_UNITTEST_EXECUTE1(Test_XML_Umlauts);
  AP_UNITTEST_EXECUTE1(Test_XML_Umlauts_Latin1);
  AP_UNITTEST_EXECUTE1(Test_XML_UTF8_French);
  AP_UNITTEST_EXECUTE1(Test_XML_UTF8_Chinese);
#endif

#if defined(AP_TEST_Base64)
  AP_UNITTEST_EXECUTE1(Test_base64);
  AP_UNITTEST_EXECUTE1(Test_base64_sha1);
#endif

#if defined(AP_TEST_Crypto)
  AP_UNITTEST_EXECUTE(Test_Crypto_Blowfish);
  AP_UNITTEST_EXECUTE(Test_Crypto_NativeWithLoginCredentials);
#endif

#if defined(AP_TEST_Random)
  AP_UNITTEST_EXECUTE(Test_Random);
#endif

#if defined(AP_TEST_Url)
  AP_UNITTEST_EXECUTE(Test_UriBuilder);
  AP_UNITTEST_EXECUTE(Test_UrlBuilder);
#endif
}
