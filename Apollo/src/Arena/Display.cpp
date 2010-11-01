// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "Local.h"
#include "Display.h"

Display::Display(const ApHandle& hContext)
:hContext_(hContext)
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

void Display::EnterRequested()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 0.8, 0.8, 1, 1);
  Msg_Scene_Draw::_(hScene_);
}

void Display::EnterBegin()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 0.5, 0.5, 1, 1);
  Msg_Scene_Draw::_(hScene_);
}

void Display::EnterComplete()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 0, 0, 1, 1);
  Msg_Scene_Draw::_(hScene_);
}

void Display::LeaveRequested()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 0, 0, 1);
  Msg_Scene_Draw::_(hScene_);
}

void Display::LeaveBegin()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 0.6, 0.6, 1);
  Msg_Scene_Draw::_(hScene_);
}

void Display::LeaveComplete()
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_PROGRESS, 1, 1, 1, 1);
  Msg_Scene_Draw::_(hScene_);
}
