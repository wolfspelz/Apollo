// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Avatar_H_INCLUDED)
#define Avatar_H_INCLUDED

#include "ApContainer.h"
#include "Image.h"

class Chatline
{
public:
  Chatline(const String& sText, const Apollo::TimeValue& tv)
    :sText_(sText)
    ,tv_(tv)
  {
  }

  String sText_;
  Apollo::TimeValue tv_;
};

typedef ApHandlePointerTree<Chatline*> ChatlineList;
typedef ApHandlePointerTreeIterator<Chatline*> ChatlineListIterator;
typedef ApHandlePointerTreeNode<Chatline*> ChatlineListNode;

class ArenaModule;
class Location;

class Avatar
{
public:
  Avatar(const ApHandle& hParticipant, ArenaModule* pModule, Location* pLocation);

  void Show();
  void Hide();
  void DetailsChanged(Apollo::ValueList& vlKeys);
  void ReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void AnimationFrame(const Apollo::Image& image);

  int MaxHeight() { return nMaxH_; }
  int MaxWidth() { return nMaxW_; }
  int Height() { return nH_; }
  int Width() { return nW_; }
  int MinHeight() { return nMinH_; }
  int MinWidth() { return nMinW_; }

#if defined(AP_TEST)
  static String Test_TruncateElementText1(const ApHandle& hScene, const String& sNickname, const String& sFont, int nSize, int nFlags, int nWidth, const String& sExpected);
  static String Test_TruncateElementText();
#endif // #if defined(AP_TEST)

protected:
  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnSubscribeDetail(const String& sKey);
  void HandleImageData(const String& sMimeType, const String& sSource, Buffer& sbData);

  void SetNickname(const String& sNickname);
  void SetChatline(const ApHandle& hChat, const String& sText);
  void SetPosition(int nX);
  void SetUnknownPosition();

  String& ElementPath();
  String NicknamePath() { return ElementPath() + "/" ELEMENT_NICKNAME; }
  String ImagePath() { return ElementPath() + "/" ELEMENT_IMAGE; }
  String ChatContainerPath() { return ElementPath() + "/" + ELEMENT_CHAT; }

  int ElementExists(const String& sPath);
  void CreateChatContainer(const String& sContainer);
  void DeleteAllChatBubbles(const String& sContainer);

  static String TruncateElementText(const ApHandle& hScene, const String& sText, const String& sFont, int nSize, int nFlags, int nWidth);

protected:
  ApHandle hParticipant_;
  ArenaModule* pModule_;
  Location* pLocation_;
  String sNickname_;
  String sPath_;

  Apollo::ValueList noMimeTypes_;
  Apollo::ValueList avatarMimeTypes_;
  
  ApHandle hAnimatedItem_;
  ChatlineList chats_;
  Apollo::TimeValue tvNewestChat_;

  int nMaxW_;
  int nMaxH_;
  int nW_;
  int nH_;
  int nMinW_;
  int nMinH_;

  int nX_;
  int nPositionConfirmed_;

  ApHandle hScene_;
};

#endif // Avatar_H_INCLUDED
