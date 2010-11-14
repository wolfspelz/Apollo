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

ApHandle ScWidgetModuleTester::hButtonScene_;
ApHandle ScWidgetModuleTester::hEditScene_;

void ScWidgetModuleTester::On_ScWidget_ButtonEvent(Msg_ScWidget_ButtonEvent* pMsg)
{
  if (hButtonScene_ != pMsg->hScene) { return; }

  apLog_Verbose((LOG_CHANNEL, "ScWidgetModuleTester::On_ScWidget_ButtonEvent", "scene=" ApHandleFormat " path=%s event=%d", ApHandleType(pMsg->hScene), StringType(pMsg->sPath), pMsg->nEvent));
}

String ScWidgetModuleTester::Button()
{
  String s;

  hButtonScene_ = Apollo::newHandle();

  if (!s) { if (!Msg_Scene_Create::_(hButtonScene_)) { s = "Msg_Scene_Create failed"; }}
  int nWidth = 200;
  int nHeight = 200;
  if (!s) { if (!Msg_Scene_Position::_(hButtonScene_, 100, 300, nWidth, nHeight)) { s = "Msg_Scene_Position failed"; }}
  if (!s) { if (!Msg_Scene_Visibility::_(hButtonScene_, 1)) { s = "Msg_Scene_Visibility failed"; }}
  if (!s) { if (!Msg_Scene_SetAutoDraw::_(hButtonScene_, 50, 1)) { s = "Msg_Scene_SetAutoDraw failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hButtonScene_, "_a_frame", 0.5, 0.5, nWidth - 1, nHeight - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hButtonScene_, "_a_frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hButtonScene_, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hButtonScene_, "_b_quarter1", 0.5, 0.5, nWidth/2 - 1, nHeight/2 - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hButtonScene_, "_b_quarter1", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hButtonScene_, "_b_quarter2", nWidth/2 - 0.5, nHeight/2 - 0.5, nWidth/2 - 1, nHeight/2 - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hButtonScene_, "_b_quarter2", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hButtonScene_, "_b_line1", 0.5, 0.5, nWidth/2 - 1 + 10, nHeight/2 - 1 + 10)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hButtonScene_, "_b_line1", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hButtonScene_, "_b_line2", 0.5, 0.5, nWidth/2 + 10 + 70, nHeight/2 + 10 + 20)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hButtonScene_, "_b_line2", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateButton::_(hButtonScene_, "button1", 10, 10, 70, 20)) { s = "Msg_ScWidget_CreateButton failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hButtonScene_, "button1", Msg_ScWidget_ButtonState_Normal, Apollo::getAppResourcePath() + "test/widget/Button.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hButtonScene_, "button1", Msg_ScWidget_ButtonState_Down, Apollo::getAppResourcePath() + "test/widget/ButtonDown.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hButtonScene_, "button1", Msg_ScWidget_ButtonState_High, Apollo::getAppResourcePath() + "test/widget/ButtonHigh.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hButtonScene_, "button1", Msg_ScWidget_ButtonState_Disabled, Apollo::getAppResourcePath() + "test/widget/ButtonDisabled.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextOffset::_(hButtonScene_, "button1", Msg_ScWidget_ButtonState_Down, 0, -1)) { s = "Msg_ScWidget_SetButtonTextOffset failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonState::_(hButtonScene_, "button1", Msg_ScWidget_ButtonState_Normal)) { s = "Msg_ScWidget_SetButtonState failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hButtonScene_, "button1", "XXXXX")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_DeleteButtonText::_(hButtonScene_, "button1")) { s = "Msg_ScWidget_DeleteButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hButtonScene_, "button1", "Send")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextFont::_(hButtonScene_, "button1", "Verdana")) { s = "Msg_ScWidget_SetButtonTextFont failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextSize::_(hButtonScene_, "button1", 12)) { s = "Msg_ScWidget_SetButtonTextSize failed"; }}
  //if (!s) { if (!Msg_ScWidget_SetButtonTextFlags::_(hButtonScene_, "button1", Msg_Scene_FontFlags::Bold | Msg_Scene_FontFlags::Italic)) { s = "Msg_ScWidget_SetButtonTextFlags failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextColor::_(hButtonScene_, "button1", 0, 0, 0, 1)) { s = "Msg_ScWidget_SetButtonTextColor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hButtonScene_, "button1", 100, 100)) { s = "Msg_Scene_TranslateElement failed"; }}
  //if (!s) { if (!Msg_ScWidget_SetButtonActive::_(hButtonScene_, "button1", 0)) { s = "Msg_ScWidget_SetButtonActive failed"; }}  

  //if (!s) { if (!Msg_Scene_DeleteElement::_(hButtonScene_, "button1")) { s = "Msg_Scene_DeleteElement failed"; }}

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_Scene_Destroy::_(hButtonScene_)) { s = "Msg_Scene_Destroy failed"; }}
  }

  return s;
}

String ScWidgetModuleTester::Edit()
{
  String s;

  hEditScene_ = Apollo::newHandle();

  if (!s) { if (!Msg_Scene_Create::_(hEditScene_)) { s = "Msg_Scene_Create failed"; }}
  int nWidth = 200;
  int nHeight = 200;
  if (!s) { if (!Msg_Scene_Position::_(hEditScene_, 100, 501, nWidth, nHeight)) { s = "Msg_Scene_Position failed"; }}
  if (!s) { if (!Msg_Scene_Visibility::_(hEditScene_, 1)) { s = "Msg_Scene_Visibility failed"; }}
  if (!s) { if (!Msg_Scene_SetAutoDraw::_(hEditScene_, 50, 1)) { s = "Msg_Scene_SetAutoDraw failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hEditScene_, "_a_frame", 0.5, 0.5, nWidth/2 - 1, nHeight - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hEditScene_, "_a_frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hEditScene_, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  // ------------------------

  //if (!s) { if (!Msg_ScWidget_CreateEdit::_(hEditScene_, "edit1", -75, -10, 150, 20)) { s = "Msg_ScWidget_CreateEdit failed"; }}
  //if (!s) { if (!Msg_Scene_SetStrokeColor::_(hEditScene_, "edit1", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  //if (!s) { if (!Msg_Scene_TranslateElement::_(hEditScene_, "edit1", 100, 50)) { s = "Msg_Scene_TranslateElement failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateEdit::_(hEditScene_, "edit2", -75, -10, 150, 20)) { s = "Msg_ScWidget_CreateEdit failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hEditScene_, "edit2", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hEditScene_, "edit2", 100, 100)) { s = "Msg_Scene_TranslateElement failed"; }}

  //if (!s) { if (!Msg_Scene_DeleteElement::_(hEditScene_, "edit2")) { s = "Msg_Scene_DeleteElement failed"; }}

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_Scene_Destroy::_(hEditScene_)) { s = "Msg_Scene_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

void ScWidgetModuleTester::Begin()
{
  { Msg_ScWidget_ButtonEvent msg; msg.Hook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_ButtonEvent, 0, ApCallbackPosNormal); }

//  AP_UNITTEST_REGISTER(ScWidgetModuleTester::Button);
  AP_UNITTEST_REGISTER(ScWidgetModuleTester::Edit);
}

void ScWidgetModuleTester::Execute()
{
//  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::Button);
  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::Edit);
}

void ScWidgetModuleTester::End()
{
  { Msg_ScWidget_ButtonEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_ButtonEvent, 0); }
}

#endif // #if defined(AP_TEST)
