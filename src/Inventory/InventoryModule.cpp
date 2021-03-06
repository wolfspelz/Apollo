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
  if (Apollo::getModuleConfig(MODULE_NAME, "CreateOnShow", 0)) {
    if (pMsg->bShow && pInventory_ == 0) {
      Msg_Inventory_Create msg;
      msg.Request();
    }
  }

  if (pInventory_ == 0) { throw ApException(LOG_CONTEXT, "no Inventory"); }
  
  pInventory_->Show(pMsg->bShow);

  if (Apollo::getModuleConfig(MODULE_NAME, "DestroyOnHide", 0)) {
    if (!pMsg->bShow && pInventory_ != 0) {
      Msg_Inventory_Destroy msg;
      msg.Request();
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(InventoryModule, Dialog_OnOpened)
{
  if (pInventory_ != 0) {
    if (pInventory_->HasDialog(pMsg->hDialog)) {
      pInventory_->OnOpened(pMsg->hDialog);
    }

    if (pInventory_->HasItemInfo(pMsg->hDialog)) {
      pInventory_->OnItemInfoOpened(pMsg->hDialog);
    }
  }
}

AP_MSG_HANDLER_METHOD(InventoryModule, Dialog_OnClosed)
{
  if (pInventory_ != 0) {
    if (pInventory_->HasDialog(pMsg->hDialog)) {
      pInventory_->OnClosed(pMsg->hDialog);

      if (Apollo::getModuleConfig(MODULE_NAME, "DestroyOnHide", 0)) {
        Msg_Inventory_Destroy msg;
        msg.Request();
      }
    }

    if (pInventory_->HasItemInfo(pMsg->hDialog)) {
      pInventory_->OnItemInfoClosed(pMsg->hDialog);
    }
  }
}

AP_MSG_HANDLER_METHOD(InventoryModule, WebView_Event_DocumentComplete)
{
  //hw DragDropInventoryItem
  //if (pInventory_ != 0 && pInventory_->HasDragItem(pMsg->hView)) {
  //  pInventory_->OnDragItemReady(pMsg->hView);
  //}
}

AP_MSG_HANDLER_METHOD(InventoryModule, WebView_ModuleCall)
{
  if (pInventory_ != 0) {
    if (pInventory_->HasDialog(pMsg->hView)) {
      pInventory_->OnModuleCall(pMsg->srpc, pMsg->response);
      pMsg->apStatus = ApMessage::Ok;
    }

    if (pInventory_->HasItemInfo(pMsg->hView)) {
      pInventory_->OnItemInfoModuleCall(pMsg->hView, pMsg->srpc, pMsg->response);
      pMsg->apStatus = ApMessage::Ok;
    }
  }

  //hw DragDropInventoryItem
  //if (pInventory_ != 0 && pInventory_->HasDragItem(pMsg->hView)) {
  //  pInventory_->OnModuleCall(pMsg->srpc, pMsg->response);
  //  pMsg->apStatus = ApMessage::Ok;
  //}
}

AP_MSG_HANDLER_METHOD(InventoryModule, WebView_Event_Position)
{
  //hw DragDropInventoryItem
  //if (pInventory_ != 0 && pInventory_->HasDragItem(pMsg->hView)) {
  //  pInventory_->OnDragItemMove(pMsg->hView, pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight);
  //}
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
  if (pInventory_ != 0) {
    if (pInventory_->ConsumeResponse(pMsg->hRequest, pMsg->srpc)) {
      pMsg->Stop();
    }
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Dialog_OnOpened, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, Dialog_OnClosed, this, ApCallbackPosNormal);
  //
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, WebView_Event_DocumentComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, WebView_ModuleCall, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, InventoryModule, WebView_Event_Position, this, ApCallbackPosEarly);
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
  AP_UNITTEST_REGISTER(InventoryModuleTester::ItemId2HandleMapper);
}

void InventoryModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(InventoryModuleTester::ItemId2HandleMapper);
}

void InventoryModuleTester::End()
{
}

//----------------------------

String InventoryModuleTester::ItemId2HandleMapper()
{
  String s;

  if (!s) {
    s = Inventory::TestItemId2HandleMapper();
  }

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
