// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "ArenaModule.h"
#include "ArenaModuleTester.h"
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
    pDisplay->SetPosition(pMsg->nLeft, pMsg->nBottom);
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

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationPublicAction)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  if (pDisplay) {
    pDisplay->OnReceivePublicAction(pMsg->hParticipant, pMsg->sAction);
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

AP_MSG_HANDLER_METHOD(ArenaModule, Animation_SequenceBegin)
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

//AP_MSG_HANDLER_METHOD(ArenaModule, Animation_Frame)
//{
//  Display* pDisplay = GetDisplayOfHandle(pMsg->hItem);
//  if (pDisplay) {
//    ApHandle hParticipant = GetParticipantOfAnimation(pMsg->hItem);
//    pDisplay->OnAvatarAnimationFrame(hParticipant, pMsg->iFrame);
//  }
//}
//
//AP_MSG_HANDLER_METHOD(ArenaModule, Animation_SequenceEnd)
//{
//  Display* pDisplay = GetDisplayOfHandle(pMsg->hItem);
//  if (pDisplay) {
//    ApHandle hParticipant = GetParticipantOfAnimation(pMsg->hItem);
//    pDisplay->OnAvatarAnimationEnd(hParticipant);
//  }
//}

//----------------------------

AP_SRPC_HANDLER_METHOD(ArenaModule, Arena_CallModuleSrpc, ApSRPCMessage)
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

ApHandle Test_Avatar_RemoveOldPublicChats_hRemovedChat_;

void Test_Avatar_RemoveOldPublicChats_DisplaySrpcMessage(DisplaySrpcMessage* pMsg)
{
  if (pMsg->srpc.getString("Method") == "RemoveAvatarChat") {
    Test_Avatar_RemoveOldPublicChats_hRemovedChat_ = Apollo::string2Handle(pMsg->srpc.getString("hChat"));
  }
}

static String Test_Avatar_RemoveOldPublicChats()
{
  String s;

  ArenaModule m;
  Display d(&m, Apollo::newHandle());
  Avatar a(&m, &d, Apollo::newHandle());

  { DisplaySrpcMessage msg(&d, "Dummy"); msg.Hook(MODULE_NAME, (ApCallback) Test_Avatar_RemoveOldPublicChats_DisplaySrpcMessage, 0, ApCallbackPosEarly); }

  ApHandle hChat2 = Apollo::newHandle();
  ApHandle hChat1 = Apollo::newHandle();
  ApHandle hChat4 = Apollo::newHandle();
  ApHandle hChat3 = Apollo::newHandle();

  a.OnReceivePublicChat(hChat4, "Nickname4", "Text4", Apollo::TimeValue(4, 0));
  a.OnReceivePublicChat(hChat3, "Nickname3", "Text3", Apollo::TimeValue(3, 0));
  a.OnReceivePublicChat(hChat1, "Nickname1", "Text1", Apollo::TimeValue(1, 0));
  a.OnReceivePublicChat(hChat2, "Nickname2", "Text2", Apollo::TimeValue(2, 0));

  if (Test_Avatar_RemoveOldPublicChats_hRemovedChat_ != hChat1) {
    s = "Did not remove oldest chat";
  }

  { DisplaySrpcMessage msg(&d, "Dummy"); msg.Unhook(MODULE_NAME, (ApCallback) Test_Avatar_RemoveOldPublicChats_DisplaySrpcMessage, 0); }

  return s;
}

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    ArenaModuleTester::Begin();
    AP_UNITTEST_REGISTER(Test_Avatar_RemoveOldPublicChats);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    ArenaModuleTester::Execute();
    AP_UNITTEST_EXECUTE(Test_Avatar_RemoveOldPublicChats);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Arena", 0)) {
    ArenaModuleTester::End();
  }
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LocationPublicAction, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LocationDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ContextDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LeaveLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LeaveLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_LeaveLocationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantAdded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, VpView_ParticipantRemoved, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_SequenceBegin, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_Frame, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_SequenceEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Arena_CallModuleSrpc, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(ArenaModule, this);

  return ok;
}

void ArenaModule::Exit()
{
  AP_UNITTEST_UNHOOK(ArenaModule, this);

  AP_MSG_REGISTRY_FINISH;
}
