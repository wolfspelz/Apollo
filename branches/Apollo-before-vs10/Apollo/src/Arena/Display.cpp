// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVpView.h"
#include "ColorString.h"
#include "Local.h"
#include "ArenaModule.h"
#include "Display.h"
#include "Avatar.h"

Display::Display(ArenaModule* pModule, const ApHandle& hContext)
:pModule_(pModule)
,hContext_(hContext)
,bVisible_(0)
,nLeft_(0)
,nBottom_(0)
,nWidth_(0)
,nHeight_(0)
,bSendVisibility_(0)
,bHasPosition_(0)
,bHasSize_(0)
,bDebug_(0)
,bViewLoaded_(0)
{
}

Display::~Display()
{
}

int Display::Create()
{
  int ok = 0;

  ApHandle hView = Apollo::newHandle();

  ok = Msg_WebView_Create::_(hView, 100, 100, 100, 100);
  if (!ok) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_Create(" ApHandleFormat ") failed", ApHandlePrintf(hView))); }

  if (ok) {
    Msg_WebView_SetWindowFlags msg;
    msg.hView = hView;
    msg.nFlags = Msg_WebView_SetWindowFlags::ToolWindow;
    ok = msg.Request();
    if (!ok) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_SetScriptAccessPolicy::Allow(" ApHandleFormat ") failed", ApHandlePrintf(hView))); }
  }

  if (ok) {
    ok = Msg_WebView_SetScriptAccessPolicy::Allow(hView);
    if (!ok) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_SetScriptAccessPolicy::Allow(" ApHandleFormat ") failed", ApHandlePrintf(hView))); }
  }

  if (ok) {
    ok = Msg_WebView_Load::_(hView, Apollo::getModuleConfig(MODULE_NAME, "File", "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "arena.html"));
    if (!ok) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_Load(" ApHandleFormat ") failed", ApHandlePrintf(hView))); }
  }

  if (ok) {
    hView_ = hView;
    pModule_->SetContextOfHandle(hView_, hContext_);
  }

  bDebug_ = Apollo::getModuleConfig(MODULE_NAME, "ShowDebugInterface", 0);

  return ok;
}

void Display::Destroy()
{
  if (ApIsHandle(hView_) && ApIsHandle(hContext_)) {
    pModule_->DeleteContextOfHandle(hView_, hContext_);
  }

  if (ApIsHandle(hView_)) {
    Msg_WebView_Destroy msg;
    msg.hView = hView_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_Destroy(" ApHandleFormat ") failed", ApHandlePrintf(msg.hView)));
    }
  }
}

void Display::SetVisibility(int bVisible)
{
  bVisible_ = bVisible;

  SendVisibility();

  //if (bVisible) {
  //  if (bHasPosition_ && bHasSize_) {
  //    SendVisibility();
  //  } else {
  //    bSendVisibility_ = 1;
  //  }
  //} else {
  //  SendVisibility();
  //}
}

void Display::SetPosition(int nLeft, int nBottom)
{
  nLeft_ = nLeft;
  nBottom_ = nBottom;

  bHasPosition_ = 1;

  SendPosition();

  //if (bVisible_ && bHasPosition_ && bHasSize_) {
  //  SendVisibility();
  //  bSendVisibility_ = 0;
  //}
}

void Display::SetSize(int nWidth, int nHeight)
{
  nWidth_ = nWidth;
  nHeight_ = nHeight;

  bHasSize_ = 1;

  SendPosition();

  //if (bVisible_ && bHasPosition_ && bHasSize_) {
  //  SendVisibility();
  //  bSendVisibility_ = 0;
  //}
}

void Display::SendVisibility()
{
  if (ApIsHandle(hView_)) {
    Msg_WebView_Visibility msg;
    msg.hView = hView_;
    msg.bVisible = bVisible_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_Visibility(" ApHandleFormat ") failed", ApHandlePrintf(msg.hView)));
    }
  }
}

void Display::SendPosition()
{
  int nExtendLeft = Apollo::getModuleConfig(MODULE_NAME, "Extend/Left", 0);
  int nExtendRight = Apollo::getModuleConfig(MODULE_NAME, "Extend/Right", 0);
  int nExtendTop = Apollo::getModuleConfig(MODULE_NAME, "Extend/Top", 0);
  int nExtendBottom = Apollo::getModuleConfig(MODULE_NAME, "Extend/Bottom", 0);

  if (bDebug_) {
    nExtendLeft += Apollo::getModuleConfig(MODULE_NAME, "Debug/Left", 0);
    nExtendRight += Apollo::getModuleConfig(MODULE_NAME, "Debug/Right", 700);
    nExtendTop += Apollo::getModuleConfig(MODULE_NAME, "Debug/Top", 0);
    nExtendBottom += Apollo::getModuleConfig(MODULE_NAME, "Debug/Bottom", 0);
  }

  if (ApIsHandle(hView_)) {
    Msg_WebView_Position msg;
    msg.hView = hView_;
    msg.nLeft = nLeft_ - nExtendLeft;
    msg.nTop = nBottom_ - nHeight_ - nExtendTop;
    msg.nWidth = nWidth_ + (nExtendLeft + nExtendRight);
    msg.nHeight = nHeight_ + (nExtendTop + nExtendBottom);
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_Position(" ApHandleFormat ") failed", ApHandlePrintf(msg.hView)));
    }
  }
}

//---------------------------

void Display::AttachLocation(const ApHandle& hLocation)
{
  hLocation_ = hLocation;

  if (pModule_) {
    pModule_->SetContextOfLocation(hLocation, hContext_);
  }

  if (bViewLoaded_ && ApIsHandle(hLocation_)) {
    StartDisplay();
  }
}

void Display::DetachLocation(const ApHandle& hLocation)
{
  if (pModule_) {
    pModule_->DeleteContextOfLocation(hLocation, hContext_);
  }

  RemoveAllAvatars();
  RemoveAllObjects();
  ResetLocationInfo();

  if (hLocation_ == hLocation) {
    hLocation_ = ApNoHandle;
  }
}

//---------------------------

void Display::OnEnterRequested()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandlePrintf(hContext_), ApHandlePrintf(hLocation_)));

  tvEnterRequested_ = Apollo::TimeValue::getTime();
}

void Display::OnEnterBegin()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandlePrintf(hContext_), ApHandlePrintf(hLocation_)));

  tvEnterBegin_ = Apollo::TimeValue::getTime();
}

void Display::OnEnterComplete()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandlePrintf(hContext_), ApHandlePrintf(hLocation_)));

  if (ApIsHandle(hLocation_)) {
    ApAsyncMessage<Msg_VpView_ReplayLocationPublicChat> msg;
    msg->hLocation = hLocation_;
    //msg->nMaxAge;
    //msg->nMaxLines;
    //msg->nMaxData;
    msg->PostAsync();
  }
}

void Display::OnLeaveRequested()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandlePrintf(hContext_), ApHandlePrintf(hLocation_)));

  tvLeaveRequested_ = Apollo::TimeValue::getTime();
}

void Display::OnLeaveBegin()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandlePrintf(hContext_), ApHandlePrintf(hLocation_)));

  tvLeaveBegin_ = Apollo::TimeValue::getTime();
}

void Display::OnLeaveComplete()
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandlePrintf(hContext_), ApHandlePrintf(hLocation_)));
}

//---------------------------------------------------

void Display::OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  if (bViewLoaded_) {
    AvatarListNode* pNode = avatars_.Find(hParticipant);
    if (pNode) {
      pNode->Value()->OnReceivePublicChat(hChat, sNickname, sText, tv);
    }
  }
}

void Display::OnReceivePublicAction(const ApHandle& hParticipant, const String& sAction)
{
  if (bViewLoaded_) {
    AvatarListNode* pNode = avatars_.Find(hParticipant);
    if (pNode) {
      pNode->Value()->OnReceivePublicAction(sAction);
    }
  }
}

void Display::OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys)
{
  if (bViewLoaded_) {
    AvatarListNode* pNode = avatars_.Find(hParticipant);
    if (pNode) {
      pNode->Value()->OnDetailsChanged(vlKeys);
    }
  }
}

void Display::OnAvatarAnimationBegin(const ApHandle& hParticipant, const String& sUrl)
{
  if (bViewLoaded_) {
    AvatarListNode* pNode = avatars_.Find(hParticipant);
    if (pNode) {
      pNode->Value()->OnAnimationBegin(sUrl);
    }
  }
}

void Display::OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  ApHandle hParticipant = Apollo::string2Handle(request.getString("hParticipant"));
  if (ApIsHandle(hParticipant)) {
    AvatarListNode* pNode = avatars_.Find(hParticipant);
    if (pNode) {
      pNode->Value()->OnModuleCall(request, response);
    }

  } else {
    String sMethod = request.getString(Srpc::Key::Method);

    if (0){
    } else if (sMethod == "OnShowDebug") {
      OnShowDebug(request.getInt("bShow"));

    } else if (sMethod == "OnShowChat") {
      OnShowChat(request.getInt("bShow"));

    } else if (sMethod == "SendPublicChat") {
      String sText = request.getString("sText");
      if (sText) {
        if (ApIsHandle(hLocation_)) {
          Msg_Vp_SendPublicChat msg;
          msg.hLocation = hLocation_;
          msg.sText = sText;
          if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Vp_SendPublicChat failed  loc=" ApHandleFormat "", ApHandlePrintf(hLocation_)); }
        }
      }

    } else {
      throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
    }
  }
}

void Display::OnShowChat(int bShow)
{
  if (ApIsHandle(hLocation_)) {
    Msg_ChatWindow_OpenForLocation msg;
    msg.hChat = Apollo::newHandle();
    msg.hLocation = hLocation_;
    if (!msg.Request()) {
      throw ApException(LOG_CONTEXT, "%s failed loc=" ApHandleFormat ": %s", _sz(msg.Type()), ApHandlePrintf(msg.hLocation), _sz(msg.sComment));
    }
  }
}

void Display::OnShowDebug(int bShow)
{
  bDebug_ = bShow;

  if (bDebug_){
    SendPosition();
    ViewSrpcMessage vsm(this, "ShowDebug"); vsm.srpc.set("bShow", bDebug_); vsm.Request();
  } else { // Same, but different order
    ViewSrpcMessage vsm(this, "ShowDebug"); vsm.srpc.set("bShow", bDebug_); vsm.Request();
    SendPosition();
  }

}

void Display::OnNavigatorCallDisplay(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString(Srpc::Key::Method);

  if (0){
  } else if (sMethod == "ShowChat") {
    int bShow = request.getInt("bShow");
    OnShowChat(bShow);

  } else if (sMethod == "ShowDebug") {
    int bShow = request.getInt("bShow");
    OnShowDebug(bShow);

  } else {
    throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
  }
}

//---------------------------------------------------

void Display::OnContextDetailsChanged(Apollo::ValueList& vlKeys)
{
  if (bViewLoaded_) {
    Msg_VpView_GetContextDetail msg;
    msg.hContext = hContext_;

    for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
      msg.sKey = e->getString();

      if (0) {
      } else if (e->getString() == Msg_VpView_ContextDetail_DocumentUrl) {
        if (msg.Request()) {
          ShowContextDetailDocumentUrl(msg.sValue);
        }
      } else if (e->getString() == Msg_VpView_ContextDetail_LocationUrl) {
        if (msg.Request()) {
          ShowContextDetailLocationUrl(msg.sValue);
        }
      }
    }
  }
}

void Display::ShowContextDetailDocumentUrl(const String& sValue)
{
  ViewSrpcMessage vsm(this, "SetDocumentUrl");
  vsm.srpc.set("sUrl", sValue);
  vsm.Request();
}

void Display::ShowContextDetailLocationUrl(const String& sValue)
{
  ViewSrpcMessage vsm(this, "SetLocationUrl");
  vsm.srpc.set("sUrl", sValue);
  vsm.Request();
}

//---------------------------------------------------

void Display::OnLocationDetailsChanged(Apollo::ValueList& vlKeys)
{
  if (bViewLoaded_) {
    Msg_VpView_GetLocationDetail msg;
    msg.hLocation = hLocation_;

    for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
      msg.sKey = e->getString();

      if (0) {
      } else if (e->getString() == Msg_VpView_LocationDetail_State) {
        if (msg.Request()) {
          ShowLocationDetailState(msg.sValue);
        }
      }
    }
  }
}

void Display::ShowLocationDetailState(const String& sValue)
{
  ViewSrpcMessage vsm(this, "SetLocationState");
  vsm.srpc.set("sState", sValue);
  vsm.Request();
}

//---------------------------------------------------

void Display::OnViewLoaded()
{
  bViewLoaded_ = 1;

  if (!Msg_WebView_SetNavigationPolicy::Deny(hView_)) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebView_SetNavigationPolicy::Deny(" ApHandleFormat ") failed", ApHandlePrintf(hView_))); }

  if (bViewLoaded_ && ApIsHandle(hLocation_)) {
    StartDisplay();
  }
}

void Display::OnViewUnload()
{
  bViewLoaded_ = 0;
}

void Display::StartDisplay()
{
  ViewSrpcMessage vsm(this, "Start");
  vsm.Request();

  OnShowDebug(bDebug_);

  { Msg_VpView_SubscribeContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_DocumentUrl; msg.Request(); }
  { Msg_VpView_SubscribeContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_LocationUrl; msg.Request(); }
  { Msg_VpView_SubscribeLocationDetail msg; msg.hLocation = hLocation_; msg.sKey = Msg_VpView_LocationDetail_State; msg.Request(); }

  { Msg_VpView_GetContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_DocumentUrl; if (msg.Request()) { ShowContextDetailDocumentUrl(msg.sValue); } }
  { Msg_VpView_GetContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_LocationUrl; if (msg.Request()) { ShowContextDetailLocationUrl(msg.sValue); } }
  { Msg_VpView_GetLocationDetail msg; msg.hLocation = hLocation_; msg.sKey = Msg_VpView_LocationDetail_State; if (msg.Request()) { ShowLocationDetailState(msg.sValue); } }

  {
    Msg_VpView_GetParticipants msg;
    msg.hLocation = hLocation_;
    if (msg.Request()) {
      ProcessAvatarList(msg.vlParticipants, msg.hSelf);
    }
  }
}

//---------------------------------------------------

void Display::OnParticipantsChanged()
{
  if (bViewLoaded_) {
    Msg_VpView_GetParticipants msg;
    msg.hLocation = hLocation_;
    if (msg.Request()) {
      ProcessAvatarList(msg.vlParticipants, msg.hSelf);
    }
  }
}

void Display::ProcessAvatarList(Apollo::ValueList& vlParticipants, ApHandle& hSelf)
{
  hSelf_ = hSelf;

  ParticipantFlags addedParticipants;
  ParticipantFlags removedParticipants;

  InitRemovedParticipants(removedParticipants);
  InitAddedParticipants(addedParticipants);
  EvaluateNewAvatarList(vlParticipants, addedParticipants, removedParticipants);
  ProcessRemovedParticipants(removedParticipants);
  ProcessAddedParticipants(addedParticipants);
}

void Display::InitRemovedParticipants(ParticipantFlags& removedParticipants)
{
  while (removedParticipants.Count() >0) {
    ApHandleTreeNode<int>* pNode = removedParticipants.Next(0);
    if (pNode) {
      removedParticipants.Unset(pNode->Key());
    }
  }

  AvatarListIterator iter(avatars_);
  for (AvatarListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hAvatar = pNode->Key();
    removedParticipants.Set(hAvatar, 1);
  }
}

void Display::InitAddedParticipants(ParticipantFlags& addedParticipants)
{
  while (addedParticipants.Count() >0) {
    ApHandleTreeNode<int>* pNode = addedParticipants.Next(0);
    if (pNode) {
      addedParticipants.Unset(pNode->Key());
    }
  }
}

void Display::EvaluateNewAvatarList(Apollo::ValueList& vlParticipants, ParticipantFlags& addedParticipants, ParticipantFlags& removedParticipants)
{
  for (Apollo::ValueElem* e = 0; e = vlParticipants.nextElem(e); ) {
    AvatarListNode* pNode = avatars_.Find(e->getHandle());
    if (pNode) {
      RemoveFromRemovedParticipants(e->getHandle(), removedParticipants);
    } else {
      AddToAddedParticipants(e->getHandle(), addedParticipants);
    }
  }
}

void Display::RemoveFromRemovedParticipants(const ApHandle& h, ParticipantFlags& removedParticipants)
{
  removedParticipants.Unset(h);
}

void Display::AddToAddedParticipants(const ApHandle& h, ParticipantFlags& addedParticipants)
{
  addedParticipants.Set(h, 1);
}

void Display::ProcessAddedParticipants(ParticipantFlags& addedParticipants)
{
  ApHandleTreeIterator<int> iter(addedParticipants);
  for (ApHandleTreeNode<int>* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hParticipant = pNode->Key();
    Avatar* pAvatar = new Avatar(pModule_, this, hParticipant);
    if (pAvatar) {
      avatars_.Set(hParticipant, pAvatar);
      pAvatar->Create(hParticipant == hSelf_);
    }
  }
}

void Display::ProcessRemovedParticipants(ParticipantFlags& removedParticipants)
{
  ApHandleTreeIterator<int> iter(removedParticipants);
  for (ApHandleTreeNode<int>* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hParticipant = pNode->Key();
    Avatar* pAvatar = 0;
    avatars_.Get(hParticipant, pAvatar);
    if (pAvatar) {
      pAvatar->Destroy();
      avatars_.Unset(hParticipant);
      delete pAvatar;
      pAvatar = 0;
    }
  }
}

//---------------------------------------------------

void Display::RemoveAllAvatars()
{
  ParticipantFlags removedParticipants;

  InitRemovedParticipants(removedParticipants);
  ProcessRemovedParticipants(removedParticipants);
}

void Display::RemoveAllObjects()
{
}

void Display::ResetLocationInfo()
{
}

//---------------------------------------------------

ViewSrpcMessage::ViewSrpcMessage(Display* pDisplay, const String& sMethod)
{
  srpc.set(Srpc::Key::Method, sMethod);
  hView = pDisplay->GetView();
  sFunction = Apollo::getModuleConfig(MODULE_NAME, "CallScriptSrpcFunctionName", "receiveSrpcMessageAsString");
}

int ViewSrpcMessage::Request()
{
  if (ApIsHandle(hView)) {
    return base::Request();
  } else {
    return 0;
  }
}
