// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Connection.h"
#include "Client.h"

Connection::Connection(Client* pClient)
:TCPConnection("XMPP")
,pClient_(pClient)
{
}

Connection::~Connection()
{
}

int Connection::OnConnected()
{
  if (pClient_ != 0) {
    pClient_->onConnectionConnected();
  }
  return 1;
}

int Connection::OnDataIn(unsigned char* pData, size_t nLen)
{
  if (pClient_ != 0) {
    pClient_->onConnectionDataIn(pData, nLen);
  }
  return 1;
}

int Connection::OnClosed()
{
  if (pClient_ != 0) {
    pClient_->onConnectionDisconnected();
  }
  AutoDelete(1);
  return 1;
}
