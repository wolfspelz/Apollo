// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ScWidgetModuleTester_H_INCLUDED)
#define ScWidgetModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"
#include "MsgScWidget.h"

class Element;

#if defined(AP_TEST)
class ScWidgetModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static void On_ScWidget_ButtonEvent(Msg_ScWidget_ButtonEvent* pMsg);

  static String Button();
  static String Edit();

  static ApHandle hButtonScene_;
  static ApHandle hEditScene_;
};
#endif

#endif // ScWidgetModuleTester_H_INCLUDED
