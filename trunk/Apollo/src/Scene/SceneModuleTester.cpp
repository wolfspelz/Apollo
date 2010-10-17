// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "ApTypes.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "MsgUnitTest.h"
#include "SceneModuleTester.h"
#include "Element.h"
#include "Surface.h"

#if defined(AP_TEST)

int SceneModuleTester::bHasCursor_ = 0;
void SceneModuleTester::On_Scene_MouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  apLog_Verbose((LOG_CHANNEL, "SceneModuleTester::On_Scene_MouseEvent", "scene=" ApHandleFormat " path=%s event=%d button=%d x=%f y=%f", ApHandleType(pMsg->hScene), StringType(pMsg->sPath), pMsg->nEvent, pMsg->nButton, pMsg->fX, pMsg->fY));

  if (!bHasCursor_) {
    double fW = 0;
    double fH = 0;
    Msg_Scene_GetImageSizeFromFile::_(pMsg->hScene, Apollo::getAppResourcePath() + "test/cursor.png", fW, fH);
    Msg_Scene_CreateImageFromFile::_(pMsg->hScene, "z_cursor", -fW/2.0, -fH/2.0, Apollo::getAppResourcePath() + "test/cursor.png");
    bHasCursor_ = 1;
  }

  if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseDown) {
    Msg_Scene_CaptureMouse::_(pMsg->hScene, pMsg->sPath);
  }
  if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseUp) {
    Msg_Scene_ReleaseMouse::_(pMsg->hScene);
  }

  double fTranslateX = 0;
  double fTranslateY = 0;
  Msg_Scene_GetTranslateElement::_(pMsg->hScene, pMsg->sPath, fTranslateX, fTranslateY);
  Msg_Scene_TranslateElement::_(pMsg->hScene, "z_cursor", pMsg->fX + fTranslateX, pMsg->fY + fTranslateY);
  //Msg_Scene_Draw msg; msg.hScene = pMsg->hScene; msg.Request();
}

void SceneModuleTester::Begin()
{
  { Msg_Scene_MouseEvent msg; msg.Hook(MODULE_NAME, (ApCallback) SceneModuleTester::On_Scene_MouseEvent, 0, ApCallbackPosNormal); }

  AP_UNITTEST_REGISTER(SceneModuleTester::Rectangle);
  AP_UNITTEST_REGISTER(SceneModuleTester::SameConstants_FontFlags);
  AP_UNITTEST_REGISTER(SceneModuleTester::SameConstants_Operator);
  AP_UNITTEST_REGISTER(SceneModuleTester::SameConstants_EventContext);
  AP_UNITTEST_REGISTER(SceneModuleTester::ElementTree);
//  AP_UNITTEST_REGISTER(SceneModuleTester::SensorListOps);
}

void SceneModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(SceneModuleTester::Rectangle);
  AP_UNITTEST_EXECUTE(SceneModuleTester::SameConstants_FontFlags);
  AP_UNITTEST_EXECUTE(SceneModuleTester::SameConstants_Operator);
  AP_UNITTEST_EXECUTE(SceneModuleTester::SameConstants_EventContext);
  AP_UNITTEST_EXECUTE(SceneModuleTester::ElementTree);
//  AP_UNITTEST_EXECUTE(SceneModuleTester::SensorListOps);
}

void SceneModuleTester::End()
{
  { Msg_Scene_MouseEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) SceneModuleTester::On_Scene_MouseEvent, 0); }
}

#include "ximagif.h"

String SceneModuleTester::Rectangle()
{
  String s;

  ApHandle hScene = Apollo::newHandle();

  if (!s) {
    Msg_Scene_Create msg;
    msg.hScene = hScene;
    if (!msg.Request()) { s = "Msg_Scene_Create failed"; }
  }

  int nWidth = 350;
  int nHeight = 350;
  if (!s) {
    Msg_Scene_Position msg;
    msg.hScene = hScene;
    msg.nX = 100;
    msg.nY = 700;
    msg.nW = nWidth;
    msg.nH = nHeight;
    if (!msg.Request()) { s = "Msg_Scene_Position failed"; }
  }

  if (!s) {
    Msg_Scene_Visibility msg;
    msg.hScene = hScene;
    msg.bVisible = 1;
    if (!msg.Request()) { s = "Msg_Scene_Visibility failed"; }
  }

  if (!s) {
    Msg_Scene_SetAutoDraw msg;
    msg.hScene = hScene;
    msg.nMilliSec = 50;
    msg.bAsync = 1;
    if (!msg.Request()) { s = "Msg_Scene_SetAutoDraw failed"; }
  }

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "_a_frame", 0, 0, nWidth, nHeight)) { s = "Msg_Scene_CreateRectangle failed"; }}
  //if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "_a_frame", 1, 1, 1, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "_a_frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "_a_frame", 1.5)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "rect1", 0, 0, 10, 10)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetRectangle::_(hScene, "rect1", -50, -50, 100, 100)) { s = "Msg_Scene_SetRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "rect1", 0, 0, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "rect1", 1, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "rect1", 10)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "rect1", 70, 70)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "rect1", 10.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}

  // ------------------------

  //ApHandle hSensor0 = Apollo::newHandle();
  //if (!s) { if (!Msg_Scene_CreateMouseSensor::_(hScene, "z_sensor0", hSensor0, 0, 0, 350, 350)) { s = "Msg_Scene_CreateMouseSensor failed"; }}

  if (!s) { if (!Msg_Scene_CreateMouseSensor::_(hScene, "z_sensor1", 0, 0, 200, 200)) { s = "Msg_Scene_CreateMouseSensor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "z_sensor1", 50, 50)) { s = "Msg_Scene_TranslateElement failed"; }}

  if (!s) { if (!Msg_Scene_CreateMouseSensor::_(hScene, "z_sensor2", 0, 0, 100, 100)) { s = "Msg_Scene_CreateMouseSensor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "z_sensor2", 200, 200)) { s = "Msg_Scene_TranslateElement failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "copymode1", -50, -25, 100, 50)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "copymode1", 1, 1, 1, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "copymode1", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "copymode1", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "copymode1", 210.5, 40.5)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "copymode2", -50, -15, 100, 30)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "copymode2", 0, 1, 0, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "copymode2", 260.5, 40.5)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_ElementCopyMode::_(hScene, "copymode2", Msg_Scene_ElementCopyMode::Source)) { s = "Msg_Scene_ElementCopyMode failed"; }}

  // ------------------------

  for (int i = 0; i < 4; i++) {
    String sPath; sPath.appendf("hide-%d/", i);
    if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, sPath + "1-outer-green", 0, 0, 40, 35)) { s = "Msg_Scene_CreateRectangle failed"; }}
    if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, sPath + "1-outer-green", 0, 1, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
    if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, sPath + "2-inner/1-blue", 5, 5, 30, 20)) { s = "Msg_Scene_CreateRectangle failed"; }}
    if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, sPath + "2-inner/1-blue", 0, 0, 1, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
    if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, sPath + "2-inner/2-red", 10, 10, 20, 20)) { s = "Msg_Scene_CreateRectangle failed"; }}
    if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, sPath + "2-inner/2-red", 1, 0, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
    if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, sPath, 160 + 42 * i, 90)) { s = "Msg_Scene_TranslateElement failed"; }}
    if (i == 1) {
      if (!s) { if (!Msg_Scene_HideElement::_(hScene, sPath + "2-inner", 1)) { s = "Msg_Scene_HideElement failed"; }}
    }
  }

  // ------------------------

  Apollo::Image apImg1; // from PNG
  {
    Buffer sbData;
    Apollo::loadFile(Apollo::getAppResourcePath() + "test/" + "test2.png", sbData);
    CxImage cxImg(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_UNKNOWN);
    apImg1.Allocate(cxImg.GetWidth(), cxImg.GetHeight());
    CxMemFile mfDest((BYTE*) apImg1.Pixels(), apImg1.Size());
    cxImg.AlphaFromTransparency();
    cxImg.Encode2RGBA(&mfDest, true);
  }

  Apollo::Image apImg2; // from animated GIF
  {
    Buffer sbData;
    Apollo::loadFile(Apollo::getAppResourcePath() + "test/tassadar/" + "idle.gif", sbData);
    CxImage cxImg(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_UNKNOWN);
    cxImg.SetRetreiveAllFrames(true);
    int nFrames = cxImg.GetNumFrames();
    cxImg.SetFrame(nFrames - 1);
    cxImg.Decode(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_GIF);
    CxImage* pCxImgFrame = cxImg.GetFrame(0);
    apImg2.Allocate(pCxImgFrame->GetWidth(), pCxImgFrame->GetHeight());
    CxMemFile mfDest((BYTE*) apImg2.Pixels(), apImg2.Size());
    pCxImgFrame->AlphaFromTransparency();
    pCxImgFrame->Encode2RGBA(&mfDest, true);
  }

  double fImage1W, fImage1H;
  if (!s) { if (!Msg_Scene_GetImageSizeFromData::_(hScene, apImg2, fImage1W, fImage1H)) { s = "Msg_Scene_GetImageSizeFromData failed"; }}
  if (!s) { if (fImage1W != 100.0 || fImage1H != 100.0) { s = "Msg_Scene_GetImageSizeFromFile returned wrong size"; }}
  if (!s) { if (!Msg_Scene_CreateImageFromData::_(hScene, "image1", - fImage1W / 2.0, - fImage1H / 2.0, apImg2)) { s = "Msg_Scene_CreateImageFromData failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "image1", 40, 190)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "image1", 30.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "image1", 0.8, 0.8)) { s = "Msg_Scene_ScaleElement failed"; }}

  // ------------------------

  double fImage2W, fImage2H;
  if (!s) { if (!Msg_Scene_GetImageSizeFromData::_(hScene, apImg2, fImage2W, fImage2H)) { s = "Msg_Scene_GetImageSizeFromData failed"; }}
  if (!s) { if (fImage2W != 100.0 || fImage2H != 100.0) { s = "Msg_Scene_GetImageSizeFromFile returned wrong size"; }}
  if (!s) { if (!Msg_Scene_CreateImageFromData::_(hScene, "image2", - fImage2W / 2.0, - fImage2H / 2.0, apImg2)) { s = "Msg_Scene_CreateImageFromData failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "image2", 100, 190)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "image2", 30.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "image2", 0.8, 0.8)) { s = "Msg_Scene_ScaleElement failed"; }}
  if (!s) { if (!Msg_Scene_SetImageAlpha::_(hScene, "image2", 0.5)) { s = "Msg_Scene_SetImageAlpha failed"; }}

  // ------------------------

  double fImage3W, fImage3H;
  if (!s) { if (!Msg_Scene_GetImageSizeFromFile::_(hScene, Apollo::getAppResourcePath() + "test/" + "test2.png", fImage3W, fImage3H)) { s = "Msg_Scene_GetImageSizeFromFile failed"; }}
  if (!s) { if (fImage3W != 100.0 || fImage3H != 100.0) { s = "Msg_Scene_GetImageSizeFromFile returned wrong size"; }}
  if (!s) { if (!Msg_Scene_CreateImageFromFile::_(hScene, "image3", - fImage3W / 2.0, - fImage3H / 2.0, Apollo::getAppResourcePath() + "test/" + "test2.png")) { s = "Msg_Scene_CreateImageFromFile failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "image3", 60, 290)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "image3", - 10.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}

  // ------------------------
  String sTextA = "Hello W" "\xC3\xB6" "rld";

  String sText1 = sTextA;
  double fText1BearingX, fText1BearingY, fText1W, fText1H, fText1AdvanceX, fText1AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::_(hScene, sText1, "Courier New", 30, Msg_Scene_FontFlags::Bold, fText1BearingX, fText1BearingY, fText1W, fText1H, fText1AdvanceX, fText1AdvanceY)) { s = "Msg_Scene_MeasureText failed"; }}
  double fText1Padding = 2.5;
  double fText1X = 130;
  double fText1Y = 140;
  double fText1Size = 30;
  if (!s) { if (!Msg_Scene_CreateText::_(hScene, "text1/text", fText1X, fText1Y, sText1, "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_CreateText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text1/text", 0.5, 0.5, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "text1/frame", fText1X - fText1Padding, fText1Y - fText1Padding, fText1W + 2*fText1Padding, fText1H + 2*fText1Padding)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text1/frame", 1, 1, 0, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "text1/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "text1/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  String sText2 = sTextA;
  double fText2BearingX, fText2BearingY, fText2W, fText2H, fText2AdvanceX, fText2AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::_(hScene, sText2, "Courier New", 30, Msg_Scene_FontFlags::Bold, fText2BearingX, fText2BearingY, fText2W, fText2H, fText2AdvanceX, fText2AdvanceY)) { s = "Msg_Scene_MeasureText failed"; }}
  double fText2Padding = 2.5;
  double fText2X = fText1X;
  double fText2Y = fText1Y + fText1H + fText1Padding + 2;
  double fText2Size = 30;
  double fText2Scale = 0.7;
  if (!s) { if (!Msg_Scene_CreateText::_(hScene, "text2/text", - fText2W / 2, - fText2H / 2, sText2, "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_CreateText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text2/text", 0, 0.5, 0.5, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "text2/frame", - fText2Padding - fText2W / 2, - fText2Padding - fText2H / 2, fText2W + 2*fText2Padding, fText2H + 2*fText2Padding)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text2/frame", 0, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "text2/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "text2/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "text2", fText2X + fText2W / 2, fText2Y + fText2H / 2)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "text2", - fText2Scale, - fText2Scale)) { s = "Msg_Scene_ScaleElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "text2", 180.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}

  // ------------------------

  String sText3 = sTextA;
  double fText3BearingX, fText3BearingY, fText3W, fText3H, fText3AdvanceX, fText3AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::_(hScene, sText3, "Courier New", 30, Msg_Scene_FontFlags::Bold, fText3BearingX, fText3BearingY, fText3W, fText3H, fText3AdvanceX, fText3AdvanceY)) { s = "Msg_Scene_MeasureText failed"; }}
  double fText3Padding = 2.5;
  double fText3X = fText2X;
  double fText3Y = fText2Y + fText2H * fText2Scale + fText2Padding + 4;
  double fText3Size = 30;
  if (!s) { if (!Msg_Scene_CreateElement::_(hScene, "text3")) { s = "Msg_Scene_CreateElement failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "text3", fText3X + fText3W / 2, fText3Y + fText3H / 2)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "text3", 0.7, 0.7)) { s = "Msg_Scene_ScaleElement failed"; }}
  if (!s) { if (!Msg_Scene_CreateText::_(hScene, "text3/text", - fText3W / 2, - fText3H / 2, sText3, "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_CreateText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text3/text", 0.5, 0, 0.5, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "text3/frame", - fText3Padding - fText3W / 2, - fText3Padding - fText3H / 2, fText3W + 2*fText3Padding, fText3H + 2*fText3Padding)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text3/frame", 1, 0, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "text3/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "text3/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "strokeimage", -40, -40, 80, 80)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "strokeimage", 1, 1, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeImageFile::_(hScene, "strokeimage", Apollo::getAppResourcePath() + "test/" + "test2.png")) { s = "Msg_Scene_SetStrokeImageFile failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeImageOffset::_(hScene, "strokeimage", -50, -50)) { s = "Msg_Scene_SetStrokeImageOffset failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "strokeimage", 40)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "strokeimage", 180, 280)) { s = "Msg_Scene_TranslateElement failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "fillimage", -49.5, -49.5, 100, 100)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillImageFile::_(hScene, "fillimage", Apollo::getAppResourcePath() + "test/" + "test2.png")) { s = "Msg_Scene_SetFillImageFile failed"; }}
  if (!s) { if (!Msg_Scene_SetFillImageOffset::_(hScene, "fillimage", -50, -50)) { s = "Msg_Scene_SetFillImageOffset failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "fillimage", 1, 1, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "fillimage", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "fillimage", 285, 280)) { s = "Msg_Scene_TranslateElement failed"; }}

  // ------------------------

  //if (!s) {
  //  Msg_Scene_Draw msg;
  //  msg.hScene = hScene;
  //  if (!msg.Request()) { s = "Msg_Scene_Draw failed"; }
  //}

  // ------------------------

  //if (!s) {
  //  Msg_Scene_Position msg;
  //  msg.hScene = hScene;
  //  msg.nX = 100;
  //  msg.nY = 400;
  //  msg.nW = 250;
  //  msg.nH = 250;
  //  if (!msg.Request()) { s = "Msg_Scene_Position failed"; }
  //}

  if (!s) {
    Msg_Scene_AutoDraw msg;
    msg.hScene = hScene;
    if (!msg.Request()) { s = "Msg_Scene_AutoDraw failed"; }
  }

    //if (!s) {
  //  Msg_Scene_Draw msg;
  //  msg.hScene = hScene;
  //  if (!msg.Request()) { s = "Msg_Scene_Draw failed"; }
  //}
  //if (!s) {
  //  Msg_Scene_Draw msg;
  //  msg.hScene = hScene;
  //  if (!msg.Request()) { s = "Msg_Scene_Draw failed"; }
  //}

  if (0) {
    Msg_Scene_Destroy msg;
    msg.hScene = hScene;
    if (!msg.Request()) { s = "Msg_Scene_Destroy failed"; }
  }

  return s;
}

String SceneModuleTester::SameConstants_FontFlags()
{
  String s;

  if (!s) { if (Msg_Scene_FontFlags::Italic != TextX::Italic    ) { s = "Msg_Scene_FontFlags::Italic != TextX::Italic"; }}
  if (!s) { if (Msg_Scene_FontFlags::Bold != TextX::Bold        ) { s = "Msg_Scene_FontFlags::Bold != TextX::Bold"; }}
  if (!s) { if (Msg_Scene_FontFlags::LastFlag != TextX::LastFlag) { s = "Msg_Scene_FontFlags::LastFlag != TextX::LastFlag"; }}
 
  return s;
}

String SceneModuleTester::SameConstants_Operator()
{
  String s;

  if (!s) { if (Msg_Scene_ElementCopyMode::Clear != CAIRO_OPERATOR_CLEAR       ) { s = "Msg_Scene_ElementCopyMode::Clear != CAIRO_OPERATOR_CLEAR"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Source != CAIRO_OPERATOR_SOURCE     ) { s = "Msg_Scene_ElementCopyMode::Source != CAIRO_OPERATOR_SOURCE"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Over != CAIRO_OPERATOR_OVER         ) { s = "Msg_Scene_ElementCopyMode::Over != CAIRO_OPERATOR_OVER"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::In != CAIRO_OPERATOR_IN             ) { s = "Msg_Scene_ElementCopyMode::In != CAIRO_OPERATOR_IN"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Out != CAIRO_OPERATOR_OUT           ) { s = "Msg_Scene_ElementCopyMode::Out != CAIRO_OPERATOR_OUT"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Atop != CAIRO_OPERATOR_ATOP         ) { s = "Msg_Scene_ElementCopyMode::Atop != CAIRO_OPERATOR_ATOP"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Dest != CAIRO_OPERATOR_DEST         ) { s = "Msg_Scene_ElementCopyMode::Dest != CAIRO_OPERATOR_DEST"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::DestOver != CAIRO_OPERATOR_DEST_OVER) { s = "Msg_Scene_ElementCopyMode::DestOver != CAIRO_OPERATOR_DEST_OVER"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::DestIn != CAIRO_OPERATOR_DEST_IN    ) { s = "Msg_Scene_ElementCopyMode::DestIn != CAIRO_OPERATOR_DEST_IN"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::DestOut != CAIRO_OPERATOR_DEST_OUT  ) { s = "Msg_Scene_ElementCopyMode::DestOut != CAIRO_OPERATOR_DEST_OUT"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::DestAtop != CAIRO_OPERATOR_DEST_ATOP) { s = "Msg_Scene_ElementCopyMode::DestAtop != CAIRO_OPERATOR_DEST_ATOP"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Xor != CAIRO_OPERATOR_XOR           ) { s = "Msg_Scene_ElementCopyMode::Xor != CAIRO_OPERATOR_XOR"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Add != CAIRO_OPERATOR_ADD           ) { s = "Msg_Scene_ElementCopyMode::Add != CAIRO_OPERATOR_ADD"; }}
  if (!s) { if (Msg_Scene_ElementCopyMode::Saturate != CAIRO_OPERATOR_SATURATE ) { s = "Msg_Scene_ElementCopyMode::Saturate != CAIRO_OPERATOR_SATURATE"; }}
 
  return s;
}

String SceneModuleTester::SameConstants_EventContext()
{
  String s;

  if (!s) { if (Msg_Scene_MouseEvent::MouseMove != EventContext::MouseMove              ) { s = "Msg_Scene_MouseEvent::MouseMove != EventContext::MouseMove              "; }}
  if (!s) { if (Msg_Scene_MouseEvent::MouseDown != EventContext::MouseDown              ) { s = "Msg_Scene_MouseEvent::MouseDown != EventContext::MouseDown              "; }}
  if (!s) { if (Msg_Scene_MouseEvent::MouseUp != EventContext::MouseUp                  ) { s = "Msg_Scene_MouseEvent::MouseUp != EventContext::MouseUp                  "; }}
  if (!s) { if (Msg_Scene_MouseEvent::MouseClick != EventContext::MouseClick            ) { s = "Msg_Scene_MouseEvent::MouseClick != EventContext::MouseClick            "; }}
  if (!s) { if (Msg_Scene_MouseEvent::MouseDoubleClick != EventContext::MouseDoubleClick) { s = "Msg_Scene_MouseEvent::MouseDoubleClick != EventContext::MouseDoubleClick"; }}

  if (!s) { if (Msg_Scene_MouseEvent::LeftButton != EventContext::LeftButton    ) { s = "Msg_Scene_MouseEvent::LeftButton != EventContext::LeftButton    "; }}
  if (!s) { if (Msg_Scene_MouseEvent::MiddleButton != EventContext::MiddleButton) { s = "Msg_Scene_MouseEvent::MiddleButton != EventContext::MiddleButton"; }}
  if (!s) { if (Msg_Scene_MouseEvent::RightButton != EventContext::RightButton  ) { s = "Msg_Scene_MouseEvent::RightButton != EventContext::RightButton  "; }}

  return s;
}

String SceneModuleTester::CheckChildren(Element* pNode, const String& sExpectedChildren)
{
  String s;

  Apollo::StringList lExpectedChildren(sExpectedChildren);

  if (!s) {
    if (pNode->pChildren_) {
      if (pNode->pChildren_->Count() != lExpectedChildren.length()) {
        s.appendf("Expected: %s, unequal length: expected=%d, got=%d", StringType(sExpectedChildren), lExpectedChildren.length(), pNode->pChildren_->Count());
      }
    } else {
      if (lExpectedChildren.length() > 0) {
        s.appendf("Expected: %s, unequal length: expected=%d, got=0", StringType(sExpectedChildren), lExpectedChildren.length());
      }
    }
  }
  
  if (!s) {
    if (pNode->pChildren_) {
      int nCnt = 0;
      ElementIterator iter(*pNode->pChildren_);
      for (Elem* e = 0; (e = lExpectedChildren.Next(e)) != 0; ) {
        nCnt++;
        ElementNode* pElementNode = iter.Next();
        if (pElementNode) {
          if (e->getName() != pElementNode->Key()) {
            s.appendf("Position %d expected=%s, got=%s", nCnt, StringType(e->getName()), StringType(pElementNode->Key()));
            break;
          }
        }
      }
    }
  }

  return s;
}

String SceneModuleTester::ElementTree()
{
  String s;

  Element elem(0);

  if (!s) { if (!elem.CreateElement("a")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a"); }

  if (!s) { if (!elem.CreateElement("a/b/c")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value()->pChildren_->Find("c")->Value(), ""); }

  if (!s) { if (!elem.CreateElement("a/b/d")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c d"); }

  if (!s) { if (!elem.CreateElement("a/e/c")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("e")->Value(), "c"); }

  if (!s) { if (!elem.CreateElement("a/e/d")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("e")->Value(), "c d"); }

  if (!s) { if (!elem.CreateElement("f/b/c")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a f"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("e")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value(), "b"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value()->pChildren_->Find("b")->Value(), "c"); }

  if (!s) { if (!elem.CreateElement("a/b/c/d")) { s = "CreateElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a f"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("e")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value(), "b"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value()->pChildren_->Find("b")->Value(), "c"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value()->pChildren_->Find("c")->Value(), "d"); }

  if (!s) { if (!elem.DeleteElement("a/b/c/d")) { s = "DeleteElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a f"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("e")->Value(), "c d"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value(), "b"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value()->pChildren_->Find("b")->Value(), "c"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value()->pChildren_->Find("b")->Value()->pChildren_->Find("c")->Value(), ""); }

  if (!s) { if (!elem.DeleteElement("a/b")) { s = "DeleteElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a f"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), "e"); }

  if (!s) { if (!elem.DeleteElement("a/e")) { s = "DeleteElement failed"; } }
  if (!s) { s = CheckChildren(&elem, "a f"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("a")->Value(), ""); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value(), "b"); }
  if (!s) { s = CheckChildren(elem.pChildren_->Find("f")->Value()->pChildren_->Find("b")->Value(), "c"); }

  return s;
}

//String SceneModuleTester::SensorListOps()
//{
//  String s;
//
//  SensorList l;
//
//  ApHandle h1 = Apollo::newHandle();
//  SensorBox b1(h1, 10,20,30,40);
//  Element* e1 = new Element(0);
//  l.Set(b1, e1);
//
//  ApHandle h2 = Apollo::newHandle();
//  SensorBox b2(h2, 50,60,70,80);
//  Element* e2 = new Element(0);
//  l.Set(b2, e2);
//
//  ApHandle h3 = Apollo::newHandle();
//  SensorBox b3(h3, 20,30,40,50);
//  Element* e3 = new Element(0);
//  l.Set(b3, e3);
//
//  if (!s) {
//    if (l.Count() != 3) {
//      s = "expected 3 elements";
//    }
//  }
//
//  if (!s) {
//    SensorListNode* n = l.FindByHandle(h2);
//    if (n == 0) {
//      s = "Did not find h2";
//    } else {
//      if (n->Key().nRight_ != 70) {
//        s = "Found h2, but wrong element";
//      }
//    }
//  }
//
//  SensorListNode* m = 0;
//  if (!s) {
//    m = l.Next(m);
//    if (m->Key().nRight_ != 30) { s = "Expected 30 for element 1"; }
//    m = l.Next(m);
//    if (m->Key().nRight_ != 70) { s = "Expected 70 for element 2"; }
//    m = l.Next(m);
//    if (m->Key().nRight_ != 40) { s = "Expected 40 for element 3"; }
//  }
//
//  l.Unset(b1); delete e1;
//  l.Unset(b2); delete e2;
//  l.Unset(b3); delete e3;
//
//  return s;
//}

#endif // #if defined(AP_TEST)
