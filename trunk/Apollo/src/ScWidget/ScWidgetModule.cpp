// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "ScWidgetModule.h"

void Widget::SetCoordinates(double fX, double fY, double fW, double fH)
{
  fX_ = fX;
  fY_ = fY;
  fW_ = fW;
  fH_ = fH;
}

Button* Widget::AsButton()
{
  Button* pWidget = (Button*) this;
  if (!pWidget->IsButton()) { throw ApException("Widget::AsButton not a button scene=" ApHandleFormat " path=" StringFormat "", ApHandleType(hScene_), StringType(sPath_)); }
  return pWidget;
}

//---------------------------

Button::Button(const ApHandle& hScene, const String& sPath)
:Widget(hScene, sPath)
,nState_(NoButtonState)
,bMouseDown_(0)
,bHasText_(0)
,fTextOffsetX_(1.0)
,fTextOffsetY_(-1.0)
{
}

String Button::GetTextPath()
{
  return sPath_ + "/n_text";
}

Button::ButtonState Button::String2State(const String& sName)
{
  ButtonState nState = NoButtonState;

  if (0) {
  } else if (sName == Msg_ScWidget_ButtonState_Normal) {
    nState = NormalButtonState;
  } else if (sName == Msg_ScWidget_ButtonState_Down) {
    nState = DownButtonState;
  } else if (sName == Msg_ScWidget_ButtonState_High) {
    nState = HighButtonState;
  } else if (sName == Msg_ScWidget_ButtonState_Disabled) {
    nState = DisabledButtonState;
  }

  return nState;
}

void Button::SetState(ButtonState nState)
{
  if (nState_ != nState) {
    nState_ = nState;
    ShowState();
  }
}

void Button::Active(int bActive)
{
  bActive_ = bActive;
}

void Button::SetText(const String& sText)
{
  if (!Msg_Scene_ElementExists::_(hScene_, GetTextPath())) {
    Msg_Scene_CreateTextElement::_(hScene_, GetTextPath());
  }
  Msg_Scene_SetText::_(hScene_, GetTextPath(), sText);
  bHasText_ = 1;

  PositionText();
}

void Button::DeleteText()
{
  if (Msg_Scene_ElementExists::_(hScene_, GetTextPath())) {
    Msg_Scene_DeleteElement::_(hScene_, GetTextPath());
  }
  bHasText_ = 0;
}

void Button::SetFontFamily(const String& sFont)
{
  Msg_Scene_SetFontFamily::_(hScene_, GetTextPath(), sFont);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetFontSize(double fSize)
{
  Msg_Scene_SetFontSize::_(hScene_, GetTextPath(), fSize);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetFontFlags(int nFlags)
{
  Msg_Scene_SetFontFlags::_(hScene_, GetTextPath(), nFlags);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetFontColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  Msg_Scene_SetFillColor::_(hScene_, GetTextPath(), fRed, fGreen, fBlue, fAlpha);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetImageFile(ButtonState nState, const String& sFile, double fX, double fY)
{
  if (states_.IsSet(nState)) {
    states_.Unset(nState);
  }
  ButtonStateConfig state;
  state.sFile_ = sFile;
  state.fX_ = fX;
  state.fY_ = fY;
  states_.Set(nState, state);
}

void Button::Create()
{
  Msg_Scene_CreateMouseSensor::_(hScene_, sPath_, fX_, fY_, fW_, fH_);
  Msg_Scene_CreateRectangle::_(hScene_, sPath_ + "/h_image", 0, 0, fW_, fH_);
}

void Button::ShowState()
{
  ButtonStateConfigListNode* pNode = states_.Find(nState_);
  if (pNode) {
    Msg_Scene_SetFillImageFile::_(hScene_, sPath_ + "/h_image", pNode->Value().sFile_);
    Msg_Scene_SetFillImageOffset::_(hScene_, sPath_ + "/h_image", fX_, fY_);
    //Msg_Scene_SetFillImageOffset::_(hScene_, sPath_ + "/h_image", fX_ + pNode->Value().fX_, fY_ + pNode->Value().fY_);
  } else {
    Msg_Scene_SetFillColor::_(hScene_, sPath_ + "/h_image", 1, 0, 0, 1);
  }

  if (bHasText_) {
    PositionText();
  }
}

void Button::PositionText()
{
  double fTextX, fTextY;
  GetTextPos(fTextX, fTextY);
  Msg_Scene_SetPosition::_(hScene_, GetTextPath(), fTextX, fTextY);
}

void Button::GetTextPos(double& fTextX, double& fTextY)
{
  double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
  if (Msg_Scene_MeasureText::_(hScene_, GetTextPath(), fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY)) {

    int nOffset = (nState_ == DownButtonState) ? 1 : 0;

    fTextX = fX_ + fW_/2 - fTextW / 2 + nOffset * fTextOffsetX_;
    fTextY = fY_ + fH_/2 - fTextH / 2 + nOffset * fTextOffsetY_;

  } else {
    fTextX = 0;
    fTextY = 0;
  }
}

void Button::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  //apLog_Debug((LOG_CHANNEL, "SensorElement::MouseEvent", "event=%d button=%d x=%d y=%d inside=%d", pMsg->nEvent, pMsg->nButton, (int) pMsg->fX, (int) pMsg->fY, pMsg->bInside));
  if (bActive_) {
  
    switch (pMsg->nEvent) {
      case Msg_Scene_MouseEvent::MouseMove: {
        if (bMouseDown_) {
          if (pMsg->bInside) {
            SetState(DownButtonState);
          } else {
            SetState(NormalButtonState);
          }
        } else {
          SetState(HighButtonState);
        }
      } break;

      case Msg_Scene_MouseEvent::MouseDown: {
        if (pMsg->nButton == Msg_Scene_MouseEvent::LeftButton) {
          Msg_Scene_CaptureMouse::_(hScene_, sPath_);
          SetState(DownButtonState);
          bMouseDown_ = 1;
        }
      } break;

      case Msg_Scene_MouseEvent::MouseUp: {
        if (pMsg->nButton == Msg_Scene_MouseEvent::LeftButton) {
          if (bMouseDown_) {
            if (nState_ == DownButtonState) {
              DoClick();
            }
          }
          bMouseDown_ = 0;
          SetState(NormalButtonState);
          Msg_Scene_ReleaseMouse::_(hScene_);
        }
      } break;

      case Msg_Scene_MouseEvent::MouseOut: {
        SetState(NormalButtonState);
      } break;
    }

  } // if bActive_
}

void Button::DoClick()
{
  Msg_ScWidget_ButtonEvent msg;
  msg.hScene = hScene_;
  msg.sPath = sPath_;
  msg.nEvent = Msg_ScWidget_ButtonEvent::ButtonClick;
  msg.Send();
}

//---------------------------

void Scene::AddWidget(const String& sPath, Widget* pWidget)
{
  widgets_.Set(sPath, pWidget);
}

void Scene::DeleteWidget(const String& sPath)
{
  Widget* pWidget = FindWidget(sPath);
  if (pWidget) {
    widgets_.Unset(sPath);
    pWidget->Delete();
    delete pWidget;
    pWidget = 0;
  }
}

Widget* Scene::FindWidget(const String& sPath)
{
  Widget* pWidget = 0;  
  widgets_.Get(sPath, pWidget);
  if (pWidget == 0) { throw ApException("Scene::FindWidget no surface=" StringFormat "", StringType(sPath)); }
  return pWidget;
}

int Scene::HasWidget(const String& sPath)
{
  return widgets_.IsSet(sPath);
}

//---------------------------

Scene* ScWidgetModule::CreateScene(const ApHandle& hScene)
{
  Scene* pScene = new Scene();
  if (pScene) {
    scenes_.Set(hScene, pScene);
  }
  return pScene;
}

void ScWidgetModule::DeleteScene(const ApHandle& hScene)
{
  Scene* pScene = FindScene(hScene);
  if (pScene) {
    scenes_.Unset(hScene);
    delete pScene;
    pScene = 0;
  }
}

Scene* ScWidgetModule::FindScene(const ApHandle& hScene)
{
  Scene* pScene = 0;
  scenes_.Get(hScene, pScene);
  if (pScene == 0) { throw ApException("ScWidgetModule::FindScene no surface=" ApHandleFormat "", ApHandleType(hScene)); }
  return pScene;
}

Scene* ScWidgetModule::FindOrCreateScene(const ApHandle& hScene)
{
  Scene* pScene = 0;  
  if (HasScene(hScene)) {
    pScene = FindScene(hScene);
  } else {
    pScene = CreateScene(hScene);
  }
  return pScene;
}

int ScWidgetModule::HasScene(const ApHandle& hScene)
{
  return scenes_.IsSet(hScene);
}

Widget* ScWidgetModule::FindWidget(const ApHandle& hScene, const String& sPath)
{
  Widget* pWidget = 0;  
  Scene* pScene = FindScene(hScene);
  pWidget = pScene->FindWidget(sPath);
  return pWidget;
}

//---------------------------

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_CreateButton)
{
  Scene* pScene = FindOrCreateScene(pMsg->hScene);
  if (pScene) {
    if (pScene->HasWidget(pMsg->sPath)) { throw ApException("ScWidgetModule::ScWidget_CreateButton widget already there, scene=" ApHandleFormat " path=%s", ApHandleType(pMsg->hScene), StringType(pMsg->sPath)); }
    Button* pButton = new Button(pMsg->hScene, pMsg->sPath);
    if (pButton) {
      pButton->SetCoordinates(pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
      pButton->Create();
      pScene->AddWidget(pMsg->sPath, pButton);
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetCoordinates)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->SetCoordinates(pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonState)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetState(Button::String2State(pMsg->sState));
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonActive)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->Active(pMsg->bActive);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonText)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetText(pMsg->sText);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_DeleteButtonText)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->DeleteText();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonTextFont)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetFontFamily(pMsg->sFont);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonTextSize)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetFontSize(pMsg->fSize);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonTextFlags)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetFontFlags(pMsg->nFlags);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonTextColor)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetFontColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonImageFile)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetImageFile(Button::String2State(pMsg->sState), pMsg->sFile, pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, Scene_DeleteElement)
{
  if (HasScene(pMsg->hScene)) {
    Scene* pScene = FindScene(pMsg->hScene);
    if (pScene->HasWidget(pMsg->sPath)) {
      pScene->DeleteWidget(pMsg->sPath);
      pMsg->apStatus = ApMessage::Ok;
    }
  }
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, Scene_MouseEvent)
{
  if (HasScene(pMsg->hScene)) {
    Scene* pScene = FindScene(pMsg->hScene);
    if (pScene->HasWidget(pMsg->sPath)) {
      pScene->FindWidget(pMsg->sPath)->OnMouseEvent(pMsg);
    }
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

#include "ScWidgetModuleTester.h"

AP_MSG_HANDLER_METHOD(ScWidgetModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ScWidget", 0)) {
    ScWidgetModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ScWidget", 0)) {
    ScWidgetModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ScWidget", 0)) {
    ScWidgetModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int ScWidgetModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_CreateButton, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetCoordinates, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonState, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonActive, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_DeleteButtonText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonTextFont, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonTextSize, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonTextFlags, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonTextColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, Scene_DeleteElement, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, Scene_MouseEvent, this, ApCallbackPosLate);

  AP_UNITTEST_HOOK(ScWidgetModule, this);

  return ok;
}

void ScWidgetModule::Exit()
{
  AP_UNITTEST_UNHOOK(ScWidgetModule, this);
  AP_MSG_REGISTRY_FINISH;
}
