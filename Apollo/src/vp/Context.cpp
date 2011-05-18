// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgDB.h"
#include "MsgVpi.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "URL.h"
#include "Local.h"
#include "Context.h"

Context::Context(const ApHandle& hContext)
:hAp_(hContext)
,tCreated_(Apollo::getNow().Sec())
,hMapping_(ApNoHandle)
{
  lThingys_.setContext(this);
  (void) lThingys_.getOrCreate(Msg_VpView_ContextDetail_DocumentUrl);
  (void) lThingys_.getOrCreate(Msg_VpView_ContextDetail_LocationUrl);
  (void) lThingys_.getOrCreate(Msg_VpView_ContextDetail_Visibility);
  (void) lThingys_.getOrCreate(Msg_VpView_ContextDetail_Position);
  (void) lThingys_.getOrCreate(Msg_VpView_ContextDetail_Size);
}

Context::~Context()
{
}

//---------------------------

int Context::navigate(const String& sUrl)
{
  int ok = 1;

  if (sDocumentUrl_ != sUrl) {
    lThingys_.beginChanges();
    sDocumentUrl_ = sUrl;
    lThingys_.setChanged(Msg_VpView_ContextDetail_DocumentUrl);
    lThingys_.endChanges();

    ok = resolve();
  }

  return ok;
}

int Context::resolve()
{
  int ok = 1;

  hMapping_ = Apollo::newHandle();

  Msg_Vpi_LocationXmlRequest msg;
  msg.sDocumentUrl = sDocumentUrl_;
  msg.hRequest = hMapping_;
  if (!msg.Request()) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, "Context::map", "Failed to map %s for display " ApHandleFormat "", StringType(sDocumentUrl_), ApHandleType(hAp_)));
    hMapping_ = ApNoHandle;
  }

  return ok;
}

int Context::setLocationXml(const String& sLocationXml)
{
  int ok = 1;

  sLocationXml_ = sLocationXml;
  ok = resolved();

  return ok;
}

int Context::resolved()
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "Context::resolved", "" ApHandleFormat " for %s", ApHandleType(apHandle()), StringType(sDocumentUrl_)));

  String sZone;

  // Is there a "zone"?
  if (ok) {
    Msg_Vpi_GetDetailXml msg;
    msg.sLocationXml = sLocationXml_;
    msg.sPath = "zone";
    msg.bInnerXml = 1;
    if (!msg.Request()) {
      apLog_Verbose((LOG_CHANNEL, "Context::resolved", "Msg_Vpi_GetDetailXml(%s) failed: %s for %s", StringType(msg.sPath), StringType(msg.sComment), StringType(sDocumentUrl_)));
    } else {
      sZone = msg.sXml;
    }
  }

  // Or at least a "destination"?
  if (ok && sZone.empty()) {
    Msg_Vpi_GetDetailXml msg;
    msg.sLocationXml = sLocationXml_;
    msg.sPath = "destination";
    msg.bInnerXml = 1;
    if (!msg.Request()) {
      apLog_Verbose((LOG_CHANNEL, "Context::resolved", "Msg_Vpi_GetDetailXml(%s) failed: %s for %s", StringType(msg.sPath), StringType(msg.sComment), StringType(sDocumentUrl_)));
    } else {
      sZone = msg.sXml;
    }
  }

  // Otherwise use the base URL as zone name
  if (ok && sZone.empty()) {
    Apollo::URL uUrl = sDocumentUrl_;
    sZone = uUrl.base();
  }

  String sSuffix;
  if (ok) {
    Msg_DB_Get msg;
    String sEscapedZone = sZone;
    sEscapedZone.escape(String::EscapeSlash);
    msg.sName = DB_NAME;
    msg.sKey.appendf("zone/%s/suffix", StringType(sEscapedZone));
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Context::resolved", "Msg_DB_Get failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
    } else {
      sSuffix = msg.sValue;
    }
  }

  String sOldSuffix;
  int bOldSuffixSaved = 0;
  if (ok && !sSuffix.empty()) {
    Msg_Vpi_GetSuffix msg;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Context::resolved", "Msg_Vpi_GetSuffix"));
    } else {
      sOldSuffix = msg.sSuffix;
      bOldSuffixSaved = 1;
    }
  }

  if (ok && !sSuffix.empty()) {
    Msg_Vpi_SetSuffix msg;
    msg.sSuffix = sSuffix;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Context::resolved", "Msg_Vpi_SetSuffix failed: suffix=%s", StringType(msg.sSuffix)));
    }
  }

  if (ok) {
    Msg_Vpi_GetLocationUrl msg;
    msg.sLocationXml = sLocationXml_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Context::resolved", "Msg_Vpi_GetLocationUrl failed: %s for %s", StringType(msg.sComment), StringType(sDocumentUrl_)));
    } else {      
      if (sLocationUrl_ == msg.sLocationUrl) {
        ok = sameLocation(msg.sLocationUrl);
      } else {
        ok = changeLocation(msg.sLocationUrl);
      }
    }
  }

  if (bOldSuffixSaved) {
    Msg_Vpi_SetSuffix msg;
    msg.sSuffix = sOldSuffix;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Context::resolved", "Msg_Vpi_SetSuffix failed: suffix=%s", StringType(msg.sSuffix)));
    }
  }

  return ok;
}

int Context::sameLocation(const String& sLocationUrl)
{
  int ok = 1;
  apLog_Info((LOG_CHANNEL, "Context::sameLocation", "%s -> %s", StringType(sDocumentUrl_), StringType(sLocationUrl)));

  return ok;
}

int Context::changeLocation(const String& sLocationUrl)
{
  int ok = 1;
  apLog_Info((LOG_CHANNEL, "Context::changeLocation", "%s -> %s", StringType(sDocumentUrl_), StringType(sLocationUrl)));

  lThingys_.beginChanges();
  sLocationUrl_ = sLocationUrl;
  lThingys_.setChanged(Msg_VpView_ContextDetail_LocationUrl);
  lThingys_.endChanges();

  return ok;
}

//----------------------------------------------------------

void Context::setVisibility(int bVisible)
{
  lThingys_.beginChanges();

  ContextThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ContextDetail_Visibility);
  if (pThingy) {
    VisibilityContextThingyProvider* pProvider = (VisibilityContextThingyProvider*) pThingy->getProvider();
    if (pProvider) {
      pProvider->setData(bVisible);
    }
  }

  lThingys_.endChanges();
}

void Context::setPosition(int nLeft, int nBottom)
{
  lThingys_.beginChanges();

  ContextThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ContextDetail_Position);
  if (pThingy) {
    KeyValueContextThingyProvider* pProvider = (KeyValueContextThingyProvider*) pThingy->getProvider();
    if (pProvider) {
      Apollo::KeyValueList kvData;
      kvData.add(Msg_VpView_ContextDetail_Position_Left, nLeft);
      kvData.add(Msg_VpView_ContextDetail_Position_Bottom, nBottom);
      pProvider->setData(kvData);
    }
  }

  lThingys_.endChanges();
}

void Context::setSize(int nWidth, int nHeight)
{
  lThingys_.beginChanges();

  ContextThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ContextDetail_Size);
  if (pThingy) {
    KeyValueContextThingyProvider* pProvider = (KeyValueContextThingyProvider*) pThingy->getProvider();
    if (pProvider) {
      Apollo::KeyValueList kvData;
      kvData.add(Msg_VpView_ContextDetail_Size_Width, nWidth);
      kvData.add(Msg_VpView_ContextDetail_Size_Height, nHeight);
      pProvider->setData(kvData);
    }
  }

  lThingys_.endChanges();
}

//----------------------------------------------------------

// @throws ApException
void Context::getDetailString(const String& sKey, String& sValue, String& sMimeType)
{
  ContextThingy* pThingy = lThingys_.getOrCreate(sKey);
  if (!pThingy) {
    throw ApException("Context::getDetailString: unknown key=%s", StringType(sKey));
  } else {
    ContextThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (sKey == Msg_VpView_ContextDetail_Visibility) {
        int xx = 1;
      }
      pProvider->getString(sValue, sMimeType);
    }
  }
}

int Context::subscribeDetail(const String& sKey, String& sValue, String& sMimeType)
{
  int ok = 0;

  ok = addSubscription(sKey);

  (void) getDetailString(sKey, sValue, sMimeType);

  return ok;
}

int Context::unsubscribeDetail(const String& sKey)
{
  int ok = 1;

  ok = removeSubscription(sKey);

  return ok;
}

int Context::addSubscription(const String& sKey)
{
  int ok = 1;

  Thingy* pThingy = lThingys_.getOrCreate(sKey);
  if (pThingy != 0) {
    pThingy->addSubscription();
  }

  return ok;
}

int Context::removeSubscription(const String& sKey)
{
  int ok = 1;

  Thingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy != 0) {
    pThingy->removeSubscription();
  }

  return ok;
}

//----------------------------------------------------------

void BoolContextThingyProvider::setData(int bValue)
{
  bHasData_ = 1;
  bValue_ = bValue;
}

void BoolContextThingyProvider::getString(String& sValue, String& sMimeType)
{
  sValue = (bValue_ ? "1" : "0");
}

//------------------------------

void VisibilityContextThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (!bHasData_) {
    if (pContext_) {
      Msg_VpView_GetContextVisibility msg;
      msg.hContext = pContext_->apHandle();
      if (msg.Request()) {
        setData(msg.bVisible);
      }
    }
  }

  BoolContextThingyProvider::getString(sValue, sMimeType);
}

//----------------------------------------------------------

void KeyValueContextThingyProvider::setData(Apollo::KeyValueList& kvList)
{
  int bChanged = 0;
  bHasData_ = 1;

  if (kvList_.length() != kvList.length()) {
    bChanged = 1;
  }

  if (!bChanged) {
    Apollo::KeyValueElem* f = 0;
    for (Apollo::KeyValueElem* e = 0; (e = kvList.nextElem(e)) != 0; ) {
      f = kvList_.nextElem(f);
      if (!f) {
        bChanged = 1;
        break;
      } else {
        if (e->getKey() != f->getKey()) {
          bChanged = 1;
          break;
        }
        if (e->getString() != f->getString()) {
          bChanged = 1;
          break;
        }
      }
    }
  }

  kvList_ = kvList;

  if (bChanged) {
    if (pThingy_) { pThingy_->setChanged(); }
  }
}

void KeyValueContextThingyProvider::getString(String& sValue, String& sMimeType)
{
  sValue = kvList_.toString();
  sMimeType = "apollo/KeyValueList";
}

//---------------------------

void DocumentUrlContextThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (pContext_) {
    sValue = pContext_->getDocumentUrl();
  }
}

void LocationUrlContextThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (pContext_) {
    sValue = pContext_->getLocationUrl();
  }
}

//----------------------------

ThingyProvider* ContextThingyList::newProvider(const String& sKey)
{
  ContextThingyProvider* pProvider = 0;

  if (0) {
  } else if (sKey == Msg_VpView_ContextDetail_DocumentUrl) { pProvider = new DocumentUrlContextThingyProvider();
  } else if (sKey == Msg_VpView_ContextDetail_LocationUrl) { pProvider = new LocationUrlContextThingyProvider();
  } else if (sKey == Msg_VpView_ContextDetail_Visibility) { pProvider = new VisibilityContextThingyProvider();
  } else if (sKey == Msg_VpView_ContextDetail_Position) { pProvider = new KeyValueContextThingyProvider();
  } else if (sKey == Msg_VpView_ContextDetail_Size) { pProvider = new KeyValueContextThingyProvider();
  } else {
    apLog_Error((LOG_CHANNEL, "ContextThingyList::newThingy", "unknown key=%s", StringType(sKey)));
  }

  if (pProvider) {
    pProvider->setContext(pContext_);
  }

  return pProvider;
}

void ContextThingyList::sendChanges(Apollo::ValueList& vlChanges)
{
  Msg_VpView_ContextDetailsChanged msg;
  msg.hContext = pContext_->apHandle();
  msg.vlKeys = vlChanges;
  msg.Send();
}
