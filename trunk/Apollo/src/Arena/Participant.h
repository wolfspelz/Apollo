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
  Participant(ApHandle hParticipant, ArenaModule* pModule, Location* pLocation);

  ApHandle apHandle() { return hAp_; }

  void Show();
  void Hide();
  void DetailsChanged(Apollo::ValueList& vlKeys);
  void ReceivePublicChat(ApHandle hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void AnimationFrame(const Apollo::Image& image);

protected:
  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnSubscribeDetail(const String& sKey);
  void HandleAvatarData(const String& sMimeType, const String& sSource, Buffer& sbData);

protected:
  ApHandle hAp_;
  ArenaModule* pModule_;
  Location* pLocation_;
  String sNickname_;

  Apollo::ValueList noMimeTypes_;
  Apollo::ValueList avatarMimeTypes_;
  
  ApHandle hAnimatedItem_;
  ChatlineList chats_;
};

#endif // Participant_H_INCLUDED
