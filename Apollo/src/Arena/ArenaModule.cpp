// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
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
//      apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "deleting location=" ApHandleFormat "", ApHandlePrintf(hLocation)));
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
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Context not found for handle=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandlePrintf(h), ApHandlePrintf(hContext)));
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

void ArenaModule::SetContextOfLocation(const ApHandle& hLocation, const ApHandle& hContext)
{
  if (!contextsOfLocation_.IsSet(hLocation)) {
    ContextHandleList locationContexts;
    contextsOfLocation_.Set(hLocation, locationContexts);
  }
  LocationContextHandleNode* pLocationContexts = contextsOfLocation_.Find(hLocation);
  if (pLocationContexts) {
    pLocationContexts->Value().Set(hContext, 1);
  }
}

void ArenaModule::DeleteContextOfLocation(const ApHandle& hLocation, const ApHandle& hContext)
{
  LocationContextHandleNode* pLocationContexts = contextsOfLocation_.Find(hLocation);
  if (pLocationContexts == 0) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Location not found loc=" ApHandleFormat "", ApHandlePrintf(hLocation)));
  } else {
    if (!pLocationContexts->Value().IsSet(hContext)) {
      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Context of location not found loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandlePrintf(hLocation), ApHandlePrintf(hContext)));
    } else {
      pLocationContexts->Value().Unset(hContext);
      if (pLocationContexts->Value().Count() == 0) {
        contextsOfLocation_.Unset(hLocation);
      }
    }
  }
}

static ContextHandleList ArenaModule_emptyContextHandleList;

ContextHandleList& ArenaModule::GetContextsOfLocation(const ApHandle& hLocation)
{
  LocationContextHandleNode* pLocationContexts = contextsOfLocation_.Find(hLocation);
  if (pLocationContexts) {
    return pLocationContexts->Value();
  } else {
    return ArenaModule_emptyContextHandleList;
  }
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
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Participant not found for loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandlePrintf(hParticipant)));
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
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Display already exists for ctxt=" ApHandleFormat "", ApHandlePrintf(pMsg->hContext)));
  } else {
    pDisplay = CreateDisplay(pMsg->hContext);
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDestroyed)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "No display for ctxt=" ApHandleFormat "", ApHandlePrintf(pMsg->hContext)));
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

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationsChanged) {}

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

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ContextDetailsChanged)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay) {
    pDisplay->OnContextDetailsChanged(pMsg->vlKeys);
  }
}

//----------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationDetailsChanged)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnLocationDetailsChanged(pMsg->vlKeys);
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationRequested)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnEnterRequested();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationBegin)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnEnterBegin();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_EnterLocationComplete)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnEnterComplete();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationContextsChanged) {}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantsChanged)
{
  //Display* pDisplay = GetDisplayOfHandle(pMsg->hLocation);
  //if (pDisplay) {
  //  pDisplay->OnParticipantsChanged();
  //}
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnParticipantsChanged();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationPublicChat)
{
  Apollo::TimeValue tv(pMsg->nSec, pMsg->nMicroSec);
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnReceivePublicChat(pMsg->hParticipant, pMsg->hChat, pMsg->sNickname, pMsg->sText, tv);
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LocationPublicAction)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnReceivePublicAction(pMsg->hParticipant, pMsg->sAction);
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantDetailsChanged)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnParticipantDetailsChanged(pMsg->hParticipant, pMsg->vlKeys);
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationRequested)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnLeaveRequested();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationBegin)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnLeaveBegin();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_LeaveLocationComplete)
{
  ContextHandleList& contextsOfLocation = GetContextsOfLocation(pMsg->hLocation);
  for (ContextHandleNode* pContextNode = 0; (pContextNode = contextsOfLocation.Next(pContextNode)) != 0; ) {
    Display* pDisplay = FindDisplay(pContextNode->Key());
    if (pDisplay) {
      pDisplay->OnLeaveComplete();
    }
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantAdded) {}

AP_MSG_HANDLER_METHOD(ArenaModule, VpView_ParticipantRemoved) {}

//----------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, WebView_Event_DocumentLoaded)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hView);
  if (pDisplay) {
    pDisplay->OnViewLoaded();
  }
}

AP_MSG_HANDLER_METHOD(ArenaModule, WebView_Event_Closing)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hView);
  if (pDisplay) {
    pDisplay->OnViewUnload();
  }
}

//----------------------------

AP_MSG_HANDLER_METHOD(ArenaModule, Inventory_OnDragItemMove)
{
  for (DisplayListNode* pNode = 0; pNode = displays_.Next(pNode); ) {
    Display* pDisplay = pNode->Value();
    if (pDisplay != 0) {
      pDisplay->OnDragItemMove(pMsg->hItem, pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight);
    }
  }
}

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

AP_MSG_HANDLER_METHOD(ArenaModule, WebView_ModuleCall)
{
  Display* pDisplay = GetDisplayOfHandle(pMsg->hView);
  if (pDisplay) {
    pDisplay->OnModuleCall(pMsg->srpc, pMsg->response);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSGCLASS_HANDLER_METHOD(ArenaModule, Navigator_CallDisplay, ApSRPCMessage)
{
  String sContext = pMsg->srpc.getString("hContext");
  if (sContext) {
    ApHandle hContext = Apollo::string2Handle(sContext);
    if (ApIsHandle(hContext)) {
      Display* pDisplay = FindDisplay(hContext);
      if (pDisplay) {
        pDisplay->OnNavigatorCallDisplay(pMsg->srpc, pMsg->response);
        pMsg->apStatus = ApMessage::Ok;
      }
    }
  }
}

#if defined(WIN32)
AP_MSG_HANDLER_METHOD(ArenaModule, BrowserInfo_GetContextWin32Window)
{
  Display* pDisplay = FindDisplay(pMsg->hContext);
  if (pDisplay == 0) { throw ApException(LOG_CONTEXT, "No display for context=" ApHandleFormat "", ApHandlePrintf(pMsg->hContext)); }

  Msg_WebView_GetWin32Window msg;
  msg.hView = pDisplay->GetView();
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_WebView_GetWin32Window(" ApHandleFormat ") failed: %s", ApHandlePrintf(msg.hView), _sz(msg.sComment)); }

  pMsg->hWnd = msg.hWnd;
  pMsg->apStatus = ApMessage::Ok;
}
#endif // defined(WIN32)

//----------------------------------------------------------

#if defined(AP_TEST)

ApHandle Test_Avatar_RemoveOldPublicChats_hView_;
ApHandle Test_Avatar_RemoveOldPublicChats_hRemovedChat_;

void Test_Avatar_RemoveOldPublicChats_ViewSrpcMessage(ViewSrpcMessage* pMsg)
{
  if (pMsg->hView == Test_Avatar_RemoveOldPublicChats_hView_) {
    if (pMsg->srpc.getString(Srpc::Key::Method) == "RemoveAvatarChat") {
      Test_Avatar_RemoveOldPublicChats_hRemovedChat_ = pMsg->srpc.getHandle("hChat");
    }
    pMsg->apStatus = ApMessage::Ok;
  }
}

static String Test_Avatar_RemoveOldPublicChats()
{
  String s;

  Test_Avatar_RemoveOldPublicChats_hView_ = ApHandle(12345, 12345);

  ArenaModule m;
  Display d(&m, Apollo::newHandle()); d._SetView(Test_Avatar_RemoveOldPublicChats_hView_);
  Avatar a(&m, &d, Apollo::newHandle());

  { ViewSrpcMessage msg(&d, "Dummy"); msg.Hook(MODULE_NAME, (ApCallback) Test_Avatar_RemoveOldPublicChats_ViewSrpcMessage, 0, ApCallbackPosEarly); }

  ApHandle hChat2 = Apollo::newHandle();
  ApHandle hChat1 = Apollo::newHandle();
  ApHandle hChat4 = Apollo::newHandle();
  ApHandle hChat3 = Apollo::newHandle();

  a.OnReceivePublicChat(hChat4, "Nickname4", "Text4", Apollo::TimeValue(4, 0));
  a.OnReceivePublicChat(hChat3, "Nickname3", "Text3", Apollo::TimeValue(3, 0));
  a.OnReceivePublicChat(hChat1, "Nickname1", "Text1", Apollo::TimeValue(1, 0));
  a.OnReceivePublicChat(hChat2, "Nickname2", "Text2", Apollo::TimeValue(2, 0));

  if (Test_Avatar_RemoveOldPublicChats_hRemovedChat_ != hChat1) {
    s = "Did not send remove oldest chat to view";
  }

  { ViewSrpcMessage msg(&d, "Dummy"); msg.Unhook(MODULE_NAME, (ApCallback) Test_Avatar_RemoveOldPublicChats_ViewSrpcMessage, 0); }

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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, WebView_Event_DocumentLoaded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, WebView_Event_Closing, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Inventory_OnDragItemMove, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_SequenceBegin, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_Frame, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Animation_SequenceEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, WebView_ModuleCall, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, Navigator_CallDisplay, this, ApCallbackPosNormal);
  #if defined(WIN32)
    AP_MSG_REGISTRY_ADD(MODULE_NAME, ArenaModule, BrowserInfo_GetContextWin32Window, this, ApCallbackPosNormal);
  #endif // defined(WIN32)

  AP_UNITTEST_HOOK(ArenaModule, this);

  return ok;
}

void ArenaModule::Exit()
{
  AP_UNITTEST_UNHOOK(ArenaModule, this);

  AP_MSG_REGISTRY_FINISH;
}
