// ============================================================================
//
// Apollo
//
// ============================================================================

#include "MessageDigest.h"

#include "Local.h"
#include "ProtocolTasks.h"
#include "XmppRoom.h"
#include "ProtocolDefinitions.h"

XmppStanza::XmppStanza(const char* szName)
:Apollo::XMLNode(szName)
{
}

XmppIQStanza::XmppIQStanza()
:XmppStanza("iq")
{
}

Apollo::XMLNode& XmppIQStanza::AddQuery(const char* szNameSpace)
{
  Apollo::XMLNode& oQuery = AddChildRef("query");
  if (oQuery) {
    oQuery.AddAttribute("xmlns", szNameSpace);
  }
  return oQuery;
}

XmppGetStanza::XmppGetStanza(int nId, const char* szTo)
{
  SString sId; sId.strcatf("%d", nId);
  AddAttribute("id", sId);
  AddAttribute("to", szTo);
  AddAttribute("type", "get");
}

XmppSetStanza::XmppSetStanza(int nId, const char* szTo)
{
  SString sId; sId.strcatf("%d", nId);
  AddAttribute("id", sId);
  AddAttribute("to", szTo);
  AddAttribute("type", "set");
}

// ----------------------------------

XmppPresenceStanza::XmppPresenceStanza(const char* szType)
:XmppStanza("presence")
{
  SString sType = szType;
  if (sType != "available") {
    AddAttribute("type", szType);
  }
}

// ----------------------------------

XmppMessageStanza::XmppMessageStanza(const char* szType)
:XmppStanza("message")
{
  AddAttribute("type", szType);
}

// -------------------------------------------------------------------

int XmppProtocolTask::OnStanza(Apollo::XMLNode* pStanza)
{
  int bConsumed = 0;
  return bConsumed;
}

// -------------------------------------------------------------------

int LoginTask::OnStanza(Apollo::XMLNode* pStanza)
{
  int bConsumed = 0;

  switch(nPhase_) {
  case StreamOpen:
    // <stream:stream xmlns:stream='http://etherx.jabber.org/streams' id='43956666' xmlns='jabber:client' from='user.virtual-presence.org'>
    if (pStanza->Name() == "stream:stream") {
      bConsumed = 1;
      sStreamId_ = pStanza->Attribute("id").Value();
      if (sStreamId_.empty()) {
        apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "<stream:stream> No stream ID"));
      } else {
        SendAuthMechQuery();
        nPhase_ = AuthMechanisms;
      }
    }
    break;
  case AuthMechanisms:
    if (pStanza->Name() == "iq") {
      // <iq id='1' type='result'><query xmlns='jabber:iq:auth'><username>lluna_484de02a54c5</username><digest/><password/><resource/></query></iq>      
      if (nId_ == SSystem::atoi(pStanza->Attribute("id").Value())) {
        bConsumed = 1;
        SString sType = pStanza->Attribute("type").Value();
        if (sType == "error") {
          SString sError; int nError = 0;
          if (pProtocol_->GetStanzaError(pStanza, nError, sError)) {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "jabber:iq:auth error result: %d %s", nError, (char*) sError));
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "jabber:iq:auth unknown error result"));
          }
        } else {
          Apollo::XMLNode& oQuery = pStanza->ChildRef("query");
          Apollo::XMLNode& oDigest = oQuery.ChildRef("digest");
          Apollo::XMLNode& oPassword = oQuery.ChildRef("password");
          Apollo::XMLNode& oResource = oQuery.ChildRef("resource");
          if (oDigest) {
            SendDigestAuth();
            nPhase_ = DigestAuth;
          } else if (oPassword) {
            SendPasswordAuth();
            nPhase_ = PasswordAuth;
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "jabber:iq:auth result: neither digest nor password auth available"));
          }
        }
      }
    }
    break;
  case DigestAuth:
    if (pStanza->Name() == "iq") {
      // <iq id='2' type='result'/>
      if (nId_ == SSystem::atoi(pStanza->Attribute("id").Value())) {
        bConsumed = 1;
        SString sType = pStanza->Attribute("type").Value();
        if (sType == "error") {
          SString sError; int nError = 0;
          if (pProtocol_->GetStanzaError(pStanza, nError, sError)) {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "DigestAuth error result: %d %s", nError, (char*) sError));
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "DigestAuth unknown error result"));
          }
        } else {
          apLog_Info((LOG_CHANNEL, "LoginTask::OnStanza", "Logged in"));
          nPhase_ = LoggedIn;
        }
      }
    }
    break;
  case PasswordAuth:
    if (pStanza->Name() == "iq") {
      // <iq id='2' type='result'/>
      if (nId_ == SSystem::atoi(pStanza->Attribute("id").Value())) {
        bConsumed = 1;
        SString sType = pStanza->Attribute("type").Value();
        if (sType == "error") {
          SString sError; int nError = 0;
          if (pProtocol_->GetStanzaError(pStanza, nError, sError)) {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "PasswordAuth error result: %d %s", nError, (char*) sError));
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "PasswordAuth unknown error result"));
          }
        } else {
          apLog_Info((LOG_CHANNEL, "LoginTask::OnStanza", "Logged in"));
          nPhase_ = LoggedIn;
        }
      }
    }
    break;
  default:
    apLog_Error((LOG_CHANNEL, "LoginTask::OnStanza", "Unknown phase"));
  }

  if (nPhase_ == LoggedIn) {
    pProtocol_->OnLoggedIn();
    bDone_ = 1;
  }

  return bConsumed;
}

int LoginTask::SendAuthMechQuery()
{
  // #login possibilities:
  // <iq id='1' to='user.virtual-presence.org' type='get'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>lluna_484de02a54c5</username>
  //   </query>
  // </iq>
  nId_ = pProtocol_->NextId();
  XmppGetStanza oRequest(nId_, pProtocol_->jid_.Host());
  Apollo::XMLNode& oQuery = oRequest.AddQuery(JABBER_NS_AUTH);

  Apollo::XMLNode& oUsername = oQuery.AddChildRef("username");
  oUsername.CData(pProtocol_->jid_.User());

  pProtocol_->OnStanzaOut(&oRequest);

  return 1;
}

int LoginTask::SendDigestAuth()
{
  // #login: stream-id(43956666) . password(a) = '43956666a' -> sha1('43956666a') = 'f944d75c9cd15c106b69f5795b0171fe86b7c182'
  // <iq id='2' type='set'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>lluna_484de02a54c5</username>
  //     <digest>f944d75c9cd15c106b69f5795b0171fe86b7c182</digest>
  //     <resource>lluna_43a83b65a57e</resource>
  //   </query>
  // </iq>
  SString sToken = sStreamId_; 
  sToken += pProtocol_->sPassword_;
  Apollo::MessageDigest sha1((unsigned char*) sToken.c_str(), sToken.bytes());
  sToken = sha1.Get_SHA1Hex();

  nId_ = pProtocol_->NextId();
  XmppSetStanza oRequest(nId_, pProtocol_->jid_.Host());
  Apollo::XMLNode& oQuery = oRequest.AddQuery(JABBER_NS_AUTH);

  Apollo::XMLNode& oUsername = oQuery.AddChildRef("username");
  oUsername.CData(pProtocol_->jid_.User());

  Apollo::XMLNode& oDigest = oQuery.AddChildRef("digest");
  oDigest.CData(sToken);

  Apollo::XMLNode& oResource = oQuery.AddChildRef("resource");
  oResource.CData(pProtocol_->sResource_);

  pProtocol_->OnStanzaOut(&oRequest);

  return 1;
}

int LoginTask::SendPasswordAuth()
{
  // <iq id='2' type='set'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>lluna_484de02a54c5</username>
  //     <password>gaga123</password>
  //     <resource>lluna_43a83b65a57e</resource>
  //   </query>
  // </iq>
  nId_ = pProtocol_->NextId();
  XmppSetStanza oRequest(nId_, pProtocol_->jid_.Host());
  Apollo::XMLNode& oQuery = oRequest.AddQuery(JABBER_NS_AUTH);

  Apollo::XMLNode& oUsername = oQuery.AddChildRef("username");
  oUsername.CData(pProtocol_->jid_.User());
  Apollo::XMLNode& oPassword = oQuery.AddChildRef("password");
  oPassword.CData(pProtocol_->sPassword_);
  Apollo::XMLNode& oResource = oQuery.AddChildRef("resource");
  oResource.CData(pProtocol_->sResource_);

  pProtocol_->OnStanzaOut(&oRequest);

  return 1;
}

// -------------------------------------------------------------------

int OnlinePresenceTask::OnStanza(Apollo::XMLNode* pStanza)
{
  // Self presence from other client
  // <presence from='wolfspelz@jabber.bluehands.de/spock-exodus' to='wolfspelz@jabber.org/0b281bd8c075'>
  //   <c node='http://exodus.jabberstudio.org/caps' ver='0.9.1.0' xmlns='http://jabber.org/protocol/caps'/>
  //   <x xmlns='vcard-temp:x:update'><photo>500e3f1574601eae394ed8613f79a314fcaad9d7</photo></x>
  //   <x xmlns='jabber:x:avatar'><hash>500e3f1574601eae394ed8613f79a314fcaad9d7</hash></x>
  //   <status>XA as a result of idle.</status>
  //   <show>xa</show>
  //   <priority>0</priority>
  //   <x xmlns='jabber:x:delay' from='wolfspelz@jabber.bluehands.de/spock-exodus' stamp='20060318T17:56:19'/>
  // </presence>

  int bConsumed = 0;

  if (pStanza->Name() == "presence") {
    apLog_Debug((LOG_CHANNEL, "OnlinePresenceTask::OnStanza", "Consuming stanza"));

    JabberId sFrom = pStanza->Attribute("from").Value();

    XmppRoom* pRoom = pProtocol_->FindRoom(sFrom.Base());
    if (pRoom == 0) {
      bConsumed = 1;

      // do normal presence stuff

    }

  }

  return bConsumed;
}

// -------------------------------------------------------------------

int RoomPresenceTask::OnStanza(Apollo::XMLNode* pStanza)
{
  // Self presence from other client
  // <presence from='wolfspelz@jabber.bluehands.de/spock-exodus' to='wolfspelz@jabber.org/0b281bd8c075'>
  //   <c node='http://exodus.jabberstudio.org/caps' ver='0.9.1.0' xmlns='http://jabber.org/protocol/caps'/>
  //   <x xmlns='vcard-temp:x:update'><photo>500e3f1574601eae394ed8613f79a314fcaad9d7</photo></x>
  //   <x xmlns='jabber:x:avatar'><hash>500e3f1574601eae394ed8613f79a314fcaad9d7</hash></x>
  //   <status>XA as a result of idle.</status>
  //   <show>xa</show>
  //   <priority>0</priority>
  //   <x xmlns='jabber:x:delay' from='wolfspelz@jabber.bluehands.de/spock-exodus' stamp='20060318T17:56:19'/>
  // </presence>

  int bConsumed = 0;

  if (pStanza->Name() == "presence") {
    apLog_Debug((LOG_CHANNEL, "RoomPresenceTask::OnStanza", "Consuming stanza"));

    JabberId sFrom = pStanza->Attribute("from").Value();

    XmppRoom* pRoom = pProtocol_->FindRoom(sFrom.Base());
    if (pRoom != 0) {
      bConsumed = 1;
      pRoom->OnPresence(pStanza);
    }

  }

  return bConsumed;
}

// -------------------------------------------------------------------

int GroupChatTask::OnStanza(Apollo::XMLNode* pStanza)
{
  int bConsumed = 0;

  if (pStanza->Name() == "message") {
    apLog_Debug((LOG_CHANNEL, "GroupChatTask::OnStanza", "Consuming stanza"));

    JabberId sFrom = pStanza->Attribute("from").Value();

    XmppRoom* pRoom = pProtocol_->FindRoom(sFrom.Base());
    if (pRoom != 0) {
      bConsumed = 1;
      pRoom->OnGroupChat(pStanza);
    }

  }

  return bConsumed;
}

