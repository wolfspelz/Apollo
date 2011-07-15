// ============================================================================
//
// libvpi
//
// ============================================================================

#include "libvpi.h"
#include "VpiContext.h"
#include "vpiLog_Component.h"

vpi_context vpi_new()
{
  VpiContext* pContext = new VpiContext();
  return (vpi_context) pContext;
}

int vpi_delete(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  delete pContext;
  return 1;
}

void* vpi_malloc(size_t len)
{
  return ::malloc(len);
}

void vpi_free(void* p)
{
  ::free(p);
}

int vpi_free_list(char** list)
{
  int ok = 0;

  if (list != 0) {
    ok = 1;

    char** p = list;
    while (*p != 0) {
      vpi_free(*p);
      p++;
    }
    vpi_free(list);
    list = 0;
  }

  return ok;
}

#include "libvpi_util_include.cpp"

int vpi_set_log_callback(vpi_log_callback callback)
{
  (void) vpiLog_Interface::SetLogCallback((vpiLog_LogCallback) callback);
  return 1;
}

int vpi_set_log_mask(int mask)
{
  return vpiLog_Interface::SetMask(mask);
}


int vpi_set_config_read_callback(vpi_config_read_callback callback)
{
  (void) vpiLog_Interface::SetReadCallback((vpiLog_ReadCallback) callback);
  return 1;
}

int vpi_set_config_write_callback(vpi_config_write_callback callback)
{
  (void) vpiLog_Interface::SetWriteCallback((vpiLog_WriteCallback) callback);
  return 1;
}

int vpi_set_requestfile_callback(vpi_context context, vpi_requestfile_callback callback, vpi_callback_ref ref)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  pContext->SetRequestFileCallback(callback, ref);
  return 1;
}

int vpi_set_composelocation_callback(vpi_context context, vpi_composelocation_callback callback, vpi_callback_ref ref)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  pContext->SetComposeLocationCallback(callback, ref);
  return 1;
}

int vpi_get_last_error(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  return pContext->GetLastError();
}

int vpi_get_last_error_string(vpi_context context, char* err, unsigned int* len)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  String sError = pContext->GetLastErrorString();
  if (err == 0 || len == 0 || *len <= sError.bytes()) {
    ok = 0;
  } else {
    ::strcpy(err, sError);
    *len = sError.bytes() + 1;
  }

  return ok;
}

int vpi_get_status(vpi_context context, VPI_OUT char*** status_list)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (status_list == 0) {
    pContext->SetError("vpi_get_status", VPI_ERROR_INVALID_PARAMETER, "status_list=0");
  } else {
    List lStatus;
    if (pContext->GetStatus(lStatus)) {
      *status_list = vpi_ListKeyValue_to_strlist(lStatus);
      ok = 1;
    }
  }

  return ok;
}

int vpi_get_traversed_files(vpi_context context, VPI_OUT char*** files_list)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (files_list == 0) {
    pContext->SetError("vpi_get_traversed_files", VPI_ERROR_INVALID_PARAMETER, "files_list=0");
  } else {
    List lFiles;
    if (pContext->GetTraversedFiles(lFiles)) {
      *files_list = vpi_ListKey_to_strlist(lFiles);
      ok = 1;
    }
  }

  return ok;
}

int vpi_set_time(vpi_context context, time_t now)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  pContext->SetTime(now);

  return ok;
}

int vpi_get_location_xml(vpi_context context, const char* document_url, char** location_xml)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (location_xml == 0) {
    pContext->SetError("vpi_get_location_xml", VPI_ERROR_INVALID_PARAMETER, "location_xml");
  } else {
    String sLocationXml;
    String sDocumentUrl(document_url);
    if (pContext->GetLocationXml(sDocumentUrl, sLocationXml)) {
      *location_xml = (char*) vpi_malloc(sLocationXml.bytes() + 1);
      ::strcpy(*location_xml, sLocationXml.c_str());
      ok = 1;
    }
  }

  return ok;
}

int vpi_get_detail_xml(vpi_context context, const char* location_xml, const char* path, int flags, char** detail_xml)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (location_xml == 0 || detail_xml == 0) {
    pContext->SetError("vpi_get_detail_xml", VPI_ERROR_INVALID_PARAMETER, "location_xml or detail_xml");
  } else {
    String sLocationXml(location_xml);
    String sPath(path);
    String sDetailXml;
    if (pContext->GetDetailXml(sLocationXml, sPath, flags, sDetailXml)) {
      *detail_xml = (char*) vpi_malloc(sDetailXml.bytes() + 1);
      ::strcpy(*detail_xml, sDetailXml.c_str());
      ok = 1;
    }
  }

  return ok;
}

int vpi_get_location_url(vpi_context context, const char* location_xml, char** location_url)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (location_xml == 0 || location_url == 0) {
    pContext->SetError("vpi_get_location_url", VPI_ERROR_INVALID_PARAMETER, "location_xml or location_url");
  } else {
    String sLocationXml(location_xml);
    String sLocationUrl;
    if (pContext->GetLocationUrl(sLocationXml, sLocationUrl)) {
      *location_url = (char*) vpi_malloc(sLocationUrl.bytes() + 1);
      ::strcpy(*location_url, sLocationUrl.c_str());
      ok = 1;
    }
  }

  return ok;
}

int vpi_get_select_options(vpi_context context, const char* location_xml, char*** options_list)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (location_xml == 0 || options_list == 0) {
    pContext->SetError("vpi_get_select_options", VPI_ERROR_INVALID_PARAMETER, "location_xml || options_list");
  } else {
    String sLocationXml(location_xml);
    List lOptions;
    if (pContext->GetSelectOptions(sLocationXml, lOptions)) {
      *options_list = vpi_ListKey_to_strlist(lOptions);
      ok = 1;
    }
  }

  return ok;
}

int vpi_get_select_option_properties(vpi_context context, const char* location_xml, const char* suffix, char*** properties_list)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (location_xml == 0 || suffix == 0|| properties_list == 0) {
    pContext->SetError("vpi_get_select_option_properties", VPI_ERROR_INVALID_PARAMETER, "location_xml or suffix or properties_list");
  } else {
    String sLocationXml(location_xml);
    String sSuffix(suffix);
    List lProperties;
    if (pContext->GetSelectOptionProperties(sLocationXml, sSuffix,  lProperties)) {
      *properties_list = vpi_ListKeyValue_to_strlist(lProperties);
      ok = 1;
    }
  }

  return ok;
}

int vpi_set_suffix(vpi_context context, const char* suffix)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  String sSuffix(suffix);
  return pContext->SetSuffix(sSuffix);
}

int vpi_get_suffix(vpi_context context, char** suffix)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  if (suffix == 0) {
    pContext->SetError("vpi_get_suffix", VPI_ERROR_INVALID_PARAMETER, "suffix");
  } else {
    String sSuffix;
    if (pContext->GetSuffix(sSuffix)) {
      *suffix = (char*) vpi_malloc(sSuffix.bytes() + 1);
      ::strcpy(*suffix, sSuffix.c_str());
      ok = 1;
    }
  }

  return ok;
}

int vpi_cache_clear(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  return pContext->Clear();
}

int vpi_cache_expire(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  return pContext->ExpireFiles();
}

int vpi_cache_add_xml(vpi_context context, const char* url, const char* xml)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  return pContext->SetXmlFile(url, xml);
}

int vpi_cache_add_nocontent(vpi_context context, const char* url)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  return pContext->SetXmlFile(url, "");
}

//----------------------------------------------------------

#if defined(_DEBUG)

static int vpi_test_expire_basic(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  pContext->Clear();

  time_t tNow = time(0);
  pContext->SetTime(tNow);
  pContext->SetXmlFile("http://www.virtual-presence.org.de/_vpi.xml", "<vpi/>");
  pContext->SetXmlFile("http://www.bluehands.de/test/_vpi.xml", "<vpi/>");
  pContext->SetTime(tNow + 2);
  pContext->SetXmlFile("http://www.bluehands.de/_vpi.xml", "<vpi/>");
  pContext->SetTime(tNow + 3601);
  pContext->ExpireFiles();

  if (pContext->CountFiles() != 1) {
    ok = 0;
    String s; s.appendf("Expected %d, got %s files still cached", 1, pContext->CountFiles());
    pContext->SetError("vpi_test_expire_basic", VPI_ERROR_TEST_FAILED, s);
  }

  pContext->Clear();

  return ok;
}

//-----------------------------

static int vpi_test_expire_ttl(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  pContext->Clear();

  time_t tNow = time(0);
  pContext->SetTime(tNow);
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/root.xml",
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <!-- No ttl -->"
    "  <delegate>"
    "    <uri>default.xml</uri>"
    "  </delegate>"
    "</vpi>"
    );
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/org.xml",
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <Info TimeToLive='100' />"
    "  <delegate>"
    "    <uri>default.xml</uri>"
    "  </delegate>"
    "</vpi>"
    );
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/default.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <Info TimeToLive='200' />"
    "  <location match='.*'>"
    "    <service>jabber:location.virtual-presence.org</service>"
    "    <ignore/>"
    "  </location>"
    "</vpi>"
    );
  pContext->SetTime(tNow + 50);
  pContext->ExpireFiles();

  if (pContext->CountFiles() != 3) {
    ok = 0;
    String s; s.appendf("Expected %d, got %d files still cached", 3, pContext->CountFiles());
    pContext->SetError("vpi_test_expire_ttl", VPI_ERROR_TEST_FAILED, s);
  }

  pContext->SetTime(tNow + 150);
  pContext->ExpireFiles();

  if (pContext->CountFiles() != 2) {
    ok = 0;
    String s; s.appendf("Expected %d, got %d files still cached", 2, pContext->CountFiles());
    pContext->SetError("vpi_test_expire_ttl", VPI_ERROR_TEST_FAILED, s);
  }

  pContext->SetTime(tNow + 250);
  pContext->ExpireFiles();

  if (pContext->CountFiles() != 1) {
    ok = 0;
    String s; s.appendf("Expected %d, got %d files still cached", 1, pContext->CountFiles());
    pContext->SetError("vpi_test_expire_ttl", VPI_ERROR_TEST_FAILED, s);
  }

  pContext->SetTime(tNow + 4000);
  pContext->ExpireFiles();

  if (pContext->CountFiles() != 0) {
    ok = 0;
    String s; s.appendf("Expected %d, got %d files still cached", 0, pContext->CountFiles());
    pContext->SetError("vpi_test_expire_ttl", VPI_ERROR_TEST_FAILED, s);
  }

  pContext->Clear();

  return ok;
}

//-----------------------------

static int g_vpi_test_requestfile_callback_hit = 0;
static int vpi_test_requestfile_callback_callback(vpi_callback_ref ref, const char* url)
{
  VPI_UNUSED_ARG(ref);
  VPI_UNUSED_ARG(url);
  g_vpi_test_requestfile_callback_hit = 1;
  return 1;
}

static int vpi_test_requestfile_callback(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  pContext->Clear();
  pContext->SetRequestFileCallback(vpi_test_requestfile_callback_callback, 0);

  if (ok) {
    String sDocumentUrl("http://www.the-vpi-for-this-domain-is-not-already-cached.com/folder1/folder2/file.html");
    String sLocation;
    if (pContext->GetLocationXml(sDocumentUrl, sLocation)) {
      ok = 0;
      pContext->SetError("vpi_test_requestfile_callback", VPI_ERROR_TEST_FAILED, "Unexpected: function completed successfully");
    }
  }

  if (ok) {
    if (!g_vpi_test_requestfile_callback_hit) {
      ok = 0;
      pContext->SetError("vpi_test_requestfile_callback", VPI_ERROR_TEST_FAILED, "Callback not hit");
    }
  }

  pContext->SetRequestFileCallback(0, 0);
  pContext->Clear();

  return ok;
}

//-----------------------------

static int vpi_test_builtin_composelocation_for_xmpp(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  pContext->Clear();
  pContext->SetComposeLocationCallback(0, 0);

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/_vpi.xml", "");
  pContext->SetXmlFile("http://www.virtual-presence.org/_vpi.xml", "");
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/root.xml",
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
    "    <service>jabber:location.virtual-presence.org</service>"
    "    <name>\\5</name>"
    "  </location>"
    "</vpi>"
    );

  String sDocumentUrl("http://www.virtual-presence.org/notes/VPTN-2.txt");
  String sLocationXml;
  ok = pContext->GetLocationXml(sDocumentUrl, sLocationXml);
  if (ok) {
    if (sLocationXml == "") {
      ok = 0;
      pContext->SetError("vpi_test_builtin_composelocation_for_xmpp", VPI_ERROR_TEST_FAILED, "pContext->GetLocationXml() failed");
    } else {
      String sLocationUrl;
      ok = pContext->GetLocationUrl(sLocationXml, sLocationUrl);
      if (!ok || sLocationUrl.empty()) {
        ok = 0;
        pContext->SetError("vpi_test_builtin_composelocation_for_xmpp", VPI_ERROR_TEST_FAILED, "pContext->GetLocationUrl() failed");
      } else {
        String sExpectedLocationUrl = "xmpp:virtual-presence.org@location.virtual-presence.org";
        if (sLocationUrl != sExpectedLocationUrl) {
          ok = 0;
          String s;
          s.appendf("Got=%s Expected=%s", _sz(sLocationUrl), _sz(sExpectedLocationUrl));
          pContext->SetError("vpi_test_builtin_composelocation_for_xmpp", VPI_ERROR_TEST_FAILED, s);
        }
      }
    }
  }

  pContext->Clear();

  return ok;
}

//-----------------------------

static int vpi_test_detailxml(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  pContext->Clear();

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/_vpi.xml", "");
  pContext->SetXmlFile("http://www.virtual-presence.org/_vpi.xml", "");
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/root.xml",
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <Info"
    "    Version='1' "
    "    Description='This is the mapping for sample-domain.org' "
    "    AdminContact='admin@sample-domain.org' "
    "    TechContact='root@sample-domain.org' "
    "    TimeToLive='3600' "
    "    ExpiryTime='86400' "
    "    RefreshInterval='3600' "
    "    TransferRetryTime='600' "
    "  />"
    "  <delegate match='^http://(www\\.)?google\\.([^./]*)($|/.*$)'>"
    "    <uri>google.xml</uri>"
    "  </delegate>"
    "  <delegate match='^http://.*\\.(org)($|/.*$)'>"
    "    <uri>\\1.xml</uri>"
    "  </delegate>"
    "  <delegate>"
    "    <uri>default.xml</uri>"
    "  </delegate>"
    "</vpi>"
    );
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/org.xml",
    "<?xml version='1.0' encoding='ISO-8859-1'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <location match='^https?://(.*\\.)?slashdot\\.org($|/.*$)'>"
    "    <service>jabber:location.virtual-presence.org</service>"
    "    <name>slashdot.org</name>"
    "  </location>"
    "  <delegate>"
    "    <uri>default.xml</uri>"
    "  </delegate>"
    "</vpi>"
    );
  pContext->SetXmlFile("http://lms.virtual-presence.org/v7/default.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
    "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
    "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
    "    <service>jabber:location.virtual-presence.org</service>"
    "    <name test='\xC3\xA4 \xE9\xA6\x96' hash='true'>\\5</name>"
    "    <destination>\\1</destination>"
    "    <topology level='domain'/>"
    "    <!--delay sec='0'/-->"
    "    <displayoptions>"
    "      <usercount multiple='15' crowd='40' massive='80'/>"
    "    </displayoptions>"
    "  </location>"
    "  <location match='.*'>"
    "    <service>jabber:location.virtual-presence.org</service>"
    "    <ignore/>"
    "  </location>"
    "</vpi>"
    );

  String sDocumentUrl("http://www.virtual-presence.org/notes/VPTN-2.txt");
  String sLocationXml;
  ok = pContext->GetLocationXml(sDocumentUrl, sLocationXml);
  if (ok) {
    //MessageBoxW(NULL, sLocationXml.w_str(), L"Test", MB_OK);
    if (sLocationXml == "") {
      ok = 0;
      pContext->SetError("vpi_test_detailxml", VPI_ERROR_TEST_FAILED, "pContext->GetLocationXml(http://www.virtual-presence.org/notes/VPTN-2.txt) failed");
    }
  }

  if (ok) {
    String sDetail = "name";
    String sXml;
    int bInner = 0;
    ok = pContext->GetDetailXml(sLocationXml, sDetail, bInner, sXml);
    if (ok) {
      String sExpected = "<name test='\xC3\xA4 \xE9\xA6\x96' hash='true'>virtual-presence.org</name>";
      if (sXml != sExpected) {
        ok = 0;
        String s;
        s.appendf("pContext->GetDetailXml() Wrong result for '%s'. Got:%s, expected:%s", _sz(sDetail), _sz(sXml), _sz(sExpected));
        pContext->SetError("vpi_test_detailxml", VPI_ERROR_TEST_FAILED, s);
      }
    }
  }

  if (ok) {
    String sDetail = "destination";
    String sXml;
    int bInner = 1;
    ok = pContext->GetDetailXml(sLocationXml, sDetail, bInner, sXml);
    if (ok) {
      String sExpected = "http://www.virtual-presence.org";
      if (sXml != sExpected) {
        ok = 0;
        String s;
        s.appendf("pContext->GetDetailXml() Wrong result for '%s'. Got:%s, expected:%s", _sz(sDetail), _sz(sXml), _sz(sExpected));
        pContext->SetError("vpi_test_detailxml", VPI_ERROR_TEST_FAILED, s);
      }
    }
  }

  if (ok) {
    List lFiles;
    pContext->GetTraversedFiles(lFiles);

    // Add numbered index
    int nCnt = 0;
    for (Elem* e = 0; (e = lFiles.Next(e)); ) { e->setInt(nCnt++); }

    if (0
      || lFiles[0].getName() != "http://www.virtual-presence.org/notes/_vpi.xml"
      || lFiles[1].getName() != "http://www.virtual-presence.org/_vpi.xml"
      || lFiles[2].getName() != "http://lms.virtual-presence.org/v7/root.xml"
      || lFiles[3].getName() != "http://lms.virtual-presence.org/v7/org.xml"
      || lFiles[4].getName() != "http://lms.virtual-presence.org/v7/default.xml"
      ) {
      ok = 0;
      String s;
      s.appendf("pContext->GetTraversedFiles() Wrong file list");
      pContext->SetError("vpi_test_detailxml", VPI_ERROR_TEST_FAILED, s);
    }
  }

  pContext->Clear();

  return ok;
}

//----------------------------------------------------------
// TEST:

static int vpi_test_composelocation_callback(vpi_callback_ref ref, const char* protocol, const char* name, const char* service, char* url, unsigned int* url_len)
{
  VPI_UNUSED_ARG(ref);
  int ok = 0;
  if (url_len != 0) {
    String sProtocol = protocol;
    String sName = name;
    String sService = service;
    String sPart;
    sService.nextToken(":", sPart);
    String sLocationUrl;
    sLocationUrl.appendf("%s:%s@%s", _sz(sProtocol), _sz(sName), _sz(sService));
    if (url == 0) {
      *url_len = sLocationUrl.bytes() + 1;
      ok = 1;
    } else {
      if (*url_len > sLocationUrl.bytes()) {
        memcpy(url, sLocationUrl.c_str(), sLocationUrl.bytes() + 1);
        ok= 1;
      }
    }
  }
  return ok;
}

//-----------------------------

static int vpi_test_map_1(vpi_context context, const char* comment, const char* vpi_url, const char* vpi, const char* document_url, const char* expected_location_url)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  String s;

  pContext->Clear();
  pContext->SetComposeLocationCallback(vpi_test_composelocation_callback, 0);

  pContext->SetXmlFile(vpi_url, vpi);
  String sDocumentUrl(document_url);
  String sLocationXml;
  if (!pContext->GetLocationXml(sDocumentUrl, sLocationXml)) {
    s.appendf("pContext->GetLocationXml failed");
  } else {
    String sLocationUrl;
    if (!pContext->GetLocationUrl(sLocationXml, sLocationUrl)) {
      s.appendf("pContext->GetLocationUrl failed");
    } else {
      if (sLocationUrl != expected_location_url) {
        s.appendf("pContext->GetLocationUrl(%s) failed: got:%s, expected:%s", _sz(document_url), _sz(sLocationUrl), _sz(expected_location_url));
      }
    }
  }

  pContext->Clear();

  if (!s.empty()) {
    pContext->SetError(comment, VPI_ERROR_TEST_FAILED, s);
  }

  return s.empty();
}

static int vpi_test_map(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  if (ok) {
    ok = vpi_test_map_1(context,
      "vpi_test_map (basic)",
      "http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <service>xmpp:location.virtual-presence.org</service>"
        "    <name>\\5</name>"
        "  </location>"
        "</vpi>"
      ,"http://www.virtual-presence.org/notes/VPTN-2.txt"
      ,"xmpp:virtual-presence.org@location.virtual-presence.org"
      );
  }

  if (ok) {
    ok = vpi_test_map_1(context,
      "vpi_test_map (hash)",
      "http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <service>xmpp:location.virtual-presence.org</service>"
        "    <name hash='true'>\\5</name>"
        "  </location>"
        "</vpi>"
      ,"http://www.virtual-presence.org/notes/VPTN-2.txt"
      ,"xmpp:341973964fa3faf9c1d6e8c9255bb3fd84fcb005@location.virtual-presence.org"
      );
  }

  if (ok) {
    ok = vpi_test_map_1(context,
      "vpi_test_map (prefix)",
      "http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <service>xmpp:location.virtual-presence.org</service>"
        "    <name prefix='prefix-'>\\5</name>"
        "  </location>"
        "</vpi>"
      ,"http://www.virtual-presence.org/notes/VPTN-2.txt"
      ,"xmpp:prefix-virtual-presence.org@location.virtual-presence.org"
      );
  }

  if (ok) {
    ok = vpi_test_map_1(context,
      "vpi_test_map (hash-prefix)",
      "http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <service>xmpp:location.virtual-presence.org</service>"
        "    <name prefix='\\2' hash='SHA1'>\\5</name>"
        "  </location>"
        "</vpi>"
      ,"http://www.virtual-presence.org/notes/VPTN-2.txt"
      ,"xmpp:www.341973964fa3faf9c1d6e8c9255bb3fd84fcb005@location.virtual-presence.org"
      );
  }

  if (ok) {
    ok = vpi_test_map_1(context,
      "vpi_test_map (url)",
      "http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <url>xmpp:\\5@location.virtual-presence.org</url>"
        "  </location>"
        "</vpi>"
      ,"http://www.virtual-presence.org/notes/VPTN-2.txt"
      ,"xmpp:virtual-presence.org@location.virtual-presence.org"
      );
  }

  return ok;
}

//-----------------------------

static int vpi_test_delegate(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  String sDocumentUrl = "http://www.virtual-presence.org/notes/VPTN-2.txt";
  String sExpectedLocationUrl = "xmpp:virtual-presence.org@location.virtual-presence.org";

  pContext->Clear();

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/_vpi.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
      "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
      "  <delegate>"
      "    <uri>delegate1.xml</uri>"
      "  </delegate>"
    "</vpi>"
    );

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/delegate1.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
      "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
      "  <delegate>"
      "    <uri>test/delegate2.xml</uri>"
      "  </delegate>"
    "</vpi>"
    );

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/test/delegate2.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
      "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
      "  <delegate>"
      "    <uri>./.././delegate3.xml</uri>"
      "  </delegate>"
    "</vpi>"
    );

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/delegate3.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
      "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
      "  <location>"
      "    <url>xmpp:virtual-presence.org@location.virtual-presence.org</url>"
      "  </location>"
    "</vpi>"
    );

  String sLocationXml;
  if (pContext->GetLocationXml(sDocumentUrl, sLocationXml)) {
    String sLocationUrl;
    if (pContext->GetLocationUrl(sLocationXml, sLocationUrl)) {
      if (sLocationUrl == sExpectedLocationUrl) {
        ok = 1;
      } else {
        String s; s.appendf("pContext->GetLocationUrl(%s) failed: got:%s, expected:%s", _sz(sDocumentUrl), _sz(sLocationUrl), _sz(sExpectedLocationUrl));
        pContext->SetError("vpi_test_delegate", VPI_ERROR_TEST_FAILED, s);
      }
    }
  }

  pContext->Clear();

  return ok;
}

//-----------------------------

static int vpi_test_suffix(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  String sDocumentUrl = "http://www.virtual-presence.org/notes/VPTN-2.txt";
  String sExpectedLocationUrl = "xmpp:virtual-presence.org-suffix@location.virtual-presence.org";

  pContext->Clear();
  pContext->SetComposeLocationCallback(vpi_test_composelocation_callback, 0);

  pContext->SetXmlFile("http://www.virtual-presence.org/notes/_vpi.xml",
    "<?xml version='1.0' encoding='UTF-8'?>"
      "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
      "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
      "    <service>xmpp:location.virtual-presence.org</service>"
      "    <name>\\5</name>"
      "  </location>"
    "</vpi>"
    );

  String sTmpSuffix("-tmp");
  pContext->SetSuffix(sTmpSuffix);

  String sOldSuffix;
  pContext->GetSuffix(sOldSuffix);

  String sSuffix("-suffix");
  pContext->SetSuffix(sSuffix);

  String sLocationXml;
  if (pContext->GetLocationXml(sDocumentUrl, sLocationXml)) {
    String sLocationUrl;
    if (pContext->GetLocationUrl(sLocationXml, sLocationUrl)) {
      if (sLocationUrl == sExpectedLocationUrl) {
        ok = 1;
      } else {
        String s; s.appendf("pContext->GetLocationUrl(%s) failed: got:%s, expected:%s", _sz(sDocumentUrl), _sz(sLocationUrl), _sz(sExpectedLocationUrl));
        pContext->SetError("vpi_test_suffix", VPI_ERROR_TEST_FAILED, s);
      }
    }
  }

  pContext->SetSuffix(sOldSuffix);

  String sCheckSuffix;
  pContext->GetSuffix(sCheckSuffix);
  if (sCheckSuffix != sTmpSuffix) {
    String s; s.appendf("Suffix Get/Set/Get failed: got:%s, expected:%s", _sz(sCheckSuffix), _sz(sTmpSuffix));
    pContext->SetError("vpi_test_suffix", VPI_ERROR_TEST_FAILED, s);
    ok = 0;
  }

  pContext->Clear();

  return ok;
}

//-----------------------------

static int vpi_test_select(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 1;

  String sDocumentUrl = "http://www.virtual-presence.org/notes/VPTN-2.txt";
  String sExpectedLocationUrl = "xmpp:virtual-presence.org-suffix@location.virtual-presence.org";

  pContext->Clear();
  pContext->SetComposeLocationCallback(vpi_test_composelocation_callback, 0);

  if (ok) {
    ok = pContext->SetXmlFile("http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <service>xmpp:location.virtual-presence.org</service>"
        "    <name>\\5</name>"
        "    <select>"
        "       <option suffix='politics' title='Politics' />"
        "       <option suffix='sports' "
        "               title='Sports \xC3\xA4 \xE9\xA6\x96' "
        "               description='Discuss sports here \xC3\xA4 \xC3\xB6 at \\5'>"
        "         <tag>lang:de</tag>"
        "         <tag>sports</tag>"
        "       </option>"
        "       <option suffix='sports-all-languages' title='Sports intl.'>"
        "         <tag>sports</tag>"
        "       </option>"
        "    </select>"
        "  </location>"
        "</vpi>"
      );
  }

  String sLocationXml;
  List lOptions;
  if (ok) {
    char* pLocationXml = 0;
    ok = vpi_get_location_xml((vpi_context) pContext, sDocumentUrl, &pLocationXml);
    if (ok) {
      sLocationXml = pLocationXml;
      (void) vpi_free(pLocationXml);

      char** pOptions = 0;
      if (ok) {
        ok = vpi_get_select_options((vpi_context) pContext, sLocationXml, &pOptions);
        if (ok) {
          vpi_strlist_to_ListKey(pOptions, lOptions);
          (void) vpi_free_list(pOptions);
        }
      }
    }
  }

  if (ok) {
    Elem* e1 = lOptions.First();
    Elem* e2 = lOptions.Next(e1);
    Elem* e3 = lOptions.Next(e2);
    if (!(
          e1->getName() == "politics" && e1->getString() == ""
       && e2->getName() == "sports" && e2->getString() == ""
       && e3->getName() == "sports-all-languages" && e3->getString() == ""
      )) {
      ok = 0;
    }
  }

  if (ok) {
    Elem* eOption = lOptions.First();
    char** pProperties = 0;
    ok = vpi_get_select_option_properties((vpi_context) pContext, sLocationXml, eOption->getName(), &pProperties);
    if (ok) {
      List lProperties;
      vpi_strlist_to_ListKeyValue(pProperties, lProperties);
      (void) vpi_free_list(pProperties);

      if (lProperties.length() != 2) { ok = 0; }
      Elem* e = 0;
      e = lProperties.Next(e); if (e->getName() != "suffix" || e->getString() != "politics") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "title" || e->getString() != "Politics") { ok = 0; }
    }
  }

  if (ok) {
    Elem* eOption = lOptions.First(); eOption = lOptions.Next(eOption);
    char** pProperties = 0;
    ok = vpi_get_select_option_properties((vpi_context) pContext, sLocationXml, eOption->getName(), &pProperties);
    if (ok) {
      List lProperties;
      vpi_strlist_to_ListKeyValue(pProperties, lProperties);
      (void) vpi_free_list(pProperties);

      if (lProperties.length() != 5) { ok = 0; }
      Elem* e = 0;
      e = lProperties.Next(e); if (e->getName() != "suffix" || e->getString() != "sports") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "title" || e->getString() != "Sports \xC3\xA4 \xE9\xA6\x96") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "description" || e->getString() != "Discuss sports here \xC3\xA4 \xC3\xB6 at virtual-presence.org") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "tag" || e->getString() != "lang:de") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "tag" || e->getString() != "sports") { ok = 0; }
    }
  }

  if (ok) {
    Elem* eOption = lOptions.First(); eOption = lOptions.Next(eOption); eOption = lOptions.Next(eOption);
    char** pProperties = 0;
    ok = vpi_get_select_option_properties((vpi_context) pContext, sLocationXml, eOption->getName(), &pProperties);
    if (ok) {
      List lProperties;
      vpi_strlist_to_ListKeyValue(pProperties, lProperties);
      (void) vpi_free_list(pProperties);

      if (lProperties.length() != 3) { ok = 0; }
      Elem* e = 0;
      e = lProperties.Next(e); if (e->getName() != "suffix" || e->getString() != "sports-all-languages") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "title" || e->getString() != "Sports intl.") { ok = 0; }
      e = lProperties.Next(e); if (e->getName() != "tag" || e->getString() != "sports") { ok = 0; }
    }
  }

  pContext->Clear();

  return ok;
}

//-----------------------------

static int vpi_test_getstatus_requestfile_callback(vpi_callback_ref ref, const char* url)
{
  VPI_UNUSED_ARG(url);
  if (ref != 0) {
    VpiContext* pContext = (VpiContext*) ref;
    pContext->SetXmlFile("http://www.virtual-presence.org/notes/_vpi.xml",
      "<?xml version='1.0' encoding='UTF-8'?>"
        "<vpi xmlns='http://virtual-presence.org/schemas/vpi'>"
        "  <location match='^(https?://((w)+([0-9]*)\\.)?([^/]+\\.([a-zA-Z]+)))($|/.*$)'>"
        "    <service>xmpp:location.virtual-presence.org</service>"
        "    <name>\\5</name>"
        "  </location>"
      "</vpi>"
      );
  }

  return 1;
}

static int vpi_test_getstatus(vpi_context context)
{
  VPI_CHECKPTR(VpiContext, pContext, context, 0);
  int ok = 0;

  pContext->Clear();
  pContext->SetComposeLocationCallback(vpi_test_composelocation_callback, 0);
  pContext->SetRequestFileCallback(vpi_test_getstatus_requestfile_callback, (long) pContext);

  String sSuffix("suffix");
  pContext->SetSuffix(sSuffix);
  time_t tNow = time(0);
  pContext->SetTime(tNow);

  String sDocumentUrl("http://www.virtual-presence.org/notes/VPTN-2.txt");
  String sLocationXml;
  pContext->GetLocationXml(sDocumentUrl, sLocationXml);
  pContext->GetLocationXml(sDocumentUrl, sLocationXml);
  pContext->SetTime(tNow + 10000);
  pContext->ExpireFiles();
  pContext->GetLocationXml(sDocumentUrl, sLocationXml);

  char** pStatus = 0;
  List lStatus;
  ok = vpi_get_status((vpi_context) pContext, &pStatus);
  if (ok) {
    vpi_strlist_to_ListKeyValue(pStatus, lStatus);
    (void) vpi_free_list(pStatus);

    if (ok) { if (lStatus["Suffix"].getString() != "suffix") { ok = 0; } }
    if (ok) { if (::atoi(lStatus["Files"].getString()) != 1) { ok = 0; } }
    if (ok) { if (::atoi(lStatus["CurrentlyRequestedFiles"].getString()) != 0) { ok = 0; } }
    if (ok) { if (::atoi(lStatus["FilesReceived"].getString()) != 2) { ok = 0; } }
    if (ok) { if (::atoi(lStatus["FilesExpired"].getString()) != 1) { ok = 0; } }
    if (ok) { if (::atoi(lStatus["FilesRequested"].getString()) != 2) { ok = 0; } }
    if (ok) { if (::atoi(lStatus["GetLocationXml"].getString()) != 3) { ok = 0; } }
  }

  pContext->Clear();

  return ok;
}

#endif

int vpi_run_tests(vpi_context context)
{
  int ok = 1;

#if defined(_DEBUG)
  if (ok) { ok = vpi_test_expire_basic(context); }
  if (ok) { ok = vpi_test_expire_ttl(context); }
  if (ok) { ok = vpi_test_requestfile_callback(context); }
  if (ok) { ok = vpi_test_builtin_composelocation_for_xmpp(context); }
  if (ok) { ok = vpi_test_detailxml(context); }
  if (ok) { ok = vpi_test_map(context); }
  if (ok) { ok = vpi_test_delegate(context); }
  if (ok) { ok = vpi_test_suffix(context); }
  if (ok) { ok = vpi_test_select(context); }
  if (ok) { ok = vpi_test_getstatus(context); }
#else
  VPI_UNUSED_ARG(context);
#endif

  return ok;
}

