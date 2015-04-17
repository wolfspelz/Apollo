// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ServerModule.h"
#include "HttpServer.h"
#include "HttpConnection.h"

Apollo::TCPConnection* HttpServer::OnConnectionAccepted(String& sName, const ApHandle& hHttpConnection, String& sClientAddress, int nClientPort)
{
  HttpConnection* pHttpConnection = new HttpConnection(sName, hHttpConnection);
  if (pHttpConnection != 0) {
    ServerModuleInstance::Get()->addHttpConnection(hHttpConnection, pHttpConnection);
  }

  return pHttpConnection;
}

int HttpServer::OnStarted()
{
  int ok = 1;
  return ok;
}

int HttpServer::OnStopped()
{
  int ok = 1;
  return ok;
}
