// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "XMLProcessor.h"

#include "Local.h"
#include "XmppRoom.h"
#include "RoomVisitor.h"
#include "ProtocolDefinitions.h"

RoomVisitor::RoomVisitor(XmppRoom* pRoom, ApHandle hVisitor, const char* szName)
:SElem(szName)
,pRoom_(pRoom)
,hVisitor_(hVisitor)
{
}

RoomVisitor::~RoomVisitor()
{
}

int RoomVisitor::OnPresence(Apollo::XMLNode* pStanza)
{
  // <presence from='google-moderated@location.virtual-presence.org/Fitzliputz' to='wolfspelz@jabber.org/f79d523a0fa3'>
  //   <x xmlns='firebat:user:jid'>fitzliputz@user.vp.bluehands.de/1e7a2f95befb</x>
  //   <x xmlns='firebat:avatar:position'><position x='350' w='990' opt='' /></x>
  //   <x xmlns='firebat:avatar:digest'>0427e5f74b9fccf0d45039140eaefa7b6e9eec6d</x>
  //   <x xmlns='http://jabber.org/protocol/muc#user'>
  //     <item jid='fitzliputz@user.vp.bluehands.de/1e7a2f95befb' affiliation='none' role='participant' />
  //   </x>
  // 
  //   <x xmlns='vp:profile' src='http://suzele.bluehands.de/~wolf/Storage/12345' digest='0427e5f74b9fccf0d45039140eaefa7b6e9eec6d' />
  //   <!-- or -->
  //   <x xmlns='vp:profile' encoding='plain' digest='0427e5f74b9fccf0d45039140eaefa7b6e9eec6d'>
  //     <container digest='9838309389834938475398475938475345827'>
  //       <item id='image' contenttype='' mimetype='' src='http://developer.lluna.de/images/logo.gif' />
  //     </container>
  //   </x>
  // </presence>

///*
  SString s; s = 
"<presence from='google-moderated@location.virtual-presence.org/Fitzliputz' to='wolfspelz@jabber.org/f79d523a0fa3'>                     "
"  <x xmlns='firebat:user:jid'>fitzliputz@user.vp.bluehands.de/1e7a2f95befb</x>       "
"  <x xmlns='firebat:avatar:position'><position x='350' w='990' opt='' /></x>       "
"  <x xmlns='firebat:avatar:digest'>0427e5f74b9fccf0d45039140eaefa7b6e9eec6d</x>       "
"  <x xmlns='http://jabber.org/protocol/muc#user'>       "
"    <item jid='fitzliputz@user.vp.bluehands.de/1e7a2f95befb' affiliation='none' role='participant' />       "
"  </x>       "
"  <x xmlns='firebat:user:identity'                                                    "
"     id='#3'                                                                          "
"     jid='3@suzele.bluehands.de'                                                      "
"     digest='0c7c9f90f6bd3525ba5aa24f8d16d4e5716794a9'                                "
"     src='http://suzele.bluehands.de/~wolf/Platform/Storage/storage.php?cid=3'        "
"     />                                                                               " 
"  <x xmlns='vp:profile' src='http://suzele.bluehands.de/~wolf/Storage/12345' digest='0427e5f74b9fccf0d45039140eaefa7b6e9eec6d' />      " 
"</presence>       "
  ;
  Apollo::XMLProcessor n;
  n.XmlText(s);
  pStanza = n.Root();
//*/

  for (Apollo::XMLNode* pNode = 0; (pNode = (Apollo::XMLNode*) pStanza->NextChild(pNode)) != 0; ) {
    if (0) {
    } else if (pNode->Name() == "x") {
      Apollo::XMLNode* pX = pNode;
      SString sXmlNs = pX->Attribute( "xmlns").Value();

      if (0) {
      // ----------------------------------------------------
      } else if (sXmlNs == FB_NS_PRESENCE_X_USER_IDENTITY) {
        SString sId = pX->Attribute("id").Value();
        SString sDigest = pX->Attribute("digest").Value();
        SString sSrc = pX->Attribute("src").Value();
/*hw
        Msg_User_VisitorIdentity msg;
        msg.hVisitor = hVisitor_;
        msg.hLocation = pRoom_->hLocation_;
        msg.sId = sId;
        msg.sDigest = sDigest;
        msg.sSrc = sSrc;

        //if (sSrc.empty()) {
        //  SString sEncoding = pX->Attribute("encoding").Value();
        //  SString sData;
        //  if (sEncoding == "plain") {
        //    sData = pX->InnerXml();
        //    sData.trimWSP();
        //  } else if (sEncoding == "base64" || sEncoding.empty()) {
        //    SString sDataEncoded = pX->CData();
        //    sDataEncoded.stripWSP();
        //    SBuffer sb;
        //    sb.base64_decode(sDataEncoded);
        //    sb.GetString(sData);
        //  }
        //  msg.sProfileXML = sData;
        //} else {
        //}
        msg.Send();
*/
      // ----------------------------------------------------
      } else if (sXmlNs == FB_NS_PRESENCE_X_USER_JID) {
        JabberId jid = pX->CData();
        jid.trimWSP();
        int nChanged = 0;
        if (sFirebatUserJID_ != jid.Base()) {
          nChanged = 1;
        }
        if (!jid.Resource().empty()) {
          sFirebatClientJID_ = jid;
        }
        if (!jid.Base().empty()) {
          sFirebatUserJID_ = jid.Base();
        }
        if (nChanged) {
/*hw
          Msg_XMPP_VisitorFirebatJID msg;
          msg.hVisitor = hVisitor_;
          msg.hLocation = pRoom_->hLocation_;
          msg.sJID = jid.Base();
          msg.Send();
*/
        }

      // ----------------------------------------------------
      } else if (sXmlNs == FB_NS_PRESENCE_X_AVATAR_POSITION) {
        Apollo::XMLNode& oPosition = pX->ChildRef("position");
        if (oPosition) {
/*hw
          Msg_XMPP_VisitorFirebatAvatarPosition msg;
          msg.hVisitor = hVisitor_;
          msg.hLocation = pRoom_->hLocation_;
          msg.sX = oPosition.Attribute("x").Value();
          msg.sW = oPosition.Attribute("w").Value();
          msg.sY = oPosition.Attribute("y").Value();
          msg.sH = oPosition.Attribute("h").Value();
          msg.sOptions = oPosition.Attribute("opt").Value();
          msg.Send();
*/
        }

      // ----------------------------------------------------
      } else if (sXmlNs == FB_NS_PRESENCE_X_AVATAR_DIGEST) {
        JabberId sDigest = pX->CData();
        sDigest.trimWSP();
        if (!sDigest.empty()) {
/*hw
          Msg_XMPP_VisitorFirebatAvatarDigest msg;
          msg.hVisitor = hVisitor_;
          msg.hLocation = pRoom_->hLocation_;
          msg.sDigest = sDigest;
          msg.Send();
*/
        }

      // ----------------------------------------------------
      } else if (sXmlNs == JABBER_NS_MUC_USER) {
        Apollo::XMLNode& oItem = pX->ChildRef("item");
        if (oItem) {
          JabberId jid = oItem.Attribute("jid").Value();
          int nChanged = 0;
          if (sMucUserJID_ != jid.Base()) {
            nChanged = 1;
          }
          if (!jid.Resource().empty()) {
            sMucClientJID_ = jid;
          }
          if (!jid.Base().empty()) {
            sMucUserJID_ = jid.Base();
          }
          if (nChanged) {
/*hw
            Msg_XMPP_VisitorMucUser msg;
            msg.hVisitor = hVisitor_;
            msg.hLocation = pRoom_->hLocation_;
            msg.sJID = jid.Base();
            msg.Send();
*/
          }
          {
/*
            Msg_XMPP_VisitorMucRole msg;
            msg.hVisitor = hVisitor_;
            msg.hLocation = pRoom_->hLocation_;
            msg.sJID = jid.Base();
            msg.sAffiliation = oItem.Attribute("affiliation").Value();
            msg.sRole = oItem.Attribute("role").Value();
            msg.Send();
*/
          }
        }

      // ----------------------------------------------------
      }
    }
  }
  
  return 1;
}

int RoomVisitor::OnGroupChat(Apollo::XMLNode* pStanza)
{
  SString sBody;
  Apollo::XMLNode& oBody = pStanza->ChildRef("body");
  if (oBody) {
    sBody = oBody.CData();
  }
/*
  Msg_Protocol_ReceivedPublicChat msg;
  msg.hLocation = pRoom_->hLocation_;
  msg.hVisitor = hVisitor_;
  msg.sVisitor = Name();
  msg.sText = sBody;
  msg.Send();
*/
  return 1;
}
