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
  ApHandle hScene = Apollo::newHandle();

  {
    Msg_Scene_Create msg;
    msg.hScene = hScene;
    msg.Request();
  }

  int nWidth = 300;
  int nHeight = 300;
  {
    Msg_Scene_Position msg;
    msg.hScene = hScene;
    msg.nX = 100;
    msg.nY = 400;
    msg.nW = nWidth;
    msg.nH = nHeight;
    msg.Request();
  }

  {
    Msg_Scene_Visibility msg;
    msg.hScene = hScene;
    msg.bVisible = 1;
    msg.Request();
  }

  {
    Msg_Scene_Rectangle msg;
    msg.hScene = hScene;
    msg.sPath = "/frame";
    msg.fX = 0;
    msg.fY = 0;
    msg.fW = nWidth;
    msg.fH = nHeight;
    msg.Request();
  }

  {
    Msg_Scene_SetFillColor msg;
    msg.hScene = hScene;
    msg.sPath = "/frame";
    msg.fRed = 1;
    msg.fGreen = 1;
    msg.fBlue = 1;
    msg.fAlpha = 0.5;
    msg.Request();
  }

  {
    Msg_Scene_SetStrokeColor msg;
    msg.hScene = hScene;
    msg.sPath = "/frame";
    msg.fWidth = 1.5;
    msg.fRed = 0;
    msg.fGreen = 0;
    msg.fBlue = 0;
    msg.fAlpha = 1;
    msg.Request();
  }
  {
    Msg_Scene_Rectangle msg;
    msg.hScene = hScene;
    msg.sPath = "/rect1";
    msg.fX = 10;
    msg.fY = 10;
    msg.fW = 100;
    msg.fH = 100;
    msg.Request();
  }

  {
    Msg_Scene_SetFillColor msg;
    msg.hScene = hScene;
    msg.sPath = "rect1";
    msg.fRed = 0;
    msg.fGreen = 0;
    msg.fBlue = 1;
    msg.fAlpha = 1;
    msg.Request();
  }

  {
    Msg_Scene_SetStrokeColor msg;
    msg.hScene = hScene;
    msg.sPath = "/rect1";
    msg.fWidth = 10;
    msg.fRed = 1;
    msg.fGreen = 0;
    msg.fBlue = 0;
    msg.fAlpha = 1;
    msg.Request();
  }

  //{
  //  Msg_Scene_DeleteElement msg;
  //  msg.hScene = hScene;
  //  msg.sPath = "/rect1";
  //  msg.Request();
  //}

  {
    Msg_Scene_Draw msg;
    msg.hScene = hScene;
    msg.Request();
  }

  if (0) {
    Msg_Scene_Destroy msg;
    msg.hScene = hScene;
    msg.Request();
  }

  return "";
}

String SceneModuleTester::CheckChildren(Element* pNode, const String& sChildren)
{
  String s;

  Apollo::StringList lChildren(sChildren);

  if (!s) { if (pNode->list_.Count() != lChildren.length()) { s.appendf("Expected: %s, unequal length: expected=%d, got=%d", StringType(sChildren), lChildren.length(), pNode->list_.Count()); }}
  if (!s) {
    int nCnt = 0;
    ElementIterator iter(pNode->list_);
    for (Elem* e = 0; (e = lChildren.Next(e)) != 0; ) {
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

  return s;
}

String SceneModuleTester::ElementTree()
{
  String s;

  Node n;

  if (!s) { n.AddElement("a", new RectangleX(1, 0, 0, 0)); }
  if (!s) { s = CheckChildren(&n, "a"); }

  if (!s) { n.AddElement("a/b/c", new RectangleX(1, 2, 3, 0)); }
  if (!s) { s = CheckChildren(&n, "a"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value()->list_.Find("c")->Value(), ""); }

  if (!s) { n.AddElement("a/b/d", new RectangleX(1, 2, 4, 0)); }
  if (!s) { s = CheckChildren(&n, "a"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c d"); }

  if (!s) { n.AddElement("a/e/c", new RectangleX(1, 5, 3, 0)); }
  if (!s) { s = CheckChildren(&n, "a"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("e")->Value(), "c"); }

  if (!s) { n.AddElement("a/e/d", new RectangleX(1, 5, 4, 0)); }
  if (!s) { s = CheckChildren(&n, "a"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("e")->Value(), "c d"); }

  if (!s) { n.AddElement("f/b/c", new RectangleX(6, 2, 3, 0)); }
  if (!s) { s = CheckChildren(&n, "a f"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("e")->Value(), "c d"); }
  if (!s) { s = CheckChildren(n.list_.Find("f")->Value(), "b"); }
  if (!s) { s = CheckChildren(n.list_.Find("f")->Value()->list_.Find("b")->Value(), "c"); }

  if (!s) { n.AddElement("a/b/c/d", new RectangleX(1, 2, 3, 4)); }
  if (!s) { s = CheckChildren(&n, "a f"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b e"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c d"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("e")->Value(), "c d"); }
  if (!s) { s = CheckChildren(n.list_.Find("f")->Value(), "b"); }
  if (!s) { s = CheckChildren(n.list_.Find("f")->Value()->list_.Find("b")->Value(), "c"); }
  if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value()->list_.Find("c")->Value(), "d"); }

  //if (!s) { n.RemoveElement("a/b/c/d", new RectangleX(1, 2, 3, 4)); }
  //if (!s) { s = CheckChildren(&n, "a f"); }
  //if (!s) { s = CheckChildren(n.list_.Find("a")->Value(), "b e"); }
  //if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value(), "c d"); }
  //if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("e")->Value(), "c d"); }
  //if (!s) { s = CheckChildren(n.list_.Find("f")->Value(), "b"); }
  //if (!s) { s = CheckChildren(n.list_.Find("f")->Value()->list_.Find("b")->Value(), "c"); }
  //if (!s) { s = CheckChildren(n.list_.Find("a")->Value()->list_.Find("b")->Value()->list_.Find("c")->Value(), ""); }

  return s;
}

#endif // #if defined(AP_TEST)
