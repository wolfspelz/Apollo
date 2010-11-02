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
#include "Avatar.h"
#include "ArenaModule.h"
#include "ColorString.h"
//#include "Location.h"

#if defined(AP_TEST)

String Avatar::Test_TruncateElementText1(const ApHandle& hScene, const String& sNickname, const String& sFont, int nSize, int nFlags, int nWidth, const String& sExpected)
{
  String s;

  String sTruncated = TruncateElementText(hScene, sNickname, sFont, nSize, nFlags, nWidth);
  if (sTruncated != sExpected) {
    s.appendf("Expected=%s got=%s for font=%s size=%d flags=%d width=%d", StringType(sExpected), StringType(sTruncated), StringType(sFont), nSize, nFlags, nWidth);
  }

  return s;
}

String Avatar::Test_TruncateElementText()
{
  String s;

  ApHandle hScene = Apollo::newHandle();
  { Msg_Scene_Create msg; msg.hScene = hScene; msg.Request(); }
  { Msg_Scene_Position msg; msg.hScene = hScene; msg.nX = 100; msg.nY = 100; msg.nW = 100; msg.nH = 100; msg.Request(); }

  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 10, "1"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 30, "1234"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 9, Msg_Scene_FontFlags::Normal, 30, "123456"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 80, "123456789 "); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Arial Narrow", 14, Msg_Scene_FontFlags::Normal, 80, "123456789 1234"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Verdana", 12, Msg_Scene_FontFlags::Normal, 80, "123456789 "); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "123456789 123"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "123456789", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "123456789"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "1", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "1"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "\xE9\xA0\x81" "\xE9\xA6\x96", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "\xE9\xA0\x81" "\xE9\xA6\x96"); }
  if (!s) { s = Test_TruncateElementText1(hScene, "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96"); }

  { Msg_Scene_Destroy msg; msg.hScene = hScene; msg.Request(); }

  return s;
}

#endif // #if defined(AP_TEST)

String Avatar::TruncateElementText(const ApHandle& hScene, const String& sText, const String& sFont, int nSize, int nFlags, int nWidth)
{
  String sResult;
  String sWork = sText;

  int nCnt = 0;
  int bDone = 0;
  while (!bDone) {
    nCnt++;
    if (nCnt == 20) {
      bDone = 1;
      sResult = String::truncate(sText, 8, "...");
    } else {
      double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
      Msg_Scene_GetTextExtents::_(hScene, sWork, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);
      if (fTextW <= nWidth) {
        bDone = 1;
        sResult = sWork;
      } else {
        int nLength = sWork.chars();
        double fFraction = ((double) nWidth) / fTextW;
        int nNextLength = ((double) nLength) * (fFraction * 1.5);
        int nDeduce = nLength - nNextLength;
        if (nDeduce < 1) {
          nDeduce = 1;
        }
        sWork = sWork.subString(0, nLength - nDeduce);
      }
    }
  }

  return sResult;
}

//----------------------------------------------------------

Avatar::Avatar(ArenaModule* pModule, Display* pDisplay, const ApHandle& hParticipant)
:pModule_(pModule)
,pDisplay_(pDisplay)
,hParticipant_(hParticipant)
,hAnimatedItem_(ApNoHandle)
,nMaxW_(100)
,nMaxH_(100)
,nW_(100)
,nH_(100)
,nMinW_(20)
,nMinH_(20)
,nX_(300)
,nPositionConfirmed_(0)
{
  avatarMimeTypes_.add("avatar/gif");
  avatarMimeTypes_.add("image/gif");
  avatarMimeTypes_.add("image/png");
}

void Avatar::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
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

void Avatar::GetDetail(const String& sKey)
{
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    GetDetailData(sKey, avatarMimeTypes_);
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
      SetNickname(msg.sValue);

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      List lCoords;
      KeyValueLfBlob2List(msg.sValue, lCoords);
      Elem* e = lCoords.FindByNameCase("x");
      if (e) {
        int nX = String::atoi(e->getString());
        SetPosition(nX);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
      //= sValue;

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
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
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
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_Create failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    } else {
      hAnimatedItem_ = msgAC.hItem;
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_SetRate msgASR;
    msgASR.hItem = hAnimatedItem_;
    msgASR.nMaxRate = 10;
    if (!msgASR.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_SetRate failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
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
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_SetData failed: participant=" ApHandleFormat " data:%d bytes, source=%s", ApHandleType(hParticipant_), sbData.Length(), StringType(sSource)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->SetContextOfAnimation(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->SetParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Start msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_Start failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }
}

void Avatar::UnSubscribeDetail(const String& sKey)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  (void) msg.Request();
}

void Avatar::Show()
{
  if (pDisplay_) {
    hScene_ = pDisplay_->GetScene();
  }

  nMaxW_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MaxWidth", 100);
  nMaxH_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MaxHeight", 100);
  nW_ = nMaxW_;
  nH_ = nMaxH_;
  nMinW_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MinWidth", 20);
  nMinH_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MinHeight", 20);

  Msg_Scene_CreateElement::_(hScene_, ElementPath());
  SetUnknownPosition();

  if (Apollo::getModuleConfig(MODULE_NAME, "DebugFrame/Avatar", 0)) {
    Msg_Scene_CreateRectangle::_(hScene_, ElementPath() + "/" ELEMENT_FRAME, - (((double) nMaxW_) / 2.0) - 0.5, -0.5, (double) nMaxW_ + 1, (double) nMaxH_ + 1);
    Msg_Scene_SetStrokeColor::_(hScene_, ElementPath() + "/" ELEMENT_FRAME, 0, 0, 1, 1);
  }

  Msg_Scene_CreateImage::_(hScene_, ImagePath(), 0, 0);
  String sDefaultImage = Apollo::getModuleResourcePath(MODULE_NAME) + Apollo::getModuleConfig(MODULE_NAME, "Avatar/Image/Default", "DefaultAvatar.png");
  double fImageW, fImageH;
  if (Msg_Scene_GetImageSizeFromFile::_(hScene_, sDefaultImage, fImageW, fImageH)) {
    Msg_Scene_TranslateElement::_(hScene_, ImagePath(), - fImageW / 2.0, 0);
  }
  Msg_Scene_SetImageFile::_(hScene_, ImagePath(), sDefaultImage);

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  //Msg_Scene_Draw::_(hScene_);
}

void Avatar::Hide()
{
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_avatar);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Message);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Position);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Condition);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->DeleteContextOfAnimation(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->DeleteParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Stop msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::Hide", "Msg_Animation_Stop failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Destroy msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::Hide", "Msg_Animation_Destroy failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }

  Msg_Scene_DeleteElement::_(hScene_, ElementPath());
}

void Avatar::DetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}

void Avatar::ReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  Chatline* pChat = 0;
  chats_.Get(hChat, pChat);
  if (pChat) {
    if (pChat->sText_ != sText) {
      pChat->sText_ = sText;
      SetChatline(hChat, sText);
    }
    if (pChat->tv_ != tv) {
      pChat->tv_ = tv;
    }
  } else {
    pChat = new Chatline(sText, tv);
    chats_.Set(hChat, pChat);

    if (tvNewestChat_ < tv) {
      tvNewestChat_ = tv;
      SetChatline(hChat, sText);
    }
  }
}

void Avatar::AnimationFrame(const Apollo::Image& image)
{
  Msg_Scene_SetImageData::_(hScene_, ImagePath(), image);
  Msg_Scene_TranslateElement::_(hScene_, ImagePath(), - (double) image.Width() / 2.0, 0);
}

//----------------------------------------------------------

String& Avatar::ElementPath()
{
  if (sPath_.empty()) {
    sPath_ = "m_avatars/" + hParticipant_.toString();
  }
  return sPath_;
}

int Avatar::ElementExists(const String& sPath)
{
  int bExists = 0;
  Msg_Scene_ElementExists::_(hScene_, sPath, bExists);
  return bExists;
}

void Avatar::SetNickname(const String& sNickname)
{
  sNickname_ = sNickname;

  String sNicknamePath = NicknamePath();

  if (ElementExists(sNicknamePath)) {
    Msg_Scene_DeleteElement::_(hScene_, sNicknamePath);
  }

  String sFont = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Font", "Arial");
  int nSize = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Size", 12);
  int nFlags = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Weight", Msg_Scene_FontFlags::Normal);
  Apollo::ColorString cText = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Color", "#000000");
  Apollo::ColorString cBackground = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Background/Color", "#ffffff");
  Apollo::ColorString cBorder = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Border/Color", "#000000");
  int nWidth = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Width", 80);
  int nLeft = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Left", 20);
  int nBottom = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Bottom", 10);
  double fBorderWidth = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Border/Width", 1.0);
  double fTextPadding = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Padding", 2.5);

  String sTruncatedNickname = TruncateElementText(hScene_, sNickname, sFont, nSize, nFlags, nWidth);
  if (sTruncatedNickname != sNickname_) {
    sTruncatedNickname += Apollo::getModuleConfig(MODULE_NAME, "Nickname/Ellipsis", "...");
  }

  double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
  Msg_Scene_GetTextExtents::_(hScene_, sTruncatedNickname, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);

  Msg_Scene_CreateElement::_(hScene_, sNicknamePath);
  Msg_Scene_TranslateElement::_(hScene_, sNicknamePath, nLeft, nBottom);

  Msg_Scene_CreateRectangle::_(hScene_, sNicknamePath + "/" ELEMENT_NICKNAME_BOX, -fTextPadding, -fTextPadding, fTextAdvanceX + 2 * fTextPadding + 1, fTextH + 2 * fTextPadding);
  Msg_Scene_SetFillColor::_(hScene_, sNicknamePath + "/" ELEMENT_NICKNAME_BOX, cBackground.r, cBackground.g, cBackground.b, cBackground.a);
  Msg_Scene_SetStrokeColor::_(hScene_, sNicknamePath + "/" ELEMENT_NICKNAME_BOX, cBorder.r, cBorder.g, cBorder.b, cBorder.a);
  Msg_Scene_SetStrokeWidth::_(hScene_, sNicknamePath + "/" ELEMENT_NICKNAME_BOX, fBorderWidth);

  Msg_Scene_CreateText::_(hScene_, sNicknamePath + "/" ELEMENT_NICKNAME_TEXT, 0, 0, sTruncatedNickname, sFont, nSize, nFlags);
  Msg_Scene_SetFillColor::_(hScene_, sNicknamePath + "/" ELEMENT_NICKNAME_TEXT, cText.r, cText.g, cText.b, cText.a);
}

void Avatar::CreateChatContainer(const String& sContainer)
{
  if (!ElementExists(sContainer)) {
    Msg_Scene_CreateElement::_(hScene_, sContainer);

    int nCenter = Apollo::getModuleConfig(MODULE_NAME, "Chat/Container/Center", 0);
    int nBottom = Apollo::getModuleConfig(MODULE_NAME, "Chat/Container/Bottom", 100);
    Msg_Scene_TranslateElement::_(hScene_, sContainer, nCenter, nBottom);
  }
}

void Avatar::DeleteAllChatBubbles(const String& sContainer)
{
  Msg_Scene_GetChildren msg;
  msg.hScene = hScene_;
  msg.sPath = sContainer;
  if (msg.Request()) {
    for (Apollo::ValueElem* e = 0; e = msg.vlChildren.nextElem(e); ) {
      Msg_Scene_DeleteElement::_(hScene_, sContainer + "/" + e->getString());
    }
  }
}

void Avatar::SetChatline(const ApHandle& hChat, const String& sText)
{
  String sContainerPath = ChatContainerPath();
  String sChatPath = sContainerPath + "/" + hChat.toString();

  CreateChatContainer(sContainerPath);
  //Msg_Scene_CreateRectangle::_(hScene_, sChatPath, -40, 0, 80, 20);
  //Msg_Scene_SetFillColor::_(hScene_, sChatPath, 0, 0, 0, 1);
  DeleteAllChatBubbles(sContainerPath);

  String sFont = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Font", "Arial");
  int nSize = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Size", 12);
  int nFlags = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Weight", Msg_Scene_FontFlags::Normal);
  Apollo::ColorString cText = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Color", "#000000");
  Apollo::ColorString cBackground = Apollo::getModuleConfig(MODULE_NAME, "Chat/Background/Color", "#ffffff");
  Apollo::ColorString cBorder = Apollo::getModuleConfig(MODULE_NAME, "Chat/Border/Color", "#000000");
  int nWidth = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Width", 150);
  double fBorderWidth = Apollo::getModuleConfig(MODULE_NAME, "Chat/Border/Width", 1.0);
  double fTextPadding = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Padding", 2.5);

  String sTruncatedChat = TruncateElementText(hScene_, sText, sFont, nSize, nFlags, nWidth);
  if (sTruncatedChat != sText) {
    sTruncatedChat += Apollo::getModuleConfig(MODULE_NAME, "Chat/Ellipsis", "...");
  }

  double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
  Msg_Scene_GetTextExtents::_(hScene_, sTruncatedChat, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);

  Msg_Scene_CreateElement::_(hScene_, sChatPath);
  Msg_Scene_TranslateElement::_(hScene_, sChatPath, -fTextAdvanceX / 2.0, 0);

  Msg_Scene_CreateRectangle::_(hScene_, sChatPath + "/" ELEMENT_CHAT_BOX, -fTextPadding, -fTextPadding, fTextAdvanceX + 2 * fTextPadding, fTextH + 2 * fTextPadding);
  Msg_Scene_SetFillColor::_(hScene_, sChatPath + "/" ELEMENT_CHAT_BOX, cBackground.r, cBackground.g, cBackground.b, cBackground.a);
  Msg_Scene_SetStrokeColor::_(hScene_, sChatPath + "/" ELEMENT_CHAT_BOX, cBorder.r, cBorder.g, cBorder.b, cBorder.a);
  Msg_Scene_SetStrokeWidth::_(hScene_, sChatPath + "/" ELEMENT_CHAT_BOX, fBorderWidth);
  Msg_Scene_RoundedRectangle::_(hScene_, sChatPath + "/" ELEMENT_CHAT_BOX, 0);

  Msg_Scene_CreateText::_(hScene_, sChatPath + "/" ELEMENT_CHAT_TEXT, 0, 0, sTruncatedChat, sFont, nSize, nFlags);
  Msg_Scene_SetFillColor::_(hScene_, sChatPath + "/" ELEMENT_CHAT_TEXT, cText.r, cText.g, cText.b, cText.a);
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
  Msg_Scene_TranslateElement::_(hScene_, ElementPath(), nX, 0);
}

void Avatar::SetPosition(int nX)
{
  nX_ = nX;
  nPositionConfirmed_ = 1;
  Msg_Scene_TranslateElement::_(hScene_, ElementPath(), nX, 0);
}
