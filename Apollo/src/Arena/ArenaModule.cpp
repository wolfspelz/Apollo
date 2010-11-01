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

Display* ArenaModule::CreateDisplay(const ApHandle& hContext)
{
  Display* pDisplay = new Display(hContext);
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

Location* ArenaModule::CreateLocation(const ApHandle& hLocation)
{
  Location* pLocation = new Location(hLocation, this);
  if (pLocation) {
    locations_.Set(hLocation, pLocation);
  }
  return pLocation;
}

void ArenaModule::DeleteLocation(const ApHandle& hLocation)
{
  Location* pLocation = FindLocation(hLocation);
  if (pLocation) {
    locations_.Unset(hLocation);
    delete pLocation;
    pLocation = 0;
  }
}

Location* ArenaModule::FindLocation(const ApHandle& hLocation)
{
  Location* pLocation = 0;  
  locations_.Get(hLocation, pLocation);
  return pLocation;
}

void ArenaModule::DeleteOldLeaveRequestedLocations()
{
  int bDone = 0;
  while (!bDone) {
    ApHandle hLocation;
    bDone = 1;

    LocationListIterator iter(locations_);
    for (LocationListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
      Location* pLocation = pNode->Value();
      if (pLocation) {
        int bTooOld = pLocation->TellDeleteMe();
        if (bTooOld) {
          hLocation = pNode->Key();
          bDone = 0;
          break;
        }
      }
    }

    if (ApIsHandle(hLocation)) {
      apLog_Info((LOG_CHANNEL, "ArenaModule::DeleteOldLeaveRequestedLocations", "deleting location=" ApHandleFormat "", ApHandleType(hLocation)));
      DeleteLocation(hLocation);
    }
  }
}

//---------------------------

void ArenaModule::SetLocationOfContext(const ApHandle& hContext, const ApHandle& hLocation)
{
  if (locationOfContext_.IsSet(hContext)) {
    locationOfContext_.Unset(hContext);
  }
  locationOfContext_.Set(hContext, hLocation);
}

void ArenaModule::DeleteLocationOfContext(const ApHandle& hContext, const ApHandle& hLocation)
{
  if (locationOfContext_.IsSet(hContext)) {
    locationOfContext_.Unset(hContext);
  }
}

ApHandle ArenaModule::GetLocationOfContext(const ApHandle& hContext)
{
  if (locationOfContext_.IsSet(hContext)) {
    return locationOfContext_.Find(hContext)->Value();
  }
  return ApNoHandle;
}

//---------------------------

void ArenaModule::RegisterLocationAvatarOfAnimatedItem(const ApHandle& hItem, const ApHandle& hLocation, const ApHandle& hAvatar)
{
  LocationAvatar lp(hLocation, hAvatar);
  locationAvatarOfAnimatedItem_.Set(hItem, lp);
}

void ArenaModule::UnregisterLocationAvatarOfAnimatedItem(const ApHandle& hItem)
{
  locationAvatarOfAnimatedItem_.Unset(hItem);
}

int ArenaModule::GetLocationAvatarOfAnimatedItem(const ApHandle& hItem, ApHandle& hLocation, ApHandle& hAvatar)
{
  int ok = 0;

  ApHandleTreeNode<LocationAvatar>* pNode = locationAvatarOfAnimatedItem_.Find(hItem);
  if (pNode) {
    ok = 1;
    hLocation = pNode->Value().hLocation_;
    hAvatar = pNode->Value().hPartcipant_;
  }

  return ok;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextCreated)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay != 0) { throw ApException("ArenaModule::VpView_ContextCreated: context=" ApHandleFormat " already exists", ApHandleType(pMsg->hContext)); }
  pDisplay = CreateDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextCreated: CreateDisplay(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDestroyed)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextDestroyed: context=" ApHandleFormat " does not already exists", ApHandleType(pMsg->hContext)); }
  if (pDisplay) { 
    DeleteDisplay(pMsg->hContext); 
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextVisibility)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextVisibility: FindDisplay(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  pDisplay->SetVisibility(pMsg->bVisible);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextPosition)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextPosition: FindDisplay(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  pDisplay->SetPosition(pMsg->nX, pMsg->nY);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextSize)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextSize: FindDisplay(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  pDisplay->SetSize(pMsg->nWidth, pMsg->nHeight);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextLocationAssigned)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) {
    pLocation = CreateLocation(pMsg->hLocation);
    if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationAssigned: CreateLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  }
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextLocationAssigned: FindDisplay(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  SetLocationOfContext(pMsg->hContext, pMsg->hLocation);
  pLocation->AssignDisplay(pDisplay);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextLocationUnassigned)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindDisplay(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  DeleteLocationOfContext(pMsg->hContext, pMsg->hLocation);
  pLocation->UnassignDisplay(pDisplay);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationRequested)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_EnterLocationRequested: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->EnterRequested();
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationBegin)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_EnterLocationBegin: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->EnterBegin();
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationComplete)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_EnterLocationBegin: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->EnterComplete();
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationContextsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantsChanged)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  Msg_VpView_GetParticipants msg;
  msg.hLocation = pMsg->hLocation;
  if (!msg.Request()) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: Msg_VpView_GetParticipants(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  pLocation->ProcessAvatarList(msg.vlParticipants);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationPublicChat)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_LocationPublicChat: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  Apollo::TimeValue tv(pMsg->nSec, pMsg->nMicroSec);
  pLocation->ReceivePublicChat(pMsg->hParticipant, pMsg->hChat, pMsg->sNickname, pMsg->sText, tv);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationDetailsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDetailsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantDetailsChanged)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->ParticipantDetailsChanged(pMsg->hParticipant, pMsg->vlKeys);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationRequested)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_LeaveLocationRequested: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->LeaveRequested();
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationBegin)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_LeaveLocationBegin: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->LeaveBegin();
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationComplete)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_LeaveLocationComplete: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->LeaveComplete();
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantAdded){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantRemoved){}

//----------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_SequenceBegin){}

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_Frame)
{
  ApHandle hLocation;
  ApHandle hAvatar;
//  if (!GetLocationAvatarOfAnimatedItem(pMsg->hItem, hLocation, hAvatar)) { throw ApException("ArenaModule::Animation_Frame: GetLocationAvatarOfAnimatedItem(" ApHandleFormat ") failed", ApHandleType(pMsg->hItem)); }
  if (!GetLocationAvatarOfAnimatedItem(pMsg->hItem, hLocation, hAvatar)) { return; }

  Location* pLocation = FindLocation(hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::Animation_Frame: FindLocation(" ApHandleFormat ") failed", ApHandleType(hLocation)); }

  pLocation->AvatarAnimationFrame(hAvatar, pMsg->iFrame);
}

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_SequenceEnd){}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, System_60SecTimer)
{
  DeleteOldLeaveRequestedLocations();
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

 // HDC dcScreen = ::GetDC(NULL);
	//HDC dcMemory = ::CreateCompatibleDC(dcScreen);
 // cairo_surface_t *surface = cairo_win32_surface_create(dcMemory);
 // cairo_t *cr = cairo_create(surface);
 // cairo_destroy(cr); 
 // cairo_surface_destroy(surface);
 // ::DeleteDC(dcMemory);
 // ::ReleaseDC(NULL, dcScreen);

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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, System_60SecTimer, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(ArenaModule, this);

  return ok;
}

void ArenaModule::Exit()
{
  AP_UNITTEST_UNHOOK(ArenaModule, this);
  AP_MSG_REGISTRY_FINISH;
}
