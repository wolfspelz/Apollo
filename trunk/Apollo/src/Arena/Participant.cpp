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

#if defined(AP_TEST)

String Participant::Test_TruncateNickname1(const ApHandle& hScene, const String& sNickname, const String& sFont, int nSize, int nFlags, int nWidth, const String& sExpected)
{
  String s;

  String sTruncated = TruncateNickname(hScene, sNickname, sFont, nSize, nFlags, nWidth);
  if (sTruncated != sExpected) {
    s.appendf("Expected=%s got=%s for font=%s size=%d flags=%d width=%d", StringType(sExpected), StringType(sTruncated), StringType(sFont), nSize, nFlags, nWidth);
  }

  return s;
}

String Participant::Test_TruncateNickname()
{
  String s;

  ApHandle hScene = Apollo::newHandle();
  { Msg_Scene_Create msg; msg.hScene = hScene; msg.Request(); }
  { Msg_Scene_Position msg; msg.hScene = hScene; msg.nX = 100; msg.nY = 100; msg.nW = 100; msg.nH = 100; msg.Request(); }

  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 10, "1"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 30, "1234"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 9, Msg_Scene_FontFlags::Normal, 30, "123456"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Courier New", 14, Msg_Scene_FontFlags::Normal, 80, "123456789 "); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Arial Narrow", 14, Msg_Scene_FontFlags::Normal, 80, "123456789 1234"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Verdana", 12, Msg_Scene_FontFlags::Normal, 80, "123456789 "); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789 123456789 123456789 123456789 123456789 ", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "123456789 123"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "123456789", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "123456789"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "1", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "1"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "\xE9\xA0\x81" "\xE9\xA6\x96", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "\xE9\xA0\x81" "\xE9\xA6\x96"); }
  if (!s) { s = Test_TruncateNickname1(hScene, "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96", "Verdana", 10, Msg_Scene_FontFlags::Normal, 80, "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96" "\xE9\xA0\x81" "\xE9\xA6\x96"); }

  { Msg_Scene_Destroy msg; msg.hScene = hScene; msg.Request(); }

  return s;
}

#endif // #if defined(AP_TEST)

String Participant::TruncateNickname(const ApHandle& hScene, const String& sText, const String& sFont, int nSize, int nFlags, int nWidth)
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

class Color
{
public:
  Color(const String& sColor)
  {
    String s = sColor;
    if (s.startsWith("#")) {
      s = s.subString(1);
      if (s.chars() == 6) {
        a = 1;
      } else if (s.chars() == 8) {
        String a1 = s.subString(0, 1);
        String a2 = s.subString(1, 1);
        a = (16 * Nibble(a1) + Nibble(a2)) / 255.0;
        s = s.subString(2);
      }
      if (s.chars() == 3) {
        String r1 = s.subString(0, 1);
        r = (16 * Nibble(r1)) / 255.0;
        String g1 = s.subString(2, 1);
        g = (16 * Nibble(g1)) / 255.0;
        String b1 = s.subString(4, 1);
        b = (16 * Nibble(b1)) / 255.0;
      } else if (s.chars() == 6) {
        String r1 = s.subString(0, 1);
        String r2 = s.subString(1, 1);
        r = (16 * Nibble(r1) + Nibble(r2)) / 255.0;
        String g1 = s.subString(2, 1);
        String g2 = s.subString(3, 1);
        g = (16 * Nibble(g1) + Nibble(g2)) / 255.0;
        String b1 = s.subString(4, 1);
        String b2 = s.subString(5, 1);
        b = (16 * Nibble(b1) + Nibble(b2)) / 255.0;
      } else {
        r = g = b = 0;
      }
    } else {
      a = r = g = b = 0;
    }
  }

  int Nibble(const String& s)
  {
    String::UTF8Char c = String::UTF8_Char(String::toLower(s).c_str());
    if (::isdigit(c)) {
      return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
      return c - 'a' + 10;
    }
    return 0;
  }

  double r;
  double g;
  double b;
  double a;
};

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

      int bExists = 0;
      if (Msg_Scene_ElementExists::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME, bExists) && bExists) {
        Msg_Scene_DeleteElement::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME);
      }

      String sFont = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Font", "Arial Narrow");
      int nSize = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Size", 12);
      int nFlags = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Weight", Msg_Scene_FontFlags::Normal);
      Color cText = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Color", "#000000");
      Color cBackground = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Background/Color", "#ffffff");
      Color cBorder = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Border/Color", "#000000");
      int nWidth = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Width", 80);
      int nLeft = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Left", 20);
      int nBottom = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Bottom", 10);
      double fBorderWidth = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Border/Width", 1.0);
      double fTextPadding = Apollo::getModuleConfig(MODULE_NAME, "Nickname/Text/Padding", 2.5);

      String sTruncatedNickname = TruncateNickname(hScene_, sNickname_, sFont, nSize, nFlags, nWidth);
      if (sTruncatedNickname != sNickname_) {
        sTruncatedNickname += Apollo::getModuleConfig(MODULE_NAME, "Nickname/Ellipsis", "...");
      }

      double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
      Msg_Scene_GetTextExtents::_(hScene_, sTruncatedNickname, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);

      Msg_Scene_CreateElement::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME);
      Msg_Scene_TranslateElement::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME, nLeft, nBottom);

      Msg_Scene_CreateRectangle::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME "/" ELEMENT_NICKNAME_BOX, -fTextPadding, -fTextPadding, fTextW + 2 * fTextPadding, fTextH + 2 * fTextPadding);
      Msg_Scene_SetFillColor::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME "/" ELEMENT_NICKNAME_BOX, cBackground.r, cBackground.g, cBackground.b, cBackground.a);
      Msg_Scene_SetStrokeColor::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME "/" ELEMENT_NICKNAME_BOX, cBorder.r, cBorder.g, cBorder.b, cBorder.a);
      Msg_Scene_SetStrokeWidth::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME "/" ELEMENT_NICKNAME_BOX, fBorderWidth);

      Msg_Scene_CreateText::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME "/" ELEMENT_NICKNAME_TEXT, 0, 0, sTruncatedNickname, sFont, nSize, nFlags);
      Msg_Scene_SetFillColor::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_NICKNAME "/" ELEMENT_NICKNAME_TEXT, cText.r, cText.g, cText.b, cText.a);

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
  if (pLocation_ != 0 && pLocation_->GetContext() != 0) {
    hScene_ = pLocation_->GetContext()->Scene();
  }

  Msg_Scene_CreateElement::_(hScene_, GetAvatarElementPath());
  Msg_Scene_TranslateElement::_(hScene_, GetAvatarElementPath(), 200, 0);

  Msg_Scene_CreateRectangle::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_FRAME, -50, 0, 100, 100);
  Msg_Scene_SetStrokeColor::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_FRAME, 0, 0, 1, 0.5);

  Msg_Scene_CreateImage::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_IMAGE, 0, 0);
  Msg_Scene_TranslateElement::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_IMAGE, -50, 0);
  String sDefaultAvatar = Apollo::getModuleResourcePath(MODULE_NAME) + Apollo::getModuleConfig(MODULE_NAME, "Avatar/Image/Default", "DefaultAvatar.png");
  Msg_Scene_SetImageFile::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_IMAGE, sDefaultAvatar);

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  //Msg_Scene_Draw::_(hScene_);
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

  Msg_Scene_DeleteElement::_(hScene_, GetAvatarElementPath());
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

void Participant::AnimationFrame(const Apollo::Image& image)
{
  Msg_Scene_SetImageData::_(hScene_, GetAvatarElementPath() + "/ " ELEMENT_IMAGE, image);
}

