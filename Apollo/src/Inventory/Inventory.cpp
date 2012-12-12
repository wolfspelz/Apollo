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
  if (hCandidate_ == hDialog || hDialog_ == hDialog) {
    hCandidate_ = ApNoHandle;
    hDialog_ = hDialog;

    SetVisibility(bVisible_);

    Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "Start");

    if (nState_ == NoState) {
      BuildPanels();
    }

  }
}

void Inventory::OnClosed(const ApHandle& hDialog)
{
  if (hDialog_ == hDialog) {
    hDialog_ = ApNoHandle;
    nState_ = NoState;
  }

  if (hCandidate_ == hDialog) {
    hCandidate_ = ApNoHandle;
  }
}

void Inventory::OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString(Srpc::Key::Method);

  if (0){
  } else if (sMethod == "OnPlayModel") {
    PlayModel();

  } else if (sMethod == "OnDragItem") {
    BeginDragItem(request.getString("sItemId")
      , request.getInt("nLeft")
      , request.getInt("nTop")
      , request.getInt("nWidth")
      , request.getInt("nHeight")
      , request.getInt("nOffsetX")
      , request.getInt("nOffsetY")
      );

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

void Inventory::BuildPanels()
{
  ApHandle h = Apollo::newHandle();

  PurgeModel();

  GetPanelsRequest* pRequest = new GetPanelsRequest(this);
  if (pRequest != 0) {
    AddRequest(h, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = h;
  msg.srpc.set(Srpc::Key::Method, "Item.GetItemIdsAndValuesByProperty");
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("sKey", "IsPanel");
  
  if (!msg.Request()) {
    DeleteRequest(h);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetPanels;
}

void Inventory::GetPanelsResponse(Apollo::SrpcMessage& kvIdValues)
{
  sPanelId_ = "";

  for (Elem* e = 0; (e = kvIdValues.Next(e)); ) {
    String sId = e->getName();
    long nId = String::atol(sId);
    int bIsPanel = String::isTrue(e->getString());
    if (bIsPanel) {
      sPanelId_ = sId;
    }
  }

  if (!sPanelId_.empty()) {
    ApHandle h = Apollo::newHandle();

    GetPanelItemsRequest* pRequest = new GetPanelItemsRequest(this, sPanelId_);
    if (pRequest != 0) {
      AddRequest(h, pRequest);
    }

    Msg_Gm_SendRequest msg;
    msg.hRequest = h;
    msg.srpc.set(Srpc::Key::Method, "Item.GetProperties");
    msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
    msg.srpc.set("nItem", sPanelId_);
    msg.srpc.set("vlKeys", "Name Nickname PanelOrder Slots Contains");

    if (!msg.Request()) {
      DeleteRequest(h);
      throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
    }

    nState_ = StateGetPanelDetails;
  }
}

void Inventory::GetPanelItemsResponse(const String& sPanel, Apollo::SrpcMessage& kvProperties)
{
  String sName = kvProperties.getString("Name");

  String sNickname = kvProperties.getString("Nickname");
  
  int nPanelOrder = kvProperties.getInt("PanelOrder");
  
  int nSlots = kvProperties.getInt("Slots");
  
  String sContains = kvProperties.getString("Contains");

  // -------------------------------

  sName_ = sNickname;
  if (sName_.empty()) { sName_ = sName; }

  nOrder_ = nPanelOrder;

  nSlots_ = nSlots;

  // -------------------------------

  if (!sContains.empty()) {
    ApHandle h = Apollo::newHandle();

    GetItemsPropertiesRequest* pRequest = new GetItemsPropertiesRequest(this, sPanelId_);
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

void Inventory::GetItemsPropertiesResponse(const String& sPanel, Apollo::SrpcMessage& kvIdKeyValues)
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
  sPanelId_ = "";
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
    Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "PurgePanels");

    { // for all grids
      {
        Apollo::SrpcMessage srpc;
        srpc.set(Srpc::Key::Method, "AddPanel");
        srpc.set("Panel", sPanelId_);
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
          srpc.set("Panel", sPanelId_);
          for (Apollo::KeyValueElem* e = 0; (e = pItem->nextElem(e)); ) {
            srpc.set(e->getKey(), e->getString());
          }
          Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
        }
      }
    }

  }
}

void Inventory::BeginDragItem(const String& sItemId, int nLeft, int nTop, int nWidth, int nHeight, int nOffsetX, int nOffsetY)
{
  if (ApIsHandle(hDragItem_)) {
    EndDragItem();
  }

  nDragOffsetX_ = nOffsetX;
  nDragOffsetY_ = nOffsetY;

  ApHandle h = Apollo::newHandle();
  ApHandle hDialogView = Msg_Dialog_GetView::_(hDialog_);

  int nInventoryLeft = 0;
  int nInventoryTop = 0;
  {
    Msg_WebView_GetPosition msg;
    msg.hView = hDialogView;
    if (msg.Request()) {
      nInventoryLeft = msg.nLeft;
      nInventoryTop = msg.nTop;
    }
  }

  int nContentLeft = 0;
  int nContentTop = 0;
  {
    Msg_Dialog_GetContentRect msg;
    msg.hDialog = hDialog_;
    if (msg.Request()) {
      nContentLeft = msg.nLeft;
      nContentTop = msg.nTop;
    }
  }

  int nAbsLeft = nInventoryLeft + nContentLeft + nLeft;
  int nAbsTop = nInventoryTop + nContentTop + nTop;

  if (!Msg_WebView_Create::_(h, nAbsLeft, nAbsTop, nWidth, nHeight)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Create failed"); }
  if (!Msg_WebView_SetScriptAccessPolicy::Allow(h)) { throw ApException(LOG_CONTEXT, "Msg_WebView_SetScriptAccessPolicy failed"); }
  if (!Msg_WebView_Load::_(h, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "DragItem.html")) { throw ApException(LOG_CONTEXT, "Msg_WebView_Load failed"); }
  if (!Msg_WebView_Visibility::_(h, 1)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Visibility failed"); }

  hDragItem_ = h;
}

void Inventory::EndDragItem()
{
  if (ApIsHandle(hDragItem_)) {
    Msg_WebView_Destroy::_(hDragItem_);
    hDragItem_ = ApNoHandle;
  }
}

void Inventory::OnDragItemReady(const ApHandle& hView)
{
  if (hDragItem_ == hView) {
    //if (!Msg_WebView_ViewCall::_(hView, "Start")) { throw ApException(LOG_CONTEXT, "Msg_WebView_ViewCall 'Start' failed"); }
    String sResult = Msg_WebView_CallScriptFunction::_(hView, "", "Start", String::from(nDragOffsetX_), String::from(nDragOffsetY_));
  }
}


