// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "ApTypes.h"
#include "MsgScene.h"
#include "MsgScWidget.h"
#include "MsgUnitTest.h"
#include "ScWidgetModuleTester.h"

#if defined(AP_TEST)

ApHandle ScWidgetModuleTester::hScene_;

void ScWidgetModuleTester::On_ScWidget_ButtonEvent(Msg_ScWidget_ButtonEvent* pMsg)
{
  if (hScene_ != pMsg->hScene) { return; }

  apLog_Verbose((LOG_CHANNEL, "ScWidgetModuleTester::On_ScWidget_ButtonEvent", "scene=" ApHandleFormat " path=%s event=%d", ApHandleType(pMsg->hScene), StringType(pMsg->sPath), pMsg->nEvent));
}

String ScWidgetModuleTester::Button()
{
  String s;

  hScene_ = Apollo::newHandle();

  if (!s) { if (!Msg_Scene_Create::_(hScene_)) { s = "Msg_Scene_Create failed"; }}
  int nWidth = 200;
  int nHeight = 200;
  if (!s) { if (!Msg_Scene_Position::_(hScene_, 100, 400, nWidth, nHeight)) { s = "Msg_Scene_Position failed"; }}
  if (!s) { if (!Msg_Scene_Visibility::_(hScene_, 1)) { s = "Msg_Scene_Visibility failed"; }}
  if (!s) { if (!Msg_Scene_SetAutoDraw::_(hScene_, 50, 1)) { s = "Msg_Scene_SetAutoDraw failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene_, "_a_frame", 0.5, 0.5, nWidth - 1, nHeight - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene_, "_a_frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene_, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene_, "_a_frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateButton::_(hScene_, "button1", -35, -10, 70, 20)) { s = "Msg_ScWidget_CreateButton failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene_, "button1", Msg_ScWidget_ButtonState_Normal, Apollo::getAppResourcePath() + "test/widget/Button.png", 0, 0)) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene_, "button1", Msg_ScWidget_ButtonState_Down, Apollo::getAppResourcePath() + "test/widget/ButtonDown.png", 0, 0)) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene_, "button1", Msg_ScWidget_ButtonState_High, Apollo::getAppResourcePath() + "test/widget/ButtonHigh.png", 0, 0)) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene_, "button1", Msg_ScWidget_ButtonState_Disabled, Apollo::getAppResourcePath() + "test/widget/ButtonDisabled.png", 0, 0)) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonState::_(hScene_, "button1", Msg_ScWidget_ButtonState_Normal)) { s = "Msg_ScWidget_SetButtonState failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hScene_, "button1", "XXXXX")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_DeleteButtonText::_(hScene_, "button1")) { s = "Msg_ScWidget_DeleteButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hScene_, "button1", "Send")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextFont::_(hScene_, "button1", "verdana")) { s = "Msg_ScWidget_SetButtonTextFont failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextSize::_(hScene_, "button1", 12)) { s = "Msg_ScWidget_SetButtonTextSize failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextFlags::_(hScene_, "button1", Msg_Scene_FontFlags::Bold | Msg_Scene_FontFlags::Italic)) { s = "Msg_ScWidget_SetButtonTextFlags failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextColor::_(hScene_, "button1", 0, 0, 1, 0.5)) { s = "Msg_ScWidget_SetButtonTextColor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene_, "button1", 100, 100)) { s = "Msg_Scene_TranslateElement failed"; }}
  //if (!s) { if (!Msg_ScWidget_SetButtonActive::_(hScene_, "button1", 0)) { s = "Msg_ScWidget_SetButtonActive failed"; }}  

  //if (!s) { if (!Msg_Scene_DeleteElement::_(hScene_, "button1")) { s = "Msg_Scene_DeleteElement failed"; }}

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_Scene_Destroy::_(hScene_)) { s = "Msg_Scene_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

void ScWidgetModuleTester::Begin()
{
  { Msg_ScWidget_ButtonEvent msg; msg.Hook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_ButtonEvent, 0, ApCallbackPosNormal); }

  AP_UNITTEST_REGISTER(ScWidgetModuleTester::Button);
}

void ScWidgetModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::Button);
}

void ScWidgetModuleTester::End()
{
  { Msg_ScWidget_ButtonEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_ButtonEvent, 0); }
}

#endif // #if defined(AP_TEST)
