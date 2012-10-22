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
  ApHandle h = Apollo::newHandle();

  Msg_Gm_SendRequest msg;
  msg.hRequest = h;
  msg.srpc.set(Srpc::Key::Method, "Item.GetItemIdsAndValuesByProperty");
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("sKey", "Template");
  
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed"); }

  RebuildRequest* pRequest = new RebuildRequest(this);
  if (pRequest != 0) {
    requests_.Set(h, pRequest);
  }
}

int Inventory::ConsumeResponse(const ApHandle& hRequest, Apollo::SrpcMessage& response)
{
  int bConsumed = 0;

  ApHandleTreeNode<Request*>* pNode = requests_.Find(hRequest);
  if (pNode != 0) {
    Request* pRequest = pNode->Value();
    requests_.Unset(hRequest);

    if (pRequest != 0) {
      pRequest->HandleResponse(response);
    }

    delete pRequest;
    pRequest = 0;

    bConsumed = 1;
  }

  return bConsumed;
}

void Inventory::Purge()
{
}

void Inventory::BuildPanes(Apollo::KeyValueList& kvValues)
{
}
