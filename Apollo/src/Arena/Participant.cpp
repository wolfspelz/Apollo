// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgVpView.h"
#include "MsgAnimation.h"
#include "MsgScene.h"
#include "Local.h"
#include "Participant.h"
#include "ArenaModule.h"

Participant::Participant(const ApHandle& hParticipant, ArenaModule* pModule, Location* pLocation)
:hAp_(hParticipant)
,pModule_(pModule)
,pLocation_(pLocation)
,hAnimatedItem_(ApNoHandle)
{
  avatarMimeTypes_.add("avatar/gif");
  avatarMimeTypes_.add("image/gif");
  avatarMimeTypes_.add("image/png");
}

String& Participant::GetAvatarElementPath()
{
  if (sPath_.empty()) {
    sPath_ = "m_avatars/" + apHandle().toString();
  }
  return sPath_;
}

void Participant::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  if (msg.Request()) {
    if (msg.bAvailable) {
      GetDetail(sKey);
    }
  }
}

void Participant::GetDetail(const String& sKey)
{
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    GetDetailData(sKey, avatarMimeTypes_);
  } else {
    GetDetailString(sKey, noMimeTypes_);
  }
}

void Participant::GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailString msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
      sNickname_ = msg.sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
      //= sValue;

    }
  }
}

void Participant::GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailData msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      HandleAvatarData(msg.sMimeType, msg.sSource, msg.sbData);
    }
  }
}

void Participant::HandleAvatarData(const String& sMimeType, const String& sSource, Buffer& sbData)
{
  if (!ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Create msgAC;
    msgAC.hItem = Apollo::newHandle();
    msgAC.sMimeType = sMimeType;
    if (!msgAC.Request()) {
      apLog_Error((LOG_CHANNEL, "Participant::HandleAvatarData", "Msg_Animation_Create failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
    } else {
      hAnimatedItem_ = msgAC.hItem;
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_SetRate msgASR;
    msgASR.hItem = hAnimatedItem_;
    msgASR.nMaxRate = 10;
    if (!msgASR.Request()) {
      apLog_Error((LOG_CHANNEL, "Participant::HandleAvatarData", "Msg_Animation_SetRate failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
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
      apLog_Error((LOG_CHANNEL, "Participant::HandleAvatarData", "Msg_Animation_SetData failed: participant=" ApHandleFormat " data:%d bytes, source=%s", ApHandleType(hAp_), sbData.Length(), StringType(sSource)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    pModule_->RegisterLocationParticipantOfAnimatedItem(hAnimatedItem_, pLocation_->apHandle(), apHandle());

    Msg_Animation_Start msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, "Participant::HandleAvatarData", "Msg_Animation_Start failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
    }
  }
}

void Participant::UnSubscribeDetail(const String& sKey)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  (void) msg.Request();
}

void Participant::Show()
{
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  if (pLocation_ != 0 && pLocation_->GetContext() != 0) {
    ApHandle hScene = pLocation_->GetContext()->Scene();
    Msg_Scene_CreateRectangle::_(hScene, GetAvatarElementPath(), -50, 0, 100, 100);
    Msg_Scene_SetFillColor::_(hScene, GetAvatarElementPath(), 1, 0, 0, 0.5);
    Msg_Scene_TranslateElement::_(hScene, GetAvatarElementPath(), 200, 0);

    Msg_Scene_CreateImage::_(hScene, GetAvatarElementPath() + "/m_image", 0,0);
    Msg_Scene_TranslateElement::_(hScene, GetAvatarElementPath() + "/m_image", -50, 100);
    //Msg_Scene_Draw::_(hScene);
  }
}

void Participant::Hide()
{
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_avatar);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Message);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Position);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Condition);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  if (ApIsHandle(hAnimatedItem_)) {
    pModule_->UnregisterLocationParticipantOfAnimatedItem(hAnimatedItem_);

    Msg_Animation_Stop msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, "Participant::Hide", "Msg_Animation_Stop failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Destroy msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Participant::Hide", "Msg_Animation_Destroy failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
    }
  }
}

void Participant::DetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}

void Participant::ReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  if (chats_.Find(hChat) != 0) {
    chats_.Unset(hChat);
  }

  Chatline chat(sText, tv);
  chats_.Set(hChat, chat);
}

#include "ximagif.h"
void Participant::AnimationFrame(const Apollo::Image& image)
{
  if (pLocation_ != 0 && pLocation_->GetContext() != 0) {
    ApHandle hScene = pLocation_->GetContext()->Scene();

    //Msg_Scene_CreateImageFromData::_(hScene, "xx", 0,0, image);
    //Msg_Scene_TranslateElement::_(hScene, "xx", 100, 100);

    Msg_Scene_SetImageData::_(hScene, GetAvatarElementPath() + "/m_image", image);
 
    //Msg_Scene_CreateImageFromFile::_(hScene, "xx", 0,0, Apollo::getAppResourcePath() + "test/test2.png");
    //Msg_Scene_TranslateElement::_(hScene, "xx", 100, 100);

    //Apollo::Image apImg2; // from animated GIF
    //{
    //  Buffer sbData;
    //  Apollo::loadFile(Apollo::getAppResourcePath() + "test/tassadar/" + "idle.gif", sbData);
    //  CxImage cxImg(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_UNKNOWN);
    //  cxImg.SetRetreiveAllFrames(true);
    //  int nFrames = cxImg.GetNumFrames();
    //  cxImg.SetFrame(nFrames - 1);
    //  cxImg.Decode(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_GIF);
    //  CxImage* pCxImgFrame = cxImg.GetFrame(0);
    //  apImg2.Allocate(pCxImgFrame->GetWidth(), pCxImgFrame->GetHeight());
    //  CxMemFile mfDest((BYTE*) apImg2.Pixels(), apImg2.Size());
    //  pCxImgFrame->AlphaFromTransparency();
    //  pCxImgFrame->Encode2RGBA(&mfDest, true);
    //}
    //  Msg_Scene_CreateImageFromData::_(hScene, GetAvatarElementPath() + "/m_image", 0,0, apImg2);
    //  Msg_Scene_TranslateElement::_(hScene, GetAvatarElementPath() + "/m_image", 100, 100);

    //Msg_Scene_Draw::_(hScene);
  }
}

