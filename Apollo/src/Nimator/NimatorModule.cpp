// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "NimatorModule.h"

NimatorModule::NimatorModule()
{
}

NimatorModule::~NimatorModule()
{
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Create)
{
  int ok = 0;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Destroy)
{
  int ok = 0;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetRate)
{
  int ok = 0;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetData)
{
  int ok = 0;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetStatus)
{
  int ok = 0;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Event)
{
  int ok = 0;
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

#if defined(AP_TEST)

String NimatorModule::Test1()
{
  String s;
  return s;
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Nimator", 0)) {
    AP_UNITTEST_REGISTER(NimatorModule::Test1);
  }
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Nimator", 0)) {
    AP_UNITTEST_EXECUTE(NimatorModule::Test1);
  }
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

#include "ximagif.h"
#include "Image.h"

int NimatorModule::Init()
{
  int ok = 1;

  CxImageGIF img;
  Buffer sbSource;
  Apollo::loadFile("tassadar-walk-l.gif", sbSource);
  CxMemFile mfSource(sbSource.Data(), sbSource.Length());
  img.Decode(&mfSource);
  //CxImage img;
  //String sFilename = "tassadar-walk-l.gif";
  //img.Load(sFilename);

  Msg_Animation_Frame msg;
  msg.iFrame.Allocate(img.GetWidth(), img.GetHeight());
  CxMemFile mfDest((BYTE*) msg.iFrame.Pixels(), msg.iFrame.Size());
  img.Encode2RGBA(&mfDest);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetRate, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetStatus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Event, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(NimatorModule, this);

  return ok;
}

void NimatorModule::Exit()
{
  AP_UNITTEST_UNHOOK(NimatorModule, this);
  AP_MSG_REGISTRY_FINISH;
}
