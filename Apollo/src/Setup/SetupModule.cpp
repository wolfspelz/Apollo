// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

void SetupModule::SendRunLevelNormal()
{
  bInSendRunLevelNormal_ = 1;

  Msg_System_RunLevel msg; 
  msg.sLevel = Msg_System_RunLevel_Normal; 
  msg.Send();
  
  bInSendRunLevelNormal_ = 0;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(SetupModule, Setup_Open)
{
  if (ApIsHandle(hDialog_)) { throw ApException(LOG_CONTEXT, "ignored, because already open"); }

  ApHandle hDialog = Apollo::newHandle(); 

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog; 
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 1;
  msg.sCaption = Apollo::translate(MODULE_NAME, "", "Setup");
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Setup.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  hDialog_ = hDialog;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SetupModule, Setup_Close)
{
  if (!ApIsHandle(hDialog_)) { throw ApException(LOG_CONTEXT, "ignored, because not open"); }

  ApHandle hDialog = Apollo::newHandle(); 

  Msg_Dialog_Destroy msg;
  msg.hDialog = hDialog_;
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SetupModule, Dialog_OnOpened)
{
  if (pMsg->hDialog == hDialog_) {
    Msg_Dialog_ContentCall msg;
    msg.hDialog = hDialog_;
    msg.sFunction = "Start";
    msg.Request();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, Dialog_OnClosed)
{
  if (pMsg->hDialog == hDialog_) {
    hDialog_ = ApNoHandle;

    if (bInterseptedRunLevelNormal_) {
      bInterseptedRunLevelNormal_ = 0;
      SendRunLevelNormal();
    }
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, System_RunLevel)
{
  if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    if (!bInSendRunLevelNormal_) {
      if (Apollo::getModuleConfig(MODULE_NAME, "FirstStart", 0)) {

        // Stop the "Normal" run level until setup is done
        pMsg->Stop();

        Apollo::setModuleConfig(MODULE_NAME, "FirstStart", 0);
        bInterseptedRunLevelNormal_ = 1;

        Msg_Setup_Open msg; 
        msg.Request();

      }
    }
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, System_BeforeEventLoop)
{
  if (Apollo::getModuleConfig(MODULE_NAME, "AlwaysFirstStart", 0)) {
    Apollo::setModuleConfig(MODULE_NAME, "FirstStart", 1);
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, WebView_ModuleCall)
{
  if (pMsg->hView == hDialog_) {
    String sMethod = pMsg->srpc.getString(Srpc::Key::Method);

    if (0){
    } else if (sMethod == "InstallFirefoxExtension") {

    } else {
      throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
    }

    pMsg->apStatus = ApMessage::Ok;
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
    SetupModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
    SetupModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
    SetupModuleTester::End();
  }
} 

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int SetupModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Setup_Open, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Setup_Close, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Dialog_OnOpened, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Dialog_OnClosed, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, System_RunLevel, this, ApCallbackPosVeryEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, System_BeforeEventLoop, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, WebView_ModuleCall, this, ApCallbackPosNormal);

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
  AP_UNITTEST_REGISTER(SetupModuleTester::Dev);
}

void SetupModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(SetupModuleTester::Dev);
}

void SetupModuleTester::End()
{
}

//----------------------------

String SetupModuleTester::Dev()
{
  String s;

  Msg_Setup_Open msg;
  if (!msg.Request()) { s = msg.Type() + " failed: " + msg.sComment; }

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
