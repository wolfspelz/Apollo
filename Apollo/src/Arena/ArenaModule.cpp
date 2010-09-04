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

Context* ArenaModule::CreateContext(ApHandle hContext)
{
  Context* pContext = new Context(hContext);
  if (pContext) {
    pContext->CreateSurface();

    contexts_.Set(hContext, pContext);
  }
  return pContext;
}

void ArenaModule::DeleteContext(ApHandle hContext)
{
  Context* pContext = FindContext(hContext);
  if (pContext) {
    contexts_.Unset(hContext);
    delete pContext;
    pContext = 0;
  }
}

Context* ArenaModule::FindContext(ApHandle hContext)
{
  Context* pContext = 0;  
  contexts_.Get(hContext, pContext);
  return pContext;
}

//---------------------------

Location* ArenaModule::CreateLocation(ApHandle hLocation)
{
  Location* pLocation = new Location(hLocation);
  if (pLocation) {
    locations_.Set(hLocation, pLocation);
  }
  return pLocation;
}

void ArenaModule::DeleteLocation(ApHandle hLocation)
{
  Location* pLocation = FindLocation(hLocation);
  if (pLocation) {
    locations_.Unset(hLocation);
    delete pLocation;
    pLocation = 0;
  }
}

Location* ArenaModule::FindLocation(ApHandle hLocation)
{
  Location* pLocation = 0;  
  locations_.Get(hLocation, pLocation);
  return pLocation;
}

//---------------------------

void ArenaModule::SetLocationOfContext(ApHandle hContext, ApHandle hLocation)
{
  if (locationOfContext_.IsSet(hContext)) {
    locationOfContext_.Unset(hContext);
  }
  locationOfContext_.Set(hContext, hLocation);
}

void ArenaModule::DeleteLocationOfContext(ApHandle hContext, ApHandle hLocation)
{
  if (locationOfContext_.IsSet(hContext)) {
    locationOfContext_.Unset(hContext);
  }
}

ApHandle ArenaModule::GetLocationOfContext(ApHandle hContext)
{
  if (locationOfContext_.IsSet(hContext)) {
    return locationOfContext_.Find(hContext)->Value();
  }
  return ApNoHandle;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextCreated)
{
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext != 0) { throw ApException("ArenaModule::VpView_ContextCreated: context=" ApHandleFormat " already exists", ApHandleType(pMsg->hContext)); }
  pContext = CreateContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextCreated: CreateContext(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDestroyed)
{
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextDestroyed: context=" ApHandleFormat " does not already exists", ApHandleType(pMsg->hContext)); }
  if (pContext) { 
    DeleteContext(pMsg->hContext); 
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextVisibility)
{
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextVisibility: FindContext(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  pContext->SetVisibility(pMsg->bVisible);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextPosition)
{
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextPosition: FindContext(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  pContext->SetPosition(pMsg->nX, pMsg->nY);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextSize)
{
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextSize: FindContext(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  pContext->SetSize(pMsg->nWidth, pMsg->nHeight);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextLocationAssigned)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) {
    pLocation = CreateLocation(pMsg->hLocation);
    if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationAssigned: CreateLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  }
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextLocationAssigned: FindContext(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  SetLocationOfContext(pMsg->hContext, pMsg->hLocation);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextLocationUnassigned)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  Context* pContext = FindContext(pMsg->hContext);
  if (pContext == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindContext(" ApHandleFormat ") failed", ApHandleType(pMsg->hContext)); }

  DeleteLocationOfContext(pMsg->hContext, pMsg->hLocation);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationRequested){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationBegin){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationComplete)
{
  Msg_VpView_ReplayLocationPublicChat msg;
  msg.hLocation = pMsg->hLocation;
  //msg.nMaxAge;
  //msg.nMaxLines;
  //msg.nMaxData;
  if (!msg.Request()) { throw ApException("ArenaModule::VpView_EnterLocationComplete: Msg_VpView_ReplayLocationPublicChat(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationContextsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantsChanged)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  Msg_VpView_GetParticipants msg;
  msg.hLocation = pMsg->hLocation;
  if (!msg.Request()) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: Msg_VpView_GetParticipants(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }
  pLocation->ProcessParticipantList(msg.vlParticipants);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationPublicChat){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationDetailsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDetailsChanged){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantDetailsChanged)
{
  Location* pLocation = FindLocation(pMsg->hLocation);
  if (pLocation == 0) { throw ApException("ArenaModule::VpView_ContextLocationUnassigned: FindLocation(" ApHandleFormat ") failed", ApHandleType(pMsg->hLocation)); }

  pLocation->ParticipantDetailsChanged(pMsg->hParticipant, pMsg->vlKeys);
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationRequested){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationBegin){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationComplete){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantAdded){}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantRemoved){}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
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

  AP_UNITTEST_HOOK(ArenaModule, this);

  return ok;
}

void ArenaModule::Exit()
{
  AP_UNITTEST_UNHOOK(ArenaModule, this);
  AP_MSG_REGISTRY_FINISH;
}
