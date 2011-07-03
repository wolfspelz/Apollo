// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(SetupModule, Setup_Get)
{
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
} 

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int SetupModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Setup_Get, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(SetupModule, this);

  return ok;
}

void SetupModule::Exit()
{
  AP_UNITTEST_UNHOOK(SetupModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

void SetupModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(SetupModuleTester::Test1);
}

void SetupModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(SetupModuleTester::Test1);
}

void SetupModuleTester::End()
{
}

//----------------------------

String SetupModuleTester::Test1()
{
  String s;

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
