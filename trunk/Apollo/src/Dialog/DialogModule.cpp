// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

Dialog* DialogModule::NewDialog(const ApHandle& hDialog, int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sContentUrl)
{
  Dialog* pDialog = new Dialog(hDialog);
  if (pDialog) {
    try {
      pDialog->Create(nLeft, nTop, nWidth, nHeight, bVisible, sContentUrl);
      dialogs_.Set(hDialog, pDialog);
    } catch (ApException& ex) {
      delete pDialog;
      pDialog = 0;
      throw ex;
    }
  }

  return pDialog;
}

void DialogModule::DeleteDialog(const ApHandle& hDialog)
{
  Dialog* pDialog = FindDialog(hDialog);
  if (pDialog) {
    pDialog->Destroy();
    dialogs_.Unset(hDialog);
    delete pDialog;
    pDialog = 0;
  }
}

Dialog* DialogModule::FindDialog(const ApHandle& hDialog)
{
  Dialog* pDialog = 0;  

  dialogs_.Get(hDialog, pDialog);
  if (pDialog == 0) { throw ApException("DialogModule::FindDialog no dialog=" ApHandleFormat "", ApHandleType(hDialog)); }

  return pDialog;
}

//----------------------------

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_Create)
{
  if (dialogs_.Find(pMsg->hDialog) != 0) { throw ApException("DialogModule::Dialog_Create: Dialog=" ApHandleFormat " already exists", ApHandleType(pMsg->hDialog)); }
  Dialog* pDialog = NewDialog(pMsg->hDialog, pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight, pMsg->bVisible, pMsg->sContentUrl);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_Destroy)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  DeleteDialog(pMsg->hDialog);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_GetView)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  pMsg->hView = pDialog->GetView();
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------

void SrpcGate_Dialog_Create(ApSRPCMessage* pMsg)
{
  Msg_Dialog_Create msg;
  msg.hDialog = Apollo::string2Handle(pMsg->srpc.getString("hDialog"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Dialog_Destroy(ApSRPCMessage* pMsg)
{
  Msg_Dialog_Destroy msg;
  msg.hDialog = Apollo::string2Handle(pMsg->srpc.getString("hDialog"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Dialog_GetView(ApSRPCMessage* pMsg)
{
  Msg_Dialog_GetView msg;
  msg.hDialog = Apollo::string2Handle(pMsg->srpc.getString("hDialog"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->srpc.getString("hView") = msg.hView.toString();
}

//----------------------------

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

//----------------------------

int DialogModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_GetView, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("Dialog_Create", SrpcGate_Dialog_Create);
  srpcGateRegistry_.add("Dialog_Destroy", SrpcGate_Dialog_Destroy);
  srpcGateRegistry_.add("Dialog_GetView", SrpcGate_Dialog_GetView);

  AP_UNITTEST_HOOK(DialogModule, this);

  return ok;
}

void DialogModule::Exit()
{
  srpcGateRegistry_.finish();
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

  ApHandle hDialog = Apollo::newHandle();

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 200;
  msg.nTop = 200;
  msg.nWidth = 500;
  msg.nHeight = 300;
  msg.bVisible = 1;
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/Content.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}


  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
