// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgXmpp.h"
#include "MsgProtocol.h"
#include "Client.h"
#include "Room.h"
#include "Stanza.h"
#include "Protocol.h"

Room::Room(const char* szJid, const ApHandle& hRoom, Client* pClient)
:Elem(szJid)
,hAp_(hRoom)
,pClient_(pClient)
,nPhase_()
,nNicknameConflictRetryCount_(0)
{
}

Room::~Room()
{
}

String Room::nextNickname(String& sNickname)
{
  String sNewNickname = sNickname;

  String sTail;
  int bDone = 0;
  while (!bDone) {
    if (sNewNickname.chars() <= 0) {
      bDone = 1;
    } else {
      String sChar = sNewNickname.subString(sNewNickname.chars() - 1, 1);
      if (String::isDigit(sChar)) {
        sTail = sChar + sTail;
        sNewNickname = sNewNickname.subString(0, sNewNickname.chars() - 1);
      } else {
        bDone = 1;
      }
    }
  }

  int nNum = String::atoi(sTail);
  nNum++;

  sNewNickname.appendf("%d", nNum);

  return sNewNickname;
}

int Room::sendState()
{
  int ok = 1;

  String sTo = getJid();
  sTo += "/";
  sTo += sNickname_;

  PresenceStanza presence(JABBER_PRESENCE_AVAILABLE, sTo);

  Msg_Protocol_Room_GetIdentity msgPGRI;
  msgPGRI.hRoom = apHandle();
  msgPGRI.Filter();

  if (!msgPGRI.sUrl.empty() && !msgPGRI.sDigest.empty()) {
    Apollo::XMLNode* pIdentity = presence.addChild("x");
    if (pIdentity) {

      if (Apollo::getModuleConfig(MODULE_NAME, "Room/SendFbUserIdentity", 0)) {
        pIdentity->addAttribute("xmlns", NS_PRESENCE_X_USER_IDENTITY_LEGACY);
        if (msgPGRI.sId) { pIdentity->addAttribute("id", msgPGRI.sId); }
        if (Apollo::getModuleConfig(MODULE_NAME, "Room/SendJidInIdentityExtension", 0)) {
          pIdentity->addAttribute("jid", pClient_->getJabberId()); // deprecated
        }
        pIdentity->addAttribute("src", msgPGRI.sUrl);
        pIdentity->addAttribute("digest", msgPGRI.sDigest);
      }

      if (Apollo::getModuleConfig(MODULE_NAME, "Room/SendVpIdentity", 1)) {
        pIdentity->addAttribute("xmlns", NS_PRESENCE_X_IDENTITY);
        if (msgPGRI.sId) { pIdentity->addAttribute("id", msgPGRI.sId); }
        pIdentity->addAttribute("src", msgPGRI.sUrl);
        pIdentity->addAttribute("digest", msgPGRI.sDigest);
      }

    }
  }

  Msg_Protocol_Room_GetPosition msgPRGP;
  msgPRGP.hRoom = apHandle();
  msgPRGP.Filter();

  Msg_Protocol_Room_GetCondition msgPRGC;
  msgPRGC.hRoom = apHandle();
  msgPRGC.Filter();

  if (msgPRGP.kvParams.length() > 0 || msgPRGC.kvParams.length() > 0) {
    if (Apollo::getModuleConfig(MODULE_NAME, "Room/SendFbAvatarState", 0)) {
      Apollo::XMLNode* pState = presence.addChild("x");
      if (pState) {
        pState->addAttribute("xmlns", NS_PRESENCE_X_AVATAR_STATE_LEGACY);
        if (msgPRGP.kvParams.length() > 0) {
          Apollo::XMLNode* pPosition = pState->addChild("position");
          if (pPosition) {
            for (Apollo::KeyValueElem* e = 0; (e = msgPRGP.kvParams.nextElem(e)) != 0; ) {
              pPosition->addAttribute(e->getKey(), e->getString());
            }
          }
        }
        if (msgPRGC.kvParams.length() > 0) {
          Apollo::XMLNode* pCondition = pState->addChild("condition");
          if (pCondition) {
            for (Apollo::KeyValueElem* e = 0; (e = msgPRGC.kvParams.nextElem(e)) != 0; ) {
              pCondition->addAttribute(e->getKey(), e->getString());
            }
          }
        }
      }
    }
    if (Apollo::getModuleConfig(MODULE_NAME, "Room/SendVpState", 1)) {
      Apollo::XMLNode* pState = presence.addChild("x");
      if (pState) {
        pState->addAttribute("xmlns", NS_PRESENCE_X_STATE);
        if (msgPRGP.kvParams.length() > 0) {
          Apollo::XMLNode* pPosition = pState->addChild("position");
          if (pPosition) {
            for (Apollo::KeyValueElem* e = 0; (e = msgPRGP.kvParams.nextElem(e)) != 0; ) {
              pPosition->addAttribute(e->getKey(), e->getString());
            }
          }
        }
        if (msgPRGC.kvParams.length() > 0) {
          Apollo::XMLNode* pCondition = pState->addChild("condition");
          if (pCondition) {
            for (Apollo::KeyValueElem* e = 0; (e = msgPRGC.kvParams.nextElem(e)) != 0; ) {
              pCondition->addAttribute(e->getKey(), e->getString());
            }
          }
        }
      }
    }
  }

  ok = pClient_->sendStanza(presence);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pClient_->sendStanza() failed " ApHandleFormat "", ApHandlePrintf(hAp_)));
  }

  return ok;
}

int Room::enter(String& sNickname)
{
  int ok = 1;

  sNickname_ = sNickname;

  ok = sendState();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "sendState() failed " ApHandleFormat " nick=%s", ApHandlePrintf(hAp_), _sz(sNickname)));
  } else {
    nPhase_ = Phase_EnterRequested;
  }

  return ok;
}

int Room::enterRetryNextNickname(String& sConflictedNickname)
{
  int ok = 0;

  if (nNicknameConflictRetryCount_ > Apollo::getModuleConfig(MODULE_NAME, "Room/NicknameConflictRetry", 20)) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Nickname conflict: too many retries: giving up, %s", _sz(getName())));
  } else {
    nNicknameConflictRetryCount_++;
    String sNextNickname = nextNickname(sConflictedNickname); // getNickname() may or may not be correct here, use the parameter
    ok = enter(sNextNickname);
  }

  return ok;
}

int Room::leave()
{
  int ok = 1;

  String sTo = getJid();
  sTo += "/";
  sTo += sNickname_;

  PresenceStanza presence(JABBER_PRESENCE_UNAVAILABLE, sTo);
  ok = pClient_->sendStanza(presence);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pClient_->sendStanza() failed " ApHandleFormat "", ApHandlePrintf(hAp_)));
  } else {
    nPhase_ = Phase_LeaveRequested;
  }

  return ok;
}

int Room::presenceAvailable(Stanza& stanza)
{
  int ok = 1;

  // If its the first one, then we take it as a confirmation, that entering has started
  if (lParticipants_.length() == 0) {
    nPhase_ = Phase_EnterBegun;

    Msg_Xmpp_EnteringRoom msg;
    msg.hClient = pClient_->apHandle();
    msg.hRoom = apHandle();
    msg.Send();
  }

  JabberId from = stanza.getAttribute("from").getValue();
  String sResource = from.resource();

  // Check if participant is already known, create one if not
  int bIsNew = 0;
  Participant* pParticipant = lParticipants_.FindByName(sResource);
  if (pParticipant == 0) {
    pParticipant = new Participant(sResource, Apollo::newHandle(), pClient_, this);
    if (pParticipant != 0) {
      bIsNew = 1;

      lParticipants_.AddFirst(pParticipant);

      if (sResource == getNickname()) {
        // Send this before Msg_Xmpp_ParticipantEntered
        // so, that VpModule::Location::addParticipant already knows it
        Msg_Xmpp_ParticipantSelf msg;
        msg.hClient = pClient_->apHandle();
        msg.hRoom = apHandle();
        msg.hParticipant = pParticipant->apHandle();
        msg.Send();
      }

      Msg_Xmpp_ParticipantEntered msg;
      msg.hClient = pClient_->apHandle();
      msg.hRoom = apHandle();
      msg.hParticipant = pParticipant->apHandle();
      msg.sNickname = sResource;
      msg.Send();
    }
  }

  // Evaluate presence stanza child nodes
  if (pParticipant != 0) {
    pParticipant->presence(stanza);
  }

  // If its my own nickname, then we are completely entered
  if (bIsNew) {
    if (sResource == getNickname()) {
      nPhase_ = Phase_Entered;

      {
        Msg_Xmpp_EnterRoomComplete msg;
        msg.hClient = pClient_->apHandle();
        msg.hRoom = apHandle();
        msg.sNickname = sNickname_;
        msg.Send();
      }
    }
  }

  //leave();

  return ok;
}

int Room::presenceUnavailable(Stanza& stanza)
{
  int ok = 1;

  JabberId from = stanza.getAttribute("from").getValue();
  String sResource = from.resource();

  if (sResource != getNickname()) {
    // Someone else is leaving

    Participant* pParticipant = lParticipants_.FindByName(sResource);
    if (pParticipant != 0) {
      Msg_Xmpp_ParticipantLeft msg;
      msg.hClient = pClient_->apHandle();
      msg.hRoom = apHandle();
      msg.hParticipant = pParticipant->apHandle();
      msg.sNickname = pParticipant->getName();
      msg.Send();

      lParticipants_.Remove(pParticipant);
      delete pParticipant;
      pParticipant = 0;
    }
  
  } else {
    // I am leaving

    {
      Msg_Xmpp_LeavingRoom msg;
      msg.hClient = pClient_->apHandle();
      msg.hRoom = apHandle();
      msg.Send();
    }

    Participant* pParticipant = 0;
    while ((pParticipant = lParticipants_.First())) {
      Msg_Xmpp_ParticipantLeft msg;
      msg.hClient = pClient_->apHandle();
      msg.hRoom = apHandle();
      msg.hParticipant = pParticipant->apHandle();
      msg.sNickname = pParticipant->getName();
      msg.Send();

      lParticipants_.Remove(pParticipant);
      delete pParticipant;
      pParticipant = 0;
    }

    nPhase_ = Phase_Left;

    {
      Msg_Xmpp_LeaveRoomComplete msg;
      msg.hClient = pClient_->apHandle();
      msg.hRoom = apHandle();
      msg.Send();
      // THE ROOM MAY BE DELETED AFTER THIS
    }
  }

  return ok;
}

int Room::presenceError(Stanza& stanza)
{
  int ok = 1;

  String sError;
  int nError = 0;
  stanza.getError(nError, sError);

  if (nError == JABBER_ERRORCODE_Conflict) {
    ok = enterRetryNextNickname(sNickname_);
  }

  return ok;
}

int Room::sendGroupchat(String& sText)
{
  int ok = 1;

  String sTo = getJid();
  GroupchatMessageStanza message(sTo);
  message.addText(sText);
  ok = pClient_->sendStanza(message);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "failed " ApHandleFormat "", ApHandlePrintf(hAp_)));
  }

  return ok;
}

int Room::parseXEP0082DateTime(const String& sStamp, Apollo::TimeValue& tvStamp)
{
  int ok = 0;

  // CCYY-MM-DDThh:mm:ss[.sss]TZD

  // CCYY-MM-DDThh:mm:ss[.sss]Z
  // CCYY-MM-DDThh:mm:ss[.sss]+GG[:gg]
  // CCYY-MM-DDThh:mm:ss[.sss]-GG[:gg]
  
  int nUtcOffsetSign = 0;
  int nUtcOffsetHour = 0;
  int nUtcOffsetMinute = 0;
  time_t tTime = 0;

  String sDateTime = sStamp;
  String sDate;
  String sYear;
  String sMonth;
  String sDay;
  String sTime;
  String sHour;
  String sMinute;
  String sSeconds;
  String sSecond;
  String sSecondFraction;
  String sTzd;
  String sTzdHour;
  String sTzdMinute;

  sDateTime.nextToken("T", sDate); // CCYY-MM-DD
  sDate.nextToken("-", sYear);
  sDate.nextToken("-", sMonth);
  sDate.nextToken("X", sDay);

  if (sDateTime.contains("Z")) {
    sDateTime.nextToken("Z", sTime); // hh:mm:ss[.sss]
  } else {
    if (0) {
    } else if (sDateTime.contains("+")) {
      nUtcOffsetSign = 1;
      sDateTime.nextToken("+", sTime); // hh:mm:ss[.sss]
      sTzd = sDateTime;
    } else if (sDateTime.contains("-")) {
      nUtcOffsetSign = -1;
      sDateTime.nextToken("-", sTime); // hh:mm:ss[.sss]
      sTzd = sDateTime;
    } else {
      sDateTime.nextToken("X", sTime); // hh:mm:ss[.sss]
    }
  }
  sTime.nextToken(":", sHour);
  sTime.nextToken(":", sMinute);
  sTime.nextToken("X", sSeconds);
  if (sSeconds.contains(".")) {
    sSeconds.nextToken(".", sSecond);
    sSeconds.nextToken("X", sSecondFraction);
  } else {
    sSecond = sSeconds;
  }
  if (sTzd) {
    sTzd.nextToken(":", sTzdHour);
    sTzd.nextToken("X", sTzdMinute);
    nUtcOffsetHour = String::atoi(sTzdHour);
    nUtcOffsetMinute = String::atoi(sTzdMinute);
  }

  if (!sYear.empty() && !sMonth.empty() && !sDay.empty() && !sHour.empty() && !sMinute.empty() && !sSecond.empty()) {

    struct tm tms;
    tms.tm_year = String::atoi(sYear);
    tms.tm_mon = String::atoi(sMonth);
    tms.tm_mday = String::atoi(sDay);
    tms.tm_hour = String::atoi(sHour);
    tms.tm_min = String::atoi(sMinute);
    tms.tm_sec = String::atoi(sSecond);
    tms.tm_isdst = 0;
    tms.tm_wday = 0;
    tms.tm_yday = 0;

    tms.tm_mon--;
    if (tms.tm_year >= 1900) { tms.tm_year -= 1900; }

    tTime = ::mktime(&tms);
    if (tTime != -1) {

      // mktime interprets struct tm as local time, converts and shifts (?) to UTC
      // But we regarded the input as UTC. The result is now shifted although it should not be.
      // I suppose the reason is, that mktime is made for local calendar display, not for converting UTC date time strings.
      // The timezone diff to UTC has been added (e.g. Berlin +1h). Therefore, shift back by the timezone offset.
      long nTimezone = 0;
      _get_timezone(&nTimezone);
      tTime -= nTimezone;

      tTime -= nUtcOffsetSign * (nUtcOffsetHour * 3600 + nUtcOffsetMinute * 60);
      int nMicroSec = 0;
      if (sSecondFraction) {
        sSecondFraction = sSecondFraction.subString(0, 6);
        nMicroSec = String::atoi(sSecondFraction);
        int nScale = 6 - sSecondFraction.chars();
        if (nScale > 0) {
          for (int i = 0; i < nScale; i++) {
            nMicroSec *= 10;
          }
        }
      }

      tvStamp = Apollo::TimeValue(tTime, nMicroSec);
      ok = 1;
    }

  } // !sYear.empty(), etc.

  return ok;
}

int Room::parseXEP0091DateTime(const String& sStamp, Apollo::TimeValue& tvStamp)
{
  int ok = 0;

  // CCYYMMDDThh:mm:ss
  
  int nUtcOffsetSign = 0;
  int nUtcOffsetHour = 0;
  int nUtcOffsetMinute = 0;
  time_t tTime = 0;

  String sDateTime = sStamp;
  String sDate;
  String sYear;
  String sMonth;
  String sDay;
  String sTime;
  String sHour;
  String sMinute;
  String sSecond;

  sDateTime.nextToken("T", sDate); // CCYYMMDD
  sYear = sDate.subString(0, 4);
  sMonth = sDate.subString(4, 2);
  sDay = sDate.subString(6, 2);

  sTime = sDateTime;
  sTime.nextToken(":", sHour);
  sTime.nextToken(":", sMinute);
  sTime.nextToken("X", sSecond);

  if (!sYear.empty() && !sMonth.empty() && !sDay.empty() && !sHour.empty() && !sMinute.empty() && !sSecond.empty()) {

    struct tm tms;
    tms.tm_year = String::atoi(sYear);
    tms.tm_mon = String::atoi(sMonth);
    tms.tm_mday = String::atoi(sDay);
    tms.tm_hour = String::atoi(sHour);
    tms.tm_min = String::atoi(sMinute);
    tms.tm_sec = String::atoi(sSecond);
    tms.tm_isdst = 0;
    tms.tm_wday = 0;
    tms.tm_yday = 0;

    tms.tm_mon--;
    if (tms.tm_year >= 1900) { tms.tm_year -= 1900; }

    tTime = ::mktime(&tms);
    if (tTime != -1) {

      // mktime interprets struct tm as local time, converts and shifts (?) to UTC
      // But we regarded the input as UTC. The result is now shifted although it should not be.
      // I suppose the reason is, that mktime is made for local calendar display, not for converting UTC date time strings.
      // The timezone diff to UTC has been added (e.g. Berlin +1h). Therefore, shift back by the timezone offset.
      long nTimezone = 0;
      _get_timezone(&nTimezone);
      tTime -= nTimezone;

      tvStamp = Apollo::TimeValue(tTime, 0);
      ok = 1;
    }

  } // !sYear.empty(), etc.

  return ok;
}

int Room::timeFromDelayNode(Apollo::XMLNode& delay, Apollo::TimeValue& tvStamp)
{
  int ok = 0;

  if (0) {
  } else if (delay.getAttribute("xmlns").getValue() == JABBER_NS_DELAY2) {
    String sStamp = delay.getAttribute("stamp").getValue();
    ok = parseXEP0082DateTime(sStamp, tvStamp);

  } else if (delay.getAttribute("xmlns").getValue() == JABBER_NS_DELAY) {
    String sStamp = delay.getAttribute("stamp").getValue();
    ok = parseXEP0091DateTime(sStamp, tvStamp);

  }

  return ok;
}

//<message 
//  from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Tassadar' 
//  to='test@wolfspelz.de/ApolloTest' 
//  type='groupchat'
//  >
//  <body>1</body>
//  <delay 
//    xmlns='urn:xmpp:delay' 
//    from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Tassadar' 
//    stamp='2009-02-13T23:31:30Z'
//  />
//</message>

//<message 
//  from='f3eb97d18dd549fa7f460fff2747f36205395f0f@muc4.virtual-presence.org/Tassadar1' 
//  to='test@wolfspelz.de/c415ed189e61fbe999d5' 
//  type='groupchat' 
//  xml:lang='de'>
//  <body>test2</body>
//  <x xmlns='jabber:x:delay' stamp='20090925T13:29:04'/>
//</message>


int Room::receiveGroupchat(Stanza& stanza)
{
  int ok = 1;

  JabberId from = stanza.getAttribute("from").getValue();
  String sResource = from.resource();
  String sText;

  Apollo::XMLNode& body = stanza.getChildRef("body");
  if (body) {
    sText = body.getCData();
  }

  int bHasDelay = 0;
  int bDropMessage = 0;
  Apollo::TimeValue tStamp;

  {
    Apollo::XMLNode* pDelay = stanza.getChild("delay");
    if (pDelay) {
      if (Apollo::getModuleConfig(MODULE_NAME, "Room/DropDelayedChat", 1)) {
        bDropMessage = 1;
      } else {
        bHasDelay = timeFromDelayNode(*pDelay, tStamp);
      }
    }
  }

  {
    Apollo::XMLNode* pDelayX = 0;
    for (Apollo::XMLNode* pNode = 0; (pNode = stanza.nextChild(pNode, "x")) != 0; ) {
      if (pNode->getAttribute("xmlns").getValue() == JABBER_NS_DELAY) {
        pDelayX = pNode;
        break;
      }
    }
    if (pDelayX) {
      if (Apollo::getModuleConfig(MODULE_NAME, "Room/DropDelayedChat", 1)) {
        bDropMessage = 1;
      } else {
        bHasDelay = timeFromDelayNode(*pDelayX, tStamp);
      }
    }
  }

  int nAgeSec = 0;
  if (bHasDelay) {
    int nDropDelayedChatSec = Apollo::getModuleConfig(MODULE_NAME, "Room/DropDelayedChatSec", 100);
    if (nDropDelayedChatSec > 0) {
      nAgeSec = Apollo::getNow().Sec() - tStamp.Sec();
      if (nAgeSec > nDropDelayedChatSec) {
        bDropMessage = 1;
      }
    }
  }

  if (bDropMessage) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Dropping delayed chat delay=%d from=%s", nAgeSec, _sz(sResource)));

  } else {
    Participant* pParticipant = lParticipants_.FindByName(sResource);
    if (pParticipant != 0 && !sText.empty()) {
      Msg_Xmpp_ReceiveGroupchat msg;
      msg.hRoom = apHandle();
      msg.hParticipant = pParticipant->apHandle();
      msg.sText = sText;
      if (bHasDelay) {
        msg.nSec = tStamp.Sec();
        msg.nMicroSec = tStamp.MicroSec();
      }
      msg.Send();
    }
  }

  return ok;
}

#if defined(AP_TEST)

String Room::test_nextNickname1(const char* szNickname, const char* szExpectedNextNickname)
{
  String s;

  String sNickName = szNickname;
  String sNextNickname = Room::nextNickname(sNickName);
  if (sNextNickname != szExpectedNextNickname) {
    s.appendf("%s: got=%s expected=%s", _sz(szNickname), _sz(sNextNickname), _sz(szExpectedNextNickname));
  }

  return s;
}

String Room::test_nextNickname()
{
  String s;

  if (!s) { s = test_nextNickname1("Fitzliputz", "Fitzliputz1"); }
  if (!s) { s = test_nextNickname1("Fitzliputz4", "Fitzliputz5"); }
  if (!s) { s = test_nextNickname1("Fitzliputz0", "Fitzliputz1"); }
  if (!s) { s = test_nextNickname1("1", "2"); }
  if (!s) { s = test_nextNickname1("f", "f1"); }
  if (!s) { s = test_nextNickname1("\xC3\xA4" "\xC3\xA4", "\xC3\xA4" "\xC3\xA4" "1"); }
  if (!s) { s = test_nextNickname1("\xC3\xA4" "\xC3\x31" "4", "\xC3\xA4" "\xC3\x31" "5"); }

  return s;
}

String Room::test_parseDelay1(int nLine, const char* szXml, Apollo::TimeValue tvExpected)
{
  String s;

  if (!s) {
    Apollo::XMLProcessor xml;
    if (!xml.XmlText(szXml)) {
      s.appendf("%d: %s", nLine, _sz(xml.GetErrorString()));
    } else {
      Apollo::XMLNode* pRoot = xml.Root();
      if (!pRoot) {
        s.appendf("%d: No xml root", nLine);
      } else {
        Apollo::TimeValue tvStamp;
        if (!timeFromDelayNode(*pRoot, tvStamp)) {
          s.appendf("%d: timeFromDelayNode failed", nLine);
        } else {
          if (tvStamp != tvExpected) {
            s.appendf("%d: got=%d.%d expected=%d.%d, diff=%d.%d", nLine, tvStamp.Sec(), tvStamp.MicroSec(), tvExpected.Sec(), tvExpected.MicroSec(), tvStamp.Sec()-tvExpected.Sec(), tvStamp.MicroSec()-tvExpected.MicroSec());
          }
        }
      }
    }
  }

  return s;
}

String Room::test_parseDelay2(int nLine, const char* szXml, Apollo::TimeValue tvExpected)
{
  String s;

  if (!s) {
    Apollo::XMLProcessor xml;
    if (!xml.XmlText(szXml)) {
      s.appendf("%d: %s", nLine, _sz(xml.GetErrorString()));
    } else {
      Apollo::XMLNode* pRoot = xml.Root();
      if (!pRoot) {
        s.appendf("%d: No xml root", nLine);
      } else {
        Apollo::TimeValue tvStamp;
        if (!timeFromDelayNode(*pRoot, tvStamp)) {
          s.appendf("%d: timeFromDelayNode failed", nLine);
        } else {
          if (tvStamp != tvExpected) {
            s.appendf("%d: got=%d.%d expected=%d.%d, diff=%d.%d", nLine, tvStamp.Sec(), tvStamp.MicroSec(), tvExpected.Sec(), tvExpected.MicroSec(), tvStamp.Sec()-tvExpected.Sec(), tvStamp.MicroSec()-tvExpected.MicroSec());
          }
        }
      }
    }
  }

  return s;
}

String Room::test_parseDelay()
{
  String s;

  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30Z' />", Apollo::TimeValue(1234567890, 0)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2001-09-09T01:46:40Z' />", Apollo::TimeValue(1000000000, 0)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-07-19T21:48:04Z' />", Apollo::TimeValue(1248040084, 0)); }

  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.443198Z' />", Apollo::TimeValue(1234567890, 443198)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.4431Z' />", Apollo::TimeValue(1234567890, 443100)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.4Z' />", Apollo::TimeValue(1234567890, 400000)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.4' />", Apollo::TimeValue(1234567890, 400000)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.' />", Apollo::TimeValue(1234567890, 0)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30' />", Apollo::TimeValue(1234567890, 0)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.4431981234Z' />", Apollo::TimeValue(1234567890, 443198)); }
  
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.443198-1' />", Apollo::TimeValue(1234567890+3600, 443198)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.443198-1:30' />", Apollo::TimeValue(1234567890+3600+1800, 443198)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30.443198+5:30' />", Apollo::TimeValue(1234567890-(5*3600+1800), 443198)); }
  if (!s) { s = test_parseDelay1(__LINE__, "<delay  xmlns='urn:xmpp:delay' stamp='2009-02-13T23:31:30-1' />", Apollo::TimeValue(1234567890+3600, 0)); }

  if (!s) { s = test_parseDelay2(__LINE__, "<x xmlns='jabber:x:delay' stamp='20090213T23:31:30' />", Apollo::TimeValue(1234567890, 0)); }
  if (!s) { s = test_parseDelay2(__LINE__, "<x xmlns='jabber:x:delay' stamp='20010909T01:46:40' />", Apollo::TimeValue(1000000000, 0)); }
  if (!s) { s = test_parseDelay2(__LINE__, "<x xmlns='jabber:x:delay' stamp='20090719T21:48:04' />", Apollo::TimeValue(1248040084, 0)); }

  return s;
}

#endif // #if defined(AP_TEST)
