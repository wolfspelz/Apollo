// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgVpView.h"
#include "Chat.h"

Chat::Chat(const ApHandle& hLocation, const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText,Apollo::TimeValue& tvStamp)
:Elem(sNickname, sText)
,hLocation_(hLocation)
,hParticipant_(hParticipant)
,hAp_(hChat)
,tvCreated_(tvStamp)
{
}

Chat::~Chat()
{
}

void Chat::send_VpView_LocationPublicChat(int bIsNew)
{
  Apollo::TimeValue tvNow = Apollo::getNow();
  Apollo::TimeValue tvAge = tvNow - tvCreated_;

  Msg_VpView_LocationPublicChat msg;
  msg.hLocation = hLocation_;
  msg.hParticipant = hParticipant_;
  msg.hChat = hAp_;
  msg.sNickname = getName();
  msg.sText = getString();
  //msg.nSec = tvAge.Sec();
  //msg.nMicroSec = tvAge.MicroSec();
  msg.nSec = tvCreated_.Sec();
  msg.nMicroSec = tvCreated_.MicroSec();
  msg.bIsNew = bIsNew;
  msg.Send();
}
