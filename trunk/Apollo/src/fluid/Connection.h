// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Connection_h_INCLUDED)
#define Connection_h_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"
#include "SrpcStreamParser.h"

class Connection: public Apollo::TCPConnection
{
public:
  Connection(const char* szName, ApHandle hConnection)
    :Apollo::TCPConnection(szName, hConnection)
  {}
  virtual ~Connection() {}

protected:
  virtual int OnConnected();
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();

protected:
  Apollo::SrpcStreamParser parser_;
};

#endif // !defined(Connection_h_INCLUDED)
