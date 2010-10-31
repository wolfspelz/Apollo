// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "Local.h"
#include "Context.h"

Context::Context(const ApHandle& hContext)
:hAp_(hContext)
,bVisible_(0)
,nX_(0)
,nY_(0)
,nW_(0)
,nH_(0)
{
}

Context::~Context()
{
}

int Context::Create()
{
  int ok = 0;

  ApHandle hScene = Apollo::newHandle();

  Msg_Scene_Create msg;
  msg.hScene = hScene;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "Context::Create", "Msg_Scene_Create(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  } else {
    hScene_ = hScene;

    Msg_Scene_SetAutoDraw msgSSAD;
    msgSSAD.hScene = hScene_;
    msgSSAD.nMilliSec = 100;
    msgSSAD.Request();
  }

  return ok;
}

void Context::Destroy()
{
  Msg_Scene_Destroy msg;
  msg.hScene = hScene_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Context::Destroy", "Msg_Scene_Destroy(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }
}

void Context::SetVisibility(int bVisible)
{
  bVisible_ = bVisible;

  Msg_Scene_Visibility msg;
  msg.hScene = hScene_;
  msg.bVisible = bVisible_;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, "Context::SetPosition", "Msg_Scene_Visibility(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }
}

void Context::SetPosition(int nX, int nY)
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
    apLog_Error((LOG_CHANNEL, "Context::SetPosition", "Msg_Scene_Position(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }
}

void Context::SetSize(int nW, int nH)
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
    apLog_Error((LOG_CHANNEL, "Context::SetSize", "Msg_Scene_Position(" ApHandleFormat ") failed", ApHandleType(msg.hScene)));
  }

  if (Apollo::getModuleConfig(MODULE_NAME, "DebugFrame/Context", 0)) {
    int bExists = 0;
    if (Msg_Scene_ElementExists::_(hScene_, "_a_frame", bExists) && bExists) {
      Msg_Scene_DeleteElement::_(hScene_, "_a_frame");
    }
    Msg_Scene_CreateRectangle::_(hScene_, "_a_frame", 0.5, 0.5, nW_ - 0.5, nH_ - 0.5);
    //Msg_Scene_SetFillColor::_(hScene_, "_a_frame", 1, 1, 1, 0.5);
    Msg_Scene_SetStrokeColor::_(hScene_, "_a_frame", 0, 0, 1, 1);
    Msg_Scene_SetStrokeWidth::_(hScene_, "_a_frame", 1);
  }
}

