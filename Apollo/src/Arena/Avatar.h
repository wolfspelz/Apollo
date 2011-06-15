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
  void OnReceivePublicAction(const String& sAction);
  void OnAnimationBegin(const String& sUrl);
  void OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

protected:
  void OnPublicChatClosed(const ApHandle& hChat);
  void OnIconAttachmentClicked(const String& sLink);
  void OnAvatarPositionReached(int nX);
  void OnAvatarDraggedBy(int nDiffX, int nDiffY);
  void OnAvatarPointerClosed();

  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailUrl(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnsubscribeDetail(const String& sKey);

  void RemoveOldPublicChats(int nMax);
  void DeletePublicChat(const ApHandle& hChat);

  void DisplaySetNickname(const String& sNickname);
  void DisplaySetImage(const String& sUrl);
  void DisplaySetCommunityAttachment(const String& sUrl, const String& sLabel, const String& sLink);
  void DisplaySetIconAttachment(const String& sUrl, const String& sLabel);
  void DisplaySetOnlineStatus(const String& sStatus);
  void DisplayAddChatline(const ApHandle& hChat, const String& sText);
  void DisplaySetChatline(const ApHandle& hChat, const String& sText);
  void DisplayRemoveChatline(const ApHandle& hChat);
  void DisplaySetAvatarPosition(int nDestX);
  void DisplayMoveAvatarPosition(int nDestX);
  //void SetUnknownPosition();

  void HandleImageData(const String& sMimeType, const String& sSource, Buffer& sbData);
  void SuspendAnimation();
  void ResumeAnimation();
  void BeginMove(int nDestX);
  void EndMove(int nDestX);
  void SetUnknownPosition();
  int IsMoving() { return bMoving_; }

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
  String sAttachedIcon_;
  String sAttachedIconName_;

  static Apollo::ValueList noMimeTypes_;
  static Apollo::ValueList avatarMimeTypes_;
  static Apollo::ValueList imageMimeTypes_;

  ApHandle hAnimatedItem_;
  ChatlineList chats_;
  Apollo::TimeValue tvNewestChat_;

  int nX_;
  int nPositionConfirmed_;
  int bMoving_;
};

#endif // Avatar_H_INCLUDED
