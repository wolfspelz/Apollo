// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgAnimation.h"
#include "Local.h"
#include "AnimationClient.h"

int AnimationClient::OnConnected()
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, "AnimationClient::OnConnected", "url=%s " ApHandleFormat, StringType(sUrl_), ApHandleType(hAp_)));
  return ok;
}

int AnimationClient::OnHeader(int nStatus, KeyValueList& kvHeaders)
{
  int ok = 1;

  nStatus_ = nStatus;

  for (KeyValueElem* e = 0; (e = kvHeaders.nextElem(e)); ) {
    if (String::toLower(e->getKey()) == "content-type") {
      sContentType_ = String::toLower(e->getString());
    }
    apLog_VeryVerbose((LOG_CHANNEL, "AnimationClient::OnHeader", "url=%s " ApHandleFormat " header: [%s][%s]", StringType(sUrl_), ApHandleType(hAp_), (const char* )e->getKey(), StringType(e->getString())));
  }

  return ok;
}

int AnimationClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  apLog_VeryVerbose((LOG_CHANNEL, "AnimationClient::OnDataIn", "url=%s " ApHandleFormat " nStatus=%d buflen=%d", StringType(sUrl_), ApHandleType(hAp_), nStatus_, sbData_.Length() + nLen));
  
  if (nStatus_ == 200 && sbData_.Length() + nLen < (unsigned int) Apollo::getModuleConfig(MODULE_NAME, "MaxAnimationFileSize", 200000)) {
    sbData_.Append(pData, nLen);
  } else {
    Cancel();
  }

  return ok;
}

int AnimationClient::OnFailed(const char* szMessage)
{
  int ok = 1;
  String sMessage = szMessage;
  Cancel();
  return ok;
}

int AnimationClient::OnClosed()
{
  int ok = 1;

  int bSuccess = 0;
  if (nStatus_ == 200 && !bCancelled_ && sbData_.Length() > 0) {
    bSuccess = 1;
  }

  {
    Msg_Animator_RequestAnimationComplete msg;
    msg.hRequest = hRequest_;
    msg.bSuccess = bSuccess;
    msg.sUrl = sUrl_;
    msg.sbData = sbData_;
    msg.sMimeType = sContentType_;
    msg.Send();
  }

  AutoDelete(1);
  return ok;
}
