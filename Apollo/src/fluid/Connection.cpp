// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "Connection.h"
#include "FluidModule.h"

int Connection::OnConnected()
{
  apLog_Info((LOG_CHANNEL, "Connection::OnConnected", "Display connection " ApHandleFormat " opened", ApHandleType(hAp_)));
  return 1;
}

int Connection::OnDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  ok = parser_.parse(pData, nLen);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "Connection::OnDataIn", "parser_.parse() failed"));
  } else {

    for (Apollo::SrpcMessage* pMessage = 0; (pMessage = parser_.getNextMessage()) != 0; ) {
      Msg_Fluid_Receive msg;
      msg.hConnection = apHandle();
      *pMessage >> msg.srpc;
      int ok = msg.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "Connection::OnDataIn", "Msg_Fluid_Receive() failed"));
      }

      delete pMessage;
      pMessage = 0;
    }
  }

  return 1;
}

int Connection::OnClosed()
{
  AutoDelete(1);
  apLog_Info((LOG_CHANNEL, "Connection::OnClosed", "Display connection " ApHandleFormat " closed", ApHandleType(hAp_)));

  FluidModuleInstance::Get()->removeConnection(apHandle());

  // Connection deletes itself
  return 1;
}
