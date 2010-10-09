// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "ApTypes.h"
#include "MsgScene.h"
#include "SceneModuleTester.h"
#include "Element.h"

#if defined(AP_TEST)

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
    msg.nY = 400;
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

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "frame", 0, 0, nWidth, nHeight)) { s = "Msg_Scene_CreateRectangle failed"; }}
  //if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "frame", 1.5)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "rect1", -50, -50, 100, 100)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "rect1", 0, 0, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "rect1", 1, 0, 0, 0.5)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "rect1", 10)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "rect1", 80, 80)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "rect1", 45.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}

  // ------------------------

  for (int i = 0; i < 5; i++) {
    String sPath; sPath.appendf("rect2-%d", i);
    if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, sPath + "/1outer", -15, -15, 30, 30)) { s = "Msg_Scene_CreateRectangle failed"; }}
    if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, sPath + "/1outer", 0, 0, 1, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
    if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, sPath + "/2inner", -10, -10, 20, 20)) { s = "Msg_Scene_CreateRectangle failed"; }}
    if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, sPath + "/2inner", 1, 0, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
    if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, sPath, 180 + 35 * i, 80)) { s = "Msg_Scene_TranslateElement failed"; }}
    if (i == 3) {
      if (!s) { if (!Msg_Scene_HideElement::_(hScene, sPath, 1)) { s = "Msg_Scene_HideElement failed"; }}
    }
  }

  // ------------------------

  Apollo::Image apImg1; // from PNG
  {
    Buffer sbData;
    Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "test.png", sbData);
    CxImage cxImg(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_UNKNOWN);
    apImg1.Allocate(cxImg.GetWidth(), cxImg.GetHeight());
    CxMemFile mfDest((BYTE*) apImg1.Pixels(), apImg1.Size());
    cxImg.AlphaFromTransparency();
    cxImg.Encode2RGBA(&mfDest, true);
  }

  Apollo::Image apImg2; // from animated GIF
  {
    Buffer sbData;
    Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "idle.gif", sbData);
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
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "image1", 80, 190)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "image1", 45.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "image1", 0.8, 0.8)) { s = "Msg_Scene_ScaleElement failed"; }}

  // ------------------------

  double fImage2W, fImage2H;
  if (!s) { if (!Msg_Scene_GetImageSizeFromFile::_(hScene, Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "test.png", fImage2W, fImage2H)) { s = "Msg_Scene_GetImageSizeFromFile failed"; }}
  if (!s) { if (fImage2W != 100.0 || fImage2H != 100.0) { s = "Msg_Scene_GetImageSizeFromFile returned wrong size"; }}
  if (!s) { if (!Msg_Scene_CreateImageFromFile::_(hScene, "image2", - fImage2W / 2.0, - fImage2H / 2.0, Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "test.png")) { s = "Msg_Scene_CreateImageFromFile failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "image2", 80, 290)) { s = "Msg_Scene_RotateElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "image2", - 10.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}

  // ------------------------

  String sText1 = "Hello W" "\xC3\xB6" "rld";
  double fText1BearingX, fText1BearingY, fText1Width, fText1Height, fText1AdvanceX, fText1AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::_(hScene, sText1, "Courier New", 30, Msg_Scene_FontFlags::Bold, fText1BearingX, fText1BearingY, fText1Width, fText1Height, fText1AdvanceX, fText1AdvanceY)) { s = "Msg_Scene_MeasureText failed"; }}
  double fText1Padding = 2.5;
  double fText1X = 130;
  double fText1Y = 150;
  double fText1Size = 30;
  if (!s) { if (!Msg_Scene_CreateText::_(hScene, "text1/text", fText1X, fText1Y, sText1, "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_CreateText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text1/text", 0.5, 0.5, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "text1/frame", fText1X - fText1Padding, fText1Y - fText1Height - fText1Padding, fText1Width + 2*fText1Padding, fText1Height + 2*fText1Padding)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text1/frame", 1, 1, 0, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "text1/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "text1/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  String sText2 = "Hello W" "\xC3\xB6" "rld";
  double fText2BearingX, fText2BearingY, fText2Width, fText2Height, fText2AdvanceX, fText2AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::_(hScene, sText2, "Courier New", 30, Msg_Scene_FontFlags::Bold, fText2BearingX, fText2BearingY, fText2Width, fText2Height, fText2AdvanceX, fText2AdvanceY)) { s = "Msg_Scene_MeasureText failed"; }}
  double fText2Padding = 2.5;
  double fText2X = 130.0 + fText2Width / 2;
  double fText2Y = 167.0 + fText2Height / 2;
  double fText2Size = 30;
  if (!s) { if (!Msg_Scene_CreateText::_(hScene, "text2/text", - fText2Width / 2, - fText2Height / 2, sText2, "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_CreateText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text2/text", 0, 0.5, 0.5, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "text2/frame", - fText2Padding - fText2Width / 2, - fText2Height - fText2Padding - fText2Height / 2, fText2Width + 2*fText2Padding, fText2Height + 2*fText2Padding)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text2/frame", 0, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "text2/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "text2/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "text2", fText2X, fText2Y)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "text2", -0.7, -0.7)) { s = "Msg_Scene_ScaleElement failed"; }}
  if (!s) { if (!Msg_Scene_RotateElement::_(hScene, "text2", 180.0 / 180.0 * 3.1415)) { s = "Msg_Scene_RotateElement failed"; }}

  // ------------------------

  String sText3 = "Hello W" "\xC3\xB6" "rld";
  double fText3BearingX, fText3BearingY, fText3Width, fText3Height, fText3AdvanceX, fText3AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::_(hScene, sText3, "Courier New", 30, Msg_Scene_FontFlags::Bold, fText3BearingX, fText3BearingY, fText3Width, fText3Height, fText3AdvanceX, fText3AdvanceY)) { s = "Msg_Scene_MeasureText failed"; }}
  double fText3Padding = 2.5;
  double fText3X = 130.0 + fText3Width / 2;
  double fText3Y = 186.0 + fText3Height / 2;
  double fText3Size = 30;
  if (!s) { if (!Msg_Scene_CreateElement::_(hScene, "text3")) { s = "Msg_Scene_CreateElement failed"; }}
  if (!s) { if (!Msg_Scene_TranslateElement::_(hScene, "text3", fText3X, fText3Y)) { s = "Msg_Scene_TranslateElement failed"; }}
  if (!s) { if (!Msg_Scene_ScaleElement::_(hScene, "text3", 0.7, 0.7)) { s = "Msg_Scene_ScaleElement failed"; }}
  if (!s) { if (!Msg_Scene_CreateText::_(hScene, "text3/text", - fText3Width / 2, - fText3Height / 2, sText3, "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_CreateText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text3/text", 0.5, 0, 0.5, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_CreateRectangle::_(hScene, "text3/frame", - fText3Padding - fText3Width / 2, - fText3Height - fText3Padding - fText3Height / 2, fText3Width + 2*fText3Padding, fText3Height + 2*fText3Padding)) { s = "Msg_Scene_CreateRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "text3/frame", 1, 0, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::_(hScene, "text3/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::_(hScene, "text3/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) {
    Msg_Scene_SetAutoDraw msg;
    msg.hScene = hScene;
    msg.nMilliSec = 100;
    msg.bAsync = 1;
    if (!msg.Request()) { s = "Msg_Scene_SetAutoDraw failed"; }
  }
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::_(hScene, "frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}

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

String SceneModuleTester::FontFlags()
{
  String s;

  if (!s) { if (Msg_Scene_FontFlags::Italic != TextX::Italic) { s = "Msg_Scene_FontFlags::Italic != TextX::Italic"; }}
  if (!s) { if (Msg_Scene_FontFlags::Bold != TextX::Bold) { s = "Msg_Scene_FontFlags::Bold != TextX::Bold"; }}
  if (!s) { if (Msg_Scene_FontFlags::LastFlag != TextX::LastFlag) { s = "Msg_Scene_FontFlags::LastFlag != TextX::LastFlag"; }}
 
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

  Element elem;

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

#endif // #if defined(AP_TEST)
