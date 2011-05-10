// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVpView.h"
#include "ColorString.h"
#include "Local.h"
#include "WebArenaModule.h"
#include "Display.h"
#include "Avatar.h"

Display::Display(WebArenaModule* pModule, const ApHandle& hContext)
:pModule_(pModule)
,hContext_(hContext)
,bVisible_(0)
,nX_(0)
,nY_(0)
,nW_(0)
,nH_(0)
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
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_Create(" ApHandleFormat ") failed", ApHandleType(hView)));
  }

  if (ok) {
    ok = Msg_WebView_SetScriptAccess::Allow(hView);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_SetScriptAccess::Allow(" ApHandleFormat ") failed", ApHandleType(hView)));
    }
  }

  if (ok) {
    ok = Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "index.html");
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_WebView_Load(" ApHandleFormat ") failed", ApHandleType(hView)));
    }
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

void Display::SetPosition(int nX, int nY)
{
  nX_ = nX;
  nY_ = nY;

  Msg_WebView_Position msg;
  msg.hView = hView_;
  msg.nX = nX_;
  msg.nY = nY_;
  msg.nW = nW_;
  msg.nH = nH_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SetPosition", "Msg_WebView_Position(" ApHandleFormat ") failed", ApHandleType(msg.hView)));
  }
}

void Display::SetSize(int nW, int nH)
{
  nW_ = nW;
  nH_ = nH;

  Msg_WebView_Position msg;
  msg.hView = hView_;
  msg.nX = nX_;
  msg.nY = nY_;
  msg.nW = nW_;
  msg.nH = nH_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SetSize", "Msg_WebView_Position(" ApHandleFormat ") failed", ApHandleType(msg.hView)));
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
}

void Display::OnEnterBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterBegin", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvEnterBegin_ = Apollo::TimeValue::getTime();
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
}

void Display::OnLeaveRequested()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveRequested", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvLeaveRequested_ = Apollo::TimeValue::getTime();
}

void Display::OnLeaveBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveBegin", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  tvLeaveBegin_ = Apollo::TimeValue::getTime();
}

void Display::OnLeaveComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveComplete", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));
}

//---------------------------------------------------

void Display::OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  AvatarListNode* pNode = avatars_.Find(hParticipant);
  if (pNode) {
    pNode->Value()->ReceivePublicChat(hChat, sNickname, sText, tv);
  }
}

void Display::OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys)
{
  AvatarListNode* pNode = avatars_.Find(hParticipant);
  if (pNode) {
    pNode->Value()->DetailsChanged(vlKeys);
  }
}

//---------------------------------------------------

void Display::OnParticipantsChanged()
{
  Msg_VpView_GetParticipants msg;
  msg.hLocation = hLocation_;
  if (msg.Request()) {
    ProcessAvatarList(msg.vlParticipants);
  }
}

void Display::ProcessAvatarList(Apollo::ValueList& vlParticipants)
{
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
      pAvatar->Show();
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
      pAvatar->Hide();
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
        //{ Meta* p = GetMeta(); if (p) { p->OnDocumentUrl(msg.sValue); }}
      }
    } else if (e->getString() == Msg_VpView_ContextDetail_LocationUrl) {
      if (msg.Request()) {
        //{ Meta* p = GetMeta(); if (p) { p->OnLocationUrl(msg.sValue); }}
      }
    }
  }
}

//---------------------------------------------------

String Display::Call(const String& sMethod, List& lArgs)
{
  Msg_WebView_CallScriptFunction msg;
  msg.hView = hView_;
  msg.sMethod = sMethod;
  msg.lArgs = lArgs;
  if (msg.Request()) {
    return msg.sResult;
  }
  return String();
}

String Display::Call(const String& sMethod, const String& sArg1)
{
  List l;
  l.AddLast(sArg1);
  return Call(sMethod, l);
}

String Display::Call(const String& sMethod, const String& sArg1, const String& sArg2)
{
  List l;
  l.AddLast(sArg1);
  l.AddLast(sArg2);
  return Call(sMethod, l);
}

String Display::Call(const String& sMethod, const String& sArg1, const String& sArg2, const String& sArg3)
{
  List l;
  l.AddLast(sArg1);
  l.AddLast(sArg2);
  l.AddLast(sArg3);
  return Call(sMethod, l);
}

