// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"

#include "Local.h"
#include "XmppRoom.h"
#include "RoomVisitor.h"
#include "ProtocolDefinitions.h"

XmppRoom::XmppRoom(XmppProtocol* pProtocol, const char* szJID, ApHandle hLocation)
:SElem(szJID)
,pProtocol_(pProtocol)
,nPhase_(Phase_None)
,hLocation_(hLocation)
,nNicknameConflictRetryCount_(0)
{
}

XmppRoom::~XmppRoom()
{
}

int XmppRoom::Enter(const char* szNickname)
{
  sNick_ = szNickname;

  XmppPresenceStanza oStanza("available");

  SString sJID = Name();
  sJID += "/";
  sJID += sNick_;
  oStanza.AddAttribute("to", sJID);

  nPhase_ = Phase_EnterRequested;

  return pProtocol_->OnStanzaOut(&oStanza);  
}

int XmppRoom::EnterRetryNextNickname(const char* szConflictedNickname)
{
  int ok = 0;

  if (nNicknameConflictRetryCount_ > 20) {
    apLog_Error((LOG_CHANNEL, "XmppRoom::EnterRetryNextNickname", "Nickname conflict: too many retries: giving up, %s %s", (char*) Name()));
  } else {
    nNicknameConflictRetryCount_++;

    SString sNewNickname = sNick_;

    SString sTail;
    int bDone = 0;
    while (!bDone) {
      if (sNewNickname.chars() <= 0) {
        bDone = 1;
      } else {
        SString sChar = sNewNickname.substr(sNick_.chars() - 1, 1);
        const char* p = sChar.c_str();
        if (::isdigit(*p)) {
          sTail = sChar + sTail;
          sNewNickname.substr(0, sNick_.chars() - 1);
        } else {
          bDone = 1;
        }
      }
    }

    int nNum = SSystem::atoi(sTail);
    nNum++;

    sNewNickname.strcatf("%d", nNum);

    ok = Enter(sNewNickname);
  }

  return ok;
}

int XmppRoom::Leave()
{
  XmppPresenceStanza oStanza("unavailable");

  SString sJID = Name();
  sJID += "/";
  sJID += sNick_;
  oStanza.AddAttribute("to", sJID);

  nPhase_ = Phase_LeaveRequested;

  return pProtocol_->OnStanzaOut(&oStanza);  
}

int XmppRoom::SendPublicChat(const char* szText)
{
  XmppMessageStanza oStanza("groupchat");

  SString sTo = Name();
  oStanza.AddAttribute("to", sTo);

  Apollo::XMLNode& oQuery = oStanza.AddChildRef("body");
  if (oQuery) {
    oQuery.CData(szText);
  }

  return pProtocol_->OnStanzaOut(&oStanza);  
}

int XmppRoom::SendPrivateChat(const char* szName, const char* szText)
{
  XmppMessageStanza oStanza("chat");

  SString sTo = Name();
  sTo += "/";
  sTo += szName;
  oStanza.AddAttribute("to", sTo);

  Apollo::XMLNode& oQuery = oStanza.AddChildRef("body");
  if (oQuery) {
    oQuery.CData(szText);
  }

  return pProtocol_->OnStanzaOut(&oStanza);  
}

int XmppRoom::OnPresence(Apollo::XMLNode* pStanza)
{
  // <presence from='org.341973964fa3faf9c1d6e8c9255bb3fd84fcb005@location.virtual-presence.org/Fitzliputz' to='wolfspelz@jabber.org/765a19041d7b'>
  //   <x xmlns='http://jabber.org/protocol/muc#user'>
  //     <item jid='wolfspelz@jabber.org/765a19041d7b' affiliation='owner' role='moderator'/>
  //   </x>
  // </presence>

  // <presence from='google-moderated@location.virtual-presence.org/Fitzliputz' to='wolfspelz@jabber.org/f79d523a0fa3'>
  //   <x xmlns='firebat:user:jid'>fitzliputz@user.vp.bluehands.de/1e7a2f95befb</x>
  //   <x xmlns='firebat:avatar:position'><position x='350' w='990' opt='' /></x>
  //   <x xmlns='firebat:avatar:digest'>0427e5f74b9fccf0d45039140eaefa7b6e9eec6d</x>
  //   <x xmlns='http://jabber.org/protocol/muc#user'>
  //     <item jid='fitzliputz@user.vp.bluehands.de/1e7a2f95befb' affiliation='none' role='participant' />
  //   </x>
  // </presence>

  int bDeleteRoom = 0;

  SString sType = pStanza->Attribute("type").Value();
  if (sType.empty()) { sType = "available"; }

  if (nPhase_ == Phase_None) {
  }

  if (nPhase_ == Phase_EnterRequested) {
    if (sType == "available") {
      nPhase_ = Phase_Entered;
/*hw
      Msg_Protocol_EnterLocationDone msg;
      msg.hLocation = hLocation_;
      msg.Send();
*/
    } else {
      if (sType == "error") {
        int bNickConflict = 0;

        // Nickname conflict:
        // <presence from='google-moderated@location.virtual-presence.org/Fitzliputz' to='wolfspelz@jabber.org/5a0c71551128' sType='error'>
        //   <error code='409' type='cancel'>
        //     <conflict xmlns='urn:ietf:params:xml:ns:xmpp-stanzas' />
        //     <text xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'>Please choose a different nickname.</text>
        //   </error>
        // </presence>

        SString sError; int nError = 0;
        if (pProtocol_->GetStanzaError(pStanza, nError, sError)) {
          if (nError == JABBER_ERRORCODE_Conflict) {
            bNickConflict = 1;
          } else {
            apLog_Error((LOG_CHANNEL, "XmppRoom::OnPresence", "Error result during Phase_EnterRequested %s %d %s", (char*) Name(), nError, (char*) sError));
          }
        } else {
          apLog_Error((LOG_CHANNEL, "XmppRoom::OnPresence", "Unknown error result during Phase_EnterRequested %s", (char*) Name()));
        }

        int bNickConflictRetryOk = 0;
        if (bNickConflict) {
          JabberId jidFrom = pStanza->Attribute("from").Value();
          SString sConflictedNickname = jidFrom.Resource();
          apLog_Verbose((LOG_CHANNEL, "XmppRoom::OnPresence", "Nickname conflict, trying different nick %s", (char*) Name()));
          bNickConflictRetryOk = EnterRetryNextNickname(sConflictedNickname);
          if (!bNickConflictRetryOk) {
            apLog_Error((LOG_CHANNEL, "XmppRoom::OnPresence", "EnterRetryNextNickname failed %s", (char*) Name()));
          }
        }

        if ((bNickConflict && !bNickConflictRetryOk) || !bNickConflict) {
          nPhase_ = Phase_None;
          bDeleteRoom = 1;
/*hw
          Msg_Protocol_EnterLocationDone msg;
          msg.hLocation = hLocation_;
          msg.bSuccess = 0;
          msg.Send();
*/
        }

      } else {
        apLog_Warning((LOG_CHANNEL, "XmppRoom::OnPresence", "Got sType=%s during Phase_EnterRequested %s", (char*) sType, (char*) Name()));
      }

    }
  }

  if (nPhase_ == Phase_Entered) {
    JabberId jidFrom = pStanza->Attribute("from").Value();
    SString sVisitor = jidFrom.Resource();

    if (sType == "available") {
      RoomVisitor* pVisitor = (RoomVisitor*) slVisitors_.FindByName(sVisitor);
      if (pVisitor == 0) {
        ApHandle hVisitor = Apollo::NewHandle();
        pVisitor = new RoomVisitor(this, hVisitor, sVisitor);
        if (pVisitor != 0) {
          slVisitors_.Add(pVisitor);
/*hw
          Msg_VP_VisitorEnter msg;
          msg.hLocation = hLocation_;
          msg.hVisitor = hVisitor;
          msg.sVisitor = sVisitor;
          msg.Send();
*/
        }
      }

      if (pVisitor != 0) {
        pVisitor->OnPresence(pStanza);
      }
    }

    if (sType == "unavailable") {
      RoomVisitor* pVisitor = (RoomVisitor*) slVisitors_.FindByName(sVisitor);
      if (pVisitor != 0) {
/*hw
        Msg_VP_VisitorLeave msg;
        msg.hLocation = hLocation_;
        msg.hVisitor = pVisitor->hVisitor_;
        msg.sVisitor = sVisitor;
        msg.Send();
*/
        slVisitors_.Remove(pVisitor);
        delete pVisitor;
      }
    }

  }

  if (nPhase_ == Phase_LeaveRequested) {
    if (sType == "unavailable") {
      nPhase_ = Phase_None;

      for (RoomVisitor* pVisitor = 0; (pVisitor = (RoomVisitor*) slVisitors_.Next(pVisitor)) != 0; ) {
/*hw
        Msg_VP_VisitorLeave msg;
        msg.hLocation = hLocation_;
        msg.hVisitor = pVisitor->hVisitor_;
        msg.sVisitor = pVisitor->Name();
        msg.Send();
*/
      }
/*hw
      Msg_Protocol_LeaveLocationDone msg;
      msg.hLocation = hLocation_;
      msg.Send();
*/
      bDeleteRoom = 1;

    } else {
      apLog_Warning((LOG_CHANNEL, "XmppRoom::OnPresence", "Got sType=%s during Phase_LeaveRequested %s", (char*) sType, (char*) Name()));
    }
  }

  if (bDeleteRoom) {
    pProtocol_->DeleteRoom(this); // deletes this
  }

  return 1;
}

int XmppRoom::OnGroupChat(Apollo::XMLNode* pStanza)
{
  JabberId jidFrom = pStanza->Attribute("from").Value();
  SString sVisitor = jidFrom.Resource();

  if (sVisitor.empty()) {
    OnSystemChat(pStanza);
  } else {
    RoomVisitor* pVisitor = (RoomVisitor*) slVisitors_.FindByName(sVisitor);
    if (pVisitor != 0) {
      pVisitor->OnGroupChat(pStanza);
    }
  }

  return 1;
}

int XmppRoom::OnSystemChat(Apollo::XMLNode* pStanza)
{
  {
    SString sSubject;
    Apollo::XMLNode& oSubject = pStanza->ChildRef("subject");
    if (oSubject) {
      sSubject = oSubject.CData();
    }
/*hw
    Msg_Protocol_ReceivedChatChannelSubject msg;
    msg.hLocation = hLocation_;
    msg.sText = sSubject;
    msg.Send();
*/
  }
  
  {
    SString sBody;
    Apollo::XMLNode& oBody = pStanza->ChildRef("body");
    if (oBody) {
      sBody = oBody.CData();
    }
/*hw
    Msg_Protocol_ReceivedChatChannelMessage msg;
    msg.hLocation = hLocation_;
    msg.sText = sBody;
    msg.Send();
*/
  }
  
  return 1;
}
