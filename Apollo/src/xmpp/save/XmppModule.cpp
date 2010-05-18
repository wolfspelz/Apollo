// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "XmppModule.h"
#include "JabberId.h"

int XmppModule::Init()
{
  int ok = 1;
  return ok;
}

int XmppModule::Exit()
{
  int ok = 1;

  while (clients_.Count() > 0) {
    ClientIterator iter(clients_);
    ClientNode* node = iter.Next();
    if (node != 0) {
      Client* pClient = node->Value();
      if (pClient != 0) {
        clients_.Unset(pClient->Handle());
        delete pClient;
        pClient = 0;
      }
    }
  }

  return ok;
}

Client* XmppModule::FindClient(ApHandle h)
{
  Client* pClient = 0; 

  ClientNode* pNode = clients_.Find(h);
  if (pNode != 0) {
    pClient = pNode->Value();
  }

  return pClient; 
}

//----------------------------------------------------------

void XmppModule::On_XMPP_GetStatus(Msg_XMPP_GetStatus* pMsg)
{
  pMsg->apStatus = ApMessage::Ok;
}

// This is the default handler. Can be overidden by other rmodules.
void XmppModule::On_XMPP_Client_GetAutoConnect(Msg_XMPP_Client_GetAutoConnect* pMsg)
{
  int ok = 1;

  pMsg->bConnect = Apollo::GetConfig("Module/" MODULE_NAME "/Connection/AutoConnect", 1);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppModule::On_XMPP_Client_Start(Msg_XMPP_Client_Start* pMsg)
{
  int ok = 1;

  Client* pClient = FindClient(pMsg->hClient);
  if (pClient != 0) {
    ok = 0;
    apLog_Warning((LOG_CHANNEL, "XmppModule::On_XMPP_Client_Start", "Client " ApHandleFormat " already exists", (ApHandleLong) pMsg->hClient));
  } else {

    pClient = new Client(pMsg->hClient);
    if (pClient != 0) {
      pClient->SetJabberId(pMsg->sJabberId);
      if (!pClient->Start()) {
        ok= 0;
        apLog_Error((LOG_CHANNEL, "XmppModule::On_XMPP_Client_Start", "Client " ApHandleFormat " Client::Start() failed, jid%s", (ApHandleLong) pMsg->hClient, (char*) pMsg->sJabberId));
        delete pClient;
        pClient = 0;
      } else {
        clients_.Set(pMsg->hClient, pClient);
      }

    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppModule::On_XMPP_Client_Stop(Msg_XMPP_Client_Stop* pMsg)
{
  int ok = 1;

  Client* pClient = FindClient(pMsg->hClient);
  if (pClient == 0) {
    ok = 0;
    apLog_Warning((LOG_CHANNEL, "XmppModule::On_XMPP_Client_Stop", "Client " ApHandleFormat " not found", (ApHandleLong) pMsg->hClient));
  } else {
    (void) pClient->Stop();
    clients_.Unset(pMsg->hClient);
    delete pClient;
    pClient = 0;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppModule::On_XMPP_Client_Connect(Msg_XMPP_Client_Connect* pMsg)
{
  int ok = 1;

  Client* pClient = FindClient(pMsg->hClient);
  if (pClient == 0) {
    ok = 0;
    apLog_Warning((LOG_CHANNEL, "XmppModule::On_XMPP_Client_Connect", "Client " ApHandleFormat " not found", (ApHandleLong) pMsg->hClient));
  } else {
    //ok = pClient->Connect(pMsg->nDelaySec);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "XmppModule::On_XMPP_Client_Connect", "Client " ApHandleFormat " Connect(%d) failed, jid %s", (ApHandleLong) pMsg->hClient, pMsg->nDelaySec, (char*) pClient->GetJabberId()));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppModule::On_XMPP_Client_Disconnect(Msg_XMPP_Client_Disconnect* pMsg)
{
  int ok = 1;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

void XmppModule::On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::GetConfig("Module/" MODULE_NAME "/Connection/AutoStart", 1)) {
    Msg_XMPP_Client_Start msg;
    msg.hClient = Apollo::NewHandle();
    msg.sJabberId = Apollo::GetConfig("Module/" MODULE_NAME "/Connection/JabberId", "");
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "XmppModule::On_MainLoop_EventLoopBegin", "Failed to start client"));
    }
  }
}

void XmppModule::On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg)
{
  AP_UNUSED_ARG(pMsg);

  ClientNode* node = 0;
  for (ClientIterator iter(clients_); (node = iter.Next()); ) {
    Client* pClient = node->Value();
    if (pClient != 0) {
      pClient->Stop();
    }
  }
}

void XmppModule::On_Net_Online(Msg_Net_Online* pMsg)
{
  AP_UNUSED_ARG(pMsg);

  int bChanged = 0;
  if (!bNetOnline_) {
    bChanged = 1;
    bNetOnline_ = 1;
  }

  if (bChanged) {
    ClientNode* node = 0;
    for (ClientIterator iter(clients_); (node = iter.Next()); ) {
      Client* pClient = node->Value();
      if (!pClient->NetOnline(1)) {
        apLog_Error((LOG_CHANNEL, "XmppModule::On_Net_Online", "pClient->NetOnline() failed"));
      }
    }
  }
}

void XmppModule::On_Net_Offline(Msg_Net_Offline* pMsg)
{
  AP_UNUSED_ARG(pMsg);

  int bChanged = 0;
  if (!bNetOnline_) {
    bChanged = 1;
    bNetOnline_ = 0;
  }

  if (bChanged) {
    ClientNode* node = 0;
    for (ClientIterator iter(clients_); (node = iter.Next()); ) {
      Client* pClient = node->Value();
      if (!pClient->NetOnline(0)) {
        apLog_Error((LOG_CHANNEL, "XmppModule::On_Net_Online", "pClient->NetOnline() failed"));
      }
    }
  }
}

void XmppModule::On_System_SecTimer(Msg_System_SecTimer* pMsg)
{
  AP_UNUSED_ARG(pMsg);

  ClientNode* node = 0;
  for (ClientIterator iter(clients_); (node = iter.Next()); ) {
    Client* pClient = node->Value();
    pClient->Idle();
  }
}

#if defined(APOLLOTEST)

static SString Test_JabberId_CheckOne(const char* szJid, const char* szUser, const char* szHost, const char* szPort, const char* szResource, const char* szBase)
{
  SString s;

  JabberId jid(szJid);
  if (0
    || jid.User() != szUser
    || jid.Host() != szHost 
    || jid.Port() != szPort
    || jid.Resource() != szResource
    || jid.Base() != szBase
    ) {
    s.strcatf("%s: user[%s %s] host[%s %s] port[%s %s] resource[%s %s] base[%s %s] ", (char*) szJid,
      (char*) jid.User(), (char*) szUser, 
      (char*) jid.Host(), (char*) szHost, 
      (char*) jid.Port(), (char*) szPort, 
      (char*) jid.Resource(), (char*) szResource, 
      (char*) jid.Base(), (char*) szBase
      );
  }

  return s;
}

static SString Test_XMPP_JabberId()
{
  SString s;

  if (s.empty()) { s = Test_JabberId_CheckOne("user@domain.org", "user", "domain.org", "", "", "user@domain.org"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("user@domain.org/resource", "user", "domain.org", "", "resource", "user@domain.org"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("user@domain.org:5222/resource", "user", "domain.org", "5222", "resource", "user@domain.org:5222"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("xmpp:user@domain.org:5222/resource", "user", "domain.org", "5222", "resource", "user@domain.org:5222"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("user@domain.org:443/resource", "user", "domain.org", "443", "resource", "user@domain.org:443"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("xmpp:user@domain.org/resource", "user", "domain.org", "", "resource", "user@domain.org"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("xmpp:user@domain.org", "user", "domain.org", "", "", "user@domain.org"); }
  if (s.empty()) { s = Test_JabberId_CheckOne("xmpp://user@domain.org", "user", "domain.org", "", "", "user@domain.org"); }

  return s;
}

//----------------------

void XmppModule::On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  AP_UNITTEST_REGISTER(Test_XMPP_JabberId);
}

void XmppModule::On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  AP_UNITTEST_EXECUTE(Test_XMPP_JabberId);
}

void XmppModule::On_UnitTest_End(Msg_UnitTest_End* pMsg)
{
  AP_UNUSED_ARG(pMsg);
}
#endif // #if defined(APOLLOTEST)
