// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Location_H_INCLUDED)
#define Location_H_INCLUDED

#include "ApContainer.h"
#include "Participant.h"

typedef ApHandlePointerTree<Participant*> ParticipantList;
typedef ApHandlePointerTreeNode<Participant*> ParticipantListNode;
typedef ApHandlePointerTreeIterator<Participant*> ParticipantListIterator;

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

  void ProcessParticipantList(Apollo::ValueList& vlParticipants);
  void ParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys);
  void ReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void ParticipantAnimationFrame(const ApHandle& hParticipant, const Apollo::Image& image);

protected:
  void InitRemovedParticipants();
  void InitAddedParticipants();
  void RemoveFromRemovedParticipants(const ApHandle& h);
  void AddToAddedParticipants(const ApHandle& h);
  void EvaluateNewParticipantList(Apollo::ValueList& vlParticipants);
  void ProcessRemovedParticipants();
  void ProcessAddedParticipants();

protected:
  ApHandle hAp_;
  ArenaModule* pModule_;
  Context* pContext_;
  ParticipantList participants_;
  ApHandleTree<int> addedParticipants_;
  ApHandleTree<int> removedParticipants_;
};

#endif // Location_H_INCLUDED
