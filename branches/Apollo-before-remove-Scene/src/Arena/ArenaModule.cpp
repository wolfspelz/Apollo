// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "ArenaModule.h"
#include "Avatar.h"

Display* ArenaModule::CreateDisplay(const ApHandle& hContext)
{
  Display* pDisplay = new Display(this, hContext);
  if (pDisplay) {
    int ok = pDisplay->Create();
    if (ok ) {
      displays_.Set(hContext, pDisplay);
    } else {
      delete pDisplay;
      pDisplay = 0;
    }
  }
  return pDisplay;
}

void ArenaModule::DeleteDisplay(const ApHandle& hContext)
{
  Display* pDisplay = FindDisplay(hContext);
  if (pDisplay) {
    pDisplay->Destroy();
    displays_.Unset(hContext);
    delete pDisplay;
    pDisplay = 0;
  }
}

Display* ArenaModule::FindDisplay(const ApHandle& hContext)
{
  Display* pDisplay = 0;  
  displays_.Get(hContext, pDisplay);
  return pDisplay;
}

//---------------------------

//void ArenaModule::DeleteOldLeaveRequestedLocations()
//{
//  int bDone = 0;
//  while (!bDone) {
//    ApHandle hLocation;
//    bDone = 1;
//
//    LocationListIterator iter(locations_);
//    for (LocationListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
//      Location* pLocation = pNode->Value();
//      if (pLocation) {
//        int bTooOld = pLocation->TellDeleteMe();
//        if (bTooOld) {
//          hLocation = pNode->Key();
//          bDone = 0;
//          break;
//        }
//      }
//    }
//
//    if (ApIsHandle(hLocation)) {
//      apLog_Info((LOG_CHANNEL, "ArenaModule::DeleteOldLeaveRequestedLocations", "deleting location=" ApHandleFormat "", ApHandleType(hLocation)));
//      DeleteLocation(hLocation);
//    }
//  }
//}

//---------------------------

void ArenaModule::SetContextOfHandle(const ApHandle& h, const ApHandle& hContext)
{
  if (contextOfHandle_.IsSet(h)) {
    contextOfHandle_.Unset(h);
  }
  contextOfHandle_.Set(h, hContext);
}

void ArenaModule::DeleteContextOfHandle(const ApHandle& h, const ApHandle& hContext)
{
  ApHandle hDelete;
  contextOfHandle_.Get(h, hDelete);
  if (hDelete != hContext) {
    apLog_Warning((LOG_CHANNEL, "ArenaModule::DeleteContextOfHandle", "Context not found for handle=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(h), ApHandleType(hContext)));
  } else {
    contextOfHandle_.Unset(h);
  }
}

ApHandle ArenaModule::GetContextOfHandle(const ApHandle& h)
{
  if (contextOfHandle_.IsSet(h)) {
    return contextOfHandle_.Find(h)->Value();
  }
  return ApNoHandle;
}

Display* ArenaModule::GetDisplayOfHandle(const ApHandle& h)
{
  ApHandle hContext = GetContextOfHandle(h);
  if (ApIsHandle(hContext)) {
    return FindDisplay(hContext);
  }
  return 0;
}

//---------------------------

void ArenaModule::SetParticipantOfAnimation(const ApHandle& hAnimation, const ApHandle& hParticipant)
{
  if (participantOfAnimation_.IsSet(hAnimation)) {
    participantOfAnimation_.Unset(hAnimation);
  }
  participantOfAnimation_.Set(hAnimation, hParticipant);
}

void ArenaModule::DeleteParticipantOfAnimation(const ApHandle& hAnimation, const ApHandle& hParticipant)
{
  ApHandle h;
  participantOfAnimation_.Get(hAnimation, h);
  if (h != hParticipant) {
    apLog_Warning((LOG_CHANNEL, "ArenaModule::DeleteParticipantOfAnimation", "Participant not found for loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(hParticipant)));
  } else {
    participantOfAnimation_.Unset(hAnimation);
  }
}

ApHandle ArenaModule::GetParticipantOfAnimation(const ApHandle& hAnimation)
{
  if (participantOfAnimation_.IsSet(hAnimation)) {
    return participantOfAnimation_.Find(hAnimation)->Value();
  }
  return ApNoHandle;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextCreated)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    apLog_Warning((LOG_CHANNEL, "ArenaModule:VpView_ContextCreated", "Display already exists for ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  } else {
    pDisplay = CreateDisplay(pMsg->hContext);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDestroyed)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) {
    apLog_Warning((LOG_CHANNEL, "ArenaModule::VpView_ContextDestroyed", "No display for ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  } else {
    DeleteDisplay(pMsg->hContext);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextVisibility)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->SetVisibility(pMsg->bVisible);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextPosition)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->SetPosition(pMsg->nX, pMsg->nY);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextSize)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->SetSize(pMsg->nWidth, pMsg->nHeight);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextLocationAssigned)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->AttachLocation(pMsg->hLocation);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextLocationUnassigned)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->DetachLocation(pMsg->hLocation);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationRequested)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnEnterRequested();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationBegin)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnEnterBegin();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationComplete)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnEnterComplete();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationContextsChanged) {}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantsChanged)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnParticipantsChanged();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationPublicChat)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    Apollo::TimeValue tv(pMsg->nSec, pMsg->nMicroSec);
    pDisplay->OnReceivePublicChat(pMsg->hParticipant, pMsg->hChat, pMsg->sNickname, pMsg->sText, tv);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationDetailsChanged) {}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDetailsChanged)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->OnContextDetailsChanged(pMsg->vlKeys);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantDetailsChanged)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnParticipantDetailsChanged(pMsg->hParticipant, pMsg->vlKeys);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationRequested)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnLeaveRequested();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationBegin)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnLeaveBegin();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationComplete)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) { 
    pDisplay->OnLeaveComplete();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantAdded){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantRemoved){}

//----------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_SequenceBegin){}

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_Frame)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hItem);
  if (pDisplay) {
    ApHandle hParticipant = GetParticipantOfAnimation(pMsg->hItem);
    pDisplay->OnAvatarAnimationFrame(hParticipant, pMsg->iFrame);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_SequenceEnd){}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, Timer_Event)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hTimer);
  if (pDisplay) { 
    int bConsumed = pDisplay->OnTimerEvent(pMsg);
    if (bConsumed) {
      pMsg->Stop();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, Scene_MouseEvent)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hScene);
  if (pDisplay) { 
    int bHandled = pDisplay->OnMouseEvent(pMsg);
    if (bHandled) {
      pMsg->Stop();
    }
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    AP_UNITTEST_REGISTER(Avatar::Test_TruncateElementText);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    AP_UNITTEST_EXECUTE(Avatar::Test_TruncateElementText);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int ArenaModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextCreated, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextDestroyed, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextVisibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextSize, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LocationsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextLocationAssigned, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextLocationUnassigned, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_EnterLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_EnterLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_EnterLocationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LocationContextsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LocationPublicChat, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LocationDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LeaveLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LeaveLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LeaveLocationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantAdded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantRemoved, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_SequenceBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_Frame, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_SequenceEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Timer_Event, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Scene_MouseEvent, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(ArenaModule, this);

  return ok;
}

void ArenaModule::Exit()
{
  AP_UNITTEST_UNHOOK(ArenaModule, this);
  AP_MSG_REGISTRY_FINISH;
}