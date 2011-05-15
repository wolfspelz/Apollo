// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_XX)
{
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(DialogModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dialog", 0)) {
    DialogModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dialog", 0)) {
    DialogModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dialog", 0)) {
    DialogModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int DialogModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_XX, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(DialogModule, this);

  return ok;
}

void DialogModule::Exit()
{
  AP_UNITTEST_UNHOOK(DialogModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

void DialogModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(DialogModuleTester::Test1);
}

void DialogModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(DialogModuleTester::Test1);
}

void DialogModuleTester::End()
{
}

//----------------------------

String DialogModuleTester::Test1()
{
  String s;

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
