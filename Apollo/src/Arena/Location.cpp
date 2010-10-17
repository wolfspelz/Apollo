// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "ApContainer.h"
#include "Local.h"
#include "Location.h"
#include "ArenaModule.h"

Location::Location(const ApHandle& hLocation, ArenaModule* pModule)
:hAp_(hLocation)
,pModule_(pModule)
{
}

void Location::ProcessParticipantList(Apollo::ValueList& vlParticipants)
{
  InitRemovedParticipants();
  InitAddedParticipants();
  EvaluateNewParticipantList(vlParticipants);
  ProcessRemovedParticipants();
  ProcessAddedParticipants();
}

void Location::InitRemovedParticipants()
{
  while (removedParticipants_.Count() >0) {
    ApHandleTreeNode<int>* pNode = removedParticipants_.Next(0);
    if (pNode) {
      removedParticipants_.Unset(pNode->Key());
    }
  }

  ParticipantListIterator iter(participants_);
  for (ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hParticipant = pNode->Key();
    removedParticipants_.Set(hParticipant, 1);
  }
}

void Location::InitAddedParticipants()
{
  while (addedParticipants_.Count() >0) {
    ApHandleTreeNode<int>* pNode = addedParticipants_.Next(0);
    if (pNode) {
      addedParticipants_.Unset(pNode->Key());
    }
  }
}

void Location::EvaluateNewParticipantList(Apollo::ValueList& vlParticipants)
{
  for (Apollo::ValueElem* e = 0; e = vlParticipants.nextElem(e); ) {
    ParticipantListNode* pNode = participants_.Find(e->getHandle());
    if (pNode) {
      RemoveFromRemovedParticipants(e->getHandle());
    } else {
      AddToAddedParticipants(e->getHandle());
    }
  }
}

void Location::RemoveFromRemovedParticipants(const ApHandle& h)
{
  removedParticipants_.Unset(h);
}

void Location::AddToAddedParticipants(const ApHandle& h)
{
  addedParticipants_.Set(h, 1);
}

void Location::ProcessAddedParticipants()
{
  ApHandleTreeIterator<int> iter(addedParticipants_);
  for (ApHandleTreeNode<int>* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hParticipant = pNode->Key();
    Participant* pParticipant = new Participant(hParticipant, pModule_, this);
    if (pParticipant) {
      participants_.Set(hParticipant, pParticipant);
      pParticipant->Show();
    }
  }
}

void Location::ProcessRemovedParticipants()
{
  ApHandleTreeIterator<int> iter(removedParticipants_);
  for (ApHandleTreeNode<int>* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hParticipant = pNode->Key();
    Participant* pParticipant = 0;
    participants_.Get(hParticipant, pParticipant);
    if (pParticipant) {
      pParticipant->Hide();
      participants_.Unset(hParticipant);
      delete pParticipant;
      pParticipant = 0;
    }
  }
}

// -------------------------

void Location::ParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys)
{
  ParticipantListNode* pNode = participants_.Find(hParticipant);
  if (pNode) {
    pNode->Value()->DetailsChanged(vlKeys);
  }
}

void Location::ReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  ParticipantListNode* pNode = participants_.Find(hParticipant);
  if (pNode) {
    pNode->Value()->ReceivePublicChat(hChat, sNickname, sText, tv);
  }
}

void Location::ParticipantAnimationFrame(const ApHandle& hParticipant, const Apollo::Image& image)
{
  ParticipantListNode* pNode = participants_.Find(hParticipant);
  if (pNode) {
    pNode->Value()->AnimationFrame(image);
  }
}
