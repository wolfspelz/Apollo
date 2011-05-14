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
class Display;

class Avatar
{
public:
  Avatar(ArenaModule* pModule, Display* pDisplay, const ApHandle& hParticipant);

  void Create(int bSelf);
  void Destroy();

  void OnDetailsChanged(Apollo::ValueList& vlKeys);
  void OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void OnAnimationBegin(const String& sUrl);

protected:
  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailRef(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnSubscribeDetail(const String& sKey);

  void RemoveOldChats(int nMax);

  void SetNickname(const String& sNickname);
  void SetImage(const String& sUrl);
  void AddChatline(const ApHandle& hChat, const String& sText);
  void SetChatline(const ApHandle& hChat, const String& sText);
  void RemoveChatline(const ApHandle& hChat);
  void SetPosition(int nX);
  void SetUnknownPosition();
  void HandleImageData(const String& sMimeType, const String& sSource, Buffer& sbData);

  void CreateChatContainer(const String& sContainer);
  void DeleteAllChatBubbles(const String& sContainer);

protected:
  ArenaModule* pModule_;
  Display* pDisplay_;
  ApHandle hParticipant_;

  String sNickname_;
  String sImage_;

  Apollo::ValueList noMimeTypes_;
  Apollo::ValueList avatarMimeTypes_;

  ApHandle hAnimatedItem_;
  ChatlineList chats_;
  Apollo::TimeValue tvNewestChat_;

  int nX_;
  int nPositionConfirmed_;

  ApHandle hView_;
};

#endif // Avatar_H_INCLUDED
