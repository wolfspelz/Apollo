// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "Local.h"
#include "Meta.h"
#include "Display.h"

Meta::Meta(Display* pDisplay)
:pDisplay_(pDisplay)
,bActive_(0)
{
}

Meta::~Meta()
{
  if (bActive_) {
    Destroy();
  }
}

int Meta::Create()
{
  int ok = 1;

  if (pDisplay_) {
    hScene_ = pDisplay_->GetScene();
  }

  Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_TRAY, 0.5, 0.5, 20, 20);
  Msg_Scene_SetStrokeColor::_(hScene_, ELEMENT_TRAY, 0, 0, 0, 1);
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 1, 1, 1, 1);

  bActive_ = ok;
  return ok;
}

void Meta::Destroy()
{
  Msg_Scene_DeleteElement::_(hScene_, ELEMENT_TRAY);

  bActive_ = 0;
}

void Meta::OnEnterRequested()
{
  nState_ = StateEnterRequested;
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 0.8, 0.8, 1, 1);
}

void Meta::OnEnterBegin()
{
  nState_ = StateEnterBegin;
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 0.5, 0.5, 1, 1);
}

void Meta::OnEnterComplete()
{
  nState_ = StateEnterComplete;
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 0, 0, 1, 1);
}

void Meta::OnLeaveRequested()
{
  nState_ = StateLeaveRequested;
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 1, 0, 0, 1);
}

void Meta::OnLeaveBegin()
{
  nState_ = StateLeaveBegin;
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 1, 0.6, 0.6, 1);
}

void Meta::OnLeaveComplete()
{
  nState_ = StateLeaveComplete;
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 1, 1, 1, 1);
}
