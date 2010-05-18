// ============================================================================
//
// Apollo
//
// ============================================================================

#include "NetAPIModule.h"

int NetAPIModule::Init()
{
  int ok = 1;
  return ok;
}

int NetAPIModule::Exit()
{
  int ok = 1;

  lTCPConnections_.Empty();
  lHTTPClients_.Empty();

  return ok;
}

//----------------------------------------------------------

void NetAPIModule::On_Net_TCP_Connected(Msg_Net_TCP_Connected* pMsg)
{
  Apollo::TCPConnection* c = NetAPIModuleInstance::Get()->FindTCPConnection(pMsg->hConnection);
  if (c != 0) {
    if (pMsg->bSuccess) {
      c->InternalConnected();
    } else {
      c->Close();
    }
  }
}

void NetAPIModule::On_Net_TCP_DataIn(Msg_Net_TCP_DataIn* pMsg)
{
  Apollo::TCPConnection* c = NetAPIModuleInstance::Get()->FindTCPConnection(pMsg->hConnection);
  if (c != 0) {
    if (!c->Cancelled()) {
      c->InternalDataIn(pMsg->sbData.Data(), pMsg->sbData.Length());
    }
  }
}

void NetAPIModule::On_Net_TCP_Closed(Msg_Net_TCP_Closed* pMsg)
{
  Apollo::TCPConnection* c = NetAPIModuleInstance::Get()->FindTCPConnection(pMsg->hConnection);
  if (c != 0) {
    c->InternalClosed();
  }
}

void NetAPIModule::On_Net_TCP_Listening(Msg_Net_TCP_Listening* pMsg)
{
  Apollo::TCPServer* pServer = NetAPIModuleInstance::Get()->FindTCPServer(pMsg->hServer);
  if (pServer != 0) {
    if (pMsg->bSuccess) {
      pServer->InternalListening();
    } else {
      pServer->Stop();
    }
  }
}

void NetAPIModule::On_Net_TCP_ConnectionAccepted(Msg_Net_TCP_ConnectionAccepted* pMsg)
{
  Apollo::TCPServer* pServer = NetAPIModuleInstance::Get()->FindTCPServer(pMsg->hServer);
  if (pServer != 0) {
    pServer->InternalConnectionAccepted(pMsg->hConnection, pMsg->sClientAddress, pMsg->nClientPort);
  }
}

void NetAPIModule::On_Net_TCP_ListenStopped(Msg_Net_TCP_ListenStopped* pMsg)
{
  Apollo::TCPServer* pServer = NetAPIModuleInstance::Get()->FindTCPServer(pMsg->hServer);
  if (pServer != 0) {
    pServer->InternalListenStopped();
  }
}

Apollo::TCPServer* NetAPIModule::FindTCPServer(ApHandle hAp)
{
  Apollo::TCPServer* pServer = NULL;
  ApHandleTreeNode<Apollo::TCPServer*>* pNode = tcpServers_.Find(hAp);
  if (pNode != 0) {
    pServer = pNode->Value();
  }
  return pServer;
}

void NetAPIModule::AddTCPServer(Apollo::TCPServer* pServer)
{
  tcpServers_.Set(pServer->apHandle(), pServer);
}

void NetAPIModule::RemoveTCPServer(Apollo::TCPServer* pServer)
{
  tcpServers_.Unset(pServer->apHandle());
}

Apollo::TCPConnection* NetAPIModule::FindTCPConnection(ApHandle h)
{
  Apollo::TCPConnection* c = 0; 
  while ((c = NetAPIModuleInstance::Get()->lTCPConnections_.Next(c)) != 0) {
    if (c->apHandle() == h) {
      break;
    }
  }
  return c; 
}

void NetAPIModule::AddTCPConnection(Apollo::TCPConnection* pConnection)
{
  lTCPConnections_.Add(pConnection);
}

void NetAPIModule::RemoveTCPConnection(Apollo::TCPConnection* pConnection)
{
  lTCPConnections_.Remove(pConnection);
}

//----------------------------------------------------------

void NetAPIModule::On_Net_HTTP_Connected(Msg_Net_HTTP_Connected* pMsg)
{
  Apollo::HTTPClient* c = NetAPIModuleInstance::Get()->FindHTTPClient(pMsg->hClient);
  if (c != 0) {
    c->InternalConnected();
  }
}

void NetAPIModule::On_Net_HTTP_Header(Msg_Net_HTTP_Header* pMsg)
{
  Apollo::HTTPClient* c = NetAPIModuleInstance::Get()->FindHTTPClient(pMsg->hClient);
  if (c != 0) {
    c->InternalHeader(pMsg->nStatus, pMsg->kvHeader);
  }
}

void NetAPIModule::On_Net_HTTP_DataIn(Msg_Net_HTTP_DataIn* pMsg)
{
  Apollo::HTTPClient* c = NetAPIModuleInstance::Get()->FindHTTPClient(pMsg->hClient);
  if (c != 0) {
    if (!c->Cancelled()) {
      c->InternalDataIn(pMsg->sbData.Data(), pMsg->sbData.Length());
    }
  }
}

void NetAPIModule::On_Net_HTTP_Failed(Msg_Net_HTTP_Failed* pMsg)
{
  Apollo::HTTPClient* c = NetAPIModuleInstance::Get()->FindHTTPClient(pMsg->hClient);
  if (c != 0) {
    c->InternalFailed(pMsg->sMessage);
  }
}

void NetAPIModule::On_Net_HTTP_Closed(Msg_Net_HTTP_Closed* pMsg)
{
  Apollo::HTTPClient* c = NetAPIModuleInstance::Get()->FindHTTPClient(pMsg->hClient);
  if (c != 0) {
    c->InternalClosed();
  }
}

Apollo::HTTPClient* NetAPIModule::FindHTTPClient(ApHandle h)
{
  Apollo::HTTPClient* c = 0; 
  while ((c = NetAPIModuleInstance::Get()->lHTTPClients_.Next(c)) != 0) {
    if (c->apHandle() == h) {
      break;
    }
  }
  return c; 
}

void NetAPIModule::AddHTTPClient(Apollo::HTTPClient* pClient)
{
  lHTTPClients_.Add(pClient);
}

void NetAPIModule::RemoveHTTPClient(Apollo::HTTPClient* pClient)
{
  lHTTPClients_.Remove(pClient);
}

//----------------------------------------------------------

void NetAPIModule::On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  NetAPIModuleInstance::Get()->Exit();
}
