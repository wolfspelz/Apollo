// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgIdentity.h"
#include "Local.h"
#include "Item.h"
#include "Container.h"

int Item::parseXmlNode(XMLNode* pNode)
{
  int ok = 1;

  String sPreviousDigest = getAttribute("digest");
  lAttributes_.Empty();

  XMLAttrList& attributes = pNode->getAttributes();
  for (XMLAttr* pAttr = 0; (pAttr = attributes.nextAttribute(pAttr)) != 0; ) {
    lAttributes_.AddLast(pAttr->getKey(), pAttr->getValue());
  }

  // Some checks
  if (getAttribute("id").empty() || getAttribute("digest").empty() || getAttribute("type").empty() && (getAttribute("src").empty() && sbData_.Length() == 0)) {
    ok = 0;
    apLog_Warning((LOG_CHANNEL, "Item::parseXmlNode", "id=%s invalid: digest=%s type=%s src=%s data-len=%d", StringType(getAttribute("id")), StringType(getAttribute("digest")), StringType(getAttribute("type")), StringType(getAttribute("src")), sbData_.Length()));
  } else {

    if (getAttribute("src").empty()) {
      // Inline data
      bDataAvailable_ = 1;
      String sCData = pNode->getCData();
      if (sCData.empty()) {
        sbData_.Empty();
      } else {
        if (getAttribute("encoding") == "base64") {
          Buffer sbTmp; sbTmp.decodeBase64(sCData);
          setData(getAttribute("mimetype"), sbTmp);
        } else {
          Buffer sbTmp; sbTmp.SetData(sCData);
          setData(getAttribute("mimetype"), sbTmp);
        }
      }
    } else {
      // External data
      if (!sPreviousDigest.empty() && sPreviousDigest != getAttribute("digest")) {
        deleteData();
      }
    }

  } // checks

  return ok;
}

int Item::getAttributes(List& lAttributes)
{
  lAttributes = lAttributes_;
  return 1;
}

String Item::getAttribute(const String& sKey)
{
  String sValue;

  if (sKey == "type" || sKey == "contenttype") {
    sValue = lAttributes_["type"].getString();
    if (sValue.empty()) {
      sValue = lAttributes_["contenttype"].getString();
    }
    if (sValue == "avatar2") {
      sValue = "avatar";
    }
  } else {
    sValue = lAttributes_[sKey].getString();
  }

  return sValue;
}

void Item::setAttribute(const String& sKey, const String& sValue)
{
  lAttributes_[sKey].setString(sValue);
}

int Item::setData(const String& sMimeType, Buffer& sbData)
{
  int ok = 1;

  int bDifferent = 0;

  if (sbData_.Length() != sbData.Length()) {
    bDifferent = 1;
  } else {
    unsigned char* p = sbData_.Data();
    unsigned char* q = sbData.Data();
    unsigned int n = sbData.Length();
    unsigned int i = 0;
    for (int i = 0; i < n && !bDifferent; i++) {
      if (*p++ != *q++) {
        bDifferent = 1;
      }
    }
  }

  if (bDifferent) {
    bDataAvailable_ = 1;
    sbData_ = sbData;
    if (lAttributes_["mimetype"].getString().empty()) {
      lAttributes_["mimetype"].setString(sMimeType);
    }

    if (pContainer_ != 0) {
      // Async to that the data is already in the cache for requests by msg receivers
      ApAsyncMessage<Msg_Identity_ItemDataAvailable> msg;
      msg->sUrl = pContainer_->getUrl();
      msg->sId = getId();
      msg->sDigest = getAttribute("digest");
      msg->sType = getAttribute("type");
      msg->sMimeType = getAttribute("mimetype");
      msg->nOrder = String::atoi(getAttribute("order"));
      msg->nSize = sbData.Length();
      msg->sSrc = getAttribute("src");
      //msg->sTrust;
      msg.Post();
    } // pContainer_
  } // bChanged

  return ok;
}

int Item::getData(String& sData)
{
  int ok = 0;

  if (bDataAvailable_) {
    sbData_.GetString(sData);
    ok = 1;
  }

  return ok;
}

int Item::getData(Buffer& sbData)
{
  int ok = 0;

  if (bDataAvailable_) {
    sbData = sbData_;
    ok = 1;
  }

  return ok;
}

int Item::deleteData()
{
  int ok = 1;

  sbData_.Empty();
  bDataAvailable_ = 0;

  return ok;
}
