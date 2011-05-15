// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

// Replace regex: 
// \(Msg_[a-zA-Z]+_[^ *]+\* pMsg\)

// This only for the "Msg_Config_GetValue" sending message example below
#include "MsgConfig.h"

AP_MSG_HANDLER_METHOD(SampleModule, Sample_Get)
{
  // Example of requesting a value by sending a message
  String sSomeConfigValue;
  Msg_Config_GetValue msg;
  msg.sPath = "path";
  msg.Request();
  sSomeConfigValue = msg.sValue;

  // Example of using the Apollo wrapper method for the same as above
  sSomeConfigValue = Apollo::getConfig("path", "default");

  // Logging example
  apLog_Info((MODULE_NAME, "Context is the current function", "Format string %d", nTheAnswer_));

  pMsg->nValue = nTheAnswer_;
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(SampleModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Sample", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(SampleModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Sample", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(SampleModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int SampleModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SampleModule, Sample_Get, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(SampleModule, this);

  return ok;
}

void SampleModule::Exit()
{
  AP_UNITTEST_UNHOOK(SampleModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

void SampleModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(SampleModuleTester::Test1);
}

void SampleModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(SampleModuleTester::Test1);
}

void SampleModuleTester::End()
{
}

//----------------------------

String SampleModuleTester::Test1()
{
  String s;

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
