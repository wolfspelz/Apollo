// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgIdentity.h"
#include "Local.h"
#include "ContainerClient.h"

int ContainerClient::OnConnected()
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, "ContainerClient::OnConnected", "url=%s " ApHandleFormat, StringType(sUrl_), ApHandleType(hAp_)));
  return ok;
}

int ContainerClient::OnHeader(int nStatus, KeyValueList& kvHeaders)
{
  int ok = 1;

  nStatus_ = nStatus;

  for (KeyValueElem* e = 0; (e = kvHeaders.nextElem(e)); ) {
    if (String::toLower(e->getKey()) == "content-type") {
      sContentType_ = String::toLower(e->getString());
    }
    apLog_VeryVerbose((LOG_CHANNEL, "ContainerClient::OnHeader", "url=%s " ApHandleFormat " header: [%s][%s]", StringType(sUrl_), ApHandleType(hAp_), StringType(e->getKey()), StringType(e->getString())));
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
  
  apLog_VeryVerbose((LOG_CHANNEL, "ContainerClient::OnHeader", "url=%s " ApHandleFormat " bContentTypeOk_=%d", StringType(sUrl_), ApHandleType(hAp_), bContentTypeOk_));

  return ok;
}

int ContainerClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  apLog_VeryVerbose((LOG_CHANNEL, "ContainerClient::OnDataIn", "" ApHandleFormat " url=%s  nStatus=%d bContentTypeOk_=%d buflen=%d", ApHandleType(hAp_), StringType(sUrl_), nStatus_, bContentTypeOk_, sbData_.Length() + nLen));
  
  if (nStatus_ == 200 && bContentTypeOk_ && sbData_.Length() + nLen < (unsigned int) Apollo::getModuleConfig(MODULE_NAME, "MaxIdentityFileSize", 100000)) {
    sbData_.Append(pData, nLen);
  } else {
    Cancel();
  }

  return ok;
}

int ContainerClient::OnFailed(const char* szMessage)
{
  int ok = 1;
  sError_ = szMessage;
  Cancel();
  return ok;
}

int ContainerClient::OnClosed()
{
  int ok = 1;

  Msg_Identity_ReceiveContainer msg;
  msg.hRequest = hAp_;
  msg.sUrl = sUrl_;
  msg.sDigest = sDigest_;

  if (nStatus_ == 200 && bContentTypeOk_ && !bCancelled_ && sbData_.Length() > 0) {
    msg.sMimeType = sContentType_;
    msg.sbData = sbData_;
  }

  if (sError_.empty()) {
    msg.sComment.appendf("url=%s status=%d contenttype=%s buflen=%d", StringType(sUrl_), nStatus_, StringType(sContentType_), sbData_.Length());
  } else {
    msg.sComment = sError_;
  }

  if (!msg.Request()) {
    apLog_Warning((LOG_CHANNEL, "ContainerClient::OnClosed", "Msg_Identity_ReceiveContainer failed, url=%s ", StringType(sUrl_)));
  }

  AutoDelete(1);
  return ok;
}
