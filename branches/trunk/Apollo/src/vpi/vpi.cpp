// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "vpi.h"
#include "MsgVpi.h"
#include "MsgUnitTest.h"
#include "ApLog.h"

#include "libvpi/libvpi.h"
#include "netapi/NetInterface.h"
#include "XMLProcessor.h"

using namespace Apollo;

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

#define MODULE_NAME "Vpi"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "Vpi",
  "URL Map Module",
  "1",
  "Location mapping module. Implements RFC whatever.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

VPI_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class LocationTask: public Elem
{
public:
  LocationTask(String& sDocumentUrl, const ApHandle& hRequest, TimeValue& tStarted)
    :sDocumentUrl_(sDocumentUrl)
    ,hRequest_(hRequest)
    ,tStarted_(tStarted)
  { apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat, _sz(sDocumentUrl_), ApHandlePrintf(hRequest_))); }
  virtual ~LocationTask() {}

public:
  String sDocumentUrl_;
  ApHandle hRequest_;
  TimeValue tStarted_;
};

//----------------------

class VpiModule
{
public:
  VpiModule()
    :pVpi_(0)
    {}

  int Init();
  void Exit();

  int RequestFile(const String& sUrl);
  int ReceiveFile(const String& sUrl, const String& sContentType, Buffer& sbData);

  void On_Vpi_Clear(Msg_Vpi_Clear* pMsg);
  void On_Vpi_GetStatus(Msg_Vpi_GetStatus* pMsg);
  void On_Vpi_GetTraversedFiles(Msg_Vpi_GetTraversedFiles* pMsg);
  void On_Vpi_LocationXmlRequest(Msg_Vpi_LocationXmlRequest* pMsg);
  void On_Vpi_GetLocationXml(Msg_Vpi_GetLocationXml* pMsg);
  void On_Vpi_ReceiveFile(Msg_Vpi_ReceiveFile* pMsg);
  void On_Vpi_GetDetailXml(Msg_Vpi_GetDetailXml* pMsg);
  void On_Vpi_GetLocationUrl(Msg_Vpi_GetLocationUrl* pMsg);
  void On_Vpi_SetSuffix(Msg_Vpi_SetSuffix* pMsg);
  void On_Vpi_GetSuffix(Msg_Vpi_GetSuffix* pMsg);
  void On_Vpi_GetSelectOptions(Msg_Vpi_GetSelectOptions* pMsg);
  void On_Vpi_GetSelectOptionProperties(Msg_Vpi_GetSelectOptionProperties* pMsg);
  void On_Vpi_ComposeLocation(Msg_Vpi_ComposeLocation* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif
  
  static void LibVpiLogOutCallback(int level, const char* channel, const char* context, const char* message);
  static const char* LibVpiLogConfigReadCallback(const char* path, const char* defaultvalue, char* buf, size_t buflen);
  static void LibVpiLogConfigWriteCallback(const char* path, const char* value);
  static int LibVpiRequestFileCallback(long ref, const char* url);
  static int LibVpiComposeLocationCallback(vpi_callback_ref ref, const char* protocol, const char* name, const char* service, char* url, unsigned int* url_len);

protected:
  vpi_context pVpi_;
  SafeListT<LocationTask> sLocationTasks_;
};

typedef ApModuleSingleton<VpiModule> VpiModuleInstance;

//----------------------

int VpiModule::Init()
{
  int ok = 1;

  pVpi_ = vpi_new();
  if (pVpi_ == 0) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "vpi_new() failed"));
  } else {

    int nMask = apLog_SetMask(0);
    nMask = apLog_SetMask(nMask);

    vpi_set_log_callback(LibVpiLogOutCallback);
    vpi_set_log_mask(nMask);
    vpi_set_config_read_callback(LibVpiLogConfigReadCallback);
    vpi_set_config_write_callback(LibVpiLogConfigWriteCallback);

    vpi_set_requestfile_callback(pVpi_, LibVpiRequestFileCallback, (vpi_callback_ref) this);
    vpi_set_composelocation_callback(pVpi_, LibVpiComposeLocationCallback, (vpi_callback_ref) this);

  }

  return ok;
}

void VpiModule::Exit()
{
  if (pVpi_ != 0) {
    vpi_delete(pVpi_);
    pVpi_ = 0;
  }
}

//----------------------

void VpiModule::LibVpiLogOutCallback(int level, const char* channel, const char* context, const char* message)
{
  //AP_UNUSED_ARG(level);
  //AP_UNUSED_ARG(channel);
  //AP_UNUSED_ARG(context);
  //AP_UNUSED_ARG(message);
  Apollo::sendLog(level, channel, context, message);
	//printf("%d %s %s %s\n", level, channel, context, message);
}

const char* VpiModule::LibVpiLogConfigReadCallback(const char* path, const char* defaultvalue, char* buf, size_t buflen)
{
  //printf("configreadfunc %s %s\n", path, defaultvalue);
  const char* szResult = defaultvalue;

  String sResult = Apollo::getModuleConfig(MODULE_NAME, path, defaultvalue);

  if (buf != 0 && buflen > sResult.bytes() && sResult.bytes() > 0) {
    ::memcpy(buf, sResult.c_str(), sResult.bytes() +1);
    szResult = buf;
  }
  
  return szResult;
}

void VpiModule::LibVpiLogConfigWriteCallback(const char* path, const char* value)
{
  AP_UNUSED_ARG(path);
  AP_UNUSED_ARG(value);
	//printf("write %s %s\n", path, value);
}

int VpiModule::LibVpiRequestFileCallback(long ref, const char* url)
{
  VpiModule* pModule = (VpiModule*) ref;
  int ok = 0;

  String sUrl(url);
  ok = pModule->RequestFile(sUrl);

  return ok; 
}

int VpiModule::LibVpiComposeLocationCallback(vpi_callback_ref ref, const char* protocol, const char* name, const char* service, char* url, unsigned int* url_len)
{
  AP_UNUSED_ARG(ref);
  //VpiModule* pModule = (VpiModule*) ref;
  int ok = 0;

  if (url_len != 0) {
    Msg_Vpi_ComposeLocation msg;
    msg.sProtocol = protocol;
    msg.sName = name;
    msg.sService = service;
    if (msg.Request()) {
      if (url == 0) {
        *url_len = 1;
        ok = 1;
      } else {
        if (*url_len > msg.sLocationUrl.bytes()) {
          memcpy(url, msg.sLocationUrl.c_str(), msg.sLocationUrl.bytes() + 1);
          ok = 1;
        }
      }
    }
  }

  return ok; 
}

//----------------------
// Fixme: OnClosed() should deliver data through a message, not direct pointer access !!!

class VpiClient: public HTTPClient
{
public:
  VpiClient(VpiModule* pModule)
    :HTTPClient("VpiClient") 
    ,nStatus_(0)
    ,bFailed_(0)
    ,bContentTypeOk_(0)
    ,pModule_(pModule)
  {}

  virtual int OnConnected();
  virtual int OnHeader(int nStatus, KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();

  int nStatus_;
  Buffer sbData_;
  List lHeader_;
  int bFailed_;

  String sContentType_;
  int bContentTypeOk_;

  VpiModule* pModule_;
};

int VpiClient::OnConnected()
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat "", _sz(sUrl_), ApHandlePrintf(hAp_)));
  return ok;
}

int VpiClient::OnHeader(int nStatus, KeyValueList& kvHeaders)
{
  int ok = 1;

  nStatus_ = nStatus;

  for (KeyValueElem* e = 0; (e = kvHeaders.nextElem(e)); ) {
    if (String::toLower(e->getKey()) == "content-type") {
      sContentType_ = String::toLower(e->getString());
    }
    apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat " header: [%s][%s]", _sz(sUrl_), ApHandlePrintf(hAp_), _sz(e->getKey()), _sz(e->getString())));
  }

  if (ok) {
    if (0) {
    } else if (sContentType_ == "text/xml") {
      bContentTypeOk_ = 1;
    } else if (sContentType_ == "application/xml") {
      bContentTypeOk_ = 1;
    } else if (sContentType_.startsWith("text/xml")) {
      bContentTypeOk_ = 1;
    } else if (sContentType_.startsWith("application/xml")) {
      bContentTypeOk_ = 1;
    } else if (sContentType_.contains("xml") != 0) {
      bContentTypeOk_ = 1;
    }
  }

  if (bContentTypeOk_) {
    sContentType_ = "text/xml";
  }
  
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat " bContentTypeOk_=%d", _sz(sUrl_), ApHandlePrintf(hAp_), bContentTypeOk_));
  
  return ok;
}

int VpiClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;
  
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat " nStatus=%d bContentTypeOk_=%d buflen=%d", _sz(sUrl_), ApHandlePrintf(hAp_), nStatus_, bContentTypeOk_, sbData_.Length() + nLen));
  
  if (nStatus_ == 200 && bContentTypeOk_ && sbData_.Length() + nLen < (unsigned int) Apollo::getModuleConfig(MODULE_NAME, "MaxVpiFileSize", 100000)) {
    sbData_.Append(pData, nLen);
  } else {
    Cancel();
  }

  return ok;
}

int VpiClient::OnFailed(const char* szMessage)
{
  int ok = 1;
  String sMessage = szMessage;
  Cancel();
  return ok;
}

int VpiClient::OnClosed()
{
  int ok = 1;

  if (nStatus_ == 200 && bContentTypeOk_ && !bCancelled_ && sbData_.Length() > 0) {
    if (pModule_ != 0) {
      apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat ": Writing valid data to file", _sz(sUrl_), ApHandlePrintf(hAp_)));
      pModule_->ReceiveFile(sUrl_, sContentType_, sbData_);
    }
  } else {
    if (pModule_ != 0) {
      apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat ": Writing empty data to file", _sz(sUrl_), ApHandlePrintf(hAp_)));
      String sEmptyString; Buffer sEmptyBuffer;
      pModule_->ReceiveFile(sUrl_, sEmptyString, sEmptyBuffer);
    }
  }

  AutoDelete(1);
  return ok;
}

int VpiModule::ReceiveFile(const String& sUrl, const String& sContentType, Buffer& sbData)
{
  int ok = 1;

  String sData;
  sbData.GetString(sData);

  if (sData.empty()) {
    // No data received, receive empty data
  } else {
    XMLProcessor xml;
    if (!xml.XmlText(sData)) {
      sData = String::truncate(sData, 100);
      sData.trim("\r\n");
      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "XML parser failed: url:%s, err:%s xml:%s", _sz(sUrl), _sz(xml.GetErrorString()), _sz(sData )));
      // Data broken, receive empty data
      sData = "";
    }
  }

  Msg_Vpi_ReceiveFile msg;
  msg.sUrl = sUrl;
  msg.sContentType = sContentType;
  msg.sData = sData;
  msg.Send();

  return ok; 
}

int VpiModule::RequestFile(const String& sUrl)
{
  int ok = 0;
/*
  int bDoRequest = 1;

  List* lFile = sRequestedFiles_.FindByName(sUrl);

  if (lFile == 0) {
    lFile = new List(sUrl);
    if (lFile != 0) {
      sRequestedFiles_.Add(e);
    }
  }
*/
  Msg_Vpi_RequestFile msg;
  msg.sUrl = sUrl;
  msg.Filter();

  VpiClient* pClient = new VpiClient(this);
  if (pClient != 0){
    ok = pClient->Get(msg.sUrl);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "VpiClient::Get(%s) failed", _sz(msg.sUrl)));
    }
  }

  return ok; 
}

//----------------------

#include "libvpi/libvpi_util_include.cpp"

//----------------------

void VpiModule::On_Vpi_Clear(Msg_Vpi_Clear* pMsg)
{
  int ok = vpi_cache_clear(pVpi_);
  { int ok = vpi_set_suffix(pVpi_, "");}

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetStatus(Msg_Vpi_GetStatus* pMsg)
{
  char** pStatus = 0;
  int ok = vpi_get_status(pVpi_, &pStatus);
  if (ok) {
    List lStatus;
    vpi_strlist_to_ListKeyValue(pStatus, lStatus);
    for (Elem* e = 0; (e = lStatus.Next(e)) != 0; ) { pMsg->kvStatus.add(e->getName(), e->getString()); }
    (void) vpi_free_list(pStatus);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetTraversedFiles(Msg_Vpi_GetTraversedFiles* pMsg)
{
  char** pFiles = 0;
  int ok = vpi_get_traversed_files(pVpi_, &pFiles);
  if (ok) {
    List lFiles;
    vpi_strlist_to_ListKey(pFiles, lFiles);
    for (Elem* e = 0; (e = lFiles.Next(e)) != 0; ) { pMsg->vlFiles.add(e->getName()); }
    (void) vpi_free_list(pFiles);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_LocationXmlRequest(Msg_Vpi_LocationXmlRequest* pMsg)
{
  (void) vpi_set_time(pVpi_, Apollo::getNow().Sec());

  char* location_xml = 0;
  int ok = vpi_get_location_xml(pVpi_, pMsg->sDocumentUrl, &location_xml);
  if (ok && location_xml != 0) {
    // If mapping works synchronously, because all data is available, then
    // generate an async message, to simulate the behaviour of an async response
    // This is just to prevent users of this message from assuming that the response comes synchronously

    apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "vpi_get_location_xml success, " ApHandleFormat " url:%s", ApHandlePrintf(pMsg->hRequest), _sz(pMsg->sDocumentUrl)));
    
    ApAsyncMessage<Msg_Vpi_LocationXmlResponse> msgVLXR;
    msgVLXR->sDocumentUrl = pMsg->sDocumentUrl;
    msgVLXR->hRequest = pMsg->hRequest;
    msgVLXR->sLocationXml = location_xml;
    msgVLXR->bSuccess = 1;
    msgVLXR.Post();

  } else {
    if (vpi_get_last_error(pVpi_) == VPI_ERROR_REQUESTED_DATA) {
      ok = 1;

      TimeValue tvNow = TimeValue::getTime();
      LocationTask* pTask = new LocationTask(pMsg->sDocumentUrl, pMsg->hRequest, tvNow);
      if (pTask != 0) {
        sLocationTasks_.AddLast(pTask);
      }
    }
  }

  if (location_xml != 0) {
    vpi_free(location_xml);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetLocationXml(Msg_Vpi_GetLocationXml* pMsg)
{
  char* location_xml = 0;
  int ok = vpi_get_location_xml(pVpi_, pMsg->sDocumentUrl, &location_xml);
  if (ok && location_xml != 0) {
    apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "vpi_get_location_xml success,  url:%s", _sz(pMsg->sDocumentUrl)));
    pMsg->sLocationXml = location_xml;
  }

  if (location_xml != 0) {
    vpi_free(location_xml);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_ReceiveFile(Msg_Vpi_ReceiveFile* pMsg)
{
  (void) vpi_set_time(pVpi_, Apollo::getNow().Sec());

  if (pMsg->sData.empty()) {
    (void) vpi_cache_add_nocontent(pVpi_, pMsg->sUrl);
  } else {
    if (pMsg->sContentType == "text/xml") {
      (void) vpi_cache_add_xml(pVpi_, pMsg->sUrl, pMsg->sData);
    } else {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Content-type:%s, not supported url:%s", _sz(pMsg->sContentType), _sz(pMsg->sUrl)));
    }
  }

  SafeListIteratorT<LocationTask> i(sLocationTasks_);
  for (LocationTask* pTask = 0; (pTask = i.Next()); ) {
    int bRemoveTask = 0;

    char* location_xml = 0;
    int ok = vpi_get_location_xml(pVpi_, pTask->sDocumentUrl_, &location_xml);
    if (ok && location_xml != 0) {
      bRemoveTask = 1;

      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "vpi_get_location_xml success, " ApHandleFormat " url:%s", ApHandlePrintf(pTask->hRequest_), _sz(pTask->sDocumentUrl_)));
      Msg_Vpi_LocationXmlResponse msg;
      msg.sDocumentUrl = pTask->sDocumentUrl_;
      msg.hRequest = pTask->hRequest_;
      msg.sLocationXml = location_xml;
      msg.bSuccess = 1;
      msg.Send();
    } else if (vpi_get_last_error(pVpi_) == VPI_ERROR_REQUESTED_DATA) {

      TimeValue tvNow = TimeValue::getTime();
      TimeValue tv100Sec(100, 0);
      if (tvNow - pTask->tStarted_ > tv100Sec) {
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "LocationTask exceeded max time: cancelled, " ApHandleFormat " url:%s", ApHandlePrintf(pTask->hRequest_), _sz(pTask->sDocumentUrl_)));
        Msg_Vpi_LocationXmlResponse msg;
        msg.sDocumentUrl = pTask->sDocumentUrl_;
        msg.hRequest = pTask->hRequest_;
        msg.bSuccess = 0;
        msg.Send();
      }

    } else {
      bRemoveTask = 1;

      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "vpi_get_location_xml returned error, err:%d, " ApHandleFormat " url:%s", vpi_get_last_error(pVpi_), ApHandlePrintf(pTask->hRequest_), _sz(pTask->sDocumentUrl_)));
      Msg_Vpi_LocationXmlResponse msg;
      msg.sDocumentUrl = pTask->sDocumentUrl_;
      msg.hRequest = pTask->hRequest_;
      msg.bSuccess = 0;
      msg.Send();
    }

    if (bRemoveTask) {
      sLocationTasks_.Remove(pTask);
      delete pTask;
      pTask = 0;
    }

    if (location_xml != 0) {
      vpi_free(location_xml);
    }

  }
}

void VpiModule::On_Vpi_GetDetailXml(Msg_Vpi_GetDetailXml* pMsg)
{
  char* detail_xml = 0;
  int ok = vpi_get_detail_xml(pVpi_, pMsg->sLocationXml, pMsg->sPath, (pMsg->bInnerXml ? VPI_GDX_INNERXML : 0), &detail_xml);
  if (ok) {
    pMsg->sXml = detail_xml;
  }

  if (detail_xml != 0) {
    vpi_free(detail_xml);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetLocationUrl(Msg_Vpi_GetLocationUrl* pMsg)
{
  char* location_url = 0;
  int ok = vpi_get_location_url(pVpi_, pMsg->sLocationXml, &location_url);
  if (ok && location_url != 0) {
    pMsg->sLocationUrl = location_url;
  }

  if (location_url != 0) {
    vpi_free(location_url);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_SetSuffix(Msg_Vpi_SetSuffix* pMsg)
{
  int ok = vpi_set_suffix(pVpi_, pMsg->sSuffix);
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetSuffix(Msg_Vpi_GetSuffix* pMsg)
{
  char* suffix = 0;
  int ok = vpi_get_suffix(pVpi_, &suffix);
  if (ok) {
    pMsg->sSuffix = suffix;
  }

  if (suffix != 0) {
    vpi_free(suffix);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetSelectOptions(Msg_Vpi_GetSelectOptions* pMsg)
{
  char** pOptions = 0;
  int ok = vpi_get_select_options(pVpi_, pMsg->sLocationXml, &pOptions);
  if (ok) {
    List lOptions;
    vpi_strlist_to_ListKey(pOptions, lOptions);
    for (Elem* e = 0; (e = lOptions.Next(e)) != 0; ) { pMsg->vlOptions.add(e->getName()); }
    (void) vpi_free_list(pOptions);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_GetSelectOptionProperties(Msg_Vpi_GetSelectOptionProperties* pMsg)
{
  char** pProperties = 0;
  int ok = vpi_get_select_option_properties(pVpi_, pMsg->sLocationXml, pMsg->sSuffix, &pProperties);
  if (ok) {
    List lProperties;
    vpi_strlist_to_ListKeyValue(pProperties, lProperties);
    for (Elem* e = 0; (e = lProperties.Next(e)) != 0; ) { pMsg->kvProperties.add(e->getName(), e->getString()); }
    (void) vpi_free_list(pProperties);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void VpiModule::On_Vpi_ComposeLocation(Msg_Vpi_ComposeLocation* pMsg)
{
  if (pMsg->sProtocol == "jabber" || pMsg->sProtocol == "xmpp") {
    // fix protocol
    String sProtocol = "xmpp";

    String sService = pMsg->sService;
    String sPart;
    sService.nextToken(":", sPart);

    pMsg->sLocationUrl.appendf("%s:%s@%s", _sz(sProtocol), _sz(pMsg->sName), _sz(sService));
    pMsg->apStatus = ApMessage::Ok;
    pMsg->Stop();
  }
}

#if defined(AP_TEST)

static String Test_libvpi()
{
  String s;

  vpi_context pVpi = vpi_new();
  if (pVpi != 0) {
    if (!vpi_run_tests(pVpi)) {
      unsigned int nLength = 10000;
      Flexbuf<char> fbError(nLength);
      if (vpi_get_last_error_string(pVpi, (char*) fbError, &nLength)) {
        s = (char*) fbError;
      } else {
        s = "failed to read error";
      }
      if (!s) {
        s = "vpi_run_tests failed";
      }
    }
    vpi_delete(pVpi);
    pVpi = 0;
  }

  return s;
}

//----------------------

void VpiModule::On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Vpi", 0)) {
    //AP_UNITTEST_REGISTER(Test_libvpi);
  }
}

void VpiModule::On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Vpi", 0)) {
    //AP_UNITTEST_EXECUTE(Test_libvpi);
  }
}

void VpiModule::On_UnitTest_End(Msg_UnitTest_End* pMsg)
{
  AP_UNUSED_ARG(pMsg);
}
#endif // #if defined(AP_TEST)

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_Clear)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetStatus)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetTraversedFiles)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_LocationXmlRequest)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetLocationXml)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_ReceiveFile)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetLocationUrl)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetDetailXml)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_SetSuffix)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetSuffix)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetSelectOptions)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_GetSelectOptionProperties)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, Vpi_ComposeLocation)
#if defined(AP_TEST)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, UnitTest_Begin)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, UnitTest_Execute)
AP_REFINSTANCE_MSG_HANDLER(VpiModule, UnitTest_End)
#endif

//----------------------------------------------------------

VPI_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  int ok = 1;

  if (!VpiModuleInstance::Get()->Init()) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Init() failed"));
  }

  if (ok) {
    { Msg_Vpi_Clear msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_Clear), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetStatus msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetStatus), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetTraversedFiles msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetTraversedFiles), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_LocationXmlRequest msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_LocationXmlRequest), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetLocationXml msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetLocationXml), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_ReceiveFile msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_ReceiveFile), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetDetailXml msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetDetailXml), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetLocationUrl msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetLocationUrl), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_SetSuffix msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_SetSuffix), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetSuffix msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetSuffix), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetSelectOptions msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetSelectOptions), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_GetSelectOptionProperties msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetSelectOptionProperties), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
    { Msg_Vpi_ComposeLocation msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_ComposeLocation), VpiModuleInstance::Get(), Apollo::modulePos(ApCallbackPosLate, MODULE_NAME)); } // ApCallbackPosLate because this is just the fallback
#if defined(AP_TEST)
    { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, UnitTest_Begin), VpiModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, UnitTest_Execute), VpiModuleInstance::Get(), ApCallbackPosNormal); }
    { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, UnitTest_End), VpiModuleInstance::Get(), ApCallbackPosNormal); }
#endif
  }

  return ok;
}

VPI_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  { Msg_Vpi_Clear msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_Clear), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetStatus msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetStatus), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetTraversedFiles msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetTraversedFiles), VpiModuleInstance::Get()); }
  { Msg_Vpi_LocationXmlRequest msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_LocationXmlRequest), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetLocationXml msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetLocationXml), VpiModuleInstance::Get()); }
  { Msg_Vpi_ReceiveFile msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_ReceiveFile), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetDetailXml msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetDetailXml), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetLocationUrl msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetLocationUrl), VpiModuleInstance::Get()); }
  { Msg_Vpi_SetSuffix msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_SetSuffix), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetSuffix msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetSuffix), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetSelectOptions msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetSelectOptions), VpiModuleInstance::Get()); }
  { Msg_Vpi_GetSelectOptionProperties msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_GetSelectOptionProperties), VpiModuleInstance::Get()); }
  { Msg_Vpi_ComposeLocation msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, Vpi_ComposeLocation), VpiModuleInstance::Get()); }
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, UnitTest_Begin), VpiModuleInstance::Get()); }
  { Msg_UnitTest_Execute msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, UnitTest_Execute), VpiModuleInstance::Get()); }
  { Msg_UnitTest_End msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(VpiModule, UnitTest_End), VpiModuleInstance::Get()); }
#endif

  VpiModuleInstance::Get()->Exit();
  VpiModuleInstance::Delete();

  return 1;
}
