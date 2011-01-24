// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "MsgScWidget.h"
#include "Local.h"
#include "Hud.h"
#include "Display.h"
#include "ArenaModule.h"

Hud::Hud(Display* pDisplay)
:Layer(pDisplay)
,nW_(0)
,nH_(0)
,fMouseDownX_(0.0)
,fMouseDownY_(0.0)
,nMouseDownW_(0)
,nMouseDownH_(0)
,bMouseDown_(0)
,bBottomBarEngaged_(0)
{
  CreateBottomSensor();
  CreateSizeSensor();
}

Hud::~Hud()
{
  DestroyBottomSensor();
  DestroySizeSensor();
//  DestroyBottomBar();
}

void Hud::OnSetSize(int nW, int nH)
{
  nW_ = nW;
  nH_ = nH;

  SizeBottomSensor();
  SizeSizeSensor();
  SizeBottomBar();

  if (Apollo::getModuleConfig(MODULE_NAME, "DebugFrame/Display", 0)) {
    if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_FRAME)) {
      Msg_Scene_DeleteElement::_(hScene_, ELEMENT_FRAME);
    }
    Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_FRAME, 0, 0, nW, nH);
    //Msg_Scene_SetFillColor::_(hScene_, ELEMENT_FRAME, 1, 1, 1, 0.5);
    GetDisplay()->SetDebugFrameColor(ELEMENT_FRAME);
  }
}

int Hud::OnTimerEvent(Msg_Timer_Event* pMsg)
{
  if (pMsg->hTimer == hBottomBarTimer_) {
    HideBottomBar();
    hBottomBarTimer_ = ApNoHandle;
    return 1;
  }
  return 0;
}

int Hud::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  if (0) {
  } else if (pMsg->sPath == ELEMENT_BOTTOMSENSOR) {
    //apLog_Debug((LOG_CHANNEL, "Hud::ProcessMouseEvent", "event=%d button=%d x=%f y=%f", pMsg->nEvent, pMsg->nButton, pMsg->fX, pMsg->fY));

    if (!BottomBarIsVisible()) {
      ShowBottomBar();
    } else {
      if (InsideBottomBar(pMsg->fX, pMsg->fY)) {
        if (BottomBarTimerActive()) {
          if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseDown) {
            CancelBottomBarTimer();
          } else {
            RestartBottomBarTimer();
          }
        }
      } else {
        if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseDown) {
          CancelBottomBarTimer();
          HideBottomBar();
        }
      }
    }

    return 1;

  } else if (pMsg->sPath == ELEMENT_SIZESENSOR) {
    //apLog_Debug((LOG_CHANNEL, "Hud::ProcessMouseEvent", "event=%d button=%d x=%f y=%f", pMsg->nEvent, pMsg->nButton, pMsg->fX, pMsg->fY));

    if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseDown) {
      StartMouseSize(pMsg->fX, pMsg->fY);
    }

    if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseUp) {
      StopMouseSize();
    }

    if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseMove) {
      if (InMouseSize()) {
        DoMouseSize(pMsg->fX, pMsg->fY);
      }
    }

    return 1;

  }
  return 0;
}

void Hud::StartMouseSize(int nX, int nY)
{
  fMouseDownX_ = nX;
  fMouseDownY_ = nY;
  nMouseDownW_ = nW_;
  nMouseDownH_ = nH_;
  bMouseDown_ = 1;
  CaptureMouseToSizeSensor();
}

void Hud::StopMouseSize()
{
  ReleaseeMouseFromSizeSensor();
  bMouseDown_ = 0;
}

void Hud::DoMouseSize(int nX, int nY)
{
  int fWOffset = nX - fMouseDownX_;
  int fHOffset = nY - fMouseDownY_;

  ApAsyncMessage<Msg_VpView_ContextSize> msg;
  msg->hContext = GetDisplay()->GetContext();
  msg->nWidth = nMouseDownW_ + fWOffset;
  msg->nHeight = nMouseDownH_ + fHOffset;
  msg.Post();
}

//-----------------------------------------------

void Hud::CreateBottomSensor()
{
  if (!Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMSENSOR)) {
    Msg_Scene_CreateMouseSensor::_(hScene_, ELEMENT_BOTTOMSENSOR, 0, 0, 1, 1);
    SizeBottomSensor();
    
    // Get mouse events
    Msg_Scene_SetFillColor::_(hScene_, ELEMENT_BOTTOMSENSOR, 1, 1, 1, 0.01);

    if (Apollo::getModuleConfig(MODULE_NAME, "DebugFrame/BottomSensor", 0)) {
      GetDisplay()->SetDebugFrameColor(ELEMENT_BOTTOMSENSOR);
    }
  }
}

void Hud::DestroyBottomSensor()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMSENSOR)) {
    Msg_Scene_DeleteElement::_(hScene_, ELEMENT_BOTTOMSENSOR);
  }
}

void Hud::SizeBottomSensor()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMSENSOR)) {
    if (bBottomBarEngaged_) {
      Msg_Scene_TranslateElement::_(hScene_, ELEMENT_BOTTOMSENSOR, 0, 0);
      Msg_Scene_SetRectangle::_(hScene_, ELEMENT_BOTTOMSENSOR, 0, 0, nW_, nH_);
    } else {
      Msg_Scene_TranslateElement::_(hScene_, ELEMENT_BOTTOMSENSOR, nW_ / 2, 0);
      Msg_Scene_SetRectangle::_(hScene_, ELEMENT_BOTTOMSENSOR, -nW_ / 2, 0, nW_, BOTTOMSENSOR_HEIGHT);
    }
  }
}

//-----------------------------------------------

void Hud::ShowBottomBar()
{
  if (!Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMBAR)) {
    CreateBottomBar();
    SizeBottomBar();
  }

  bBottomBarEngaged_ = 1;

  SizeBottomSensor();
  Msg_Scene_ShowElement::_(hScene_, ELEMENT_BOTTOMBAR, 1);
  RestartBottomBarTimer();
}

void Hud::RestartBottomBarTimer()
{
  CancelBottomBarTimer();
  StartBottomBarTimer();
}

void Hud::StartBottomBarTimer()
{
  if (!BottomBarTimerActive()) {
    hBottomBarTimer_ = Apollo::startTimeout(2, 0);
    GetModule()->SetContextOfHandle(hBottomBarTimer_, GetDisplay()->GetContext());
  }
}

void Hud::CancelBottomBarTimer()
{
  if (BottomBarTimerActive()) {
    Apollo::cancelTimeout(hBottomBarTimer_);
    GetModule()->DeleteContextOfHandle(hBottomBarTimer_, GetDisplay()->GetContext());
    hBottomBarTimer_ = ApNoHandle;
  }
}

void Hud::HideBottomBar()
{
  bBottomBarEngaged_ = 0;

  Msg_Scene_ShowElement::_(hScene_, ELEMENT_BOTTOMBAR, 0);
  SizeBottomSensor();
}

void Hud::CreateBottomBar()
{
  if (!Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMBAR)) {
    Msg_Scene_CreateElement::_(hScene_, ELEMENT_BOTTOMBAR);

    Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_RECT, 0, 0, 1, 1);
    Msg_Scene_SetFillColor::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_RECT, 1, 0, 0, 0.7);

    Msg_ScWidget_CreateEdit::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, 0, 0, 100, 20);
    //Msg_Scene_SetStrokeColor::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, 0, 0, 0, 1);
    //Msg_Scene_SetFillColor::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, 1, 1, 1, 1);
    Msg_ScWidget_SetEditText::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, "Hallo Welt");
    Msg_ScWidget_SetEditTextFont::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, "Arial");
    Msg_ScWidget_SetEditTextSize::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, 14);
    Msg_ScWidget_SetEditTextColor::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, 0, 0, 1, 1);
    Msg_ScWidget_SetEditPadding::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, 3);
    Msg_ScWidget_SetEditAlign::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_EDIT, Msg_ScWidget_SetEditAlign::AlignCenter, Msg_ScWidget_SetEditAlign::AlignMiddle);
  }
}

void Hud::DestroyBottomBar()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMBAR)) {
    Msg_Scene_DeleteElement::_(hScene_, ELEMENT_BOTTOMBAR);
    bBottomBarEngaged_ = 0;
  }
}

void Hud::SizeBottomBar()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_BOTTOMBAR)) {
    Msg_Scene_TranslateElement::_(hScene_, ELEMENT_BOTTOMBAR, nW_ / 2, 0);
    Msg_Scene_SetRectangle::_(hScene_, ELEMENT_BOTTOMBAR "/" ELEMENT_BOTTOMBAR_RECT, -GetBottomBarWidth() / 2, 0, GetBottomBarWidth(), GetBottomBarHeight());
  }
}

int Hud::GetBottomBarWidth()
{
  int nWidth = 300;
  if (nWidth > nW_ - 20) { nWidth = nW_ - 20; }
  if (nWidth < 20) { nWidth = 20; }
  return nWidth;
}

int Hud::GetBottomBarHeight()
{
  int nHeight = 30;
  return nHeight;
}

int Hud::InsideBottomBar(int nX, int nY)
{
  int nLeft = nW_ / 2 - GetBottomBarWidth() / 2;
  int nBottom = 0;
  int nWidth = GetBottomBarWidth();
  int nHeight = GetBottomBarHeight();

  if (nX > nLeft && nX < nLeft + nWidth && nY > nBottom && nY < nBottom + nHeight) {
    return 1;
  }

  return 0;
}

//-----------------------------------------------

void Hud::CreateSizeSensor()
{
  if (!Msg_Scene_ElementExists::_(hScene_, ELEMENT_SIZESENSOR)) {
    Msg_Scene_CreateMouseSensor::_(hScene_, ELEMENT_SIZESENSOR, 0, 0, 1, 1);
    SizeSizeSensor();
    
    // Get mouse events
    Msg_Scene_SetFillColor::_(hScene_, ELEMENT_SIZESENSOR, 1, 1, 1, 0.01);

    GetDisplay()->SetDebugFrameColor(ELEMENT_SIZESENSOR);
  }
}

void Hud::DestroySizeSensor()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_SIZESENSOR)) {
    Msg_Scene_DeleteElement::_(hScene_, ELEMENT_SIZESENSOR);
  }
}

void Hud::SizeSizeSensor()
{
  if (Msg_Scene_ElementExists::_(hScene_, ELEMENT_SIZESENSOR)) {
    Msg_Scene_SetRectangle::_(hScene_, ELEMENT_SIZESENSOR, nW_ - 20, nH_ - 20, 20, 20);
  }
}

void Hud::CaptureMouseToSizeSensor()
{
  Msg_Scene_CaptureMouse::_(hScene_, ELEMENT_SIZESENSOR);
}

void Hud::ReleaseeMouseFromSizeSensor()
{
  Msg_Scene_ReleaseMouse::_(hScene_);
}

