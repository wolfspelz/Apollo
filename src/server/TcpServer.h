// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(TcpServer_H_INCLUDED)
#define TcpServer_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

class TcpServer: public Apollo::TCPServer
{
public:
  TcpServer() {}
  virtual ~TcpServer() {}

protected:
  virtual Apollo::TCPConnection* OnConnectionAccepted(String& sName, const ApHandle& hTcpConnection, String& sClientAddress, int nClientPort);
  virtual int OnStarted();
  virtual int OnStopped();
};

#endif // HttpServer_H_INCLUDED
