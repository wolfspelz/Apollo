// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SceneModuleTester_H_INCLUDED)
#define SceneModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"
#include "MsgScene.h"

class Element;

#if defined(AP_TEST)
class SceneModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static void On_Scene_MouseEvent(Msg_Scene_MouseEvent* pMsg);

  static String CheckChildrenByInspection(Element* pNode, const String& sExpectedChildren);
  static String CheckChildrenByApi(Element* pNode, const String& sExpectedChildren);

  static String Scene();
  static String Primitives();
  static String SameConstants_FontFlags();
  static String SameConstants_Operator();
  static String SameConstants_EventContext();
  static String ElementTree();
  static String GetChildren();

  static int bHasCursor_;
  static ApHandle hScene_;
};
#endif

#endif // SceneModuleTester_H_INCLUDED
