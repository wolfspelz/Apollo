// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Display_H_INCLUDED)
#define Display_H_INCLUDED

#include "ApContainer.h"

class Avatar;

typedef ApHandlePointerTree<Avatar*> AvatarList;
typedef ApHandlePointerTreeNode<Avatar*> AvatarListNode;
typedef ApHandlePointerTreeIterator<Avatar*> AvatarListIterator;

class ArenaModule;

class Display
{
public:
  Display(ArenaModule* pModule, const ApHandle& hContext);
  virtual ~Display();

  int Create();
  void Destroy();

  void SetVisibility(int bVisible);
  void SetPosition(int nX, int nY);
  void SetSize(int nW, int nH);

  inline ApHandle GetContext() { return hContext_; }
  inline ApHandle GetLocation() { return hLocation_; }
  void AttachLocation(const ApHandle& hLocation);
  void DetachLocation(const ApHandle& hLocation);

  typedef enum _State { NoState
    ,StateEnterRequested
    ,StateEnterBegin
    ,StateEnterComplete
    ,StateLeaveRequested
    ,StateLeaveBegin
    ,StateLeaveComplete
  } State;

  inline State GetState() { return nState_; }
  
  void OnEnterRequested();
  void OnEnterBegin();
  void OnEnterComplete();
  void OnLeaveRequested();
  void OnLeaveBegin();
  void OnLeaveComplete();

  void OnParticipantsChanged();
  void OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys);
  void OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void OnAvatarAnimationFrame(const ApHandle& hParticipant, const Apollo::Image& image);

  inline int GetWidth() { return nW_; }
  inline int GetHeight() { return nH_; }
  inline ApHandle GetScene() { return hScene_; }

  int CheckLeaveRequestedAndAbandoned();

protected:
  void ProcessAvatarList(Apollo::ValueList& vlParticipants);
  void InitRemovedParticipants();
  void InitAddedParticipants();
  void RemoveFromRemovedParticipants(const ApHandle& h);
  void AddToAddedParticipants(const ApHandle& h);
  void EvaluateNewAvatarList(Apollo::ValueList& vlParticipants);
  void ProcessRemovedParticipants();
  void ProcessAddedParticipants();

  void RemoveAllAvatars();
  void RemoveAllObjects();
  void ResetLocationInfo();

protected:
  ArenaModule* pModule_;
  ApHandle hContext_;
  ApHandle hLocation_;

  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  ApHandle hScene_;

  State nState_;
  Apollo::TimeValue tvEnterRequested_;
  Apollo::TimeValue tvEnterBegin_;
  Apollo::TimeValue tvLeaveRequested_;
  Apollo::TimeValue tvLeaveBegin_;

  AvatarList avatars_;
  ApHandleTree<int> addedParticipants_;
  ApHandleTree<int> removedParticipants_;
};

#endif // Display_H_INCLUDED
