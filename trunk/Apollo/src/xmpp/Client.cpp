// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgXmpp.h"
#include "Client.h"
#include "Connection.h"
#include "Parser.h"
#include "Stanza.h"
#include "Room.h"
#include "Protocol.h"

const char* Client::LogId()
{
  if (sLogId_.empty()) {
    sLogId_.appendf("jid=%s state=%d " ApHandleFormat, _sz(JabberId()), (int) nState_, ApHandlePrintf(hAp_));
  }
  return sLogId_;
}

void Client::setJabberId(String& sJabberId)
{
  jid_ = sJabberId;
}

JabberId& Client::getJabberId()
{
  return jid_;
}

void Client::setPassword(String& sPassword)
{
  sPassword_ = sPassword;
}

String& Client::getPassword()
{
  return sPassword_;
}

void Client::setResource(String& sResource)
{
  sResource_ = sResource;
}

String& Client::getResource()
{
  return sResource_;
}

void Client::setHost(String& sHost)
{
  sHost_ = sHost;
}

String& Client::getHost()
{
  if (sHost_.empty()) {
    return jid_.host();
  }
  return sHost_;
}

void Client::setPort(int nPort)
{
  nPort_ = nPort;
}

int Client::getPort()
{
  int nPort = nPort_;

  if (nPort == 0) {
    nPort = ::atoi(jid_.port());
  }
  if (nPort == 0) {
    nPort = 5222;
  }

  return nPort;
}

int Client::isDefault()
{
  return bDefault_;
}

void Client::setDefault(int bDefault)
{
  bDefault_ = bDefault;
}

// -------------------------------------------------------------------

int Client::start()
{
  int ok = 1;

  nState_ = ClientState_NotConnected;

  Msg_Xmpp_GetAutoConnect msg;
  msg.hClient = hAp_;
  if (msg.Request()) {
    if (msg.bConnect) {
      ok = connect();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "connect() failed (%s), ", _sz(LogId())));
      }
    }
  }

  return ok;
}

int Client::stop()
{
  int ok = 1;

  for (Task* pTask = 0; (pTask = lTasks_.First()); ) {
    lTasks_.Remove(pTask);
    delete pTask;
    pTask = 0;
  }

  bReconnectAfterDisconnect_ = 0;

  switch (nState_) {
    case ClientState_NotConnected: {
      } break;
    case ClientState_ConnectDelay: {
        nState_ = ClientState_NotConnected;
      } break;
    case ClientState_Connecting: {
        disconnect();
      } break;
    case ClientState_Connected: {
        disconnect();
      } break;
    case ClientState_Disconnecting: {
      } break;
    default:;
  }

  return ok;
}

int Client::idle()
{
  int ok = 1;

  if (nState_ == ClientState_ConnectDelay) {
    connect();
  }

  if (nState_ == ClientState_Connected) {
    time_t tNow = Apollo::getNow().Sec();
    if (tNow - tLastTraffic_ >= nHeartbeatDelay_) {
      sendHeartbeat();
    }
  }

  return ok;
}

int Client::netOnline(int bOnline)
{
  AP_UNUSED_ARG(bOnline);
  bOnline_ = bOnline;
  return 1;
}

int Client::getConnectDelay()
{
  int nConnectDelay = 10;

  if (bConnectImmediately_) {
    nConnectDelay = 1;
  } else {
    int nMinDelay = Apollo::getModuleConfig(MODULE_NAME, "ReconnectMinDelay", 3);
    int nRandomDelay = Apollo::getRandom(Apollo::getModuleConfig(MODULE_NAME, "ReconnectRandomDelay", 10));
    nConnectDelay = nMinDelay + nRandomDelay;
  }

  return nConnectDelay;
}

int Client::connect()
{
  int ok = 1;

  bReconnectAfterDisconnect_ = 0;

  if (nNextConnectDelay_ == 0) {
    nNextConnectDelay_ = getConnectDelay();
    nNextConnectBase_ = Apollo::getNow().Sec();
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Choosing reconnect interval in %d seconds from now", nNextConnectDelay_));
  }

  time_t nSecsUntilConnect = nNextConnectBase_ + nNextConnectDelay_ - Apollo::getNow().Sec();
  if (nSecsUntilConnect <= 0) {
    if (!bOnline_) {
      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Waiting for network since %d seconds", -nSecsUntilConnect));
    } else {
      if (!connectNow()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "connectNow() failed: %s", _sz(getJabberId())));
      }
    }
  } else {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Connect in %d seconds", nSecsUntilConnect));
    nState_ = ClientState_ConnectDelay;
  }

  return ok;
}

String Client::getConnectionName()
{
  String sName;
  sName.appendf("XMPP:%s", _sz(JabberId()));
  return sName;
}

int Client::connectNow()
{
  int ok = 1;

  if (nState_ == ClientState_Connected || nState_ == ClientState_Connecting) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Ignoring connectNow, already nState=%d", nState_));
  } else {
    bConnectImmediately_ = 0;
    nNextConnectDelay_ = 0;

    pConnection_ = new Connection(this);
    if (pConnection_ != 0) {
      Msg_Xmpp_ConnectDetails msg;
      msg.hClient = hAp_;
      msg.sJabberId = getJabberId();
      msg.sPassword = getPassword();
      msg.sResource = getResource();
      msg.sHost = getHost();
      msg.nPort = getPort();
      msg.Filter();
      setJabberId(msg.sJabberId);
      setPassword(msg.sPassword);
      setResource(msg.sResource);
      setHost(msg.sHost);
      setPort(msg.nPort);

      pConnection_->setName(getConnectionName());

      ok = pConnection_->Connect(getHost(), getPort());
      if (ok) {
        nState_ = ClientState_Connecting;
        bReconnectAfterDisconnect_ = 1;
      } else {
        ok = connectDeferred(getConnectDelay());
      }
    }

  }

  return ok;
}

int Client::connectDeferred(int nDelaySec)
{
  int ok = 1;

  nNextConnectDelay_ = nDelaySec;
  nNextConnectBase_ = Apollo::getNow().Sec();
  ok = connect();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "failed nDelaySec=%d", nDelaySec));
  }

  return ok;
}

int Client::disconnect()
{
  int ok = 1;

  if (nState_ == ClientState_Connecting || nState_ == ClientState_Connected) {
    bReconnectAfterDisconnect_ = 0;

    if (pConnection_ != 0) {
      nState_ = ClientState_Disconnecting;
      ok = pConnection_->Close();
    } else {
      nState_ = ClientState_NotConnected;
    }

  } else if (nState_ == ClientState_ConnectDelay) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Cancelling delayed connect"));
    
    if (pConnection_ != NULL) {
      nState_ = ClientState_Disconnecting;
      ok = pConnection_->Close(); 
    } else {
      nState_ = ClientState_NotConnected;
    }    
    
    nNextConnectDelay_ = 0;
    nNextConnectBase_ = 0;
  } else {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Ignoring disconnect, already nState=%d", nState_));
  }

  return ok;
}

int Client::sendHeartbeat()
{
  int ok = 1;

  String sOut = " ";

  {
    Msg_Xmpp_DataOut msg;
    msg.hClient = hAp_;
    msg.sbData.SetData((unsigned char*) sOut.c_str(), sOut.bytes());
    msg.Request();
  }

  return ok;
}

// -------------------------------------------------------------------

int Client::getNextStanzaId()
{
  return nStanzaId_++;
}

int Client::sendStanza(Apollo::XMLNode& stanza)
{
  int ok = 0;

  Msg_Xmpp_StanzaOut msg;
  msg.hClient = hAp_;
  stanza.outerXml(msg.sData);
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " <%s ...", ApHandlePrintf(hAp_), _sz(stanza.getName())));
  }

  return ok;
}

// -------------------------------------------------------------------
// Lowlevel data from message

int Client::dataIn(unsigned char* pData, size_t nLen)
{
  int ok = 0;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d", ApHandlePrintf(hAp_), nLen));

  if (apLog_IsVerbose) {
    String sTraffic;
    sTraffic.set((const char*) pData, nLen);
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%d %s", nLen, _sz(sTraffic)));
  }

  if (pParser_ == 0) {
    pParser_ = new Parser(this);
    if (pParser_ != 0) {
      ok = pParser_->Init();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pParser_->Init failed"));
      }
    }
  }

  if (pParser_ != 0) {
    ok = pParser_->Parse((char*) pData, nLen, 0);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Parse failed: %s", _sz(pParser_->GetErrorString())));
      pConnection_->Close();
    }
  }

  return ok;
}

int Client::dataOut(unsigned char* pData, size_t nLen)
{
  int ok = 0;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d", ApHandlePrintf(hAp_), nLen));

  if (apLog_IsVerbose) {
    String sTraffic;
    sTraffic.set((const char*) pData, nLen);
    if (nLen == 1 && sTraffic == " ") {
      // ignore keepalive packet
    } else {
      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%d %s", nLen, _sz(sTraffic)));
    }
  }

  if (pConnection_ != 0) {
    ok = pConnection_->DataOut(pData, nLen);
    if (ok) {
      tLastTraffic_ = Apollo::getNow().Sec();
    }
  }

  return ok;
}

// -------------------------------------------------------------------
// Highlevel data from message

int Client::stanzaIn(const String& sData)
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d", ApHandlePrintf(hAp_), sData.chars()));

  Stanza* pStanza = 0;

  Apollo::XMLProcessor doc;
  if (!doc.XmlText(sData)) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Filtered stanza parser error: %s", _sz(doc.GetErrorString())));
  } else {
    pStanza = (Stanza*) doc.Root();
  }

  if (pStanza != 0) {
    int bHandled = 0;
    int bClose = 0;
    int bDone = 0;
    SafeListIteratorT<Task> i(lTasks_);
    for (Task* pTask = 0; (pTask = i.Next()) && ok && !bDone; ) {
      StanzaHandlerResult result;
      ok = pTask->handleStanza(*pStanza, result);
      if (ok) {
        bHandled = bHandled || result.stanzaHandled();
        if (result.taskFinished()) {
          lTasks_.Remove(pTask);
          delete pTask;
          pTask = 0;
        }
        if (result.stanzaConsumed()) {
          bDone = 1;
        }
      }
      if (result.connectionFinished()) {
        bClose = 1;
        bDone = 1;
      }
    }

    if (!bHandled) {
      String sStanza;
      pStanza->outerXml(sStanza);
      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Unhandled stanza: %s", _sz(sStanza)));
    }

    if (bClose) {
      this->disconnect();
    }
  }

  return ok;
}

int Client::stanzaOut(const String& sData)
{
  int ok = 0;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d", ApHandlePrintf(hAp_), sData.chars()));

  Msg_Xmpp_DataOut msg;
  msg.hClient = hAp_;
  msg.sbData.SetData((unsigned char*) sData.c_str(), sData.bytes());
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Xmpp_DataOut failed %s", _sz(String::truncate(sData, 100))));
  }

  return ok;
}

// -------------------------------------------------------------------
// Protocoll events

int Client::onProtocolStart()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));
  int ok = 1;

  StreamErrorTask* pTask = new StreamErrorTask(this);
  if (pTask != 0) {
    lTasks_.Add(pTask);
  }

  return ok;
}

int Client::onProtocolStartFailed()
{
  apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));
  int ok = 1;

  return ok;
}

int Client::onProtocolLogin()
{
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));
  int ok = 1;

  bLoggedIn_ = 1;

  {
    PresenceTask* pTask = new PresenceTask(this);
    if (pTask != 0) {
      lTasks_.Add(pTask);
    }
  }

  {
    VersionTask* pTask = new VersionTask(this);
    if (pTask != 0) {
      lTasks_.Add(pTask);
    }
  }

  {
    GroupchatTask* pTask = new GroupchatTask(this);
    if (pTask != 0) {
      lTasks_.Add(pTask);
    }
  }

  String sTo = getJabberId();
  PresenceStanza presence(JABBER_PRESENCE_AVAILABLE, sTo);
  ok = sendStanza(presence);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "failed " ApHandleFormat " <%s to=%s ...", ApHandlePrintf(hAp_), _sz(presence.getName()), _sz(sTo)));
  }

  Msg_Xmpp_LoggedIn msg;
  msg.hClient = hAp_;
  msg.Send();

  return ok;
}

int Client::onProtocolLoginFailed()
{
  apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));
  int ok = 1;

  return ok;
}

// -------------------------------------------------------------------
// Lowlevel events from connection

int Client::onConnectionConnected()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));
  int ok = 1;

  nState_ = ClientState_Connected;

  {
    Msg_Xmpp_Connected msg;
    msg.hClient = hAp_;
    msg.Send();
  }

  lTasks_.Empty();

  LoginTask* pTask = new LoginTask(this);
  if (pTask != 0) {
    lTasks_.Add(pTask);
  }

  nHeartbeatDelay_ = Apollo::getModuleConfig(MODULE_NAME, "HeartbeatDelay", 30);

  String sOut;
  sOut.appendf("<?xml version='1.0'?>\n<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams'>", _sz(jid_.host()));

  {
    Msg_Xmpp_DataOut msg;
    msg.hClient = hAp_;
    msg.sbData.SetData((unsigned char*) sOut.c_str(), sOut.bytes());
    msg.Request();
  }

  return ok;
}

int Client::onConnectionDataIn(unsigned char* pData, size_t nLen)
{
  int ok = 0;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d", ApHandlePrintf(hAp_), nLen));

  Msg_Xmpp_DataIn msg;
  msg.hClient = hAp_;
  msg.sbData.SetData(pData, nLen);
  ok = msg.Request();

  return ok;
}

int Client::onConnectionDisconnected()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));
  int ok = 1;

  // Leave all rooms
  if (lRooms_.length() > 0) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Cleaning up %d rooms", lRooms_.length()));
    while (lRooms_.length() > 0) {
      Room* pRoom = lRooms_.First();
      if (pRoom != 0) {
        (void) leaveRoomComplete(pRoom->apHandle());
      }
    }
  }

  pConnection_ = 0;
  nState_ = ClientState_NotConnected;

  if (pParser_ != 0) {
    pParser_->Exit();
    delete pParser_;
    pParser_ = 0;
  }

  if (bLoggedIn_) {
    Msg_Xmpp_LoggedOut msg;
    msg.hClient = hAp_;
    msg.Send();

    bLoggedIn_ = 0;
  }

  {
    Msg_Xmpp_Disconnected msg;
    msg.hClient = hAp_;
    msg.Send();
  }

  if (bReconnectAfterDisconnect_) {
    ok = connect();
  }

  return ok;
}

// Parser events

int Client::onParserBegin(Apollo::XMLNode* pNode)
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "<%s ...", _sz(pNode->getName())));

  String sData;
  pNode->outerXml(sData);
  ok = stanzaIn(sData);

  return ok;
}

int Client::onParserStanza(Apollo::XMLNode* pStanza)
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "<%s ...", _sz(pStanza->getName())));

  Msg_Xmpp_StanzaIn msg;
  msg.hClient = hAp_;
  pStanza->outerXml(msg.sData);
  ok = msg.Request();

  return ok;
}

int Client::onParserEnd(const char* szName)
{
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "</%s>", _sz(szName)));

  if (!disconnect()) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "disconnect() failed"));
  }

  return ok;
}

// -------------------------------------------------------------------

int Client::selfPresenceAvailable(Stanza& stanza)
{
  AP_UNUSED_ARG(stanza);
  
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "not really handled"));
  return ok;
}

int Client::selfPresenceUnavailable(Stanza& stanza)
{
  AP_UNUSED_ARG(stanza);
  
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "not really handled"));
  return ok;
}

int Client::selfPresenceError(Stanza& stanza)
{
  AP_UNUSED_ARG(stanza);
  int ok = 1;
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "not really handled"));
  return ok;
}

// -------------------------------------------------------------------

Buddy* Client::findBuddy(const String& szJid)
{
  Buddy* pBuddy = lBuddys_.FindByName(szJid);
  return pBuddy;
}

// -------------------------------------------------------------------

Room* Client::findRoom(const String& szJid)
{
  Room* pRoom = lRooms_.FindByName(szJid);
  return pRoom;
}

Room* Client::findRoom(const ApHandle& hRoom)
{
  Room* pResult = 0;

  for (Room* pRoom = 0; (pRoom = lRooms_.Next(pRoom)) != 0; ) {
    if (pRoom->apHandle() == hRoom) {
      pResult = pRoom;
      break;
    }
  }

  return pResult;
}

ApHandle Client::getRoomHandle(String& sJid)
{
  ApHandle h = ApNoHandle;

  Room* pRoom = findRoom(sJid);
  if (pRoom != 0) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Room unknown: %s", _sz(sJid)));
  } else {
    h = pRoom->apHandle();
  }

  return h;
}

int Client::enterRoom(String& sJid, String& sNickname, const ApHandle& hRoom)
{
  int ok = 0;

  Room* pRoom = findRoom(sJid);
  if (pRoom != 0) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Room already registered: %s", _sz(sJid)));
  } else {
    pRoom = new Room(sJid, hRoom, this);
    if (pRoom != 0) {
      lRooms_.Add(pRoom);
    }
  }

  if (pRoom != 0) {
    ok = pRoom->enter(sNickname);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pRoom->enter() failed " ApHandleFormat " room=%s nick=%s", ApHandlePrintf(hAp_), _sz(pRoom->getJid()), _sz(sNickname)));
    }
  }

  return ok;
}

int Client::enterRoomComplete(const ApHandle& hRoom)
{
  int ok = 0;

  ok = 1;

  return ok;
}

int Client::leaveRoom(const ApHandle& hRoom)
{
  int ok = 1;

  Room* pRoom = findRoom(hRoom);
  if (pRoom == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Room unknown: " ApHandleFormat "", ApHandlePrintf(hRoom)));
  } else {
    ok = pRoom->leave();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pRoom->leave() failed " ApHandleFormat " room=%s", ApHandlePrintf(hAp_), _sz(pRoom->getJid())));
    }
  }

  return ok;
}

int Client::leaveRoomComplete(const ApHandle& hRoom)
{
  int ok = 1;

  Room* pRoom = findRoom(hRoom);
  if (pRoom == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Room unknown: " ApHandleFormat "", ApHandlePrintf(hRoom)));
  } else {
    lRooms_.Remove(pRoom);
    delete pRoom;
    pRoom = 0;
  }

  return ok;
}

int Client::sendGroupchat(const ApHandle& hRoom, String& sText)
{
  int ok = 1;

  Room* pRoom = findRoom(hRoom);
  if (pRoom == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT,  "Room unknown: " ApHandleFormat "", ApHandlePrintf(hRoom)));
  } else {
    ok = pRoom->sendGroupchat(sText);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pRoom->sendGroupchat() failed " ApHandleFormat " room=%s", ApHandlePrintf(hAp_), _sz(pRoom->getJid())));
    }
  }

  return ok;
}

int Client::sendRoomState(const ApHandle& hRoom)
{
  int ok = 1;

  Room* pRoom = findRoom(hRoom);
  if (pRoom == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT,  "Room unknown: " ApHandleFormat "", ApHandlePrintf(hRoom)));
  } else {
    ok = pRoom->sendState();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pRoom->sendState failed " ApHandleFormat " room=%s", ApHandlePrintf(hAp_), _sz(pRoom->getJid())));
    }
  }

  return ok;
}
