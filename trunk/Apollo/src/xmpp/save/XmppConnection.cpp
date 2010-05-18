// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"

#include "Local.h"
#include "XmppConnection.h"

XmppConnection::XmppConnection(XmppProtocol& protocol)
:TCPConnection("Xmpp")
,protocol_(protocol)
{
}

XmppConnection::~XmppConnection()
{
}

int XmppConnection::OnConnected()
{
  return protocol_.OnConnected();
}

int XmppConnection::OnDataIn(unsigned char* pData, size_t nLen)
{
  return protocol_.OnDataIn(pData, nLen);
}

int XmppConnection::OnClosed()
{
  AutoDelete(1);
  return protocol_.OnClosed();
}
