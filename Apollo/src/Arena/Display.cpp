// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "MsgVpView.h"
#include "Local.h"
#include "ArenaModule.h"
#include "Display.h"
#include "Avatar.h"

Display::Display(ArenaModule* pModule, const ApHandle& hContext)
:pModule_(pModule)
,hContext_(hContext)
,bVisible_(0)
,nX_(0)
,nY_(0)
,nW_(0)
,nH_(0)
,nState_(NoState)
{
}

Display::~Display()
{
}

int Display::Create()
{
  int ok = 0;

  ApHandle hScene = Apollo::newHandle();

  Msg_Scene_Create msg;
  msg.hScene = hScene;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "Display::Create", "Msg_Scene_Create(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  } else {
    hScene_ = hScene;

    Msg_Scene_SetAutoDraw msgSSAD;
    msgSSAD.hScene = hScene_;
    msgSSAD.nMilliSec = 100;
    msgSSAD.Request();

    Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_PROGRESS, 0.5, 0.5, 20, 20);
    Msg_Scene_SetStrokeColor::_(hScene_, ELEMENT_PROGRESS, 0, 0, 0, 1);
    Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 1, 1, 1);
  }

  return ok;
}

void Display::Destroy()
{
  Msg_Scene_Destroy msg;
  msg.hScene = hScene_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::Destroy", "Msg_Scene_Destroy(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }
}

void Display::SetVisibility(int bVisible)
{
  bVisible_ = bVisible;

  Msg_Scene_Visibility msg;
  msg.hScene = hScene_;
  msg.bVisible = bVisible_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SetPosition", "Msg_Scene_Visibility(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }
}

void Display::SetPosition(int nX, int nY)
{
  nX_ = nX;
  nY_ = nY;
  
  Msg_Scene_Position msg;
  msg.hScene = hScene_;
  msg.nX = nX_;
  msg.nY = nY_;
  msg.nW = nW_;
  msg.nH = nH_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SetPosition", "Msg_Scene_Position(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }
}

void Display::SetSize(int nW, int nH)
{
  nW_ = nW;
  nH_ = nH;

  Msg_Scene_Position msg;
  msg.hScene = hScene_;
  msg.nX = nX_;
  msg.nY = nY_;
  msg.nW = nW_;
  msg.nH = nH_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Display::SetSize", "Msg_Scene_Position(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }

  if (Apollo::getModuleConfig(MODULE_NAME, "DebugFrame/Display", 0)) {
    int bExists = 0;
    if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_FRAME, bExists) && bExists) {
      Msg_Scene_DeleteElement::_(hScene_, ELEMENT_FRAME);
    }
    Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_FRAME, 0.5, 0.5, nW_ - 0.5, nH_ - 0.5);
    //Msg_Scene_SetFillColor::_(hScene_, ELEMENT_FRAME, 1, 1, 1, 0.5);
    Msg_Scene_SetStrokeColor::_(hScene_, ELEMENT_FRAME, 0, 0, 1, 1);
    Msg_Scene_SetStrokeWidth::_(hScene_, ELEMENT_FRAME, 1);
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
    pModule_->SetContextOfLocation(hLocation, hContext_);
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
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterRequested", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  nState_ = StateEnterRequested;
  tvEnterRequested_ = Apollo::TimeValue::getTime();

  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 0.8, 0.8, 1, 1);
  //Msg_Scene_Draw::_(hScene_);
}

void Display::OnEnterBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterBegin", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  nState_ = StateEnterBegin;
  tvEnterBegin_ = Apollo::TimeValue::getTime();

  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 0.5, 0.5, 1, 1);
  //Msg_Scene_Draw::_(hScene_);
}

void Display::OnEnterComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnEnterComplete", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  nState_ = StateEnterComplete;

  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 0, 0, 1, 1);
  //Msg_Scene_Draw::_(hScene_);

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

  nState_ = StateLeaveRequested;
  tvLeaveRequested_ = Apollo::TimeValue::getTime();

  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 0, 0, 1);
  //Msg_Scene_Draw::_(hScene_);
}

void Display::OnLeaveBegin()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveBegin", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  nState_ = StateLeaveBegin;
  tvLeaveBegin_ = Apollo::TimeValue::getTime();

  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 0.6, 0.6, 1);
  //Msg_Scene_Draw::_(hScene_);
}

void Display::OnLeaveComplete()
{
  apLog_Verbose((LOG_CHANNEL, "Display::OnLeaveComplete", "ctxt=" ApHandleFormat " loc=" ApHandleFormat "", ApHandleType(hContext_), ApHandleType(hLocation_)));

  nState_ = StateLeaveComplete;

  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 1, 1, 1);
  //Msg_Scene_Draw::_(hScene_);
}

//---------------------------------------------------

int Display::CheckLeaveRequestedAndAbandoned()
{
  int bResult = 0;

  if (nState_ == StateLeaveRequested) {
    Apollo::TimeValue tvSinceLeaveRequested = Apollo::TimeValue::getTime() - tvLeaveRequested_;
    if (tvSinceLeaveRequested.Sec() > Apollo::getModuleConfig(MODULE_NAME, "CheckLeaveRequestedAndAbandonedAfterSec", 180)) {
      bResult = 1;
    }
  }

  return bResult;
}

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

void Display::OnAvatarAnimationFrame(const ApHandle& hParticipant, const Apollo::Image& image)
{
  AvatarListNode* pNode = avatars_.Find(hParticipant);
  if (pNode) {
    pNode->Value()->AnimationFrame(image);
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
  InitRemovedParticipants();
  InitAddedParticipants();
  EvaluateNewAvatarList(vlParticipants);
  ProcessRemovedParticipants();
  ProcessAddedParticipants();
}

void Display::InitRemovedParticipants()
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

void Display::InitAddedParticipants()
{
  while (addedParticipants_.Count() >0) {
    ApHandleTreeNode<int>* pNode = addedParticipants_.Next(0);
    if (pNode) {
      addedParticipants_.Unset(pNode->Key());
    }
  }
}

void Display::EvaluateNewAvatarList(Apollo::ValueList& vlParticipants)
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

void Display::RemoveFromRemovedParticipants(const ApHandle& h)
{
  removedParticipants_.Unset(h);
}

void Display::AddToAddedParticipants(const ApHandle& h)
{
  addedParticipants_.Set(h, 1);
}

void Display::ProcessAddedParticipants()
{
  ApHandleTreeIterator<int> iter(addedParticipants_);
  for (ApHandleTreeNode<int>* pNode = 0; (pNode = iter.Next()) != 0; ) {
    ApHandle hParticipant = pNode->Key();
    Avatar* pAvatar = new Avatar(pModule_, this, hParticipant);
    if (pAvatar) {
      avatars_.Set(hParticipant, pAvatar);
      pAvatar->Show();
    }
  }
}

void Display::ProcessRemovedParticipants()
{
  ApHandleTreeIterator<int> iter(removedParticipants_);
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
  InitRemovedParticipants();
  ProcessRemovedParticipants();
}

void Display::RemoveAllObjects()
{
}

void Display::ResetLocationInfo()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 1, 1, 1);
  //Msg_Scene_Draw::_(hScene_);
}

