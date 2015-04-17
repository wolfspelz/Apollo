// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ArenaModuleTester_H_INCLUDED)
#define ArenaModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"
#include "MsgWebView.h"

#if defined(AP_TEST)
class ArenaModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Dev();
};
#endif

#endif // ArenaModuleTester_H_INCLUDED
