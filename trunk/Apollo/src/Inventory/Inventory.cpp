// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Inventory.h"

Inventory::~Inventory()
{
}

void Inventory::Create()
{
}

void Inventory::Destroy()
{
}

void Inventory::Show(int bShow)
{
  Msg_Gm_SendRequest msg;
  msg.hRequest = Apollo::newHandle();
  msg.srpc.set(Srpc::Key::Method, "Item.GetItemIdsAndValuesByProperty");
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("sKey", "Template");
  
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed"); }
}
