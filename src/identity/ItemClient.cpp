// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgIdentity.h"
#include "ItemClient.h"

int ItemClient::OnConnected()
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat, _sz(sUrl_), ApHandlePrintf(hAp_)));
  return ok;
}

int ItemClient::OnHeader(int nStatus, KeyValueList& kvHeaders)
{
  int ok = 1;

  nStatus_ = nStatus;

  for (KeyValueElem* e = 0; (e = kvHeaders.nextElem(e)); ) {
    if (String::toLower(e->getKey()) == "content-type") {
      sContentType_ = String::toLower(e->getString());
    }
    apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat " header: [%s][%s]", _sz(sUrl_), ApHandlePrintf(hAp_), (const char* )e->getKey(), _sz(e->getString())));
  }

  return ok;
}

int ItemClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "url=%s " ApHandleFormat " nStatus=%d buflen=%d", _sz(sUrl_), ApHandlePrintf(hAp_), nStatus_, sbData_.Length() + nLen));
  
  if (nStatus_ == 200 && sbData_.Length() + nLen < (unsigned int) Apollo::getModuleConfig(MODULE_NAME, "MaxIdentityItemFileSize", 100000)) {
    sbData_.Append(pData, nLen);
  } else {
    Cancel();
  }

  return ok;
}

int ItemClient::OnFailed(const char* szMessage)
{
  int ok = 1;
  String sMessage = szMessage;
  Cancel();
  return ok;
}

int ItemClient::OnClosed()
{
  int ok = 1;

  Msg_Identity_ReceiveItem msg;
  msg.hRequest = hAp_;
  msg.sUrl = sContainerUrl_;
  msg.sId = sId_;
  msg.sSrc = sUrl_;
  msg.sDigest = sDigest_;

  if (nStatus_ == 200 && !bCancelled_ && sbData_.Length() > 0) {
    msg.sMimeType = sContentType_;
    msg.sbData = sbData_;
  }

  if (!msg.Request()) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_ReceiveItem failed, url=%s ", _sz(sUrl_)));
  }

  AutoDelete(1);
  return ok;
}
