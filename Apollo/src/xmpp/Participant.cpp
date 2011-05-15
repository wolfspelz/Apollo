// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgXmpp.h"
#include "MsgProtocol.h"
#include "MsgVp.h"
#include "Client.h"
#include "Room.h"
#include "Participant.h"
#include "Stanza.h"
#include "Protocol.h"
#include "SAutoPtr.h"

Participant::Participant(const char* szNickname, const ApHandle& hParticipant, Client* pClient, Room* pRoom)
:Elem(szNickname)
,hAp_(hParticipant)
,pClient_(pClient)
,pRoom_(pRoom)
{
}

Participant::~Participant()
{
}

static String& selectBetterJid(String& s1, String& s2)
{
  if (s1.empty()) { return s2; }
  if (s2.empty()) { return s1; }

  JabberId j1 = s1;
  JabberId j2 = s2;

  if (!j1.resource().empty()) { return s1; }
  return s2;
}

int Participant::presence(Stanza& stanza)
{
  int ok = 1;

/*

<presence 
  from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Fitzliputz' 
  to='12344151@xmpp1.zweitgeist.com/zg_6effc4c5a878'
  >
x <status>Available</status>
x <show>available</show>
  <priority xmlns='jabber:component:accept'>0</priority>
x <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/></x>
  <c node='http://exodus.jabberstudio.org/caps' ver='0.9.1.0' xmlns='http://jabber.org/protocol/caps'/>
  <x xmlns='vcard-temp:x:update'><photo/></x>
</presence>

<presence 
  from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Tassadar' 
  to='12344151@xmpp1.zweitgeist.com/zg_6effc4c5a878'
  >
x <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/></x>
x <x xmlns='firebat:user:identity' 
x   id='id:weblin:12344151' 
x   jid='12344151@xmpp1.zweitgeist.com' 
x   digest='c4423ad4bf2e7e23d179c2c0bd83e88bf9a35610' 
x   src='http://storage.zweitgeist.com/index.php/12344151'
x />
x <x xmlns='firebat:user:jid'>12344151@xmpp1.zweitgeist.com/zg_6561c374d14a</x>
  <x xmlns='firebat:avatar:position'><position x='105' w='279'/></x>
x <x xmlns='firebat:avatar:state'><position x='105' w='279'/></x>
  <x xmlns='firebat:features'>pc,pp,cw,ms,lo,wo.wp,cl.wp,sg.wp,fb.2.7.13</x>
</presence>

<presence 
  from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Tassadar1' 
  to='12344151@xmpp1.zweitgeist.com/zg_6effc4c5a878'
  >
x  <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/><status code='110'/></x>
x  <x xmlns='firebat:user:identity' 
x		id='id:weblin:12344151' 
x		jid='12344151@xmpp1.zweitgeist.com' 
x		digest='c4423ad4bf2e7e23d179c2c0bd83e88bf9a35610' 
x		src='http://storage.zweitgeist.com/index.php/12344151'
x	/>
  <x xmlns='firebat:avatar:position'><position x='189'/></x>
x <x xmlns='firebat:avatar:state'><position x='189'/></x>
x  <x xmlns='firebat:user:jid'>12344151@xmpp1.zweitgeist.com</x>
</presence>

*/

  // Notifications to be sent even if not in <presence/>
  AutoPtr<Msg_Xmpp_Participant_Status> pMsgStatus(new Msg_Xmpp_Participant_Status());
  if (pMsgStatus != 0) {
    pMsgStatus->sStatus = Msg_Xmpp_Participant_Status_Available;
  }

  AutoPtr<Msg_Protocol_Participant_Condition> pMsgAvatarCondition(new Msg_Protocol_Participant_Condition());
  if (pMsgAvatarCondition != 0) {
  }

  // Potential notifications
  AutoPtr<Msg_Xmpp_Participant_StatusMessage> pMsgStatusMessage(new Msg_Xmpp_Participant_StatusMessage());
  AutoPtr<Msg_Xmpp_Participant_JabberId> pMsgJabberId;
  AutoPtr<Msg_Xmpp_Participant_MucUser> pMsgMucUser;
  AutoPtr<Msg_Protocol_Participant_VpIdentity> pMsgIdentity;
  AutoPtr<Msg_Protocol_Participant_Position> pMsgAvatarPosition;
  AutoPtr<Msg_Xmpp_Participant_FirebatFeatures> pMsgFirebatFeatures;

  for (Apollo::XMLNode* pNode = 0; (pNode = stanza.nextChild(pNode)); ) {
    if (0) {
    } else if (pNode->getName() == "status") {
      // <status>Available</status>
      if (pMsgStatusMessage != 0) {
        pMsgStatusMessage->sMessage = pNode->getCData();
      }

    } else if (pNode->getName() == "show") {
      // <show>away</show>
      if (pMsgStatus != 0) {
        pMsgStatus->sStatus = pNode->getCData();
      }

    } else if (pNode->getName() == "x") {
      Apollo::XMLNode* pX = pNode;
      String sXmlNs = pX->getAttribute( "xmlns").getValue();

      if (0) {
      } else if (sXmlNs == FB_NS_PRESENCE_X_USER_IDENTITY) {
        // 
        // <x xmlns='firebat:user:identity' 
        //   id='id:weblin:12344151' 
        //   jid='12344151@xmpp1.zweitgeist.com' 
        //   digest='c4423ad4bf2e7e23d179c2c0bd83e88bf9a35610' 
        //   src='http://storage.zweitgeist.com/index.php/12344151'
        // />
        if (pMsgIdentity == 0) { pMsgIdentity = new Msg_Protocol_Participant_VpIdentity(); }
        if (pMsgIdentity != 0) {
          pMsgIdentity->sId = pX->getAttribute("id").getValue();
          pMsgIdentity->sDigest = pX->getAttribute("digest").getValue();
          pMsgIdentity->sSrc = pX->getAttribute("src").getValue();

          String sJid = pX->getAttribute("jid").getValue();
          if (!sJid.empty()) {
            if (pMsgJabberId == 0) {
              pMsgJabberId = new Msg_Xmpp_Participant_JabberId();
              if (pMsgJabberId != 0) {
                pMsgJabberId->sJabberId = sJid;
              }
            } else {
              String sPreviousJid = pMsgJabberId->sJabberId;
              pMsgJabberId->sJabberId = selectBetterJid(sJid, sPreviousJid);
            }
          }
        }
        
      } else if (sXmlNs == FB_NS_PRESENCE_X_USER_JID) {
        // <x xmlns='firebat:user:jid'>12344151@xmpp1.zweitgeist.com</x>
        String sJid = pX->getCData();
        sJid.trimWSP();

        if (!sJid.empty()) {
          if (pMsgJabberId == 0) {
            pMsgJabberId = new Msg_Xmpp_Participant_JabberId();
            if (pMsgJabberId != 0) {
              pMsgJabberId->sJabberId = sJid;
            }
          } else {
            String sPreviousJid = pMsgJabberId->sJabberId;
            pMsgJabberId->sJabberId  = selectBetterJid(sJid, sPreviousJid);
          }
        }

      } else if (sXmlNs == JABBER_NS_MUC_USER) {
        // <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/></x>
        // <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/><status code='110'/></x>
        // <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='owner' role='moderator'/></x>
        // <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='admin' role='moderator'/></x>
        // <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='member' role='participant'/></x>
        // <x xmlns='http://jabber.org/protocol/muc#user'><item jid='12344151@xmpp1.zweitgeist.com/zg_6effc4c5a878' affiliation='owner' role='moderator'/><status code='201'/></x>

        Apollo::XMLNode* pItem = pX->getChild("item");
        if (pItem != 0) {
          if (pMsgMucUser == 0) { pMsgMucUser = new Msg_Xmpp_Participant_MucUser(); }
          if (pMsgMucUser != 0) { 
            pMsgMucUser->sAffiliation = pItem->getAttribute("affiliation").getValue();
            pMsgMucUser->sRole = pItem->getAttribute("role").getValue();

            String sJid = pItem->getAttribute("jid").getValue();
            pMsgMucUser->sJabberId = sJid;

            if (!sJid.empty()) {
              if (pMsgJabberId == 0) {
                pMsgJabberId = new Msg_Xmpp_Participant_JabberId();
                if (pMsgJabberId != 0) {
                  pMsgJabberId->sJabberId = sJid;
                }
              } else {
                String sPreviousJid = pMsgJabberId->sJabberId;
                pMsgJabberId->sJabberId  = selectBetterJid(sJid, sPreviousJid);
              }
            }

          }
        }

      } else if (sXmlNs == FB_NS_PRESENCE_X_AVATAR_STATE) {
        // <x xmlns='firebat:avatar:state'><position x='189' y='0' z='0'/><condition status='sleep'/></x>

        Apollo::XMLNode* pPosition = pX->getChild("position");
        if (pPosition != 0) {
          if (pMsgAvatarPosition == 0) { pMsgAvatarPosition = new Msg_Protocol_Participant_Position(); }
          if (pMsgAvatarPosition != 0) {
            Apollo::XMLAttrList& attributes = pPosition->getAttributes();
            for (Apollo::XMLAttr* pAttr = 0; (pAttr = attributes.nextAttribute(pAttr)) != 0; ) {
              pMsgAvatarPosition->kvParams.add(pAttr->getKey(), pAttr->getValue());
            }
          }
        }

        Apollo::XMLNode* pCondition = pX->getChild("condition");
        if (pCondition != 0) {
          if (pMsgAvatarCondition != 0) {
            pMsgAvatarCondition->kvParams.removeAll();
            Apollo::XMLAttrList& attributes = pCondition->getAttributes();
            for (Apollo::XMLAttr* pAttr = 0; (pAttr = attributes.nextAttribute(pAttr)) != 0; ) {
              pMsgAvatarCondition->kvParams.add(pAttr->getKey(), pAttr->getValue());
            }
          }
        }

      } else if (sXmlNs == FB_NS_PRESENCE_X_FEATURES) {
        // <x xmlns='firebat:features'>pc,pp,cw,ms,lo,wo.wp,cl.wp,sg.wp,fb.2.7.13</x>

        if (pMsgFirebatFeatures == 0) { pMsgFirebatFeatures = new Msg_Xmpp_Participant_FirebatFeatures(); }
        if (pMsgFirebatFeatures != 0) { 
          pMsgFirebatFeatures->sFeatures = pX->getCData();
          pMsgFirebatFeatures->sFeatures.trimWSP();
        }

      } // end <x>-nodes

    }
  }

  {
    Msg_Xmpp_Participant_Begin msg;
    msg.hClient = pClient_->apHandle();
    msg.hRoom = pRoom_->apHandle();
    msg.hParticipant = apHandle();
    msg.Send();
  }

  if (pMsgStatus != 0) {
    pMsgStatus->hClient = pClient_->apHandle();
    pMsgStatus->hRoom = pRoom_->apHandle();
    pMsgStatus->hParticipant = apHandle();
    pMsgStatus->Send();
  }

  if (pMsgStatusMessage != 0) {
    pMsgStatusMessage->hClient = pClient_->apHandle();
    pMsgStatusMessage->hRoom = pRoom_->apHandle();
    pMsgStatusMessage->hParticipant = apHandle();
    pMsgStatusMessage->Send();
  }

  if (pMsgJabberId != 0) {
    pMsgJabberId->hClient = pClient_->apHandle();
    pMsgJabberId->hRoom = pRoom_->apHandle();
    pMsgJabberId->hParticipant = apHandle();
    pMsgJabberId->Send();
  }

  if (pMsgMucUser != 0) {
    pMsgMucUser->hClient = pClient_->apHandle();
    pMsgMucUser->hRoom = pRoom_->apHandle();
    pMsgMucUser->hParticipant = apHandle();
    pMsgMucUser->Send();
  }

  if (pMsgIdentity != 0) {
    pMsgIdentity->hRoom = pRoom_->apHandle();
    pMsgIdentity->hParticipant = apHandle();
    pMsgIdentity->Send();
  }

  if (pMsgAvatarPosition != 0) {
    pMsgAvatarPosition->hRoom = pRoom_->apHandle();
    pMsgAvatarPosition->hParticipant = apHandle();
    pMsgAvatarPosition->Send();
  }

  if (pMsgAvatarCondition != 0) {
    pMsgAvatarCondition->hRoom = pRoom_->apHandle();
    pMsgAvatarCondition->hParticipant = apHandle();
    pMsgAvatarCondition->Send();
  }

  if (pMsgFirebatFeatures != 0) {
    pMsgFirebatFeatures->hRoom = pRoom_->apHandle();
    pMsgFirebatFeatures->hParticipant = apHandle();
    pMsgFirebatFeatures->Send();
  }

  {
    Msg_Xmpp_Participant_End msg;
    msg.hClient = pClient_->apHandle();
    msg.hRoom = pRoom_->apHandle();
    msg.hParticipant = apHandle();
    msg.Send();
  }

  return ok;
}

#if defined(AP_TEST)

// ------------------------------------
// Scenario base class

class Test_Participant_presence: public Elem
{
public:
  Test_Participant_presence(const char* szName, const char* szStanza)
    :Elem(szName)
    ,sStanza_(szStanza)
  {}

  virtual String begin() { return String("not implemented"); }
  virtual String exec();
  virtual String end() { return String("not implemented"); }
  virtual String evaluate() { return String("not implemented"); }

  String sStanza_;
};

String Test_Participant_presence::exec()
{
  String s;

  Apollo::XMLProcessor xml;
  if (!xml.XmlText(sStanza_)) {
    s = "XML parser failed";
  } else {
    Stanza* pStanza = (Stanza*) xml.Root();
    if (pStanza == 0) {
      s = "No XML root";
    } else {
      Client client(Apollo::newHandle());
      Room room("testroom@testserver.com", Apollo::newHandle(), &client);
      Participant p("TestParticipant", Apollo::newHandle(), &client, &room);
      p.presence(*pStanza);
    }
  }

  return s;
}

// ------------------------------------
// Scenario Simple

class Test_Participant_presence_Simple: public Test_Participant_presence
{
public:
  Test_Participant_presence_Simple(const char* szName, const char* szStanza)
    :Test_Participant_presence(szName, szStanza)
    ,bXmpp_Participant_Begin_(0)
    ,bXmpp_Participant_End_(0)
    ,bXmpp_Participant_Status_(0)
    ,bXmpp_Participant_StatusMessage_(0)
  {}

  String begin();
  String end();
  String evaluate();

  static void on_Xmpp_Participant_Begin(Msg_Xmpp_Participant_Begin* pMsg);
  static void on_Xmpp_Participant_End(Msg_Xmpp_Participant_End* pMsg);
  static void on_Xmpp_Participant_Status(Msg_Xmpp_Participant_Status* pMsg);
  static void on_Xmpp_Participant_StatusMessage(Msg_Xmpp_Participant_StatusMessage* pMsg);

  int bXmpp_Participant_Begin_;
  int bXmpp_Participant_End_;
  int bXmpp_Participant_Status_;
  int bXmpp_Participant_StatusMessage_;
};

void Test_Participant_presence_Simple::on_Xmpp_Participant_Begin(Msg_Xmpp_Participant_Begin* pMsg)
{
  Test_Participant_presence_Simple* pScenario = (Test_Participant_presence_Simple*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->bXmpp_Participant_Begin_ = 1;
  }
}

void Test_Participant_presence_Simple::on_Xmpp_Participant_End(Msg_Xmpp_Participant_End* pMsg)
{
  Test_Participant_presence_Simple* pScenario = (Test_Participant_presence_Simple*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->bXmpp_Participant_End_ = 1;
  }
}

void Test_Participant_presence_Simple::on_Xmpp_Participant_Status(Msg_Xmpp_Participant_Status* pMsg)
{
  Test_Participant_presence_Simple* pScenario = (Test_Participant_presence_Simple*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->bXmpp_Participant_Status_ = 1;
  }
}

void Test_Participant_presence_Simple::on_Xmpp_Participant_StatusMessage(Msg_Xmpp_Participant_StatusMessage* pMsg)
{
  Test_Participant_presence_Simple* pScenario = (Test_Participant_presence_Simple*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->bXmpp_Participant_StatusMessage_ = 1;
  }
}

String Test_Participant_presence_Simple::begin()
{
  { Msg_Xmpp_Participant_Begin msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_Begin, this, 0); }
  { Msg_Xmpp_Participant_Status msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_Status, this, 0); }
  { Msg_Xmpp_Participant_StatusMessage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_StatusMessage, this, 0); }
  { Msg_Xmpp_Participant_End msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_End, this, 0); }
  return "";
}

String Test_Participant_presence_Simple::end()
{
  { Msg_Xmpp_Participant_Begin msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_Begin, this); }
  { Msg_Xmpp_Participant_Status msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_Status, this); }
  { Msg_Xmpp_Participant_StatusMessage msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_StatusMessage, this); }
  { Msg_Xmpp_Participant_End msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_Simple::on_Xmpp_Participant_End, this); }
  return "";
}

String Test_Participant_presence_Simple::evaluate()
{
  String s;

  if (! (1
    && bXmpp_Participant_Begin_ 
    && bXmpp_Participant_End_
    && bXmpp_Participant_Status_
    && bXmpp_Participant_StatusMessage_
    ) ) {
    s.appendf("Missing Xmpp_Participant_Begin=%d Xmpp_Participant_Status=%d Xmpp_Participant_StatusMessage=%d Xmpp_Participant_End=%d ", bXmpp_Participant_Begin_, bXmpp_Participant_Status_, bXmpp_Participant_StatusMessage_, bXmpp_Participant_End_);
  }

  return s;
}

// ------------------------------------
// Scenario Simple

class Test_Participant_presence_FirebatDetail: public Test_Participant_presence
{
public:
  Test_Participant_presence_FirebatDetail(const char* szName, List lExpected, const char* szStanza)
    :Test_Participant_presence(szName, szStanza)
    ,bXmpp_Participant_Begin_(0)
    ,bXmpp_Participant_End_(0)
    ,lExpected_(lExpected)
  {}

  String begin();
  String end();
  String evaluate();

  static void on_Xmpp_Participant_Begin(Msg_Xmpp_Participant_Begin* pMsg);
  static void on_Xmpp_Participant_End(Msg_Xmpp_Participant_End* pMsg);

  static void on_Xmpp_Participant_Status(Msg_Xmpp_Participant_Status* pMsg);
  static void on_Xmpp_Participant_StatusMessage(Msg_Xmpp_Participant_StatusMessage* pMsg);
  static void on_Xmpp_Participant_JabberId(Msg_Xmpp_Participant_JabberId* pMsg);
  static void on_Xmpp_Participant_MucUser(Msg_Xmpp_Participant_MucUser* pMsg);
  static void on_Protocol_Participant_VpIdentity(Msg_Protocol_Participant_VpIdentity* pMsg);
  static void on_Protocol_Participant_Position(Msg_Protocol_Participant_Position* pMsg);
  static void on_Protocol_Participant_Condition(Msg_Protocol_Participant_Condition* pMsg);
  static void on_Xmpp_Participant_FirebatFeatures(Msg_Xmpp_Participant_FirebatFeatures* pMsg);

  int bXmpp_Participant_Begin_;
  int bXmpp_Participant_End_;
  List lExpected_;
  List lReal_;
};

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_Begin(Msg_Xmpp_Participant_Begin* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->bXmpp_Participant_Begin_ = 1;
  }
}

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_End(Msg_Xmpp_Participant_End* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->bXmpp_Participant_End_ = 1;
  }
}

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_Status(Msg_Xmpp_Participant_Status* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->lReal_["Status"] = pMsg->sStatus;
  }
}

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_StatusMessage(Msg_Xmpp_Participant_StatusMessage* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->lReal_["StatusMessage"] = pMsg->sMessage;
  }
}

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_JabberId(Msg_Xmpp_Participant_JabberId* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->lReal_["JabberId"] = pMsg->sJabberId;
  }
}

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_MucUser(Msg_Xmpp_Participant_MucUser* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->lReal_["MucUserJabberId"] = pMsg->sJabberId;
    pScenario->lReal_["MucUserAffiliation"] = pMsg->sAffiliation;
    pScenario->lReal_["MucUserRole"] = pMsg->sRole;
  }
}

void Test_Participant_presence_FirebatDetail::on_Protocol_Participant_VpIdentity(Msg_Protocol_Participant_VpIdentity* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->lReal_["IdentityId"] = pMsg->sId;
    pScenario->lReal_["IdentityDigest"] = pMsg->sDigest;
    pScenario->lReal_["IdentitySrc"] = pMsg->sSrc;
  }
}

void Test_Participant_presence_FirebatDetail::on_Protocol_Participant_Position(Msg_Protocol_Participant_Position* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    for (Apollo::KeyValueElem* e = 0; (e = pMsg->kvParams.nextElem(e)) != 0; ) {
      String sKey = "Position-" + e->getKey();
      pScenario->lReal_[sKey] = e->getString();
    }
  }
}

void Test_Participant_presence_FirebatDetail::on_Protocol_Participant_Condition(Msg_Protocol_Participant_Condition* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    for (Apollo::KeyValueElem* e = 0; (e = pMsg->kvParams.nextElem(e)) != 0; ) {
      String sKey = "Condition-" + e->getKey();
      pScenario->lReal_[sKey] = e->getString();
    }
  }
}

void Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_FirebatFeatures(Msg_Xmpp_Participant_FirebatFeatures* pMsg)
{
  Test_Participant_presence_FirebatDetail* pScenario = (Test_Participant_presence_FirebatDetail*) pMsg->Ref();
  if (pScenario != 0) {
    pScenario->lReal_["Features"] = pMsg->sFeatures;
  }
}

String Test_Participant_presence_FirebatDetail::begin()
{
  { Msg_Xmpp_Participant_Begin msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_Begin, this, 0); }
  { Msg_Xmpp_Participant_End msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_End, this, 0); }
  { Msg_Xmpp_Participant_Status msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_Status, this, 0); }
  { Msg_Xmpp_Participant_StatusMessage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_StatusMessage, this, 0); }
  { Msg_Xmpp_Participant_JabberId msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_JabberId, this, 0); }
  { Msg_Xmpp_Participant_MucUser msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_MucUser, this, 0); }
  { Msg_Protocol_Participant_VpIdentity msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Protocol_Participant_VpIdentity, this, 0); }
  { Msg_Protocol_Participant_Position msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Protocol_Participant_Position, this, 0); }
  { Msg_Protocol_Participant_Condition msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Protocol_Participant_Condition, this, 0); }
  { Msg_Xmpp_Participant_FirebatFeatures msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_FirebatFeatures, this, 0); }
  return "";
}

String Test_Participant_presence_FirebatDetail::end()
{
  { Msg_Xmpp_Participant_Begin msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_Begin, this); }
  { Msg_Xmpp_Participant_End msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_End, this); }
  { Msg_Xmpp_Participant_Status msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_Status, this); }
  { Msg_Xmpp_Participant_StatusMessage msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_StatusMessage, this); }
  { Msg_Xmpp_Participant_JabberId msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_JabberId, this); }
  { Msg_Xmpp_Participant_MucUser msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_MucUser, this); }
  { Msg_Protocol_Participant_VpIdentity msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Protocol_Participant_VpIdentity, this); }
  { Msg_Protocol_Participant_Position msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Protocol_Participant_Position, this); }
  { Msg_Protocol_Participant_Condition msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Protocol_Participant_Condition, this); }
  { Msg_Xmpp_Participant_FirebatFeatures msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Participant_presence_FirebatDetail::on_Xmpp_Participant_FirebatFeatures, this); }
  return "";
}

String Test_Participant_presence_FirebatDetail::evaluate()
{
  String s;

  if (! (1
    && bXmpp_Participant_Begin_ 
    && bXmpp_Participant_End_
    ) ) {
    s.appendf("Missing Xmpp_Participant_Begin=%d Xmpp_Participant_End=%d ", bXmpp_Participant_Begin_, bXmpp_Participant_End_);
  }

  for (Elem* eExpected = 0; (eExpected = lExpected_.Next(eExpected)); ) {
    Elem* eReal = lReal_.FindByName(eExpected->getName());
    if (eReal == 0) {
      s.appendf(" %s missing, expected=%s", StringType(eExpected->getName()), StringType(eExpected->getString()));
    } else if (eExpected->getString() != eReal->getString()) {
      s.appendf(" %s=%s, expected=%s", StringType(eExpected->getName()), StringType(eReal->getString()), StringType(eExpected->getString()));
    }
  }

  return s;                                                         
}                                                                   

// ------------------------------------
// Scenarios and general driver

String Participant::test_presence()
{
  String s;

  ListT<Test_Participant_presence, Elem> lScenarios;

  {
    Test_Participant_presence* pScenario = new Test_Participant_presence_Simple("Exodus-alike",
      "<presence"
      "  from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Fitzliputz'"
      "  to='12344151@xmpp1.zweitgeist.com/zg_6effc4c5a878'"
      "  >"
      " <status>Available</status>"
      "  <show>available</show>"
      "  <priority xmlns='jabber:component:accept'>0</priority>"
      " <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/></x>"
      "  <c node='http://exodus.jabberstudio.org/caps' ver='0.9.1.0' xmlns='http://jabber.org/protocol/caps'/>"
      "  <x xmlns='vcard-temp:x:update'><photo/></x>"
      "</presence>"
      );
    if (pScenario != 0) {
      lScenarios.AddLast(pScenario);
    }
  }

  {
    List lExpected;
    lExpected.AddLast("Status"             , "available" );
    lExpected.AddLast("StatusMessage"      , "" );
    lExpected.AddLast("MucUserAffiliation" , "none" );
    lExpected.AddLast("MucUserRole"        , "participant" );
    lExpected.AddLast("IdentityId"         , "id:weblin:12344151" );
    lExpected.AddLast("IdentityDigest"     , "c4423ad4bf2e7e23d179c2c0bd83e88bf9a35610" );
    lExpected.AddLast("IdentitySrc"        , "http://storage.zweitgeist.com/index.php/12344151" );
    lExpected.AddLast("JabberId"           , "12344151@xmpp1.zweitgeist.com/zg_6561c374d14a" );
    lExpected.AddLast("Position-x"         , "105" );
    lExpected.AddLast("Position-y"         , "" );
    //lExpected.AddLast("Position-z"         , "" );
    lExpected.AddLast("Condition-status"    , "idle" );
    lExpected.AddLast("Features"           , "pc,pp,cw,ms,lo,wo.wp,cl.wp,sg.wp,fb.2.7.13" );

    Test_Participant_presence* pScenario = new Test_Participant_presence_FirebatDetail("Firebat-alike", lExpected,
      "<presence"
      "  from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org/Tassadar'"
      "  to='12344151@xmpp1.zweitgeist.com/zg_6effc4c5a878'"
      "  >  "
      "  <x xmlns='http://jabber.org/protocol/muc#user'><item affiliation='none' role='participant'/></x>"
      "  <x xmlns='firebat:user:identity'"
      "    id='id:weblin:12344151'"
      "    jid='12344151@xmpp1.zweitgeist.com'"
      "    digest='c4423ad4bf2e7e23d179c2c0bd83e88bf9a35610'"
      "    src='http://storage.zweitgeist.com/index.php/12344151'"
      "  />  "
      "  <x xmlns='firebat:user:jid'>12344151@xmpp1.zweitgeist.com/zg_6561c374d14a</x>"
      "  <x xmlns='firebat:avatar:position'><position x='105' w='279'/></x>"
      "  <x xmlns='firebat:avatar:state'><position x='105' w='279' y=''/><condition status='idle'/></x>"
      "  <x xmlns='firebat:features'>pc,pp,cw,ms,lo,wo.wp,cl.wp,sg.wp,fb.2.7.13</x>"
      "</presence>"
      );
    if (pScenario != 0) {
      lScenarios.AddLast(pScenario);
    }
  }

  Test_Participant_presence* pScenario = 0;
  while ((pScenario = lScenarios.Next(pScenario)) && s.empty()) {
    String t;
    if (!s) { t = pScenario->begin(); if (!t.empty()) { s.appendf("%s: begin: %s", StringType(pScenario->getName()), StringType(t)); }}
    if (!s) { t = pScenario->exec(); if (!t.empty()) { s.appendf("%s: exec: %s", StringType(pScenario->getName()), StringType(t)); }}
    if (!s) { t = pScenario->end(); if (!t.empty()) { s.appendf("%s: end: %s", StringType(pScenario->getName()), StringType(t)); }}
    if (!s) { t = pScenario->evaluate(); if (!t.empty()) { s.appendf("%s: %s", StringType(pScenario->getName()), StringType(t)); }}
  }

  return s;
}

#endif // #if defined(AP_TEST)
