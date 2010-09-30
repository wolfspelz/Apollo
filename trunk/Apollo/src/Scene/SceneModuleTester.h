// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SceneModuleTester_H_INCLUDED)
#define SceneModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"

class Element;

#if defined(AP_TEST)
class SceneModuleTester
{
public:
  static String CheckChildren(Element* pNode, const String& sExpectedChildren);
  static String Rectangle();
  static String FontFlags();
  static String ElementTree();
};
#endif

#endif // SceneModuleTester_H_INCLUDED
