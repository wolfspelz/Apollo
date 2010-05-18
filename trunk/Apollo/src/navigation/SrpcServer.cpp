// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "SrpcServer.h"
#include "Connection.h"
#include "NavigationModule.h"

Apollo::TCPConnection* SrpcServer::OnConnectionAccepted(String& sName, ApHandle hConnection, String& sClientAddress, int nClientPort)
{
  Connection* pConnection = new Connection(sName, hConnection);
  if (pConnection != 0) {
    NavigationModuleInstance::Get()->addConnection(hConnection, pConnection);
  }

  return pConnection;
}

int SrpcServer::OnStarted()
{
  int ok = 1;
  return ok;
}

int SrpcServer::OnStopped()
{
  int ok = 1;
  return ok;
}
