// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(TcpConnection_h_INCLUDED)
#define TcpConnection_h_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"
#include "SrpcStreamParser.h"

class TcpConnection: public Apollo::TCPConnection
{
public:
  TcpConnection(const char* szName, const ApHandle& hConnection)
    :Apollo::TCPConnection(szName, hConnection)
  {}
  virtual ~TcpConnection() {}

protected:
  virtual int OnConnected();
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();

protected:
  Apollo::SrpcStreamParser parser_;
};

#endif // !defined(TcpConnection_h_INCLUDED)
