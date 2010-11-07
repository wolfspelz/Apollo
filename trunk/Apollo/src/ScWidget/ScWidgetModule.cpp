// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "ScWidgetModule.h"

//---------------------------

//AP_MSG_HANDLER_METHOD(ScWidgetModule, ScWidget_Create)
//{
//  pMsg->apStatus = ApMessage::Ok;
//}

//----------------------------------------------------------

#if defined(AP_TEST)

#include "ScWidgetModuleTester.h"

AP_MSG_HANDLER_METHOD(ScWidgetModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ScWidget", 0)) {
    ScWidgetModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ScWidget", 0)) {
    ScWidgetModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(ScWidgetModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ScWidget", 0)) {
    ScWidgetModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int ScWidgetModule::Init()
{
  int ok = 1;

  //AP_MSG_REGISTRY_ADD(MODULE_NAME, ScWidgetModule, ScWidget_Create, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(ScWidgetModule, this);

  return ok;
}

void ScWidgetModule::Exit()
{
  AP_UNITTEST_UNHOOK(ScWidgetModule, this);
  AP_MSG_REGISTRY_FINISH;
}
