// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

// Ignore this:
// Replace regex: \(Msg_[a-zA-Z]+_[^ *]+\* pMsg\)

Inventory* InventoryModule::NewInventory(const ApHandle& hInventory)
{
  Inventory* pInventory = new Inventory(hInventory);
  if (pInventory) {
    try {
      pInventory->Create();
      inventories_.Set(hInventory, pInventory);
    } catch (ApException& ex) {
      delete pInventory;
      pInventory = 0;
      throw ex;
    }
  }

  return pInventory;
}

void InventoryModule::DeleteInventory(const ApHandle& hInventory)
{
  Inventory* pInventory = FindInventory(hInventory);
  if (pInventory) {
    ApHandle h = hInventory;
    if (!ApIsHandle(h)) {
      h = pInventory->apHandle();
    }
    pInventory->Destroy();
    inventories_.Unset(h);
    delete pInventory;
    pInventory = 0;
  }
}

Inventory* InventoryModule::FindInventory(const ApHandle& hInventory)
{
  Inventory* pInventory = 0;

  if (ApIsHandle(hInventory)) {
    inventories_.Get(hInventory, pInventory);
  } else {
    if (inventories_.Count() == 0) { throw ApException(LOG_CONTEXT, "no default Inventory"); }
    inventories_.Get(FirstInventoryHandle(), pInventory);
  }

  return pInventory;
}

Inventory* InventoryModule::GetInventory(const ApHandle& hInventory)
{
  Inventory* pInventory = FindInventory(hInventory);
  if (pInventory == 0) { throw ApException(LOG_CONTEXT, "no Inventory=" ApHandleFormat "", ApHandlePrintf(hInventory)); }
  return pInventory;
}

ApHandle InventoryModule::FirstInventoryHandle()
{
  InventoryListNode* pNode = inventories_.Next(0);
  if (pNode != 0) { return pNode->Key(); }
  return ApNoHandle;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(InventoryModule, Inventory_Create)
{
  if (!ApIsHandle(pMsg->hInventory)) { throw ApException(LOG_CONTEXT, "empty hInventory"); }
  if (inventories_.Find(pMsg->hInventory) != 0) { throw ApException(LOG_CONTEXT, "Inventory=" ApHandleFormat " already exists", ApHandlePrintf(pMsg->hInventory)); }
  Inventory* pInventory = NewInventory(pMsg->hInventory);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(InventoryModule, Inventory_Destroy)
{
  DeleteInventory(pMsg->hInventory);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(InventoryModule, Inventory_Show)
{
  GetInventory(pMsg->hInventory)->Show(pMsg->bShow);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(InventoryModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {

    Msg_Inventory_Create msg;
    msg.hInventory = Apollo::newHandle();
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to create default inventory"));
    } else {
      hDefaultInventory_ = msg.hInventory;
    }

  } else if (pMsg->sLevel == Msg_System_RunLevel_Shutdown) {

    Msg_Inventory_Destroy msg;
    msg.hInventory = hDefaultInventory_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to delete default inventory"));
    } else {
      hDefaultInventory_ = ApNoHandle;
    }

  }
}

AP_MSG_HANDLER_METHOD(InventoryModule, Config_GetValue)
{
  if (pMsg->sPath == "Inventory/Available") {
    pMsg->sValue = "1";
    pMsg->apStatus = ApMessage::Ok;
  }
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, System_RunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Config_GetValue, this, ApCallbackPosEarly);

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
