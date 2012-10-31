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

    {
      Msg_Dialog_ContentCall msg;
      msg.hDialog = hDialog_;
      msg.sFunction = "Start";
      msg.Request();
    }

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

void Inventory::PurgeModel()
{
  nState_ = NoState;
  nGrid_ = 0;
  sGridName_ = "";
  nGridOrder_ = -1;

  while (items_.Count() > 0) {
    ItemListNode* pNode = items_.Next(0);
    if (pNode != 0) {
      items_.Unset(pNode->Key());
      Item* pItem = pNode->Value();
      delete pItem;
    }
  }
}

void Inventory::PlayModel()
{

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
  nGrid_ = 0;

  for (Elem* e = 0; (e = kvIdValues.Next(e)); ) {
    long nId = String::atol(e->getName());
    int bIsGrid = String::isTrue(e->getString());
    if (bIsGrid) {
      nGrid_ = nId;
    }
  }

  if (nGrid_ != 0) {
    ApHandle h = Apollo::newHandle();

    GetGridItemsRequest* pRequest = new GetGridItemsRequest(this, nGrid_);
    if (pRequest != 0) {
      AddRequest(h, pRequest);
    }

    Msg_Gm_SendRequest msg;
    msg.hRequest = h;
    msg.srpc.set(Srpc::Key::Method, "Item.GetProperties");
    msg.srpc.set("nItem", nGrid_);
    msg.srpc.set("vlKeys", "Name Nickname GridOrder Slots Contains");

    if (!msg.Request()) {
      DeleteRequest(h);
      throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
    }

    nState_ = StateGetGridDetails;
  }
}

void Inventory::GetGridItemsResponse(long nGrid, Apollo::SrpcMessage& kvProperties)
{
  String sName = kvProperties.getString("Name");

  String sNickname = kvProperties.getString("Nickname");
  
  int nGridOrder = kvProperties.getInt("GridOrder");
  
  int nSlots = kvProperties.getInt("Slots");
  
  String sContains = kvProperties.getString("Contains");

  // -------------------------------

  sGridName_ = sNickname;
  if (sGridName_.empty()) { sGridName_ = sName; }

  nGridOrder_ = nGridOrder;

  // -------------------------------

  if (!sContains.empty()) {
    ApHandle h = Apollo::newHandle();

    GetItemsPropertiesRequest* pRequest = new GetItemsPropertiesRequest(this, nGrid_);
    if (pRequest != 0) {
      AddRequest(h, pRequest);
    }

    Msg_Gm_SendRequest msg;
    msg.hRequest = h;
    msg.srpc.set(Srpc::Key::Method, "Item.GetMultiItemProperties");
    msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
    msg.srpc.set("vlItems", sContains);
    msg.srpc.set("vlKeys", "Name Nickname Icon32Url Stacksize Slot");

    if (!msg.Request()) {
      DeleteRequest(h);
      throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
    }

    nState_ = StateGetItemDetails;
  }

}


void Inventory::GetItemsPropertiesResponse(long nGrid, Apollo::SrpcMessage& kvIdKeyValues)
{
  for (Elem* e = 0; (e = kvIdKeyValues.Next(e)); ) {
    String sId = e->getName();
    long nId = String::atol(sId);
    if (nId > 0) {
      Apollo::KeyValueList kvProperties;
      kvIdKeyValues.getKeyValueList(sId, kvProperties);
      String sName = kvProperties["Name"].getString();
      String sNickname = kvProperties["Nickname"].getString();
      String sIcon32Url = kvProperties["Icon32Url"].getString();
      int nStacksize = kvProperties["Stacksize"].getInt();
      int nSlot = kvProperties["Slot"].getInt();

      Item* pItem = new Item();
      if (pItem != 0) {
        if (sName) { pItem->add("Name", sName); }
        if (sName) { pItem->add("Nickname", sNickname); }
        if (sName) { pItem->add("Icon32Url", sIcon32Url); }
        if (nStacksize > 1) { pItem->add("Stacksize", nStacksize); }
        if (nSlot != 0) { pItem->add("Slot", nSlot); }
        items_.Set(sId, pItem);
      }
    }
  }

  nState_ = StateReady;

  {
    Msg_Dialog_ContentCall msg;
    msg.hDialog = hDialog_;
    msg.sFunction = "Ready";
    msg.Request();
  }
}

