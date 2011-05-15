// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

// Ignore this:
// Replace regex: \(Msg_[a-zA-Z]+_[^ *]+\* pMsg\)

Sample* SampleModule::NewSample(const ApHandle& hSample)
{
  Sample* pSample = new Sample(hSample);
  if (pSample) {
    try {
      pSample->Create();
      samples_.Set(hSample, pSample);
    } catch (ApException& ex) {
      delete pSample;
      pSample = 0;
      throw ex;
    }
  }

  return pSample;
}

void SampleModule::DeleteSample(const ApHandle& hSample)
{
  Sample* pSample = FindSample(hSample);
  if (pSample) {
    pSample->Destroy();
    samples_.Unset(hSample);
    delete pSample;
    pSample = 0;
  }
}

Sample* SampleModule::FindSample(const ApHandle& hSample)
{
  Sample* pSample = 0;  

  samples_.Get(hSample, pSample);
  if (pSample == 0) { throw ApException("SampleModule::FindSample no Sample=" ApHandleFormat "", ApHandleType(hSample)); }

  return pSample;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(SampleModule, Sample_Create)
{
  if (samples_.Find(pMsg->hSample) != 0) { throw ApException("SampleModule::Sample_Create: Sample=" ApHandleFormat " already exists", ApHandleType(pMsg->hSample)); }
  Sample* pSample = NewSample(pMsg->hSample);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SampleModule, Sample_Destroy)
{
  Sample* pSample = FindSample(pMsg->hSample);
  DeleteSample(pMsg->hSample);
  pMsg->apStatus = ApMessage::Ok;
}

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

  String sKey = "TestKeyUsedByLoadModuleTest";
  if (pMsg->sKey == sKey) {
    pMsg->nValue = nTheAnswer_;
  } else {
    throw ApException("Only have an answer for key=%s", StringType(sKey));
  }

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

void SrpcGate_Sample_Create(ApSRPCMessage* pMsg)
{
  Msg_Sample_Create msg;
  msg.hSample = Apollo::string2Handle(pMsg->srpc.getString("hSample"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Sample_Destroy(ApSRPCMessage* pMsg)
{
  Msg_Sample_Destroy msg;
  msg.hSample = Apollo::string2Handle(pMsg->srpc.getString("hSample"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Sample_Get(ApSRPCMessage* pMsg)
{
  Msg_Sample_Get msg;
  msg.hSample = Apollo::string2Handle(pMsg->srpc.getString("hSample"));
  msg.sKey = pMsg->srpc.getString("sKey");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->srpc.setInt("nValue", msg.nValue);
}

//----------------------------------------------------------

int SampleModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SampleModule, Sample_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SampleModule, Sample_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SampleModule, Sample_Get, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("Sample_Create", SrpcGate_Sample_Create);
  srpcGateRegistry_.add("Sample_Destroy", SrpcGate_Sample_Destroy);
  srpcGateRegistry_.add("Sample_Get", SrpcGate_Sample_Get);

  AP_UNITTEST_HOOK(SampleModule, this);

  return ok;
}

void SampleModule::Exit()
{
  srpcGateRegistry_.finish();
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
