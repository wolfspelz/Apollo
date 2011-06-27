// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "TcpConnection.h"
#include "ServerModule.h"

int TcpConnection::OnConnected()
{
  apLog_Info((LOG_CHANNEL, "TcpConnection::OnConnected", "Navigation connection " ApHandleFormat " opened", ApHandleType(hAp_)));

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
    apLog_Error((LOG_CHANNEL, "TcpConnection::OnDataIn", "parser_.parse() failed"));
  } else {

    for (Apollo::SrpcMessage* pMessage = 0; (pMessage = parser_.getNextMessage()) != 0; ) {
      Msg_TcpServer_SrpcRequest msg;
      msg.hConnection = apHandle();
      *pMessage >> msg.srpc;
      int ok = msg.Call();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "TcpConnection::OnDataIn", "Msg_TcpServer_SrpcRequest() failed conn=" ApHandleFormat "", ApHandleType(msg.hConnection)));
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
  apLog_Info((LOG_CHANNEL, "TcpConnection::OnClosed", "Navigation connection " ApHandleFormat " closed", ApHandleType(hAp_)));

  Msg_TcpServer_Disconnected msg;
  msg.hConnection = apHandle();
  msg.Send();

  ServerModuleInstance::Get()->removeTcpConnection(apHandle());

  // TcpConnection deletes itself
  return 1;
}
