// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "ApLog.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Vpi)
#include "MsgVpi.h"
#include "MsgNet.h"
#include "MsgConfig.h"

static int Test_Vpi_UnitTest_nCntAsyncJobs = 0;
static String Test_Vpi_UnitTest_sOriginalPlane;

static void Test_Vpi_UnitTest_TokenBegin()
{
  Test_Vpi_UnitTest_sOriginalPlane = Msg_Config_GetPlane::_();
  Msg_Config_SetPlane::_("test");
  Msg_Config_Clear::_();
  Apollo::setModuleConfig("Vpi", "LocalVpiFileName", "_vpi.xml");
  Apollo::setModuleConfig("Vpi", "GlobalRoot", "http://lms.virtual-presence.org/v7/root.xml");
  Apollo::setModuleConfig("Vpi", "DefaultTimeToLive", 3600);
  Apollo::setModuleConfig("Vpi", "MinTimeToLive", 20);
  Apollo::setModuleConfig("Vpi", "MaxVpiFileSize", 30000);
  Apollo::setModuleConfig("Vpi", "ErrorTimeToLive", 60);
  { Msg_Vpi_Clear msg; msg.Request(); }
}

static void Test_Vpi_UnitTest_TokenEnd()
{
  Msg_Config_SetPlane::_(Test_Vpi_UnitTest_sOriginalPlane);
  { Msg_Vpi_Clear msg; msg.Request(); }

  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Finished Test/Vpi"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Vpi_UnitTest_TokenInc()
{
  if (Test_Vpi_UnitTest_nCntAsyncJobs == 0) {
    Test_Vpi_UnitTest_TokenBegin();
  }
  Test_Vpi_UnitTest_nCntAsyncJobs++;
}

static void Test_Vpi_UnitTest_TokenDec()
{
  Test_Vpi_UnitTest_nCntAsyncJobs--;
  if (Test_Vpi_UnitTest_nCntAsyncJobs == 0) {
    Test_Vpi_UnitTest_TokenEnd();
  }
}

//----------------------------------------------------------

static String g_sTest_Vpi_WithExistingData_Vpi_DocumentURL = "http://www.virtual-presence.org/notes/VPTN-2.txt";
static String g_sTest_Vpi_WithExistingData_Vpi_VpiURL = "http://www.virtual-presence.org/notes/_vpi.xml";
static String g_sTest_Vpi_WithExistingData_Vpi_sVpiXml = 
    "<?xml version='1.0' encoding='UTF-8'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
    "    <service>xmpp:location.virtual-presence.org</service>"
    "    <name prefix='\\2' hash='SHA1'>\\5</name>"
    "  </location>"
    "</vpi>";
    ;
static String g_sTest_Vpi_WithExistingData_Vpi_ExpectedLocationUrl = "xmpp:www.341973964fa3faf9c1d6e8c9255bb3fd84fcb005@location.virtual-presence.org";

static ApHandle g_hTest_Vpi_WithExistingData_Vpi_LocationXmlResponse = ApNoHandle;
static int Test_Vpi_WithExistingData_Vpi_LocationXmlResponse(Msg_Vpi_LocationXmlResponse* pMsg)
{
  String s;

  if (pMsg->hRequest == g_hTest_Vpi_WithExistingData_Vpi_LocationXmlResponse) {

    if (pMsg->bSuccess) {
      Msg_Vpi_GetLocationUrl msg;
      msg.sLocationXml = pMsg->sLocationXml;
      if (msg.Request()) {
        if (msg.sLocationUrl == g_sTest_Vpi_WithExistingData_Vpi_ExpectedLocationUrl) {
          // ok
        } else {
          s.appendf("Got:%s, expected:%s, url:%s", _sz(msg.sLocationUrl), _sz(g_sTest_Vpi_WithExistingData_Vpi_ExpectedLocationUrl), _sz(pMsg->sDocumentUrl));
        }
      } else {
        s.appendf("Location url failed for url:%s", _sz(pMsg->sDocumentUrl));
      }
    } else {
      s.appendf("Location request failed for url:%s", _sz(pMsg->sDocumentUrl));
    }

    { Msg_Vpi_LocationXmlResponse msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_WithExistingData_Vpi_LocationXmlResponse, 0); }
  }

  if (s.empty()) {
    AP_UNITTEST_SUCCESS(Test_Vpi_WithExistingData_End);
  } else {
    AP_UNITTEST_FAILED(Test_Vpi_WithExistingData_End, s);
  }

  return 1;
}

static String Test_Vpi_WithExistingData_Start()
{
  String s;

  { Msg_Vpi_Clear msg; msg.Request(); }

  if (s.empty()) {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = g_sTest_Vpi_WithExistingData_Vpi_VpiURL;
    msg.sContentType = "text/xml";
    msg.sData = g_sTest_Vpi_WithExistingData_Vpi_sVpiXml;
    msg.Send();
  }
  
  { Msg_Vpi_LocationXmlResponse msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vpi_WithExistingData_Vpi_LocationXmlResponse, 0, ApCallbackPosNormal); }  

  if (s.empty()) {
    Msg_Vpi_LocationXmlRequest msg;
    msg.hRequest = g_hTest_Vpi_WithExistingData_Vpi_LocationXmlResponse = Apollo::newHandle();
    msg.sDocumentUrl = g_sTest_Vpi_WithExistingData_Vpi_DocumentURL;
    if (!msg.Request()) {
      s = "Failed to map URL";
    }
  }

  return s;
}

//----------------------------

static String Test_Vpi_Synchronous()
{
  String s;

  String sOriginalPlane = Msg_Config_GetPlane::_();
  Msg_Config_SetPlane::_("test");
  Msg_Config_Clear::_();

  Apollo::setModuleConfig("Vpi", "LocalVpiFileName", "_vpi.xml");
  Apollo::setModuleConfig("Vpi", "GlobalRoot", "http://lms.virtual-presence.org/v7/root.xml");
  Apollo::setModuleConfig("Vpi", "DefaultTimeToLive", 3600);
  Apollo::setModuleConfig("Vpi", "MinTimeToLive", 20);
  Apollo::setModuleConfig("Vpi", "MaxVpiFileSize", 30000);
  Apollo::setModuleConfig("Vpi", "ErrorTimeToLive", 60);

  { Msg_Vpi_Clear msg; msg.Request(); }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://www.virtual-presence.org/notes/_vpi.xml";
    msg.sContentType = "text/xml";
    msg.sData = "";
    msg.Send();
  }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://www.virtual-presence.org/_vpi.xml";
    msg.sContentType = "text/xml";
    msg.sData = "";
    msg.Send();
  }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://lms.virtual-presence.org/v7/root.xml";
    msg.sContentType = "text/xml";
    msg.sData = 
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <delegate match='^http://.*\\.(org)($|/.*$)'><uri>\\1.xml</uri></delegate>"
    "</vpi>"
      ;
    msg.Send();
  }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://lms.virtual-presence.org/v7/org.xml";
    msg.sContentType = "text/xml";
    msg.sData = 
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <delegate><uri>default.xml</uri></delegate>"
    "</vpi>"
      ;
    msg.Send();
  }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://lms.virtual-presence.org/v7/default.xml";
    msg.sContentType = "text/xml";
    msg.sData = 
    "<?xml version='1.0' encoding='UTF-8'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <location>"
    "    <service>xmpp:location.virtual-presence.org</service>"
    "    <name hash='true'>\\5</name>"
    "  </location>"
    "</vpi>"
      ;
    msg.Send();
  }

  if (s.empty()) {
    Msg_Vpi_GetLocationXml msg;
    msg.sDocumentUrl = "http://www.virtual-presence.org/notes/VPTN-2.txt";
    if (!msg.Request()) {
      s = "Msg_Vpi_GetLocationXml failed: " + msg.sComment;
    } else {
      if (msg.apStatus != ApMessage::Ok) {
        s = "Msg_Vpi_GetLocationXml success, but result !Ok";
      }
    }
  }

  if (s.empty()) {
    Msg_Vpi_GetTraversedFiles msg;
    if (!msg.Request()) {
      s = "Msg_Vpi_GetTraversedFiles failed";
    } else {
      Apollo::ValueList vlExpected;
      vlExpected.add("http://www.virtual-presence.org/notes/_vpi.xml");
      vlExpected.add("http://www.virtual-presence.org/_vpi.xml");
      vlExpected.add("http://lms.virtual-presence.org/v7/root.xml");
      vlExpected.add("http://lms.virtual-presence.org/v7/org.xml");
      vlExpected.add("http://lms.virtual-presence.org/v7/default.xml");
      s = Test_CompareLists("Test_Vpi_Synchronous", msg.vlFiles, vlExpected);
    }
  }

  Msg_Config_SetPlane::_(sOriginalPlane);

  return s;
}

static String Test_Vpi_SynchronousIncomplete()
{
  String s;

  String sOriginalPlane = Msg_Config_GetPlane::_();
  Msg_Config_SetPlane::_("test");
  Msg_Config_Clear::_();

  Apollo::setModuleConfig("Vpi", "LocalVpiFileName", "_vpi.xml");
  Apollo::setModuleConfig("Vpi", "GlobalRoot", "http://lms.virtual-presence.org/v7/root.xml");
  Apollo::setModuleConfig("Vpi", "DefaultTimeToLive", 3600);
  Apollo::setModuleConfig("Vpi", "MinTimeToLive", 20);
  Apollo::setModuleConfig("Vpi", "MaxVpiFileSize", 30000);
  Apollo::setModuleConfig("Vpi", "ErrorTimeToLive", 60);

  { Msg_Vpi_Clear msg; msg.Request(); }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://www.virtual-presence.org/notes/_vpi.xml";
    msg.sContentType = "text/xml";
    msg.sData = "";
    msg.Send();
  }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://www.virtual-presence.org/_vpi.xml";
    msg.sContentType = "text/xml";
    msg.sData = "";
    msg.Send();
  }

  {
    Msg_Vpi_ReceiveFile msg;
    msg.sUrl = "http://lms.virtual-presence.org/v7/root.xml";
    msg.sContentType = "text/xml";
    msg.sData = "";
    msg.Send();
  }

  if (s.empty()) {
    Msg_Vpi_GetLocationXml msg;
    msg.sDocumentUrl = "http://www.virtual-presence.org/notes/VPTN-2.txt";
    if (msg.Request()) {
      s = "Msg_Vpi_GetLocationXml succeeds unexpectedly";
    }
  }

  if (s.empty()) {
    Msg_Vpi_GetTraversedFiles msg;
    if (!msg.Request()) {
      s = "Msg_Vpi_GetTraversedFiles failed";
    } else {
      Apollo::ValueList vlExpected;
      vlExpected.add("http://www.virtual-presence.org/notes/_vpi.xml");
      vlExpected.add("http://www.virtual-presence.org/_vpi.xml");
      vlExpected.add("http://lms.virtual-presence.org/v7/root.xml");
      s = Test_CompareLists("Test_Vpi_SynchronousIncomplete", msg.vlFiles, vlExpected);
    }
  }

  Msg_Config_SetPlane::_(sOriginalPlane);

  return s;
}

//----------------------------

#if defined(AP_TEST_Net_HTTP)

static int g_bTest_Vpi_Online = 0;

static String g_sTest_Vpi_DetailedMap_Vpi_DocumentURL = "http://www.virtual-presence.org/notes/VPTN-2.txt";
static String g_sTest_Vpi_DetailedMap_Vpi_VpiURL = "http://www.virtual-presence.org/notes/_vpi.xml";
static String g_sTest_Vpi_DetailedMap_Vpi_ExpectedLocationUrl = "xmpp:341973964fa3faf9c1d6e8c9255bb3fd84fcb005@location.virtual-presence.org";

static int g_bTest_Vpi_DetailedMap_Vpi_ReceiveFile = 0;
static int Test_Vpi_DetailedMap_Vpi_ReceiveFile(Msg_Vpi_ReceiveFile* pMsg)
{
  if (pMsg->sUrl == g_sTest_Vpi_DetailedMap_Vpi_VpiURL) {
    g_bTest_Vpi_DetailedMap_Vpi_ReceiveFile = 1;
  }

  if (pMsg->sUrl == g_sTest_Vpi_DetailedMap_Vpi_VpiURL) {
    { Msg_Vpi_ReceiveFile msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_ReceiveFile, 0); }
  }

  return 1;
}

static int g_bTest_Vpi_DetailedMap_Vpi_RequestFile = 0;
static int Test_Vpi_DetailedMap_Vpi_RequestFile(Msg_Vpi_RequestFile* pMsg)
{
  if (pMsg->sUrl == g_sTest_Vpi_DetailedMap_Vpi_VpiURL) {
    g_bTest_Vpi_DetailedMap_Vpi_RequestFile = 1;
  }

  if (pMsg->sUrl == g_sTest_Vpi_DetailedMap_Vpi_VpiURL) {
    { Msg_Vpi_RequestFile msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_RequestFile, 0); }
  }

  return 1;
}

static ApHandle g_hTest_Vpi_DetailedMap = ApNoHandle;
static int g_bTest_Vpi_DetailedMap = 0;
static int Test_Vpi_DetailedMap_Vpi_LocationXmlResponse(Msg_Vpi_LocationXmlResponse* pMsg)
{
  String s;

  if (pMsg->hRequest == g_hTest_Vpi_DetailedMap) {
    g_bTest_Vpi_DetailedMap = 1;

    if (g_bTest_Vpi_DetailedMap && g_bTest_Vpi_DetailedMap_Vpi_RequestFile && g_bTest_Vpi_DetailedMap_Vpi_ReceiveFile) {
      if (pMsg->bSuccess) {
        Msg_Vpi_GetLocationUrl msg;
        msg.sLocationXml = pMsg->sLocationXml;
        if (msg.Request()) {
          if (msg.sLocationUrl == g_sTest_Vpi_DetailedMap_Vpi_ExpectedLocationUrl) {
            // ok
          } else {
            s.appendf("Got:%s, expected:%s", _sz(msg.sLocationUrl), _sz(g_sTest_Vpi_DetailedMap_Vpi_ExpectedLocationUrl));
          }
        } else {
          s.appendf("Location url failed for url:%s", _sz(pMsg->sDocumentUrl));
        }
      } else {
        s.appendf("Location request failed for url:%s", _sz(pMsg->sDocumentUrl));
      }
    } else {
      s.appendf("response=%d request=%d receive=%d", g_bTest_Vpi_DetailedMap, g_bTest_Vpi_DetailedMap_Vpi_RequestFile, g_bTest_Vpi_DetailedMap_Vpi_ReceiveFile);
    }

    { Msg_Vpi_LocationXmlResponse msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_LocationXmlResponse, 0); }
  }

  if (s.empty()) {
    Msg_Vpi_GetTraversedFiles msg;
    if (!msg.Request()) {
      s.appendf("Msg_Vpi_GetTraversedFiles failed");
    } else {

    }
  }

  if (s.empty()) {
    AP_UNITTEST_SUCCESS(Test_Vpi_DetailedMap_End);
  } else {
    AP_UNITTEST_FAILED(Test_Vpi_DetailedMap_End, s);
  }

  { Msg_Vpi_LocationXmlResponse msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_LocationXmlResponse, 0); }
  { Msg_Vpi_RequestFile msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_RequestFile, 0); }
  { Msg_Vpi_ReceiveFile msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_ReceiveFile, 0); }

  Test_Vpi_UnitTest_TokenDec();

  return 1;
}

static String Test_Vpi_DetailedMap_Start()
{
  String s;

  { Msg_Vpi_LocationXmlResponse msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_LocationXmlResponse, 0, ApCallbackPosNormal); }  
  { Msg_Vpi_RequestFile msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_RequestFile, 0, ApCallbackPosNormal); }  
  { Msg_Vpi_ReceiveFile msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vpi_DetailedMap_Vpi_ReceiveFile, 0, ApCallbackPosEarly); }  

  Test_Vpi_UnitTest_TokenInc();

  if (s.empty()) {
    Msg_Vpi_LocationXmlRequest msg;
    msg.sDocumentUrl = g_sTest_Vpi_DetailedMap_Vpi_DocumentURL;
    msg.hRequest = g_hTest_Vpi_DetailedMap = Apollo::newHandle();
    if (!msg.Request()) {
      s.appendf("Failed to map '%s'", _sz(g_sTest_Vpi_DetailedMap_Vpi_DocumentURL));
      Test_Vpi_UnitTest_TokenDec();
    }
  }

  return s;
}

//----------------------------

class Test_Vpi_Map_Data : public Elem
{
public:
  Test_Vpi_Map_Data(const ApHandle& hAp, const char* szDocumentURL, const char* szLocationUrl)
    :sDocumentUrl_(szDocumentURL)
    ,sLocationUrl_(szLocationUrl)
    ,bSuccess_(0)
    ,bMapped_(0)
    ,hAp_(hAp)
  {}
  inline ApHandle apHandle() { return hAp_; }
  String sDocumentUrl_;
  String sLocationUrl_;
  int bSuccess_;
  int bMapped_;
  ApHandle hAp_;
};

List lTest_Vpi_Map_DataList;
static unsigned int nTest_Vpi_Map_Mapped = 0;
static unsigned int nTest_Vpi_Map_Success = 0;

Test_Vpi_Map_Data* Test_Vpi_Map_Data_FindByHandle(const ApHandle& h)
{
  Test_Vpi_Map_Data* pResult = 0;
  for (Test_Vpi_Map_Data* e = 0; (e = (Test_Vpi_Map_Data*) lTest_Vpi_Map_DataList.Next(e)) != 0; ) {
    if (e->apHandle() == h) {
      pResult = e;
      break;
    }
  }
  return pResult;
}

static int Test_Vpi_Map_Vpi_LocationXmlResponse(Msg_Vpi_LocationXmlResponse* pMsg)
{
  String s;
  int bDone = 0;

  Test_Vpi_Map_Data* d = Test_Vpi_Map_Data_FindByHandle(pMsg->hRequest);
  if (d != 0) {
    if (pMsg->hRequest == d->apHandle()) {
      if (pMsg->bSuccess) {
        d->bMapped_ = 1;
        nTest_Vpi_Map_Mapped++;

        Msg_Vpi_GetLocationUrl msg;
        msg.sLocationXml = pMsg->sLocationXml;
        if (msg.Request()) {
          if (d->sLocationUrl_ == msg.sLocationUrl) {
            d->bSuccess_ = 1;
            nTest_Vpi_Map_Success++;
          } else {
            bDone = 1;
            s.appendf("Got:%s, expected:%s for:%s ", _sz(msg.sLocationUrl), _sz(d->sLocationUrl_), _sz(d->sDocumentUrl_));
          }
        } else {
          bDone = 1;
          s.appendf("Location url failed for url:%s ", _sz(pMsg->sDocumentUrl));
        }
      } else {
        bDone = 1;
        s.appendf("Location request failed for url:%s ", _sz(pMsg->sDocumentUrl));
      }
    }
  }

  if (nTest_Vpi_Map_Mapped == lTest_Vpi_Map_DataList.length()) {
    bDone = 1;
    if (nTest_Vpi_Map_Mapped == nTest_Vpi_Map_Success) {
      Msg_Vpi_GetStatus msg;
      msg.Request();

      int status_ok = 1;
      String sWhich;
      if (status_ok) { if (::atoi(msg.kvStatus["FilesRequested"].getString()) != 21) { status_ok = 0; sWhich.appendf("aFilesRequested gt=%d expected=%d", ::atoi(msg.kvStatus["FilesRequested"].getString()), 21); } }
      if (status_ok) { if (::atoi(msg.kvStatus["FilesReceived"].getString()) != 21) { status_ok = 0; sWhich.appendf("aFilesReceived gt=%d expected=%d", ::atoi(msg.kvStatus["FilesReceived"].getString()), 21); } }
      if (status_ok) { if (::atoi(msg.kvStatus["Files"].getString()) != 21) { status_ok = 0; sWhich.appendf("aFiles gt=%d expected=%d", ::atoi(msg.kvStatus["Files"].getString()), 21); } }
      if (status_ok) { if (::atoi(msg.kvStatus["CurrentlyRequestedFiles"].getString()) != 0) { status_ok = 0; sWhich.appendf("aCurrentlyRequestedFiles gt=%d expected=%d", ::atoi(msg.kvStatus["CurrentlyRequestedFiles"].getString()), 0); } }
      if (status_ok) { if (::atoi(msg.kvStatus["FilesExpired"].getString()) != 0) { status_ok = 0; sWhich.appendf("aFilesExpired gt=%d expected=%d", ::atoi(msg.kvStatus["FilesExpired"].getString()), 0); } }

      if (status_ok) {
        AP_UNITTEST_SUCCESS(Test_Vpi_Map_End);
      } else {
        s.appendf("mapped ok, but VPI module status is different: %s", _sz(sWhich));
      }

    } else {
      s.appendf("mapped=%d success=%d", nTest_Vpi_Map_Mapped, nTest_Vpi_Map_Success);
    }
  }

  if (!s.empty()) {
    AP_UNITTEST_FAILED(Test_Vpi_Map_End, s);
  }

  if (bDone) {
    { Msg_Vpi_LocationXmlResponse msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_Map_Vpi_LocationXmlResponse, 0); }
    Test_Vpi_UnitTest_TokenDec();
  }

  return 1;
}

static String Test_Vpi_Map_Start()
{
  String s;

  { Msg_Vpi_LocationXmlResponse msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vpi_Map_Vpi_LocationXmlResponse, 0, ApCallbackPosNormal); }  

  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-no-vpi/test1/index.html", "xmpp:80190f173d84129117b2cb406c7e6d7667e9f77e@muc4.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-no-vpi/test2/index.html", "xmpp:80190f173d84129117b2cb406c7e6d7667e9f77e@muc4.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-has-vpi/test-no-vpi/index.html", "xmpp:f6013a00b362253c64368d6eebc50ea2131754e2@location.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-has-vpi/test-fixed-name/index.html", "xmpp:test-fixed@location.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-has-vpi/test-file-name/index.html", "xmpp:index.html@location.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-empty-vpi/index.html", "xmpp:80190f173d84129117b2cb406c7e6d7667e9f77e@muc4.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-broken-xml-vpi/index.html", "xmpp:80190f173d84129117b2cb406c7e6d7667e9f77e@muc4.virtual-presence.org"));
  lTest_Vpi_Map_DataList.AddLast(new Test_Vpi_Map_Data(Apollo::newHandle(), "http://lms.virtual-presence.org/test/test-html-instead-vpi/index.html", "xmpp:80190f173d84129117b2cb406c7e6d7667e9f77e@muc4.virtual-presence.org"));

  Test_Vpi_UnitTest_TokenInc();

  if (s.empty()) {
    for (Test_Vpi_Map_Data* d = 0; (d = (Test_Vpi_Map_Data*) lTest_Vpi_Map_DataList.Next(d)) != 0; ) {

      Msg_Vpi_LocationXmlRequest msg;
      msg.sDocumentUrl = d->sDocumentUrl_;
      msg.hRequest = d->apHandle();
      if (!msg.Request()) {
        s.appendf("Failed to map '%s'", _sz(d->sDocumentUrl_));
      }
    }
  }

  return s;
}

#endif // AP_TEST_Net_HTTP

//----------------------------------------------------------

#endif // AP_TEST_Vpi

static void Test_Vpi_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Vpi_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Starting Test/Vpi"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(Test_Vpi_WithExistingData_Start);
  AP_UNITTEST_EXECUTE(Test_Vpi_Synchronous);
  AP_UNITTEST_EXECUTE(Test_Vpi_SynchronousIncomplete);

  #if defined(AP_TEST_Net_HTTP)
    if (g_bTest_Vpi_Online) {
      bTokenEndNow = 0;
      AP_UNITTEST_EXECUTE(Test_Vpi_DetailedMap_Start);
      AP_UNITTEST_EXECUTE(Test_Vpi_Map_Start);
    }
  #endif // AP_TEST_Net_HTTP

  if (bTokenEndNow) { Test_Vpi_UnitTest_TokenEnd(); }
}

void Test_Vpi_Register()
{
#if defined(AP_TEST_Vpi)

  #if defined(AP_TEST_Net_HTTP)
    if (Apollo::isLoadedModule("Net")) {
      g_bTest_Vpi_Online = Msg_Net_IsOnline::_();
    }
  #endif // AP_TEST_Net_HTTP

  if (Apollo::isLoadedModule("Vpi")) {
    AP_UNITTEST_REGISTER(Test_Vpi_WithExistingData_Start);
    AP_UNITTEST_REGISTER(Test_Vpi_WithExistingData_End);
    AP_UNITTEST_REGISTER(Test_Vpi_Synchronous);
    AP_UNITTEST_REGISTER(Test_Vpi_SynchronousIncomplete);

    #if defined(AP_TEST_Net_HTTP)
      if (g_bTest_Vpi_Online) {
        AP_UNITTEST_REGISTER(Test_Vpi_DetailedMap_Start);
        AP_UNITTEST_REGISTER(Test_Vpi_DetailedMap_End);
        AP_UNITTEST_REGISTER(Test_Vpi_Map_Start);
        AP_UNITTEST_REGISTER(Test_Vpi_Map_End);
      }
    #endif // AP_TEST_Net_HTTP

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Vpi_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Vpi
}

void Test_Vpi_Begin()
{
#if defined(AP_TEST_Vpi)
  if (Apollo::isLoadedModule("Vpi")) {
  }
#endif // AP_TEST_Vpi
}

void Test_Vpi_End()
{
#if defined(AP_TEST_Vpi)
  if (Apollo::isLoadedModule("Vpi")) {
  }
#endif // AP_TEST_Vpi
}
