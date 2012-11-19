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

//---------------------------------------------------
// Interface

void Inventory::Create()
{
  ApHandle h = Apollo::newHandle();

  Msg_Dialog_Create msg;
  msg.hDialog = h; 
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 0;
  msg.sCaption = Apollo::translate(MODULE_NAME, "", "Inventory");
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Inventory.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  hCandidate_ = h;
}

void Inventory::Destroy()
{
  if (ApIsHandle(hDialog_)) {
    Msg_Dialog_Destroy msg;
    msg.hDialog = hDialog_;
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
    hDialog_ = ApNoHandle;
  }
}

void Inventory::SetVisibility(int bShow)
{
  if (ApIsHandle(hDialog_)) {
    ApHandle hView = Msg_Dialog_GetView::_(hDialog_);
    if (ApIsHandle(hView)) {
      Msg_WebView_Visibility::_(hView, bShow);
    }
  }
}

void Inventory::Show(int bShow)
{
  bVisible_ = bShow;

  SetVisibility(bShow);
}

void Inventory::OnOpened(const ApHandle& hDialog)
{
  if (hCandidate_ == hDialog) {
    hDialog_ = hDialog;

    SetVisibility(bVisible_);

    Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "Start");

    if (nState_ == NoState) {
      BuildGrids();
    }

  }
}

void Inventory::OnClosed(const ApHandle& hDialog)
{
  if (hDialog_ == hDialog) {
    hDialog_ = ApNoHandle;
    nState_ = NoState;
  }
}

void Inventory::OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString(Srpc::Key::Method);

  if (0){
  } else if (sMethod == "PlayModel") {
    PlayModel();

  } else {
    throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
  }
}

//---------------------------------------------------
// Internal

void Inventory::AddRequest(const ApHandle& hRequest, Request* pRequest)
{
  if (pRequest != 0) {
    requests_.Set(hRequest, pRequest);
  }
}

void Inventory::DeleteRequest(const ApHandle& hRequest)
{
  ApHandleTreeNode<Request*>* pNode = requests_.Find(hRequest);
  if (pNode != 0) {
    Request* pRequest = pNode->Value();
    requests_.Unset(hRequest);
    delete pRequest;
    pRequest = 0;
  }
}

int Inventory::ConsumeResponse(const ApHandle& hRequest, Apollo::SrpcMessage& response)
{
  int bConsumed = 0;

  ApHandleTreeNode<Request*>* pNode = requests_.Find(hRequest);
  if (pNode != 0) {

    Request* pRequest = pNode->Value();
    if (pRequest != 0) {
      pRequest->HandleResponse(response);
    }

    bConsumed = 1;
  }

  if (bConsumed) {
    DeleteRequest(hRequest);
  }

  return bConsumed;
}

void Inventory::BuildGrids()
{
  ApHandle h = Apollo::newHandle();

  PurgeModel();

  GetGridsRequest* pRequest = new GetGridsRequest(this);
  if (pRequest != 0) {
    AddRequest(h, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = h;
  msg.srpc.set(Srpc::Key::Method, "Item.GetItemIdsAndValuesByProperty");
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("sKey", "IsGrid");
  
  if (!msg.Request()) {
    DeleteRequest(h);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetGrids;
}

void Inventory::GetGridsResponse(Apollo::SrpcMessage& kvIdValues)
{
  sGridId_ = "";

  for (Elem* e = 0; (e = kvIdValues.Next(e)); ) {
    String sId = e->getName();
    long nId = String::atol(sId);
    int bIsGrid = String::isTrue(e->getString());
    if (bIsGrid) {
      sGridId_ = sId;
    }
  }

  if (!sGridId_.empty()) {
    ApHandle h = Apollo::newHandle();

    GetGridItemsRequest* pRequest = new GetGridItemsRequest(this, sGridId_);
    if (pRequest != 0) {
      AddRequest(h, pRequest);
    }

    Msg_Gm_SendRequest msg;
    msg.hRequest = h;
    msg.srpc.set(Srpc::Key::Method, "Item.GetProperties");
    msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
    msg.srpc.set("nItem", sGridId_);
    msg.srpc.set("vlKeys", "Name Nickname GridOrder Slots Contains");

    if (!msg.Request()) {
      DeleteRequest(h);
      throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
    }

    nState_ = StateGetGridDetails;
  }
}

void Inventory::GetGridItemsResponse(const String& sGrid, Apollo::SrpcMessage& kvProperties)
{
  String sName = kvProperties.getString("Name");

  String sNickname = kvProperties.getString("Nickname");
  
  int nGridOrder = kvProperties.getInt("GridOrder");
  
  int nSlots = kvProperties.getInt("Slots");
  
  String sContains = kvProperties.getString("Contains");

  // -------------------------------

  sName_ = sNickname;
  if (sName_.empty()) { sName_ = sName; }

  nOrder_ = nGridOrder;

  nSlots_ = nSlots;

  // -------------------------------

  if (!sContains.empty()) {
    ApHandle h = Apollo::newHandle();

    GetItemsPropertiesRequest* pRequest = new GetItemsPropertiesRequest(this, sGridId_);
    if (pRequest != 0) {
      AddRequest(h, pRequest);
    }

    Msg_Gm_SendRequest msg;
    msg.hRequest = h;
    msg.srpc.set(Srpc::Key::Method, "Item.GetMultiItemProperties");
    msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
    msg.srpc.set("vlItems", sContains);
    msg.srpc.set("vlKeys", "Id Name Nickname Icon32Url Stacksize Slot");

    if (!msg.Request()) {
      DeleteRequest(h);
      throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
    }

    nState_ = StateGetItemDetails;
  }

}

void Inventory::GetItemsPropertiesResponse(const String& sGrid, Apollo::SrpcMessage& kvIdKeyValues)
{
  for (Elem* e = 0; (e = kvIdKeyValues.Next(e)); ) {
    String sId = e->getName();
    long nId = String::atol(sId);
    if (nId > 0) {
      Apollo::KeyValueList kvProperties;
      kvIdKeyValues.getKeyValueList(sId, kvProperties);

      Item* pItem = new Item();
      if (pItem != 0) {
        for (Apollo::KeyValueElem* e = 0; (e = kvProperties.nextElem(e)); ) {
          pItem->add(e->getKey(), e->getString());
        }
        items_.Set(sId, pItem);
      }
    }
  }

  nState_ = StateReady;

  PlayModel();
}

void Inventory::PurgeModel()
{
  nState_ = NoState;
  sGridId_ = "";
  sName_ = "";
  nOrder_ = -1;

  while (items_.Count() > 0) {
    ItemListNode* pNode = items_.Next(0);
    if (pNode != 0) {
      Item* pItem = pNode->Value();
      items_.Unset(pNode->Key());
      delete pItem;
    }
  }
}

String Inventory::GetScriptFunctionName()
{
  return Apollo::getModuleConfig(MODULE_NAME, "CallScriptSrpcFunctionName", "ReceiveMessageFromModule");
}

void Inventory::PlayModel()
{
  if (nState_ == StateReady) {
    Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "PurgeGrids");

    { // for all grids
      {
        Apollo::SrpcMessage srpc;
        srpc.set(Srpc::Key::Method, "AddGrid");
        srpc.set("Grid", sGridId_);
        srpc.set("Name", sName_);
        srpc.set("Order", nOrder_);
        srpc.set("Slots", nSlots_);
        Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
      }
  
      ItemListNode* node = 0;
      for (ItemListIterator iter(items_); (node = iter.Next()); ) {
        Item* pItem = node->Value();
        if (pItem != 0) {
          Apollo::SrpcMessage srpc;
          srpc.set(Srpc::Key::Method, "AddItem");
          srpc.set("Grid", sGridId_);
          for (Apollo::KeyValueElem* e = 0; (e = pItem->nextElem(e)); ) {
            srpc.set(e->getKey(), e->getString());
          }
          Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
        }
      }
    }

  }
}
