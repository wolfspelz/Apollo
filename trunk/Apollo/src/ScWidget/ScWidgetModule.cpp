// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "ScWidgetModule.h"
#include "Widget.h"
#include "Button.h"
#include "Edit.h"

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
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetImageFile(Button::String2State(pMsg->sState), pMsg->sFile);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_SetButtonTextOffset)
{
  FindWidget(pMsg->hScene, pMsg->sPath)->AsButton()->SetTextOffset(Button::String2State(pMsg->sState), pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

//---------------------------

AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_CreateEdit)
{
  Scene* pScene = FindOrCreateScene(pMsg->hScene);
  if (pScene) {
    if (pScene->HasWidget(pMsg->sPath)) { throw ApException("ScWidgetModule::ScWidget_CreateEdit widget already there, scene=" ApHandleFormat " path=%s", ApHandleType(pMsg->hScene), StringType(pMsg->sPath)); }
    Edit* pEdit = new Edit(pMsg->hScene, pMsg->sPath);
    if (pEdit) {
      pEdit->SetCoordinates(pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
      pEdit->Create();
      pScene->AddWidget(pMsg->sPath, pEdit);
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

//---------------------------

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

AP_MSG_HANDLER_METHOD(ScWidgetModule, Scene_KeyEvent)
{
  if (HasScene(pMsg->hScene)) {
    Scene* pScene = FindScene(pMsg->hScene);
    if (pScene->HasWidget(pMsg->sPath)) {
      pScene->FindWidget(pMsg->sPath)->OnKeyEvent(pMsg);
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_SetButtonTextOffset, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_CreateEdit, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, Scene_DeleteElement, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, Scene_MouseEvent, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, Scene_KeyEvent, this, ApCallbackPosLate);

  AP_UNITTEST_HOOK(ScWidgetModule, this);

  return ok;
}

void ScWidgetModule::Exit()
{
  AP_UNITTEST_UNHOOK(ScWidgetModule, this);
  AP_MSG_REGISTRY_FINISH;
}
