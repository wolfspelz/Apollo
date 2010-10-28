// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Participant_H_INCLUDED)
#define Participant_H_INCLUDED

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

typedef ApHandleTree<Chatline> ChatlineList;
typedef ApHandleTreeIterator<Chatline> ChatlineListIterator;
typedef ApHandleTreeNode<Chatline> ChatlineListNode;

class ArenaModule;
class Location;

class Participant
{
public:
  Participant(const ApHandle& hParticipant, ArenaModule* pModule, Location* pLocation);

  inline ApHandle apHandle() { return hAp_; }

  void Show();
  void Hide();
  void DetailsChanged(Apollo::ValueList& vlKeys);
  void ReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void AnimationFrame(const Apollo::Image& image);

#if defined(AP_TEST)
  static String Test_TruncateNickname1(const ApHandle& hScene, const String& sNickname, const String& sFont, int nSize, int nFlags, int nWidth, const String& sExpected);
  static String Test_TruncateNickname();
#endif // #if defined(AP_TEST)

protected:
  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnSubscribeDetail(const String& sKey);
  void HandleAvatarData(const String& sMimeType, const String& sSource, Buffer& sbData);
  
  String& GetAvatarElementPath();
  static String TruncateNickname(const ApHandle& hScene, const String& sText, const String& sFont, int nSize, int nFlags, int nWidth);

protected:
  ApHandle hAp_;
  ArenaModule* pModule_;
  Location* pLocation_;
  String sNickname_;
  String sPath_;

  Apollo::ValueList noMimeTypes_;
  Apollo::ValueList avatarMimeTypes_;
  
  ApHandle hAnimatedItem_;
  ChatlineList chats_;

  ApHandle hScene_;
};

#endif // Participant_H_INCLUDED
