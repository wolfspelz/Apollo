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
  void OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tvTimestamp);
  void OnAnimationBegin(const String& sUrl);
  void OnCallModuleSrpc(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

protected:
  void OnPublicChatClosed(const ApHandle& hChat);
  void OnIconAttachmentClicked(const String& sLink);

  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailRef(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnSubscribeDetail(const String& sKey);

  void RemoveOldPublicChats(int nMax);
  void DeletePublicChat(const ApHandle& hChat);

  void DisplaySetNickname(const String& sNickname);
  void DisplaySetImage(const String& sUrl);
  void DisplaySetIconAttachment(const String& sUrl, const String& sLabel, const String& sLink);
  void DisplaySetOnlineStatus(const String& sStatus);
  void DisplayAddChatline(const ApHandle& hChat, const String& sText);
  void DisplaySetChatline(const ApHandle& hChat, const String& sText);
  void DisplayRemoveChatline(const ApHandle& hChat);
  void DisplaySetAvatarPosition(int nX);
  //void SetUnknownPosition();

  void HandleImageData(const String& sMimeType, const String& sSource, Buffer& sbData);
  void SuspendAnimation();
  void ResumeAnimation();

  void DisplayCreateChatContainer(const String& sContainer);
  void DisplayDeleteAllChatBubbles(const String& sContainer);

protected:
  ArenaModule* pModule_;
  Display* pDisplay_;
  ApHandle hParticipant_;

  String sNickname_;
  String sImage_;
  String sOnlineStatus_;
  String sCommunityTag_;
  String sCommunityName_;
  String sCommunityPage_;

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
