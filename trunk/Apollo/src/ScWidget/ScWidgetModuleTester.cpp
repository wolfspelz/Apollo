// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "ApTypes.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "MsgScWidget.h"
#include "MsgUnitTest.h"
#include "ScWidgetModuleTester.h"

#if defined(AP_TEST)

ApHandle ScWidgetModuleTester::hScene_;

String ScWidgetModuleTester::Button()
{
  String s;

  hScene_ = Apollo::newHandle();

  if (!s) { if (!Msg_Scene_Create::_(hScene_)) { s = "Msg_Scene_Create failed"; }}
  int nWidth = 200;
  int nHeight = 100;
  if (!s) { if (!Msg_Scene_Position::_(hScene_, 100, 400, nWidth, nHeight)) { s = "Msg_Scene_Position failed"; }}
  if (!s) { if (!Msg_Scene_Visibility::_(hScene_, 1)) { s = "Msg_Scene_Visibility failed"; }}
  if (!s) { if (!Msg_Scene_SetAutoDraw::_(hScene_, 50, 1)) { s = "Msg_Scene_SetAutoDraw failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene_, "_a_frame", 0.5, 0.5, nWidth - 0.5, nHeight - 0.5)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene_, "_a_frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene_, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene_, "_a_frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) { if (!Msg_ScWidget_CreateButton::_(hScene_, "button1", -20, -10, 40, 20)) { s = "Msg_ScWidget_CreateButton failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene_, "button1", 100, 50)) { s = "Msg_Scene_TranslateElement failed"; }}

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_Scene_Destroy::_(hScene_)) { s = "Msg_Scene_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

void ScWidgetModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(ScWidgetModuleTester::Button);
}

void ScWidgetModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(ScWidgetModuleTester::Button);
}

void ScWidgetModuleTester::End()
{
}

#endif // #if defined(AP_TEST)
