// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(HttpConnection_h_INCLUDED)
#define HttpConnection_h_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"
#include "HttpParser.h"

class HttpConnection: public Apollo::TCPConnection
{
public:
  HttpConnection(const char* szName, ApHandle hHttpConnection)
    :Apollo::TCPConnection(szName, hHttpConnection)
  {}
  virtual ~HttpConnection() {}

protected:
  int OnConnected();
  int OnDataIn(unsigned char* pData, size_t nLen);
  int OnClosed();

protected:
  HttpParser parser_;
};

#endif // !defined(HttpConnection_h_INCLUDED)
