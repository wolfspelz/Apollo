// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "ServerModule.h"
#include "HttpConnection.h"

int HttpConnection::OnConnected()
{
  apLog_VeryVerbose((LOG_CHANNEL, "HttpConnection::OnConnected", "Display HttpConnection " ApHandleFormat " opened", ApHandleType(hAp_)));
  return 1;
}

int HttpConnection::OnDataIn(unsigned char* pData, size_t nLen) throw()
{
  int ok = 1;

  parser_.parse(pData, nLen);
  while (parser_.hasRequest()) {

    Msg_Server_HttpRequest msg;
    msg.hConnection = apHandle();
    msg.sMethod = parser_.method();
    msg.sUri = parser_.uri();
    msg.sProtocol = parser_.protocol();
    msg.sRemoteAddress = sAddress_;

    apLog_Verbose((LOG_CHANNEL, "HttpConnection::OnDataIn", "conn=" ApHandleFormat " method=%s uri=%s", ApHandleType(msg.hConnection), StringType(msg.sMethod), StringType(msg.sUri)));

    {
      List lHeaders;
      parser_.getHeaders(lHeaders);
      for (Elem* e = 0; (e = lHeaders.Next(e)) != 0; ) {
        msg.kvHeader.add(e->getName(), e->getString());
      }
    }

    parser_.getBody(msg.sbBody);

    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "HttpConnection::OnDataIn", "Msg_Server_HttpRequest failed conn=" ApHandleFormat " method=%s uri=%s body=%d", ApHandleType(msg.hConnection), StringType(msg.sMethod), StringType(msg.sUri), msg.sbBody.Length()));
    }

    parser_.skipRequest();
  }

  return 1;
}

int HttpConnection::OnClosed()
{
  AutoDelete(1);
  apLog_VeryVerbose((LOG_CHANNEL, "HttpConnection::OnClosed", "Display HttpConnection " ApHandleFormat " closed", ApHandleType(hAp_)));

  ServerModuleInstance::Get()->removeHttpConnection(apHandle());

  return 1;
}
