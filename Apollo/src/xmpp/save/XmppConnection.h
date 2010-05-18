// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XmppConnection_h_INCLUDED)
#define XmppConnection_h_INCLUDED

#include "Apollo.h"

#include "XmppProtocol.h"

class XmppConnection : public Apollo::TCPConnection
{
public:
  XmppConnection(XmppProtocol& oProtocol);
  virtual ~XmppConnection();

  int OnConnected();
  int OnDataIn(unsigned char* pData, size_t nLen);
  int OnClosed();

protected:
  XmppProtocol& protocol_;
};

#endif // !defined(XmppConnection_h_INCLUDED)
