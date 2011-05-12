// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVpView.h"
#include "MsgAnimation.h"
#include "ColorString.h"
#include "Local.h"
#include "Avatar.h"
#include "WebArenaModule.h"

Avatar::Avatar(WebArenaModule* pModule, Display* pDisplay, const ApHandle& hParticipant)
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
    GetDetailRef(sKey, avatarMimeTypes_);
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

void Avatar::GetDetailRef(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailRef msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      SetImage(msg.sUrl);
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
  nMaxW_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MaxWidth", 100);
  nMaxH_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MaxHeight", 100);
  nW_ = nMaxW_;
  nH_ = nMaxH_;
  nMinW_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MinWidth", 20);
  nMinH_ = Apollo::getModuleConfig(MODULE_NAME, "Avatar/MinHeight", 20);

  {
    Msg_VpView_GetParticipantDetailString msg;
    msg.hParticipant = hParticipant_;
    msg.sKey = Msg_VpView_ParticipantDetail_Nickname;
    msg.Request();
    sNickname_ = msg.sValue;
  }

  DisplaySrpcMessage dsm(pDisplay_, "ShowAvatar");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("sNickname", "noname");
  dsm.srpc.setString("sImageUrl", "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + Apollo::getModuleConfig(MODULE_NAME, "Avatar/Image/Default", "DefaultAvatar.png"));
  dsm.srpc.setInt("nX", nX_);
  dsm.Request();

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
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

  DisplaySrpcMessage dsm(pDisplay_, "HideAvatar");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.Request();
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

//----------------------------------------------------------

void Avatar::SetNickname(const String& sNickname)
{
  sNickname_ = sNickname;

  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarNickname");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("sNickname", sNickname_);
  dsm.Request();
}

void Avatar::SetImage(const String& sUrl)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarImage");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("sUrl", sUrl);
  dsm.Request();
}

void Avatar::CreateChatContainer(const String& sContainer)
{
}

void Avatar::DeleteAllChatBubbles(const String& sContainer)
{
}

void Avatar::SetChatline(const ApHandle& hChat, const String& sText)
{
  //CreateChatContainer(sContainerPath);
  //DeleteAllChatBubbles(sContainerPath);

  //String sFont = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Font", "Arial");
  //int nSize = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Size", 12);
  //int nFlags = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Weight", Msg_Scene_FontFlags::Normal);
  //Apollo::ColorString cText = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Color", "#000000");
  //Apollo::ColorString cBackground = Apollo::getModuleConfig(MODULE_NAME, "Chat/Background/Color", "#ffffff");
  //Apollo::ColorString cBorder = Apollo::getModuleConfig(MODULE_NAME, "Chat/Border/Color", "#000000");
  //int nWidth = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Width", 150);
  //double fBorderWidth = Apollo::getModuleConfig(MODULE_NAME, "Chat/Border/Width", 1.0);
  //double fTextPadding = Apollo::getModuleConfig(MODULE_NAME, "Chat/Text/Padding", 2.5);

  //String sTruncatedChat = TruncateElementText(hView_, sText, sFont, nSize, nFlags, nWidth);
  //if (sTruncatedChat != sText) {
  //  sTruncatedChat += Apollo::getModuleConfig(MODULE_NAME, "Chat/Ellipsis", "...");
  //}

  //double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
  //Msg_Scene_GetTextExtents::_(hView_, sTruncatedChat, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);

  //Msg_Scene_CreateElement::_(hView_, sChatPath);
  //Msg_Scene_TranslateElement::_(hView_, sChatPath, -fTextAdvanceX / 2.0, 0);

  //Msg_Scene_CreateRectangle::_(hView_, sChatPath + "/" ELEMENT_CHAT_BOX, -fTextPadding, -fTextPadding, fTextAdvanceX + 2 * fTextPadding, fTextH + 2 * fTextPadding);
  //Msg_Scene_SetFillColor::_(hView_, sChatPath + "/" ELEMENT_CHAT_BOX, cBackground.r, cBackground.g, cBackground.b, cBackground.a);
  //Msg_Scene_SetStrokeColor::_(hView_, sChatPath + "/" ELEMENT_CHAT_BOX, cBorder.r, cBorder.g, cBorder.b, cBorder.a);
  //Msg_Scene_SetStrokeWidth::_(hView_, sChatPath + "/" ELEMENT_CHAT_BOX, fBorderWidth);
  //Msg_Scene_SetRoundedRectangle::_(hView_, sChatPath + "/" ELEMENT_CHAT_BOX, 0);

  //Msg_Scene_CreateText::_(hView_, sChatPath + "/" ELEMENT_CHAT_TEXT, 0, 0, sTruncatedChat, sFont, nSize, nFlags);
  //Msg_Scene_SetFillColor::_(hView_, sChatPath + "/" ELEMENT_CHAT_TEXT, cText.r, cText.g, cText.b, cText.a);
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

  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarPosition");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setInt("nX", nX);
  dsm.Request();
}

void Avatar::SetPosition(int nX)
{
  nX_ = nX;
  nPositionConfirmed_ = 1;

  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarPosition");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setInt("nX", nX);
  dsm.Request();
}

//----------------------------------------------------------

String Avatar::TruncateElementText(const ApHandle& hView, const String& sText, const String& sFont, int nSize, int nFlags, int nWidth)
{
  String sResult = sText;

  //int nCnt = 0;
  //int bDone = 0;
  //while (!bDone) {
  //  nCnt++;
  //  if (nCnt == 20) {
  //    bDone = 1;
  //    sResult = String::truncate(sText, 8, "...");
  //  } else {
  //    double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
  //    Msg_Scene_GetTextExtents::_(hView, sWork, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);
  //    if (fTextW <= nWidth) {
  //      bDone = 1;
  //      sResult = sWork;
  //    } else {
  //      int nLength = sWork.chars();
  //      double fFraction = ((double) nWidth) / fTextW;
  //      int nNextLength = ((double) nLength) * (fFraction * 1.5);
  //      int nDeduce = nLength - nNextLength;
  //      if (nDeduce < 1) {
  //        nDeduce = 1;
  //      }
  //      sWork = sWork.subString(0, nLength - nDeduce);
  //    }
  //  }
  //}

  return sResult;
}

//----------------------------------------------------------

#if defined(AP_TEST)

String Avatar::Test_TruncateElementText1(const ApHandle& hView, const String& sNickname, const String& sFont, int nSize, int nFlags, int nWidth, const String& sExpected)
{
  String s;

  String sTruncated = TruncateElementText(hView, sNickname, sFont, nSize, nFlags, nWidth);
  if (sTruncated != sExpected) {
    s.appendf("Expected=%s got=%s for font=%s size=%d flags=%d width=%d", StringType(sExpected), StringType(sTruncated), StringType(sFont), nSize, nFlags, nWidth);
  }

  return s;
}

String Avatar::Test_TruncateElementText()
{
  String s;

  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 10, "1"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 30, "1234"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 9, Msg_Scene_FontFlags::Normal, 30, "123456"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 80, "123456789 "); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Arial Narrow", 14, Msg_Scene_FontFlags::Normal, 80, "123456789 1234"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Verdana", 12, Msg_Scene_FontFlags::Normal, 80, "123456789 "); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789 123456789 123456789 123456789 123456789 ", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "123456789 123"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "123456789", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "123456789"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "1", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "1"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "\xE9\xA0\x81" "\xE9\xA6\x96", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "\xE9\xA0\x81" "\xE9\xA6\x96"); }
  //if (!s) { s = Test_TruncateElementText1(hView, "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96"); }

  return s;
}

#endif // #if defined(AP_TEST)
