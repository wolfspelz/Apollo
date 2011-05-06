// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgIdentity.h"
#include "Local.h"
#include "SrpcClient.h"

int SrpcClient::Post(const String& sUrl, SrpcMessage& srpc)
{
  String sBody = srpc.toString();

  return HTTPClient::Post(sUrl, (const unsigned char*) (const char*) sBody, sBody.bytes());
}

int SrpcClient::OnConnected()
{
  int ok = 1;
  apLog_Verbose((MODULE_NAME, "SrpcClient::OnConnected", "url=%s " ApHandleFormat "", StringType(sUrl_), ApHandleType(hAp_)));
  return ok;
}

int SrpcClient::OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders)
{
  int ok = 1;

  nStatus_ = nStatus;

  for (Apollo::KeyValueElem* e = 0; (e = kvHeaders.nextElem(e)); ) {
    if (String::toLower(e->getKey()) == "content-type") {
      sContentType_ = String::toLower(e->getString());
    }
    apLog_VeryVerbose((MODULE_NAME, "SrpcClient::OnHeader", "url=%s " ApHandleFormat " header: [%s][%s]", StringType(sUrl_), ApHandleType(hAp_), StringType(e->getKey()), StringType(e->getString())));
  }

  if (sContentType_.contains("text/plain")) {
    bContentTypeOk_ = 1;
  }

  apLog_Verbose((MODULE_NAME, "SrpcClient::OnHeader", "url=%s " ApHandleFormat " sContentType_=%s", StringType(sUrl_), ApHandleType(hAp_), StringType(sContentType_)));
  
  return ok;
}

int SrpcClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;
  
  apLog_VeryVerbose((MODULE_NAME, "SrpcClient::OnDataIn", "url=%s " ApHandleFormat " nStatus=%d sContentType_=%s buflen=%d", StringType(sUrl_), ApHandleType(hAp_), nStatus_, StringType(sContentType_), sbData_.Length() + nLen));
  
  if (nStatus_ == 200 && bContentTypeOk_ && sbData_.Length() + nLen < (unsigned int) Apollo::getModuleConfig(MODULE_NAME, "Srpc/MaxDataSize", 30000)) {
    sbData_.Append(pData, nLen);
  } else {
    Cancel();
  }

  return ok;
}

int SrpcClient::OnFailed(const char* szMessage)
{
  int ok = 1;
  sError_ = szMessage;
  Cancel();
  return ok;
}

int SrpcClient::OnClosed()
{
  int ok = 1;

  apLog_Verbose((MODULE_NAME, "SrpcClient::OnClosed", "url=%s " ApHandleFormat "", StringType(sUrl_), ApHandleType(hAp_)));

  int bSuccess = 0;

  if (nStatus_ == 200 && bContentTypeOk_ && !bCancelled_ && sbData_.Length() > 0) {
    String sData;
    sbData_.GetString(sData);

    Apollo::SrpcMessage srpc;
    srpc.fromString(sData);

    if (srpc.length() == 0) {
      sError_ = "Login request returned no SRPC data";
    } else {
      int nStatus = srpc.getInt("Status");
      if (nStatus == 0) {
        sError_ = "The server returned: " + srpc.getString("Message");
      } else {
        bSuccess = 1;
        OnResult(srpc);
      }
    }
  } else {
    sError_.appendf("HTTP status=%d content-type=%s cancelled= data-length=%d", nStatus_, StringType(sContentType_), bCancelled_, sbData_.Length());
  }

  if (!bSuccess) {
    OnError(sError_);
  }

  AutoDelete(1);
  return ok;
}
