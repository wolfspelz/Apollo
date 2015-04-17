// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVpView.h"
#include "MsgAnimation.h"
#include "Local.h"
#include "Avatar.h"
#include "ArenaModule.h"

Apollo::ValueList Avatar::noMimeTypes_;
Apollo::ValueList Avatar::avatarMimeTypes_;
Apollo::ValueList Avatar::imageMimeTypes_;

Avatar::Avatar(ArenaModule* pModule, Display* pDisplay, const ApHandle& hParticipant)
:pModule_(pModule)
,pDisplay_(pDisplay)
,hParticipant_(hParticipant)
,hAnimatedItem_(ApNoHandle)
,nX_(300)
,nPositionConfirmed_(0)
,bMoving_(0)
{
  if (avatarMimeTypes_.length() == 0) {
    avatarMimeTypes_.add("avatar/gif");
  }

  if (imageMimeTypes_.length() == 0) {
    imageMimeTypes_.add("image/gif");
    imageMimeTypes_.add("image/png");
    imageMimeTypes_.add("image/jpeg");
    imageMimeTypes_.add("image/jpg");
  }
}

void Avatar::Create(int bSelf)
{
  ViewSrpcMessage vsm(pDisplay_, "AddAvatar");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("bSelf", bSelf);
  vsm.srpc.set("nX", nX_);
  vsm.Request();

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Image);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
  SubscribeAndGetDetail("CommunityTag");
  SubscribeAndGetDetail("CommunityName");
  SubscribeAndGetDetail("CommunityPage");
  SubscribeAndGetDetail("AttachedIcon");
  SubscribeAndGetDetail("AttachedIconName");
}

void Avatar::Destroy()
{
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Image);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Avatar);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_Message);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Position);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_Condition);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
  UnsubscribeDetail("CommunityTag");
  UnsubscribeDetail("CommunityName");
  UnsubscribeDetail("CommunityPage");
  UnsubscribeDetail("AttachedIcon");
  UnsubscribeDetail("AttachedIconName");

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->DeleteContextOfHandle(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->DeleteParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Stop msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_Stop failed: participant=" ApHandleFormat "", ApHandlePrintf(hParticipant_)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Destroy msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_Destroy failed: participant=" ApHandleFormat "", ApHandlePrintf(hParticipant_)));
    }
  }

  ViewSrpcMessage vsm(pDisplay_, "RemoveAvatar");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.Request();
}

void Avatar::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
    msg.vlMimeTypes = imageMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  if (msg.Request()) {
    if (msg.bAvailable) {
      GetDetail(sKey);
    }
  }
}

void Avatar::GetDetail(const String& sKey)
{
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
    GetDetailData(sKey, avatarMimeTypes_);
  } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
    GetDetailUrl(sKey, imageMimeTypes_);
  } else {
    GetDetailString(sKey, noMimeTypes_);
  }
}

void Avatar::GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailString msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
      if (sNickname_ != msg.sValue) {
        sNickname_ = msg.sValue;
        DisplaySetNickname(sNickname_);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      if (sOnlineStatus_ != msg.sValue) {
        sOnlineStatus_ = msg.sValue;

        if (sOnlineStatus_ == "away" || sOnlineStatus_ == "xa" || sOnlineStatus_ == "dnd") {
          SuspendAnimation();
        } else {
          ResumeAnimation();
        }

        DisplaySetOnlineStatus(sOnlineStatus_);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {

    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      List lCoords;
      KeyValueLfBlob2List(msg.sValue, lCoords);
      Elem* e = lCoords.FindByNameCase("x");
      if (e) {
        int nX = String::atoi(e->getString());
        int bPositionValid = 1;

        if (nX > 10000) {
          bPositionValid = 0;
        }

        if (nX < 0) {
          nX = 0;
        }

        if (bPositionValid) {
          if (nX_ != nX) {
            if (!nPositionConfirmed_) {
              nPositionConfirmed_ = 1;
              DisplaySetAvatarPosition(nX);
            } else {
              BeginMove(nX);
            }
            nX_ = nX;
          }
        } else {
          SetUnknownPosition();
        }

      }

    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {

    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {

    } else if (sKey == "CommunityTag") {
      if (sCommunityTag_ != msg.sValue) {
        sCommunityTag_ = msg.sValue;
        if (sCommunityTag_) {
          DisplaySetCommunityAttachment(sCommunityTag_, sCommunityName_, sCommunityPage_);
        }
      }

    } else if (sKey == "CommunityName") {
      if (sCommunityName_ != msg.sValue) {
        sCommunityName_ = msg.sValue;
        if (sCommunityTag_) {
          DisplaySetCommunityAttachment(sCommunityTag_, sCommunityName_, sCommunityPage_);
        }
      }

    } else if (sKey == "CommunityPage") {
      if (sCommunityPage_ != msg.sValue) {
        sCommunityPage_ = msg.sValue;
        if (sCommunityTag_) {
          DisplaySetCommunityAttachment(sCommunityTag_, sCommunityName_, sCommunityPage_);
        }
      }

    } else if (sKey == "AttachedIcon") {
      if (sAttachedIcon_ != msg.sValue) {
        sAttachedIcon_ = msg.sValue;
        if (sAttachedIcon_) {
          DisplaySetIconAttachment(sAttachedIcon_, sAttachedIconName_);
        }
      }

    } else if (sKey == "AttachedIconName") {
      if (sAttachedIconName_ != msg.sValue) {
        sAttachedIconName_ = msg.sValue;
        if (sAttachedIcon_) {
          DisplaySetIconAttachment(sAttachedIcon_, sAttachedIconName_);
        }
      }

    }
  }
}

void Avatar::GetDetailUrl(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailUrl msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
      if (!ApIsHandle(hAnimatedItem_)) {
        String sUrl = msg.sUrl;
        if (sUrl != sImage_) {
          DisplaySetImage(sUrl);
          sImage_ = sUrl;
        }
      }

    }
  }
}

void Avatar::GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailData msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
      HandleImageData(msg.sMimeType, msg.sSource, msg.sbData);
    }
  }
}

void Avatar::HandleImageData(const String& sMimeType, const String& sSource, Buffer& sbData)
{
  if (!ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Create msgAC;
    msgAC.hItem = Apollo::newHandle();
    msgAC.sMimeType = sMimeType;
    if (!msgAC.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_Create failed: participant=" ApHandleFormat "", ApHandlePrintf(hParticipant_)));
    } else {
      hAnimatedItem_ = msgAC.hItem;
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_SetRate msgASR;
    msgASR.hItem = hAnimatedItem_;
    msgASR.nMaxRate = 10;
    if (!msgASR.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_SetRate failed: participant=" ApHandleFormat "", ApHandlePrintf(hParticipant_)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_SetData msgASD;
    msgASD.hItem = hAnimatedItem_;
    msgASD.sbData = sbData;

    String sSourceTokenizer = sSource;
    String sSourceType;
    sSourceTokenizer.nextToken(Msg_VpView_ParticipantDetail_SourceSeparator, sSourceType);
    if (sSourceType == Msg_VpView_ParticipantDetail_SourcePrefix_IdentityItemUrl) {
      String sUrl = sSourceTokenizer;
      msgASD.sSourceUrl = sUrl;
    }

    if (!msgASD.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_SetData failed: participant=" ApHandleFormat " data:%d bytes, source=%s", ApHandlePrintf(hParticipant_), sbData.Length(), _sz(sSource)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->SetContextOfHandle(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->SetParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Start msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_Start failed: participant=" ApHandleFormat "", ApHandlePrintf(hParticipant_)));
    }
  }
}

void Avatar::SuspendAnimation()
{
  if (ApIsHandle(hAnimatedItem_)) {
    //{
    //  Msg_Animation_Static msg;
    //  msg.hItem = hAnimatedItem_;
    //  msg.bState = 1;
    //  if (!msg.Request()) {
    //    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat "", _sz(msg.Type()), ApHandlePrintf(hParticipant_)));
    //  }
    //}

    {
      Msg_Animation_Activity msg;
      msg.hItem = hAnimatedItem_;
      msg.sActivity = Msg_Animation_Activity_Idle;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat " activity=%s", _sz(msg.Type()), ApHandlePrintf(hParticipant_), _sz(msg.sActivity)));
      }
    }

    {
      Msg_Animation_Stop msg;
      msg.hItem = hAnimatedItem_;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat "", _sz(msg.Type()), ApHandlePrintf(hParticipant_)));
      }
    }
  }
}

void Avatar::ResumeAnimation()
{
  if (ApIsHandle(hAnimatedItem_)) {
    //{
    //  Msg_Animation_Static msg;
    //  msg.hItem = hAnimatedItem_;
    //  msg.bState = 0;
    //  if (!msg.Request()) {
    //    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat "", _sz(msg.Type()), ApHandlePrintf(hParticipant_)));
    //  }
    //}

    {
      Msg_Animation_Activity msg;
      msg.hItem = hAnimatedItem_;
      msg.sActivity = "";
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat "", _sz(msg.Type()), ApHandlePrintf(hParticipant_)));
      }
    }

    {
      Msg_Animation_Start msg;
      msg.hItem = hAnimatedItem_;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat "", _sz(msg.Type()), ApHandlePrintf(hParticipant_)));
      }
    }
  }
}

void Avatar::BeginMove(int nDestX)
{
  bMoving_ = 1;

  if (ApIsHandle(hAnimatedItem_) && nX_ != nDestX) {

    Msg_Animation_Activity msg;
    msg.hItem = hAnimatedItem_;
    msg.sActivity = (nDestX > nX_ ? Msg_Animation_Activity_MoveRight : Msg_Animation_Activity_MoveLeft);
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat " activity=%s", _sz(msg.Type()), ApHandlePrintf(hParticipant_), _sz(msg.sActivity)));
    }

  }

  DisplayMoveAvatarPosition(nDestX);
}

void Avatar::EndMove(int nDestX)
{
  bMoving_ = 0;

  if (ApIsHandle(hAnimatedItem_)) {

    Msg_Animation_Activity msg;
    msg.hItem = hAnimatedItem_;
    msg.sActivity = "";
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat "", _sz(msg.Type()), ApHandlePrintf(hParticipant_)));
    }

  }
}

void Avatar::SetUnknownPosition()
{
  int nX = 300;
  int nMin = 100;
  int nMax = 500;

  if (pDisplay_) {
    nMax = pDisplay_->GetWidth();
  }

  nX = nMin + Apollo::getRandom(nMax - nMin);

  nPositionConfirmed_ = 0;

  DisplaySetAvatarPosition(nX);
}

void Avatar::UnsubscribeDetail(const String& sKey)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  (void) msg.Request();
}


void Avatar::OnDetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}

void Avatar::OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tvTimestamp)
{
  Apollo::TimeValue tv = tvTimestamp;
  if (tv.isNull()) {
    tv = Apollo::TimeValue::getTime();
  }

  Chatline* pChat = 0;
  chats_.Get(hChat, pChat);
  if (pChat) {
    if (pChat->sText_ != sText) {
      pChat->sText_ = sText;
      DisplaySetChatline(hChat, sText);
    }
    if (pChat->tv_ != tv) {
      pChat->tv_ = tv;
    }
  } else {
    pChat = new Chatline(sText, tv);
    chats_.Set(hChat, pChat);

    RemoveOldPublicChats(3);

    DisplayAddChatline(hChat, sText);
  }
}

void Avatar::OnReceivePublicAction(const String& sAction)
{
  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Event msg;
    msg.hItem = hAnimatedItem_;
    msg.sEvent = sAction;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Animation_Event failed: participant=" ApHandleFormat " event=%s", ApHandlePrintf(hParticipant_), _sz(sAction)));
    }
  }
}

void Avatar::OnAnimationBegin(const String& sUrl)
{
  if (sUrl != sImage_) {
    DisplaySetImage(sUrl);
    sImage_ = sUrl;
  }
}

//----------------------------------------------------------

void Avatar::OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString(Srpc::Key::Method);

  if (0) {
  } else if (sMethod == "OnPublicChatTimedOut" || sMethod == "OnPublicChatClosed") {
    ApHandle hChat = Apollo::string2Handle(request.getString("hChat"));
    if (ApIsHandle(hChat)){
      OnPublicChatClosed(hChat);
    }

  } else if (sMethod == "OnIconAttachmentClicked") {
    String sLink = request.getString("sLink");
    if (sLink){
      OnIconAttachmentClicked(sLink);
    }

  } else if (sMethod == "OnAvatarPositionReached") {
    int nX = request.getInt("nX");
    OnAvatarPositionReached(nX);

  } else if (sMethod == "OnAvatarDraggedBy") {
    int nX = request.getInt("nX");
    int nY = request.getInt("nY");
    OnAvatarDraggedBy(nX, nY);

  } else if (sMethod == "OnAvatarPointerClosed") {
    OnAvatarPointerClosed();

  } else {
    throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
  }
  
}

void Avatar::OnPublicChatClosed(const ApHandle& hChat)
{
  DeletePublicChat(hChat);
}

void Avatar::OnIconAttachmentClicked(const String& sLink)
{
  // Navigate external browser
}

void Avatar::OnAvatarPositionReached(int nX)
{
  EndMove(nX);
}

void Avatar::OnAvatarDraggedBy(int nDiffX, int nDiffY)
{
  int nNewX = nX_ + nDiffX;

  Msg_Vp_SendPosition msg;
  msg.hLocation = pDisplay_->GetLocation();
  msg.kvParams.add(Msg_Vp_Position_X, nNewX);
  if (!msg.Request()) {
    apLog_Warning(( MODULE_NAME, LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment) ));
  }
}

void Avatar::OnAvatarPointerClosed()
{
}

//----------------------------------------------------------

void Avatar::RemoveOldPublicChats(int nMax)
{
  while (chats_.Count() > nMax) {
    Apollo::TimeValue tvOldestChat;
    ApHandle hOldestChat;
    ChatlineListNode* node = 0;
    for (ChatlineListIterator iter(chats_); (node = iter.Next()); ) {
      if (tvOldestChat.isNull() || node->Value()->tv_ < tvOldestChat) {
        tvOldestChat = node->Value()->tv_;
        hOldestChat = node->Key();
      }
    }
    if (ApIsHandle(hOldestChat)) {
      DeletePublicChat(hOldestChat);
    }
  }
}

void Avatar::DeletePublicChat(const ApHandle& hChat)
{
  Chatline* pChat = 0;
  chats_.Get(hChat, pChat);
  if (pChat) {
    chats_.Unset(hChat);
    delete pChat;
    pChat = 0;

    DisplayRemoveChatline(hChat);
  }
}

//----------------------------------------------------------

void Avatar::DisplaySetNickname(const String& sNickname)
{
  ViewSrpcMessage vsm(pDisplay_, "SetAvatarNickname");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("sNickname", sNickname);
  vsm.Request();
}

void Avatar::DisplaySetImage(const String& sUrl)
{
  ViewSrpcMessage vsm(pDisplay_, "SetAvatarImage");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("sUrl", sUrl);
  vsm.Request();
}

void Avatar::DisplaySetCommunityAttachment(const String& sUrl, const String& sLabel, const String& sLink)
{
  ViewSrpcMessage vsm(pDisplay_, "SetCommunityAttachment");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("sUrl", sUrl);
  vsm.srpc.set("sLabel", sLabel);
  vsm.srpc.set("sLink", sLink);
  vsm.Request();
}

void Avatar::DisplaySetIconAttachment(const String& sUrl, const String& sLabel)
{
  ViewSrpcMessage vsm(pDisplay_, "SetIconAttachment");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("sUrl", sUrl);
  vsm.srpc.set("sLabel", sLabel);
  vsm.Request();
}

void Avatar::DisplaySetOnlineStatus(const String& sStatus)
{
  ViewSrpcMessage vsm(pDisplay_, "SetOnlineStatus");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("sStatus", sStatus);
  vsm.Request();
}

void Avatar::DisplayAddChatline(const ApHandle& hChat, const String& sText)
{
  ViewSrpcMessage vsm(pDisplay_, "AddAvatarChat");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("hChat", hChat);
  vsm.srpc.set("sText", sText);
  vsm.Request();
}

void Avatar::DisplaySetChatline(const ApHandle& hChat, const String& sText)
{
  ViewSrpcMessage vsm(pDisplay_, "SetAvatarChat");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("hChat", hChat);
  vsm.srpc.set("sText", sText);
  vsm.Request();
}

void Avatar::DisplayRemoveChatline(const ApHandle& hChat)
{
  ViewSrpcMessage vsm(pDisplay_, "RemoveAvatarChat");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("hChat", hChat);
  vsm.Request();
}

void Avatar::DisplaySetAvatarPosition(int nDestX)
{
  ViewSrpcMessage vsm(pDisplay_, "SetAvatarPosition");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("nX", nDestX);
  vsm.Request();
}

void Avatar::DisplayMoveAvatarPosition(int nDestX)
{
  int nSpeedX = 50;

  if (ApIsHandle(hAnimatedItem_)) {
    String sSequence;

    {
      Msg_Animation_GetGroupSequences msg;
      msg.hItem = hAnimatedItem_;
      msg.sGroup = (nDestX > nX_ ? Msg_Animation_Activity_MoveRight : Msg_Animation_Activity_MoveLeft);
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat " group=%s", _sz(msg.Type()), ApHandlePrintf(hParticipant_), _sz(msg.sGroup)));
      } else {
        sSequence = msg.vlSequences.atIndex(0, (nDestX > nX_ ? Msg_Animation_Activity_MoveRight : Msg_Animation_Activity_MoveLeft));
      }
    }

    if (sSequence) {
      Msg_Animation_GetSequenceInfo msg;
      msg.hItem = hAnimatedItem_;
      msg.sSequence = sSequence;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: participant=" ApHandleFormat " sequence=%s", _sz(msg.Type()), ApHandlePrintf(hParticipant_), _sz(msg.sSequence)));
      } else {
        if (msg.nDx != 0) {
          nSpeedX = abs(msg.nDx);
        }
      }
    }

  }

  ViewSrpcMessage vsm(pDisplay_, "MoveAvatarPosition");
  vsm.srpc.set("hParticipant", hParticipant_);
  vsm.srpc.set("nX", nDestX);
  vsm.srpc.set("nSpeedX", nSpeedX);
  vsm.Request();
}
