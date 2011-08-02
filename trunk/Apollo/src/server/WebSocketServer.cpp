// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ServerModule.h"
#include "WebSocketServer.h"
#include "WebSocketConnection.h"

Apollo::TCPConnection* WebSocketServer::OnConnectionAccepted(String& sName, const ApHandle& hWebSocketConnection, String& sClientAddress, int nClientPort)
{
  WebSocketConnection* pWebSocketConnection = new WebSocketConnection(sName, hWebSocketConnection);
  if (pWebSocketConnection != 0) {
    ServerModuleInstance::Get()->addWebSocketConnection(hWebSocketConnection, pWebSocketConnection);
  }

  return pWebSocketConnection;
}

int WebSocketServer::OnStarted()
{
  int ok = 1;
  return ok;
}

int WebSocketServer::OnStopped()
{
  int ok = 1;
  return ok;
}
