// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Connection_h_INCLUDED)
#define Connection_h_INCLUDED

#include "netapi/NetInterface.h"

class Client;

class Connection : public Apollo::TCPConnection
{
public:
  Connection(Client* pClient);
  virtual ~Connection();

  int OnConnected();
  int OnDataIn(unsigned char* pData, size_t nLen);
  int OnClosed();

protected:
  Client* pClient_;
};

#endif // !defined(Connection_h_INCLUDED)
