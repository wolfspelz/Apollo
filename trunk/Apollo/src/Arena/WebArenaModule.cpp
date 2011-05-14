// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "WebArenaModule.h"
#include "Avatar.h"

Display* WebArenaModule::CreateDisplay(const ApHandle& hContext)
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

void WebArenaModule::DeleteDisplay(const ApHandle& hContext)
{
  Display* pDisplay = FindDisplay(hContext);
  if (pDisplay) {
    pDisplay->Destroy();
    displays_.Unset(hContext);
    delete pDisplay;
    pDisplay = 0;
  }
}

Display* WebArenaModule::FindDisplay(const ApHandle& hContext)
{
  Display* pDisplay = 0;
  displays_.Get(hContext, pDisplay);
  return pDisplay;
}

//---------------------------

//void WebArenaModule::DeleteOldLeaveRequestedLocations()
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
//      apLog_Info((LOG_CHANNEL, "WebArenaModule::DeleteOldLeaveRequestedLocations", "deleting location=" ApHandleFormat "", ApHandleType(hLocation)));
//      DeleteLocation(hLocation);
//    }
//  }
//}

//---------------------------

void WebArenaModule::SetContextOfHandle(const ApHandle& h, const ApHandle& hContext)
{
  if (contextOfHandle_.IsSet(h)) {
    contextOfHandle_.Unset(h);
  }
  contextOfHandle_.Set(h, hContext);
}

void WebArenaModule::DeleteContextOfHandle(const ApHandle& h, const ApHandle& hContext)
{
  ApHandle hDelete;
  contextOfHandle_.Get(h, hDelete);
  if (hDelete != hContext) {
    apLog_Warning((LOG_CHANNEL, "WebArenaModule::DeleteContextOfHandle", "Context not found for handle=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(h), ApHandleType(hContext)));
  } else {
    contextOfHandle_.Unset(h);
  }
}

ApHandle WebArenaModule::GetContextOfHandle(const ApHandle& h)
{
  if (contextOfHandle_.IsSet(h)) {
    return contextOfHandle_.Find(h)->Value();
  }
  return ApNoHandle;
}

Display* WebArenaModule::GetDisplayOfHandle(const ApHandle& h)
{
  ApHandle hContext = GetContextOfHandle(h);
  if (ApIsHandle(hContext)) {
    return FindDisplay(hContext);
  }
  return 0;
}

//---------------------------

void WebArenaModule::SetParticipantOfAnimation(const ApHandle& hAnimation, const ApHandle& hParticipant)
{
  if (participantOfAnimation_.IsSet(hAnimation)) {
    participantOfAnimation_.Unset(hAnimation);
  }
  participantOfAnimation_.Set(hAnimation, hParticipant);
}

void WebArenaModule::DeleteParticipantOfAnimation(const ApHandle& hAnimation, const ApHandle& hParticipant)
{
  ApHandle h;
  participantOfAnimation_.Get(hAnimation, h);
  if (h != hParticipant) {
    apLog_Warning((LOG_CHANNEL, "WebArenaModule::DeleteParticipantOfAnimation", "Participant not found for loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(hParticipant)));
  } else {
    participantOfAnimation_.Unset(hAnimation);
  }
}

ApHandle WebArenaModule::GetParticipantOfAnimation(const ApHandle& hAnimation)
{
  if (participantOfAnimation_.IsSet(hAnimation)) {
    return participantOfAnimation_.Find(hAnimation)->Value();
  }
  return ApNoHandle;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextCreated)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    apLog_Warning((LOG_CHANNEL, "WebArenaModule:VpView_ContextCreated", "Display already exists for ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  } else {
    pDisplay = CreateDisplay(pMsg->hContext);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextDestroyed)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) {
    apLog_Warning((LOG_CHANNEL, "WebArenaModule::VpView_ContextDestroyed", "No display for ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  } else {
    DeleteDisplay(pMsg->hContext);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextVisibility)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->SetVisibility(pMsg->bVisible);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextPosition)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->SetPosition(pMsg->nX, pMsg->nY);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextSize)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->SetSize(pMsg->nWidth, pMsg->nHeight);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LocationsChanged){}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextLocationAssigned)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->AttachLocation(pMsg->hLocation);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextLocationUnassigned)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->DetachLocation(pMsg->hLocation);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_EnterLocationRequested)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnEnterRequested();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_EnterLocationBegin)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnEnterBegin();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_EnterLocationComplete)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnEnterComplete();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LocationContextsChanged) {}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ParticipantsChanged)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnParticipantsChanged();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LocationPublicChat)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    Apollo::TimeValue tv(pMsg->nSec, pMsg->nMicroSec);
    pDisplay->OnReceivePublicChat(pMsg->hParticipant, pMsg->hChat, pMsg->sNickname, pMsg->sText, tv);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LocationDetailsChanged) {}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ContextDetailsChanged)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->OnContextDetailsChanged(pMsg->vlKeys);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ParticipantDetailsChanged)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnParticipantDetailsChanged(pMsg->hParticipant, pMsg->vlKeys);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LeaveLocationRequested)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnLeaveRequested();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LeaveLocationBegin)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnLeaveBegin();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_LeaveLocationComplete)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnLeaveComplete();
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ParticipantAdded){}

AP_MSG_HANDLER_METHOD(WebArenaModule, VpView_ParticipantRemoved){}

//----------------------------

AP_MSG_HANDLER_METHOD(WebArenaModule, Animation_SequenceBegin)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hItem);
  if (pDisplay) {
    ApHandle hParticipant = GetParticipantOfAnimation(pMsg->hItem);
    String sUrl = pMsg->sUrl;
    if (!sUrl) {
      sUrl = pMsg->sSrc;
    }
    pDisplay->OnAvatarAnimationBegin(hParticipant, sUrl);
  }
}

//AP_MSG_HANDLER_METHOD(WebArenaModule, Animation_Frame)
//{
//  Display* pDisplay = GetDisplayOfHandle(pMsg->hItem);
//  if (pDisplay) {
//    ApHandle hParticipant = GetParticipantOfAnimation(pMsg->hItem);
//    pDisplay->OnAvatarAnimationFrame(hParticipant, pMsg->iFrame);
//  }
//}
//
//AP_MSG_HANDLER_METHOD(WebArenaModule, Animation_SequenceEnd)
//{
//  Display* pDisplay = GetDisplayOfHandle(pMsg->hItem);
//  if (pDisplay) {
//    ApHandle hParticipant = GetParticipantOfAnimation(pMsg->hItem);
//    pDisplay->OnAvatarAnimationEnd(hParticipant);
//  }
//}

//----------------------------

AP_SRPC_HANDLER_METHOD(WebArenaModule, WebArena_CallModuleSrpc, ApSRPCMessage)
{
  String sView = pMsg->srpc.getString("hView");
  if (!sView) { throw ApException("Missing hView"); }

  ApHandle hView = Apollo::string2Handle(sView);
  if (!ApIsHandle(hView)) { throw ApException("Not a handle: %s", StringType(sView)); }

  Display* pDisplay = GetDisplayOfHandle(hView);
  if (pDisplay) {
    pDisplay->OnCallModuleSrpc(pMsg->srpc, pMsg->response);
  }
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

ApHandle Test_Avatar_RemoveOldChats_hRemovedChat_;

void Test_Avatar_RemoveOldChats_DisplaySrpcMessage(DisplaySrpcMessage* pMsg)
{
  if (pMsg->srpc.getString("Method") == "RemoveAvatarChat") {
    Test_Avatar_RemoveOldChats_hRemovedChat_ = Apollo::string2Handle(pMsg->srpc.getString("hChat"));
  }
}

static String Test_Avatar_RemoveOldChats()
{
  String s;

  WebArenaModule m;
  Display d(&m, Apollo::newHandle());
  Avatar a(&m, &d, Apollo::newHandle());

  { DisplaySrpcMessage msg(&d, "Dummy"); msg.Hook(MODULE_NAME, (ApCallback) Test_Avatar_RemoveOldChats_DisplaySrpcMessage, 0, ApCallbackPosEarly); }  

  ApHandle hChat2 = Apollo::newHandle();
  ApHandle hChat1 = Apollo::newHandle();
  ApHandle hChat4 = Apollo::newHandle();
  ApHandle hChat3 = Apollo::newHandle();

  a.OnReceivePublicChat(hChat4, "Nickname4", "Text4", Apollo::TimeValue(4, 0));
  a.OnReceivePublicChat(hChat3, "Nickname3", "Text3", Apollo::TimeValue(3, 0));
  a.OnReceivePublicChat(hChat1, "Nickname1", "Text1", Apollo::TimeValue(1, 0));
  a.OnReceivePublicChat(hChat2, "Nickname2", "Text2", Apollo::TimeValue(2, 0));

  if (Test_Avatar_RemoveOldChats_hRemovedChat_ != hChat1) {
    s = "Did not remove oldest chat";
  }

  { DisplaySrpcMessage msg(&d, "Dummy"); msg.UnHook(MODULE_NAME, (ApCallback) Test_Avatar_RemoveOldChats_DisplaySrpcMessage, 0); }  

  return s;
}

AP_MSG_HANDLER_METHOD(WebArenaModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    AP_UNITTEST_REGISTER(Test_Avatar_RemoveOldChats);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    AP_UNITTEST_EXECUTE(Test_Avatar_RemoveOldChats);
  }
}

AP_MSG_HANDLER_METHOD(WebArenaModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int WebArenaModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextCreated, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextDestroyed, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextVisibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextSize, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LocationsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextLocationAssigned, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextLocationUnassigned, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_EnterLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_EnterLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_EnterLocationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LocationContextsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ParticipantsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LocationPublicChat, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LocationDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ContextDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ParticipantDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LeaveLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LeaveLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_LeaveLocationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ParticipantAdded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, VpView_ParticipantRemoved, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, Animation_SequenceBegin, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, Animation_Frame, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, Animation_SequenceEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebArenaModule, WebArena_CallModuleSrpc, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(WebArenaModule, this);

  return ok;
}

void WebArenaModule::Exit()
{
  AP_UNITTEST_UNHOOK(WebArenaModule, this);

  AP_MSG_REGISTRY_FINISH;
}
