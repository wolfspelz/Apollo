// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ServerModule.h"
#include "HttpConnection.h"

int HttpConnection::OnConnected()
{
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "Display HttpConnection " ApHandleFormat " opened", ApHandlePrintf(hAp_)));
  return 1;
}

int HttpConnection::OnDataIn(unsigned char* pData, size_t nLen) throw()
{
  int ok = 1;

  parser_.parse(pData, nLen);
  while (parser_.hasRequest()) {

    Msg_HttpServer_Request msg;
    msg.hConnection = apHandle();
    msg.sMethod = parser_.method();
    msg.sUri = parser_.uri();
    msg.sProtocol = parser_.protocol();
    msg.sRemoteAddress = sAddress_;

    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " method=%s uri=%s", ApHandlePrintf(msg.hConnection), _sz(msg.sMethod), _sz(msg.sUri)));

    {
      List lHeaders;
      parser_.getHeaders(lHeaders);
      for (Elem* e = 0; (e = lHeaders.Next(e)) != 0; ) {
        msg.kvHeader.add(e->getName(), e->getString());
      }
    }

    parser_.getBody(msg.sbBody);

    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_HttpServer_Request failed conn=" ApHandleFormat " method=%s uri=%s body=%d", ApHandlePrintf(msg.hConnection), _sz(msg.sMethod), _sz(msg.sUri), msg.sbBody.Length()));
    }

    parser_.skipRequest();
  }

  return 1;
}

int HttpConnection::OnClosed()
{
  AutoDelete(1);
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "Display HttpConnection " ApHandleFormat " closed", ApHandlePrintf(hAp_)));

  ServerModuleInstance::Get()->removeHttpConnection(apHandle());

  return 1;
}
