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
{
}

Display::~Display()
{
}

int Display::Create()
{
  int ok = 0;

  ApHandle hView = Apollo::newHandle();

  ok = Msg_WebView_Create::_(hView);
  if (!ok) { apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_Create(" ApHandleFormat ") failed", ApHandleType(hView))); }

  if (ok) {
    ok = Msg_WebView_SetScriptAccessPolicy::Allow(hView);
    if (!ok) { apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_SetScriptAccessPolicy::Allow(" ApHandleFormat ") failed", ApHandleType(hView))); }
  }

  if (ok) {
    ok = Msg_WebView_Load::_(hView, Apollo::getModuleConfig(MODULE_NAME, "DisplayHtml", "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "arena.html"));
    if (!ok) { apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_Load(" ApHandleFormat ") failed", ApHandleType(hView))); }
  }

  if (ok) {
    ok = Msg_WebView_SetNavigationPolicy::Deny(hView);
    if (!ok) { apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_SetNavigationPolicy::Deny(" ApHandleFormat ") failed", ApHandleType(hView))); }
  }

  if (ok) {
    hView_ = hView;
    pModule_->SetContextOfHandle(hView_, hContext_);

    { Msg_VpView_SubscribeContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_DocumentUrl; msg.Request(); }
    { Msg_VpView_SubscribeContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_LocationUrl; msg.Request(); }

    //{ Msg_VpView_GetContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_DocumentUrl; if (msg.Request()) { Meta* p = GetMeta(); if (p) { p->OnDocumentUrl(msg.sValue); }} }
    //{ Msg_VpView_GetContextDetail msg; msg.hContext = hContext_; msg.sKey = Msg_VpView_ContextDetail_LocationUrl; if (msg.Request()) { Meta* p = GetMeta(); if (p) { p->OnLocationUrl(msg.sValue); }} }
  }

  return ok;
}

void Display::Destroy()
{
  pModule_->DeleteContextOfHandle(hView_, hContext_);

  Msg_WebView_Destroy msg;
  msg.hView = hView_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::Destroy", "Msg_WebView_Destroy(" ApHandleFormat ") failed", ApHandleType(msg.hView)));
  }
}

void Display::SetVisibility(int bVisible)
{
  bVisible_ = bVisible;

  Msg_WebView_Visibility msg;
  msg.hView = hView_;
  msg.bVisible = bVisible_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SetPosition", "Msg_WebView_Visibility(" ApHandleFormat ") failed", ApHandleType(msg.hView)));
  }
}

void Display::SetPosition(int nLeft, int nBottom)
{
  nLeft_ = nLeft;
  nBottom_ = nBottom;

  SendPosition();
}

void Display::SetSize(int nWidth, int nHeight)
{
  nWidth_ = nWidth;
  nHeight_ = nHeight;

  SendPosition();
}

void Display::SendPosition()
{
  Msg_WebView_Position msg;
  msg.hView = hView_;
  msg.nLeft = nLeft_;
  msg.nTop = nBottom_ - nHeight_;
  msg.nWidth = nWidth_;
  msg.nHeight = nHeight_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SendPosition", "Msg_WebView_Position(" ApHandleFormat ") failed", ApHandleType(msg.hView)));
  }
}

//---------------------------

void Display::AttachLocation(const ApHandle& hLocation)
{
  hLocation_ = hLocation;

  //if (pLocationInfo_) {
  //  pLocationInfo_->SetLocation(hLocation);
  //}

  if (pModule_) {
    pModule_->SetContextOfHandle(hLocation, hContext_);
  }
}

void Display::DetachLocation(const ApHandle& hLocation)
{
  if (pModule_) {
    pModule_->DeleteContextOfHandle(hLocation, hContext_);
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
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterRequested", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvEnterRequested_ = Apollo::TimeValue::getTime();

  DisplaySrpcMessage dsm(this, "SetLocationState");
  dsm.srpc.setString("sState", "EnterRequested");
  dsm.Request();
}

void Display::OnEnterBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterBegin", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvEnterBegin_ = Apollo::TimeValue::getTime();

  DisplaySrpcMessage dsm(this, "SetLocationState");
  dsm.srpc.setString("sState", "EnterBegin");
  dsm.Request();
}

void Display::OnEnterComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterComplete", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  {
    ApAsyncMessage<Msg_VpView_ReplayLocationPublicChat> msg;
    msg->hLocation = hLocation_;
    //msg->nMaxAge;
    //msg->nMaxLines;
    //msg->nMaxData;
    msg->PostAsync();
  }

  DisplaySrpcMessage dsm(this, "SetLocationState");
  dsm.srpc.setString("sState", "EnterComplete");
  dsm.Request();
}

void Display::OnLeaveRequested()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveRequested", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvLeaveRequested_ = Apollo::TimeValue::getTime();

  DisplaySrpcMessage msg(this, "SetLocationState");
  msg.srpc.setString("sState", "LeaveRequested");
  msg.Request();
}

void Display::OnLeaveBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveBegin", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvLeaveBegin_ = Apollo::TimeValue::getTime();

  DisplaySrpcMessage dsm(this, "SetLocationState");
  dsm.srpc.setString("sState", "LeaveBegin");
  dsm.Request();
}

void Display::OnLeaveComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveComplete", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  DisplaySrpcMessage dsm(this, "SetLocationState");
  dsm.srpc.setString("sState", "LeaveComplete");
  dsm.Request();
}

//---------------------------------------------------

void Display::OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  AvatarListNode* pNode = avatars_.Find(hParticipant);
  if (pNode) {
    Avatar* pAvatar = pNode->Value();
    if (pAvatar) {
      pAvatar->OnReceivePublicChat(hChat, sNickname, sText, tv);
    }
  }
}

void Display::OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys)
{
  AvatarListNode* pNode = avatars_.Find(hParticipant);
  if (pNode) {
    Avatar* pAvatar = pNode->Value();
    if (pAvatar) {
      pAvatar->OnDetailsChanged(vlKeys);
    }
  }
}

void Display::OnAvatarAnimationBegin(const ApHandle& hParticipant, const String& sUrl)
{
  AvatarListNode* pNode = avatars_.Find(hParticipant);
  if (pNode) {
    Avatar* pAvatar = pNode->Value();
    if (pAvatar) {
      pAvatar->OnAnimationBegin(sUrl);
    }
  }
}

void Display::OnCallModuleSrpc(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString("Method");

  if (0){
  } else if (sMethod == "SendPublicChat") {
    String sText = request.getString("sText");
    if (sText){
      Msg_Vp_SendPublicChat msg;
      msg.hLocation = hLocation_;
      msg.sText = sText;
      if (!msg.Request()) { throw ApException("Msg_Vp_SendPublicChat failed  loc=" ApHandleFormat "", ApHandleType(hLocation_)); }
    }

  } else {
    throw ApException("Unknown Method=%s", StringType(sMethod));
  }
}

//---------------------------------------------------

void Display::OnParticipantsChanged()
{
  Msg_VpView_GetParticipants msg;
  msg.hLocation = hLocation_;
  if (msg.Request()) {
    ProcessAvatarList(msg.vlParticipants, msg.hSelf);
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

void Display::OnContextDetailsChanged(Apollo::ValueList& vlKeys)
{
  Msg_VpView_GetContextDetail msg;
  msg.hContext = hContext_;

  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    msg.sKey = e->getString();

    if (0) {
    } else if (e->getString() == Msg_VpView_ContextDetail_DocumentUrl) {
      if (msg.Request()) {
        DisplaySrpcMessage dsm(this, "SetDocumentUrl");
        dsm.srpc.setString("sUrl", msg.sValue);
        dsm.Request();
      }
    } else if (e->getString() == Msg_VpView_ContextDetail_LocationUrl) {
      if (msg.Request()) {
        DisplaySrpcMessage dsm(this, "SetLocationUrl");
        dsm.srpc.setString("sUrl", msg.sValue);
        dsm.Request();
      }
    }
  }
}

//---------------------------------------------------

DisplaySrpcMessage::DisplaySrpcMessage(Display* pDisplay, const String& sMethod)
{
  srpc.setString("Method", sMethod);
  hView = pDisplay->GetView();
  sFunction = Apollo::getModuleConfig(MODULE_NAME, "CallScriptSrpcFunctionName", "receiveSrpcMessageAsString");
}

