// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "Local.h"
#include "Hud.h"
#include "Display.h"

Hud::Hud(Display* pDisplay)
:Layer(pDisplay)
{
}

Hud::~Hud()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMHUDSENSOR)) {
    Msg_Scene_DeleteElement::_(hScene_, ELEMENT_BOTTOMHUDSENSOR);
  }
}

void Hud::OnSetSize(int nW, int nH)
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMHUDSENSOR)) {
    Msg_Scene_DeleteElement::_(hScene_, ELEMENT_BOTTOMHUDSENSOR);
  }
  Msg_Scene_CreateMouseSensor::_(hScene_, ELEMENT_BOTTOMHUDSENSOR, 0.5, 0.5, nW, Apollo::getModuleConfig(MODULE_NAME, "Hud/BottomSensor/Height", 20));
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_BOTTOMHUDSENSOR, 1, 1, 1, 0.01);
  pDisplay_->SetDebugFrameColor(ELEMENT_BOTTOMHUDSENSOR);

  if (Apollo::getModuleConfig(MODULE_NAME, "DebugFrame/Display", 0)) {
    if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_FRAME)) {
      Msg_Scene_DeleteElement::_(hScene_, ELEMENT_FRAME);
    }
    Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_FRAME, 0.5, 0.5, nW - 0.5, nH - 0.5);
    //Msg_Scene_SetFillColor::_(hScene_, ELEMENT_FRAME, 1, 1, 1, 0.5);
    pDisplay_->SetDebugFrameColor(ELEMENT_FRAME);
  }
}

int Hud::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  if (pMsg->sPath == ELEMENT_BOTTOMHUDSENSOR) {
    apLog_Debug((LOG_CHANNEL, "Hud::ProcessMouseEvent", "event=%d button=%d x=%f y=%f", pMsg->nEvent, pMsg->nButton, pMsg->fX, pMsg->fY));
    return 1;
  }
  return 0;
}

