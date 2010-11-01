// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Location_H_INCLUDED)
#define Location_H_INCLUDED

#include "ApContainer.h"
#include "Avatar.h"

typedef ApHandlePointerTree<Avatar*> AvatarList;
typedef ApHandlePointerTreeNode<Avatar*> AvatarListNode;
typedef ApHandlePointerTreeIterator<Avatar*> AvatarListIterator;

class ArenaModule;
class Context;

class Location
{
public:
  Location(const ApHandle& hLocation, ArenaModule* pModule);

  inline ApHandle apHandle() { return hAp_; }

  inline Context* GetContext() { return pContext_; }
  inline void ContextAssigned(Context* pContext) { pContext_ = pContext; }
  inline void ContextUnassigned(Context* pContext) { pContext_ = 0; }

  typedef enum _State { NoState
    ,StateEnterRequested
    ,StateEnterBegin
    ,StateEnterComplete
    ,StateLeaveRequested
    ,StateLeaveBegin
    ,StateLeaveComplete
  } State;

  inline State GetState() { return nState_; }
  int TellDeleteMe();

  void EnterRequested();
  void EnterBegin();
  void EnterComplete();
  void LeaveRequested();
  void LeaveBegin();
  void LeaveComplete();

  void ProcessAvatarList(Apollo::ValueList& vlParticipants);
  void ParticipantDetailsChanged(const ApHandle& hAvatar, Apollo::ValueList& vlKeys);
  void ReceivePublicChat(const ApHandle& hAvatar, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void ParticipantAnimationFrame(const ApHandle& hAvatar, const Apollo::Image& image);

protected:
  void InitRemovedParticipants();
  void InitAddedParticipants();
  void RemoveFromRemovedParticipants(const ApHandle& h);
  void AddToAddedParticipants(const ApHandle& h);
  void EvaluateNewAvatarList(Apollo::ValueList& vlParticipants);
  void ProcessRemovedParticipants();
  void ProcessAddedParticipants();

protected:
  ApHandle hAp_;
  ArenaModule* pModule_;
  Context* pContext_;
  AvatarList avatars_;
  ApHandleTree<int> addedParticipants_;
  ApHandleTree<int> removedParticipants_;

  State nState_;
  Apollo::TimeValue tvEnterRequested_;
  Apollo::TimeValue tvEnterBegin_;
  Apollo::TimeValue tvLeaveRequested_;
  Apollo::TimeValue tvLeaveBegin_;
};

#endif // Location_H_INCLUDED
