// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(QWebViewModuleTester_H_INCLUDED)
#define QWebViewModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"

#if defined(AP_TEST)
class QWebViewModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Dev();
};
#endif

#endif // QWebViewModuleTester_H_INCLUDED
