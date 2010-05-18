// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "XMLProcessor.h"
#include "MessageDigest.h"
#include "MsgProtocol.h"
#include "MsgMainLoop.h"

#include "Local.h"
#include "XmppProtocol.h"
#include "XmppConnection.h"
#include "XmppProcessor.h"
#include "XmppRoom.h"
#include "ProtocolTasks.h"

// -------------------------------------------------------------------

XmppProtocol::XmppProtocol()
:hConnHandle_(ApNoHandle)
,bFirstConnect_(1)
,bConnectSoon_(0)
,bConnecting_(0)
,pConnection_(0)
,nConnectedPort_(0)
,pProcessor_(0)
,tLastDisconnect_(0)
,nNextConnectDelay_(0)
,nRequestId_(0)
{
}

XmppProtocol::~XmppProtocol()
{
  if (pProcessor_ != 0) {
    delete pProcessor_;
  }
}

int XmppProtocol::Init()
{
  return 1;
}

int XmppProtocol::Exit()
{
  if (pConnection_ != 0) {
    (void) pConnection_->Close();
    delete pConnection_;
    pConnection_ = 0;
  }

  return 1;
}

XmppRoom* XmppProtocol::FindRoom(const char* szJID)
{
  return (XmppRoom*) slRooms_.FindByName(szJID);
}

XmppRoom* XmppProtocol::FindRoom(ApHandle hLocation)
{
  XmppRoom* pRoom = 0;
  while  ((pRoom = (XmppRoom*) slRooms_.Next(pRoom)) != 0) {
    if (pRoom->hLocation_ == hLocation) {
      break;
    }
  }
  return (XmppRoom*) pRoom;
}

XmppRoom* XmppProtocol::NewRoom(const char* szRoomJID, ApHandle hLocation)
{
  XmppRoom* pRoom = new XmppRoom(this, szRoomJID, hLocation);
  if (pRoom != 0) {
    slRooms_.Add(pRoom);
  }
  return pRoom;
}


int XmppProtocol::DeleteRoom(XmppRoom* pRoom)
{
  slRooms_.Remove(pRoom);
  delete pRoom;
  return 1;
}

int XmppProtocol::GetStanzaError(Apollo::XMLNode* pStanza, int& nError, SString& sError)
{
  // <iq id='0' type='error' from='googlemail.com'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>wolf.heiner</username>
  //   </query>
  //   <error code='405' type='cancel'>
  //     <not-allowed xmlns='urn:ietf:params:xml:ns:xmpp-stanzas' />
  //     <text xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'>Connection must be encrypted.</text>
  //   </error>
  // </iq>
  int ok = 0;

  Apollo::XMLNode& oError = pStanza->ChildRef("error");
  if (oError) {
    ok = 1;

    sError = oError.CData();
    sError.trimWSP();

    if (sError.empty()) {
      Apollo::XMLNode& oText = oError.ChildRef("text");
      sError = oText.CData();
    }

    nError = SSystem::atoi(oError.Attribute("code").Value());
  }

  return ok;
}

// -------------------------------------------------------------------
// Message Handler

void XmppProtocol::On_Protocol_Feature(Msg_Protocol_Feature* pMsg)
{
  int ok = 0;

  if (pMsg->sProtocol == "jabber" || pMsg->sProtocol == "xmpp") {
    ok = 1;
    switch (pMsg->nFeature) {
      case Msg_Protocol_Feature::Feature_GroupChat: pMsg->bSupported = 1; break;
      default: ;
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppProtocol::On_Protocol_EnterLocation(Msg_Protocol_EnterLocation* pMsg)
{
  int ok = 0;

  if (pMsg->sProtocol == "jabber" || pMsg->sProtocol == "xmpp") {
    ok = 1;
    SString sRoomJID = pMsg->sLocation;
    SString sProtocol;
    sRoomJID.tok(":", sProtocol);
    if (sProtocol == pMsg->sProtocol && !sRoomJID.empty()) {
      XmppRoom* pRoom = FindRoom(sRoomJID);
      if (pRoom != 0) {
        ok = 0;
        apLog_Warning((LOG_CHANNEL, "XmppProtocol::On_Protocol_EnterLocation", "Already in room %s", (char*) sRoomJID));
      } else {
        pRoom = NewRoom(sRoomJID, pMsg->hLocation);
        if (pRoom != 0) {
          SString sNickname = Apollo::GetConfig("DisplayName", Apollo::UniqueId());
          ok = pRoom->Enter(sNickname);
        }
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppProtocol::On_Protocol_LeaveLocation(Msg_Protocol_LeaveLocation* pMsg)
{
  int ok = 0;

  if (pMsg->sProtocol == "jabber" || pMsg->sProtocol == "xmpp") {
    ok = 1;
    SString sRoomJID = pMsg->sLocation;
    SString sProtocol;
    sRoomJID.tok(":", sProtocol);
    if (sProtocol == pMsg->sProtocol && !sRoomJID.empty()) {
      XmppRoom* pRoom = FindRoom(sRoomJID);
      if (pRoom == 0) {
        ok = 0;
        apLog_Warning((LOG_CHANNEL, "XmppProtocol::On_Protocol_EnterLocation", "Not in room %s", (char*) sRoomJID));
      } else {
        ok = pRoom->Leave();
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppProtocol::On_Net_Online(Msg_Net_Online* pMsg)
{
  if (!Connected()) {
    if (Apollo::GetConfig("Module/" MODULE_NAME "/Connection/AutoConnect", 1)) {
      ConnectSoon();
    }
  }
}

void XmppProtocol::On_Net_Offline(Msg_Net_Offline* pMsg)
{
}

void XmppProtocol::On_System_SecTimer(Msg_System_SecTimer* pMsg)
{
  if (!Connected() && bConnectSoon_ && !bConnecting_) {
    ConnectSoon();
  }
}

void XmppProtocol::On_Protocol_SendPublicChat(Msg_Protocol_SendPublicChat* pMsg)
{
  int ok = 1;
  
  XmppRoom* pRoom = FindRoom(pMsg->hLocation);
  if (pRoom != 0) {
    ok= pRoom->SendPublicChat(pMsg->sText);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void XmppProtocol::On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg)
{
  int ok = 1;

  if (pConnection_ != 0) {
    ok = pConnection_->Close();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

// -------------------------------------------------------------------
// Internal

int XmppProtocol::GetConnectDelay()
{
  int nConnectDelay = Apollo::GetConfig("Module/" MODULE_NAME "/ReconnectMinDelay", 3);
  nConnectDelay += Apollo::Random(Apollo::GetConfig("Module/" MODULE_NAME "/ReconnectRandomDelay", 10));
  return nConnectDelay;
}

void XmppProtocol::ConnectSoon()
{
  if (nNextConnectDelay_ == 0) {
    nNextConnectDelay_ = GetConnectDelay();
  }

  int nSecsUntilConnect = nNextConnectDelay_ - (Apollo::Now() - tLastDisconnect_);
  if (nSecsUntilConnect <= 0) {
    Connect();
  } else {
    apLog_Verbose((LOG_CHANNEL, "XmppProtocol::ConnectSoon", "Connect in %d seconds", nSecsUntilConnect));
    bConnectSoon_ = 1;
  }
}

void XmppProtocol::Connect()
{
  bConnectSoon_ = 0;
  bFirstConnect_ = 0;
  bConnecting_ = 1;

  pConnection_ = new XmppConnection(*this);
  if (pConnection_ != 0) {

    sPassword_ = Apollo::GetConfig("Module/" MODULE_NAME "/Account/Password", "");
    sResource_ = Apollo::GetConfig("Module/" MODULE_NAME "/Account/Resource", Apollo::UniqueId());

    jid_ = Apollo::GetConfig("Module/" MODULE_NAME "/Account/JID", "");
    if (jid_.empty()) {
      apLog_Error((LOG_CHANNEL, "XmppProtocol::Connect", "no JID"));
    } else {

      sConnectedHost_ = Apollo::GetConfig("Module/" MODULE_NAME "/Account/Server", "");
      if (sConnectedHost_.empty()) {
        sConnectedHost_ = jid_.Host();
        nConnectedPort_ = ::atoi(jid_.Port());
      } else {
        nConnectedPort_ = Apollo::GetConfig("Module/" MODULE_NAME "/Account/Port", 0);
        if (nConnectedPort_ == 0) {
          nConnectedPort_ = 5222; // should use DNS
        }
      }

      Msg_XMPP_Connect msg;
      msg.sHost = sConnectedHost_;
      msg.nPort = nConnectedPort_;
      msg.Filter();
      sConnectedHost_ = msg.sHost;
      nConnectedPort_ = msg.nPort;
      
      pConnection_->Connect(sConnectedHost_, nConnectedPort_);
    }
  }
}

int XmppProtocol::NextId()
{
  return nRequestId_++;
}

// -------------------------------------------------------------------
// Connection events

int XmppProtocol::OnConnected()
{
  apLog_Verbose((LOG_CHANNEL, "XmppProtocol::OnConnected", ""));

  bConnecting_ = 0;
  nNextConnectDelay_ = 0;

  Msg_XMPP_Connected msg;
  msg.Send();

  SString sOut;
  sOut.strcatf("<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams'>", (char*) jid_.Host());

  LoginTask* pTask = new LoginTask(this);
  if (pTask != 0) {
    slTasks_.Add(pTask);
  }

  pConnection_->DataOut((unsigned char*) sOut.c_str(), sOut.bytes());

  return 1;
}

int XmppProtocol::OnDataIn(unsigned char* pData, size_t nLen)
{
  apLog_VeryVerbose((LOG_CHANNEL, "XmppProtocol::OnDataIn", "%d", nLen));
  int ok = 0;

  if (pProcessor_ == 0) {
    pProcessor_ = new XmppProcessor(*this);
    if (pProcessor_ != 0) {
      ok = pProcessor_->Init();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "XmppProtocol::OnDataIn", "pProcessor_->Init failed"));
      }
    }
  }
  if (pProcessor_ != 0) {
    ok = pProcessor_->Parse((char*) pData, nLen, 0);
    if (!ok) {
      apLog_Warning((LOG_CHANNEL, "XmppProtocol::OnDataIn", "Parse failed: %s", (char*) pProcessor_->GetErrorString()));
      pConnection_->Close();
    }
  }

  return 1;
}

int XmppProtocol::OnClosed()
{
  apLog_Verbose((LOG_CHANNEL, "XmppProtocol::OnClosed", ""));

  tLastDisconnect_ = Apollo::Now();

  Msg_XMPP_Closed msg;
  msg.Send();

  pConnection_ = 0;
  if (Apollo::GetConfig("Module/" MODULE_NAME "/AutoReconnect", 1)) {
    bConnectSoon_ = 1;
  }

  if (pProcessor_ != 0) {
    pProcessor_->Exit();
    delete pProcessor_;
    pProcessor_ = 0;
  }

  return 1;
}

int XmppProtocol::OnStanzaIn(Apollo::XMLNode* pStanza)
{
  apLog_VeryVerbose((LOG_CHANNEL, "XmppProtocol::OnStanzaIn", "<%s ...", (char*) pStanza->Name()));

  Msg_XMPP_StanzaIn msg;
  pStanza->OuterXml(msg.sData);
  msg.Send();

  Apollo::XMLNode* pUseStanza = pStanza;
  
  {
    SString sStanza;
    pUseStanza->OuterXml(sStanza);
    apLog_VeryVerbose((LOG_CHANNEL, "XmppProtocol::OnStanzaIn", "%s", (char*) sStanza));
  }

  Apollo::XMLProcessor oDoc;
  if (!oDoc.XmlText(msg.sData)) {
    apLog_Error((LOG_CHANNEL, "XmppProtocol::OnStanzaIn", "Filtered stanza parser error: %s", (char*) oDoc.GetErrorString()));
  } else {
    Apollo::XMLNode* pFilteredStanza = oDoc.Root();
    if (pFilteredStanza != 0) {
      pUseStanza = pFilteredStanza;
    }
  }

  // Move tasks to temporary list
  SListT<XmppProtocolTask, SElem> slTmpTaskList;
  {
    XmppProtocolTask* eTmp = 0;
    while (eTmp = slTasks_.First()) {
      slTasks_.Remove(eTmp);
      slTmpTaskList.Add(eTmp);
    }
  }

  // Let tasks use the stanza
  // Tasks may add new tasks to the list, because we iterate a temporary list
  {
    int bConsumed = 0;
    XmppProtocolTask* pTask = slTmpTaskList.First();
    while (pTask != 0 && !bConsumed) {
      bConsumed = pTask->OnStanza(pUseStanza);
      if (!bConsumed) {
        pTask = slTmpTaskList.Next(pTask);
      }
    }

    if (!bConsumed) {
      SString sStanza;
      pUseStanza->OuterXml(sStanza);
      apLog_Warning((LOG_CHANNEL, "XmppProtocol::OnStanzaIn", "Unhandled stanza: %s", (char*) sStanza));
    }
  }

  // Delete tasks, which are finished after the stanza
  {
    int bFoundOne = 0;
    do {
      bFoundOne = 0;
      XmppProtocolTask* pTask = 0;
      while (pTask = slTmpTaskList.Next(pTask)) {
        if (pTask->Done()) {
          bFoundOne = 1;
          break;
        }
      }
      if (bFoundOne) {
        slTmpTaskList.Remove(pTask);
        delete pTask;
      }
    } while (bFoundOne);
  }

  // Restore original list
  {
    XmppProtocolTask* eTmp = 0;
    while (eTmp = slTmpTaskList.First()) {
      slTmpTaskList.Remove(eTmp);
      slTasks_.Add(eTmp);
    }
  }

  return 1;
}

int XmppProtocol::OnStanzaOut(Apollo::XMLNode* pStanza)
{
  if (pConnection_ != 0) {

    Msg_XMPP_StanzaOut msg;
    pStanza->OuterXml(msg.sData);
    msg.Filter();
    pConnection_->DataOut((unsigned char*) msg.sData.c_str(), msg.sData.bytes());
  }

  return 1;
}

// -------------------------------------------------------------------

int XmppProtocol::OnLoggedIn()
{
  Msg_XMPP_LoggedIn msg;
  msg.sJID = jid_;
  msg.Send();

  {
    OnlinePresenceTask* pTask = new OnlinePresenceTask(this);
    if (pTask != 0) {
      slTasks_.Add(pTask);
    }
  }

  {
    RoomPresenceTask* pTask = new RoomPresenceTask(this);
    if (pTask != 0) {
      slTasks_.Add(pTask);
    }
  }

  {
    GroupChatTask* pTask = new GroupChatTask(this);
    if (pTask != 0) {
      slTasks_.Add(pTask);
    }
  }

  SendOnlinePresence();

  return 1;
}

// -------------------------------------------------------------------

int XmppProtocol::SendOnlinePresence()
{
  // <presence>
  //   <status>Available</status>
  //   <priority>0</priority>
  //   <show>xa</show>
  // </presence>
  XmppPresenceStanza oStanza("available");

  Msg_XMPP_GetLocalPresenceStatus msg;
  msg.sStatus = "I am here";
  msg.sShow = "xa";
  msg.Request();

  Apollo::XMLNode& oStatus = oStanza.AddChildRef("status");
  oStatus.CData(msg.sStatus);

  Apollo::XMLNode& oShow = oStanza.AddChildRef("show");
  oShow.CData(msg.sShow);

  SString sPriority; 
  int nPriority = Apollo::GetConfig("Module/" MODULE_NAME "/Account/Priority", -1);
  sPriority.strcatf("%d", nPriority);
  Apollo::XMLNode& oPriority = oStanza.AddChildRef("priority");
  oPriority.CData(sPriority);

  OnStanzaOut(&oStanza);  

  return 1;
}
