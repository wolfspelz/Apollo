// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "ServerModule.h"
#include "TcpServer.h"
#include "TcpConnection.h"

Apollo::TCPConnection* TcpServer::OnConnectionAccepted(String& sName, const ApHandle& hTcpConnection, String& sClientAddress, int nClientPort)
{
  TcpConnection* pTcpConnection = new TcpConnection(sName, hTcpConnection);
  if (pTcpConnection != 0) {
    ServerModuleInstance::Get()->addTcpConnection(hTcpConnection, pTcpConnection);
  }

  return pTcpConnection;
}

int TcpServer::OnStarted()
{
  int ok = 1;
  return ok;
}

int TcpServer::OnStopped()
{
  int ok = 1;
  return ok;
}
