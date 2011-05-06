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
#include "Edit.h"

#if defined(AP_TEST)

ApHandle ScWidgetModuleTester::hButtonScene_;
ApHandle ScWidgetModuleTester::hEditScene_;

void ScWidgetModuleTester::On_ScWidget_ButtonEvent(Msg_ScWidget_ButtonEvent* pMsg)
{
  if (hButtonScene_ != pMsg->hScene) { return; }

  apLog_Verbose((LOG_CHANNEL, "ScWidgetModuleTester::On_ScWidget_ButtonEvent", "scene=" ApHandleFormat " path=%s event=%d", ApHandleType(pMsg->hScene), StringType(pMsg->sPath), pMsg->nEvent));
}

void ScWidgetModuleTester::On_ScWidget_EditEvent(Msg_ScWidget_EditEvent* pMsg)
{
  if (hEditScene_ != pMsg->hScene) { return; }

  if (pMsg->nEvent == Msg_ScWidget_EditEvent::EditEnter) {
    String sText;
    Msg_ScWidget_GetEditText::_(hEditScene_, pMsg->sPath, sText);
    apLog_Verbose((LOG_CHANNEL, "ScWidgetModuleTester::On_ScWidget_EditEvent", "scene=" ApHandleFormat " path=%s event=%d text=%s", ApHandleType(pMsg->hScene), StringType(pMsg->sPath), pMsg->nEvent, StringType(sText)));
  } else {
    apLog_Verbose((LOG_CHANNEL, "ScWidgetModuleTester::On_ScWidget_EditEvent", "scene=" ApHandleFormat " path=%s event=%d", ApHandleType(pMsg->hScene), StringType(pMsg->sPath), pMsg->nEvent));
  }
}

String ScWidgetModuleTester::Button()
{
  String s;

  ApHandle hScene = Apollo::newHandle();
  hButtonScene_ = hScene;

  if (!s) { if (!Msg_Scene_Create::_(hScene)) { s = "Msg_Scene_Create failed"; }}
  int nWidth = 200;
  int nHeight = 200;
  if (!s) { if (!Msg_Scene_Position::_(hScene, 100, 300, nWidth, nHeight)) { s = "Msg_Scene_Position failed"; }}
  if (!s) { if (!Msg_Scene_Visibility::_(hScene, 1)) { s = "Msg_Scene_Visibility failed"; }}
  if (!s) { if (!Msg_Scene_SetAutoDraw::_(hScene, 50, 1)) { s = "Msg_Scene_SetAutoDraw failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_a_frame", 0.5, 0.5, nWidth - 1, nHeight - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "_a_frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_b_quarter1", 0.5, 0.5, nWidth/2 - 1, nHeight/2 - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_b_quarter1", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_b_quarter2", nWidth/2 - 0.5, nHeight/2 - 0.5, nWidth/2 - 1, nHeight/2 - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_b_quarter2", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_b_line1", 0.5, 0.5, nWidth/2 - 1 + 10, nHeight/2 - 1 + 10)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_b_line1", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_b_line2", 0.5, 0.5, nWidth/2 + 10 + 70, nHeight/2 + 10 + 20)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_b_line2", 0, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateButton::_(hScene, "button1", 10, 10, 70, 20)) { s = "Msg_ScWidget_CreateButton failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button1", Msg_ScWidget_ButtonState_Normal, Apollo::getAppResourcePath() + "test/widget/Button.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button1", Msg_ScWidget_ButtonState_Down, Apollo::getAppResourcePath() + "test/widget/ButtonDown.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button1", Msg_ScWidget_ButtonState_High, Apollo::getAppResourcePath() + "test/widget/ButtonHigh.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button1", Msg_ScWidget_ButtonState_Disabled, Apollo::getAppResourcePath() + "test/widget/ButtonDisabled.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextOffset::_(hScene, "button1", Msg_ScWidget_ButtonState_Down, 0, -1)) { s = "Msg_ScWidget_SetButtonTextOffset failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonState::_(hScene, "button1", Msg_ScWidget_ButtonState_Normal)) { s = "Msg_ScWidget_SetButtonState failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hScene, "button1", "XXXXX")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_DeleteButtonText::_(hScene, "button1")) { s = "Msg_ScWidget_DeleteButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hScene, "button1", "Send")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextFont::_(hScene, "button1", "Courier New")) { s = "Msg_ScWidget_SetButtonTextFont failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextSize::_(hScene, "button1", 12)) { s = "Msg_ScWidget_SetButtonTextSize failed"; }}
  //if (!s) { if (!Msg_ScWidget_SetButtonTextFlags::_(hScene, "button1", Msg_Scene_FontFlags::Bold | Msg_Scene_FontFlags::Italic)) { s = "Msg_ScWidget_SetButtonTextFlags failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonTextColor::_(hScene, "button1", 0, 0, 1, 0.5)) { s = "Msg_ScWidget_SetButtonTextColor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "button1", 100, 100)) { s = "Msg_Scene_TranslateElement failed"; }}
  //if (!s) { if (!Msg_ScWidget_SetButtonActive::_(hScene, "button1", 0)) { s = "Msg_ScWidget_SetButtonActive failed"; }}  

  //if (!s) { if (!Msg_Scene_DeleteElement::_(hScene, "button1")) { s = "Msg_Scene_DeleteElement failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateButton::_(hScene, "button2", 0, 0, 70, 20)) { s = "Msg_ScWidget_CreateButton failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button2", Msg_ScWidget_ButtonState_Normal, Apollo::getAppResourcePath() + "test/widget/Button.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button2", Msg_ScWidget_ButtonState_Down, Apollo::getAppResourcePath() + "test/widget/ButtonDown.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button2", Msg_ScWidget_ButtonState_High, Apollo::getAppResourcePath() + "test/widget/ButtonHigh.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonImageFile::_(hScene, "button2", Msg_ScWidget_ButtonState_Disabled, Apollo::getAppResourcePath() + "test/widget/ButtonDisabled.png")) { s = "Msg_ScWidget_SetButtonImageFile failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonState::_(hScene, "button2", Msg_ScWidget_ButtonState_Normal)) { s = "Msg_ScWidget_SetButtonState failed"; }}
  if (!s) { if (!Msg_ScWidget_SetButtonText::_(hScene, "button2", "Hello World")) { s = "Msg_ScWidget_SetButtonText failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "button2", 10, 10)) { s = "Msg_Scene_TranslateElement failed"; }}
  //if (!s) { if (!Msg_ScWidget_SetButtonActive::_(hScene, "button2", 0)) { s = "Msg_ScWidget_SetButtonActive failed"; }}  

  //if (!s) { if (!Msg_Scene_DeleteElement::_(hScene, "button2")) { s = "Msg_Scene_DeleteElement failed"; }}

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_Scene_Destroy::_(hScene)) { s = "Msg_Scene_Destroy failed"; }}
  }

  return s;
}

String ScWidgetModuleTester::Edit()
{
  String s;

  ApHandle hScene = Apollo::newHandle();
  hEditScene_ = hScene;

  if (!s) { if (!Msg_Scene_Create::_(hScene)) { s = "Msg_Scene_Create failed"; }}
  int nWidth = 200;
  int nHeight = 200;
  if (!s) { if (!Msg_Scene_Position::_(hScene, 100, 501, nWidth, nHeight)) { s = "Msg_Scene_Position failed"; }}
  if (!s) { if (!Msg_Scene_Visibility::_(hScene, 1)) { s = "Msg_Scene_Visibility failed"; }}
  if (!s) { if (!Msg_Scene_SetAutoDraw::_(hScene, 50, 1)) { s = "Msg_Scene_SetAutoDraw failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_a_frame", 0.5, 0.5, nWidth/2 - 1, nHeight - 1)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "_a_frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateEdit::_(hScene, "edit1", -75, -10, 150, 20)) { s = "Msg_ScWidget_CreateEdit failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "edit1", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "edit1", 1, 1, 1, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_ScWidget_SetEditText::_(hScene, "edit1", "Hallo Welt \xe9\x87\x91")) { s = "Msg_ScWidget_SetEditText failed"; }}
  if (!s) { if (!Msg_ScWidget_SetEditTextFont::_(hScene, "edit1", "Arial")) { s = "Msg_ScWidget_SetEditTextFont failed"; }}
//  if (!s) { if (!Msg_ScWidget_SetEditTextFont::_(hScene, "edit1", "Microsoft YaHei")) { s = "Msg_ScWidget_SetEditTextFont failed"; }}
  if (!s) { if (!Msg_ScWidget_SetEditTextSize::_(hScene, "edit1", 14)) { s = "Msg_ScWidget_SetEditTextSize failed"; }}
//  if (!s) { if (!Msg_ScWidget_SetEditTextFlags::_(hScene, "edit1", Msg_Scene_FontFlags::Bold)) { s = "Msg_ScWidget_SetEditTextFlags failed"; }}
  if (!s) { if (!Msg_ScWidget_SetEditTextColor::_(hScene, "edit1", 0, 0, 1, 1)) { s = "Msg_ScWidget_SetEditTextColor failed"; }}
  if (!s) { if (!Msg_ScWidget_SetEditPadding::_(hScene, "edit1", 3)) { s = "Msg_ScWidget_SetEditPadding failed"; }}
  if (!s) { if (!Msg_ScWidget_SetEditAlign::_(hScene, "edit1", Msg_ScWidget_SetEditAlign::AlignCenter, Msg_ScWidget_SetEditAlign::AlignMiddle)) { s = "Msg_ScWidget_SetEditAlign failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "edit1", 100, 50)) { s = "Msg_Scene_TranslateElement failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateEdit::_(hScene, "edit2", -75, -10, 150, 20)) { s = "Msg_ScWidget_CreateEdit failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "edit2", 0.5, 0.5, 0.5, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "edit2", 100, 100)) { s = "Msg_Scene_TranslateElement failed"; }}

  //if (!s) { if (!Msg_Scene_DeleteElement::_(hScene, "edit2")) { s = "Msg_Scene_DeleteElement failed"; }}

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_Scene_Destroy::_(hScene)) { s = "Msg_Scene_Destroy failed"; }}
  }

  return s;
}

String ScWidgetModuleTester::SameConstants()
{
  String s;

  if (!s) { if (Msg_ScWidget_SetEditAlign::NoAlign != Edit::NoAlign) { s = "Msg_ScWidget_SetEditAlign::NoAlign != Edit::NoAlign"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::AlignLeft != Edit::AlignLeft) { s = "Msg_ScWidget_SetEditAlign::AlignLeft != Edit::AlignLeft"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::AlignCenter != Edit::AlignCenter) { s = "Msg_ScWidget_SetEditAlign::AlignCenter != Edit::AlignCenter"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::AlignRight != Edit::AlignRight) { s = "Msg_ScWidget_SetEditAlign::AlignRight != Edit::AlignRight"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::AlignTop != Edit::AlignTop) { s = "Msg_ScWidget_SetEditAlign::AlignTop != Edit::AlignTop"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::AlignMiddle != Edit::AlignMiddle) { s = "Msg_ScWidget_SetEditAlign::AlignMiddle != Edit::AlignMiddle"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::AlignBottom != Edit::AlignBottom) { s = "Msg_ScWidget_SetEditAlign::AlignBottom != Edit::AlignBottom"; }}
  if (!s) { if (Msg_ScWidget_SetEditAlign::LastAlign != Edit::LastAlign) { s = "Msg_ScWidget_SetEditAlign::LastAlign != Edit::LastAlign"; }}

  return s;
}

//----------------------------------------------------------

void ScWidgetModuleTester::Begin()
{
  { Msg_ScWidget_ButtonEvent msg; msg.Hook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_ButtonEvent, 0, ApCallbackPosNormal); }
  { Msg_ScWidget_EditEvent msg; msg.Hook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_EditEvent, 0, ApCallbackPosNormal); }

  AP_UNITTEST_REGISTER(ScWidgetModuleTester::Button);
  AP_UNITTEST_REGISTER(ScWidgetModuleTester::Edit);
  AP_UNITTEST_REGISTER(ScWidgetModuleTester::SameConstants);
}

void ScWidgetModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::Button);
  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::Edit);
  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::SameConstants);
}

void ScWidgetModuleTester::End()
{
  { Msg_ScWidget_ButtonEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_ButtonEvent, 0); }
  { Msg_ScWidget_EditEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) ScWidgetModuleTester::On_ScWidget_EditEvent, 0); }
}

#endif // #if defined(AP_TEST)
