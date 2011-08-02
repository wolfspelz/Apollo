// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebSocketServer_H_INCLUDED)
#define WebSocketServer_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

class WebSocketServer: public Apollo::TCPServer
{
public:
  WebSocketServer() {}
  virtual ~WebSocketServer() {}

protected:
  virtual Apollo::TCPConnection* OnConnectionAccepted(String& sName, const ApHandle& hWebSocketConnection, String& sClientAddress, int nClientPort);
  virtual int OnStarted();
  virtual int OnStopped();
};

#endif // WebSocketServer_H_INCLUDED
