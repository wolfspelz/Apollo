// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "TcpConnection.h"
#include "ServerModule.h"

int TcpConnection::OnConnected()
{
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Navigation connection " ApHandleFormat " opened", ApHandlePrintf(hAp_)));

  Msg_TcpServer_Connected msg;
  msg.hConnection = apHandle();
  msg.Send();

  return 1;
}

int TcpConnection::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  ok = parser_.parse(pData, nLen);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "parser_.parse() failed"));
  } else {

    for (Apollo::SrpcMessage* pMessage = 0; (pMessage = parser_.getNextMessage()) != 0; ) {
      Msg_TcpServer_SrpcRequest msg;
      msg.hConnection = apHandle();
      *pMessage >> msg.srpc;

      if (apLog_IsVerbose) {
        String sMsg = msg.srpc.toString();
        apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " receive: %s", ApHandlePrintf(hAp_), _sz(sMsg)));
      }

      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_TcpServer_SrpcRequest() failed conn=" ApHandleFormat "", ApHandlePrintf(msg.hConnection)));
      } else {

        // If a response was provided, then send it
        if (msg.response.length() > 0) {
          Msg_TcpServer_SendSrpc msgTSSS;
          msg.response >> msgTSSS.srpc;
          msgTSSS.hConnection = hAp_;
          if (!msgTSSS.Request()) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed conn=", _sz(msgTSSS.Type()), ApHandlePrintf(msgTSSS.hConnection)));
          }
        }
      }

      delete pMessage;
      pMessage = 0;
    }

  }

  return 1;
}

int TcpConnection::OnClosed()
{
  AutoDelete(1);
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Navigation connection " ApHandleFormat " closed", ApHandlePrintf(hAp_)));

  Msg_TcpServer_Disconnected msg;
  msg.hConnection = apHandle();
  msg.Send();

  ServerModuleInstance::Get()->removeTcpConnection(apHandle());

  // TcpConnection deletes itself
  return 1;
}
