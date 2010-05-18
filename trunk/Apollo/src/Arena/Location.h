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

class Location
{
public:
  Location(ApHandle hLocation);

  void ProcessParticipantList(Apollo::ValueList& vlParticipants);
  void ParticipantDetailsChanged(ApHandle hParticipant, Apollo::ValueList& vlKeys);

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
  ParticipantList participants_;
  ApHandleTree<int> addedParticipants_;
  ApHandleTree<int> removedParticipants_;
};

#endif // Location_H_INCLUDED
