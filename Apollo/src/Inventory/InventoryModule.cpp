// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(InventoryModule, Inventory_Create)
{
  if (pInventory_ != 0) { throw ApException(LOG_CONTEXT, "Inventory already exists"); }

  pInventory_ = new Inventory();
  if (pInventory_ != 0) {
    pInventory_->Create();
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(InventoryModule, Inventory_Destroy)
{
  if (pInventory_ == 0) { throw ApException(LOG_CONTEXT, "no Inventory"); }

  pInventory_->Destroy();
  delete pInventory_;
  pInventory_ = 0;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(InventoryModule, Inventory_Show)
{
  if (pInventory_ == 0) { throw ApException(LOG_CONTEXT, "no Inventory"); }
  
  pInventory_->Show(pMsg->bShow);

  pMsg->apStatus = ApMessage::Ok;
}

//AP_MSG_HANDLER_METHOD(InventoryModule, System_RunLevel)
//{
//  if (0) {
//  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
//
//    Msg_Inventory_Create msg;
//    msg.hInventory = Apollo::newHandle();
//    if (!msg.Request()) {
//      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to create default inventory"));
//    } else {
//      hDefaultInventory_ = msg.hInventory;
//    }
//
//  } else if (pMsg->sLevel == Msg_System_RunLevel_Shutdown) {
//
//    Msg_Inventory_Destroy msg;
//    msg.hInventory = hDefaultInventory_;
//    if (!msg.Request()) {
//      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to delete default inventory"));
//    } else {
//      hDefaultInventory_ = ApNoHandle;
//    }
//
//  }
//}

AP_MSG_HANDLER_METHOD(InventoryModule, Config_GetValue)
{
  if (pMsg->sPath == "Inventory/Available") {
    pMsg->sValue = "1";
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(InventoryModule, Gm_ReceiveResponse)
{
  pMsg->hRequest;
  pMsg->srpc;
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(InventoryModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Inventory", 0)) {
    InventoryModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(InventoryModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Inventory", 0)) {
    InventoryModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(InventoryModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Inventory", 0)) {
    InventoryModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int InventoryModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Inventory_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Inventory_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Inventory_Show, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, System_RunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Config_GetValue, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Gm_ReceiveResponse, this, ApCallbackPosEarly);

  AP_UNITTEST_HOOK(InventoryModule, this);

  return ok;
}

void InventoryModule::Exit()
{
  AP_UNITTEST_UNHOOK(InventoryModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

void InventoryModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(InventoryModuleTester::CreateDeleteInventory);
  AP_UNITTEST_REGISTER(InventoryModuleTester::CreateDeleteDefaultInventory);
}

void InventoryModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(InventoryModuleTester::CreateDeleteInventory);
  AP_UNITTEST_EXECUTE(InventoryModuleTester::CreateDeleteDefaultInventory);
}

void InventoryModuleTester::End()
{
}

//----------------------------

String InventoryModuleTester::CreateDeleteInventory()
{
  String s;

  InventoryModule m;
  ApHandle hInventory = Apollo::newHandle();

  m.NewInventory(hInventory);
  if (!s) { if (m.inventories_.Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.FindInventory(hInventory) == 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.FirstInventoryHandle() != hInventory) { s = String::from(__LINE__); }}
  m.DeleteInventory(hInventory);
  if (!s) { if (m.inventories_.Count() != 0) { s = String::from(__LINE__); }}

  return s;
}

String InventoryModuleTester::CreateDeleteDefaultInventory()
{
  String s;

  InventoryModule m;
  ApHandle hInventory = Apollo::newHandle();

  m.NewInventory(hInventory);
  if (!s) { if (m.inventories_.Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.FindInventory(ApNoHandle) == 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.FirstInventoryHandle() != hInventory) { s = String::from(__LINE__); }}
  m.DeleteInventory(ApNoHandle);
  if (!s) { if (m.inventories_.Count() != 0) { s = String::from(__LINE__); }}

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
