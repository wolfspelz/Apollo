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
,sState_(Msg_ScWidget_ButtonState_Normal)
{
}

void Button::SetState(const String& sName)
{
  sState_ = sName;
  ShowState();
}

void Button::SetImageFile(const String& sName, const String& sFile, double fX, double fY)
{
  if (states_.IsSet(sName)) {
    states_.Unset(sName);
  }
  ButtonState state;
  state.sFile_ = sFile;
  state.fX_ = fX;
  state.fY_ = fY;
  states_.Set(sName, state);
}

void Button::Create()
{
  Msg_Scene_CreateMouseSensor::_(hScene_, sPath_, fX_, fY_, fW_, fH_);
  Msg_Scene_CreateRectangle::_(hScene_, sPath_ + "/h_image", fX_, fY_, fW_, fH_);
  ShowState();
}

void Button::ShowState()
{
  ButtonStateListNode* pNode = states_.Find(sState_);
  if (pNode) {
    Msg_Scene_SetFillImageFile::_(hScene_, sPath_ + "/h_image", pNode->Value().sFile_);
    Msg_Scene_SetFillImageOffset::_(hScene_, sPath_ + "/h_image", fX_, fY_);
    //Msg_Scene_SetFillImageOffset::_(hScene_, sPath_ + "/h_image", fX_ + pNode->Value().fX_, fY_ + pNode->Value().fY_);
    //Msg_Scene_SetFillColor::_(hScene_, sPath_ + "/h_image", 1, 0, 0, 1);
  } else {
    Msg_Scene_SetFillColor::_(hScene_, sPath_ + "/h_image", 1, 0, 0, 1);
  }
}

void Button::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseMove) {
    if (sState_ != Msg_ScWidget_ButtonState_Up) {
      SetState(Msg_ScWidget_ButtonState_Up);
    }

  } else if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseOut) {
    if (sState_ != Msg_ScWidget_ButtonState_Normal) {
      SetState(Msg_ScWidget_ButtonState_Normal);
    }

  } else {

    if (pMsg->nButton == Msg_Scene_MouseEvent::LeftButton) {
      switch (pMsg->nEvent) {
        case Msg_Scene_MouseEvent::MouseDown: {
          Msg_Scene_CaptureMouse::_(hScene_, sPath_);
          SetState(Msg_ScWidget_ButtonState_Down);
        } break;

        case Msg_Scene_MouseEvent::MouseUp: {
          SetState(Msg_ScWidget_ButtonState_Normal);
          Msg_Scene_ReleaseMouse::_(hScene_);
        } break;
      }
    }

  }
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
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetState(pMsg->sState);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonImageFile)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetImageFile(pMsg->sState, pMsg->sFile, pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_Show)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->Show();
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_Show, this, ApCallbackPosNormal);
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
