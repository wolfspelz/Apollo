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

class Location
{
public:
  Location(ApHandle hLocation, ArenaModule* pModule);

  ApHandle apHandle() { return hAp_; }

  void ProcessParticipantList(Apollo::ValueList& vlParticipants);
  void ParticipantDetailsChanged(ApHandle hParticipant, Apollo::ValueList& vlKeys);
  void ReceivePublicChat(ApHandle hParticipant, ApHandle hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void ParticipantAnimationFrame(ApHandle hParticipant, const Apollo::Image& image);

protected:
  void InitRemovedParticipants();
  void InitAddedParticipants();
  void RemoveFromRemovedParticipants(ApHandle h);
  void AddToAddedParticipants(ApHandle h);
  void EvaluateNewParticipantList(Apollo::ValueList& vlParticipants);
  void ProcessRemovedParticipants();
  void ProcessAddedParticipants();

protected:
  ApHandle hAp_;
  ArenaModule* pModule_;
  ParticipantList participants_;
  ApHandleTree<int> addedParticipants_;
  ApHandleTree<int> removedParticipants_;
};

#endif // Location_H_INCLUDED
