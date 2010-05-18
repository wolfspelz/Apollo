// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "SampleModule.h"

// Replace regex: 
// \(Msg_[a-zA-Z]+_[^ *]+\* pMsg\)

AP_MSG_HANDLER_METHOD(SampleModule, Sample_Get)
{
  String sSomeConfigValue;
  Msg_Config_GetValue msg;
  msg.sPath = "path";
  msg.Request();
  sSomeConfigValue = msg.sValue;

  sSomeConfigValue = Apollo::getConfig("path", "default");

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

int SampleModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SampleModule, Sample_Get, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(SampleModule, this);

  return ok;
}

void SampleModule::exit()
{
  AP_UNITTEST_UNHOOK(SampleModule, this);
  AP_MSG_REGISTRY_FINISH;
}
