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
  int nHeight = 300;
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

  if (!s) { if (!Msg_Scene_CreateElement::Invoke(hScene, "frame")) { s = "Msg_Scene_CreateElement failed"; }}
  if (!s) { if (!Msg_Scene_SetRectangle::Invoke(hScene, "frame", 0, 0, nWidth, nHeight)) { s = "Msg_Scene_SetRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::Invoke(hScene, "frame", 1, 1, 1, 0.5)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::Invoke(hScene, "frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::Invoke(hScene, "frame", 1.5)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

// ------------------------

  if (!s) { if (!Msg_Scene_CreateElement::Invoke(hScene, "rect1")) { s = "Msg_Scene_CreateElement failed"; }}
  if (!s) { if (!Msg_Scene_SetRectangle::Invoke(hScene, "rect1", 10, 10, 100, 100)) { s = "Msg_Scene_SetRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::Invoke(hScene, "rect1", 0, 0, 1, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::Invoke(hScene, "rect1", 1, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::Invoke(hScene, "rect1", 10)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

// ------------------------

  double fText1X = 110;
  double fText1Y = 110;
  double fText1Size = 30;
  if (!s) { if (!Msg_Scene_CreateElement::Invoke(hScene, "text1/text")) { s = "Msg_Scene_CreateElement failed"; }}
  if (!s) { if (!Msg_Scene_SetText::Invoke(hScene, "text1/text", fText1X, fText1Y, "Hello W" "\xC3\xB6" "rld", "Courier New", 30, Msg_Scene_FontFlags::Bold)) { s = "Msg_Scene_SetText failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::Invoke(hScene, "text1/text", 0.7, 0.7, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}

  // ------------------------

  double fText1BearingX;
  double fText1BearingY;
  double fText1Width;
  double fText1Height;
  double fText1AdvanceX;
  double fText1AdvanceY;
  if (!s) { if (!Msg_Scene_MeasureText::Invoke(hScene, "Hello W" "\xC3\xB6" "rld", "Courier New", 30, Msg_Scene_FontFlags::Bold, fText1BearingX, fText1BearingY, fText1Width, fText1Height, fText1AdvanceX, fText1AdvanceY)) { s = "Msg_Scene_MeasureText failed"); }}
  if (!s) { if (!Msg_Scene_CreateElement::Invoke(hScene, "text1/frame")) { s = "Msg_Scene_CreateElement failed"; }}
  double fBorder = 3;
  if (!s) { if (!Msg_Scene_SetRectangle::Invoke(hScene, "text1/frame", fText1X - fBorder, fText1Y - fBorder, fText1Width + 2*fBorder, fText1Height + 2*fBorder,)) { s = "Msg_Scene_SetRectangle failed"; }}
  if (!s) { if (!Msg_Scene_SetFillColor::Invoke(hScene, "text1/frame", 1, 1, 0, 1)) { s = "Msg_Scene_SetFillColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeColor::Invoke(hScene, "text1/frame", 0, 0, 0, 1)) { s = "Msg_Scene_SetStrokeColor failed"; }}
  if (!s) { if (!Msg_Scene_SetStrokeWidth::Invoke(hScene, "text1/frame", 1)) { s = "Msg_Scene_SetStrokeWidth failed"; }}

  // ------------------------

  if (!s) {
    Msg_Scene_Draw msg;
    msg.hScene = hScene;
    if (!msg.Request()) { s = "Msg_Scene_Draw failed"; }
  }

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
