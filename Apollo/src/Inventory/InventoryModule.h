// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(InventoryModule_H_INCLUDED)
#define InventoryModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgInventory.h"
#include "MsgConfig.h"
#include "MsgSystem.h"
#include "Inventory.h"

#if defined(AP_TEST)

class InventoryModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String ItemId2HandleMapper();
};
#endif

class InventoryModule
{
public:
  InventoryModule()
    :pInventory_(0)
    {}

  int Init();
  void Exit();

  void On_Inventory_Create(Msg_Inventory_Create* pMsg);
  void On_Inventory_Destroy(Msg_Inventory_Destroy* pMsg);
  void On_Inventory_Show(Msg_Inventory_Show* pMsg);
  // Dialog -> WebView
  //void On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg);
  void On_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg);

  // Dialog -> WebView
  //void On_WebView_Event_Closing(Msg_WebView_Event_Closing* pMsg);
  void On_Dialog_OnClosed(Msg_Dialog_OnClosed* pMsg);

  //void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg);
  void On_WebView_ModuleCall(Msg_WebView_ModuleCall* pMsg);
  void On_WebView_Event_Position(Msg_WebView_Event_Position* pMsg);
  void On_Config_GetValue(Msg_Config_GetValue* pMsg);
  void On_Gm_ReceiveResponse(Msg_Gm_ReceiveResponse* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class InventoryModuleTester;
#endif

protected:
  Inventory* pInventory_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<InventoryModule> InventoryModuleInstance;

#endif // InventoryModule_H_INCLUDED
