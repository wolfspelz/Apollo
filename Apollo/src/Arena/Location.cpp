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
,pContext_(0)
,nState_(NoState)
{
}

void Location::EnterRequested()
{
  apLog_Verbose((LOG_CHANNEL, "Location::EnterRequested", "" ApHandleFormat "", ApHandleType(apHandle())));

  nState_ = StateEnterRequested;
  tvEnterRequested_ = Apollo::TimeValue::getTime();

  if (pContext_) {
    pContext_->EnterRequested();
  }
}

void Location::EnterBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Location::EnterBegin", "" ApHandleFormat "", ApHandleType(apHandle())));

  nState_ = StateEnterBegin;
  tvEnterBegin_ = Apollo::TimeValue::getTime();

  if (pContext_) {
    pContext_->EnterBegin();
  }
}

void Location::EnterComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Location::EnterComplete", "" ApHandleFormat "", ApHandleType(apHandle())));

  nState_ = StateEnterComplete;

  if (pContext_) {
    pContext_->EnterComplete();
  }

  ApAsyncMessage<Msg_VpView_ReplayLocationPublicChat> msg;
  msg->hLocation = apHandle();
  //msg.nMaxAge;
  //msg.nMaxLines;
  //msg.nMaxData;
  //if (!msg.Request()) { throw ApException("ArenaModule::VpView_EnterLocationComplete: Msg_VpView_ReplayLocationPublicChat(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  msg->PostAsync();
}

void Location::LeaveRequested()
{
  apLog_Verbose((LOG_CHANNEL, "Location::LeaveRequested", "" ApHandleFormat "", ApHandleType(apHandle())));

  nState_ = StateLeaveRequested;
  tvLeaveRequested_ = Apollo::TimeValue::getTime();

  if (pContext_) {
    pContext_->LeaveRequested();
  }
}

void Location::LeaveBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Location::LeaveBegin", "" ApHandleFormat "", ApHandleType(apHandle())));

  nState_ = StateLeaveBegin;
  tvLeaveBegin_ = Apollo::TimeValue::getTime();

  if (pContext_) {
    pContext_->LeaveBegin();
  }
}

void Location::LeaveComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Location::LeaveComplete", "" ApHandleFormat "", ApHandleType(apHandle())));

  nState_ = StateLeaveComplete;

  if (pContext_) {
    pContext_->LeaveComplete();
  }
}

int Location::TellDeleteMe()
{
  int bResult = 0;

  if (nState_ == StateLeaveRequested) {
    Apollo::TimeValue tvSinceLeaveRequested = Apollo::TimeValue::getTime() - tvLeaveRequested_;
    if (tvSinceLeaveRequested.Sec() > Apollo::getModuleConfig(MODULE_NAME, "DisposeLocationInLeaveRequestedStateAfterSec", 180)) {
      bResult = 1;
    }
  }

  return bResult;
}

// -------------------------

void Location::ProcessAvatarList(Apollo::ValueList& vlParticipants)
{
  InitRemovedParticipants();
  InitAddedParticipants();
  EvaluateNewAvatarList(vlParticipants);
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

  AvatarListIterator iter(avatars_);
  for (AvatarListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hAvatar = pNode->Key();
    removedParticipants_.Set(hAvatar, 1);
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

void Location::EvaluateNewAvatarList(Apollo::ValueList& vlParticipants)
{
  for (Apollo::ValueElem* e = 0; e = vlParticipants.nextElem(e); ) {
    AvatarListNode* pNode = avatars_.Find(e->getHandle());
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
    ApHandle hAvatar = pNode->Key();
    Avatar* pAvatar = new Avatar(hAvatar, pModule_, this);
    if (pAvatar) {
      avatars_.Set(hAvatar, pAvatar);
      pAvatar->Show();
    }
  }
}

void Location::ProcessRemovedParticipants()
{
  ApHandleTreeIterator<int> iter(removedParticipants_);
  for (ApHandleTreeNode<int>* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hAvatar = pNode->Key();
    Avatar* pAvatar = 0;
    avatars_.Get(hAvatar, pAvatar);
    if (pAvatar) {
      pAvatar->Hide();
      avatars_.Unset(hAvatar);
      delete pAvatar;
      pAvatar = 0;
    }
  }
}

// -------------------------

void Location::ParticipantDetailsChanged(const ApHandle& hAvatar, Apollo::ValueList& vlKeys)
{
  AvatarListNode* pNode = avatars_.Find(hAvatar);
  if (pNode) {
    pNode->Value()->DetailsChanged(vlKeys);
  }
}

void Location::ReceivePublicChat(const ApHandle& hAvatar, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  AvatarListNode* pNode = avatars_.Find(hAvatar);
  if (pNode) {
    pNode->Value()->ReceivePublicChat(hChat, sNickname, sText, tv);
  }
}

void Location::AvatarAnimationFrame(const ApHandle& hAvatar, const Apollo::Image& image)
{
  AvatarListNode* pNode = avatars_.Find(hAvatar);
  if (pNode) {
    pNode->Value()->AnimationFrame(image);
  }
}
