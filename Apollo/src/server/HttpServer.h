// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(HttpServer_H_INCLUDED)
#define HttpServer_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

class HttpServer: public Apollo::TCPServer
{
public:
  HttpServer() {}
  virtual ~HttpServer() {}

protected:
  virtual Apollo::TCPConnection* OnConnectionAccepted(String& sName, ApHandle hHttpConnection, String& sClientAddress, int nClientPort);
  virtual int OnStarted();
  virtual int OnStopped();
};

#endif // HttpServer_H_INCLUDED
