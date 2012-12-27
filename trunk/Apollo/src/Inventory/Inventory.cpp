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
// ItemId to Handle mapper

ApHandle Inventory::GetOrCreateItemHandle(const String& sItem)
{
  ApHandle hItem = GetItemHandle(sItem);
  if (ApIsHandle(hItem)) {
    return hItem;
  } else {
    return CreateItemHandle(sItem);
  }
}

ApHandle Inventory::CreateItemHandle(const String& sItem)
{
  if (id2Handle_.Find(sItem) != 0) { throw ApException(LOG_CONTEXT, "Item %s already mapped", _sz(sItem)); }
  ApHandle hItem = Apollo::newHandle();
  handle2Id_.Set(hItem, sItem);
  id2Handle_.Set(sItem, hItem);
  return hItem;
}

ApHandle Inventory::GetItemHandle(const String& sItem)
{
  ItemId2HandleListNode* node = id2Handle_.Find(sItem);
  if (node != 0) {
    return node->Value();
  }
  return ApNoHandle;
}

String Inventory::GetItemId(const ApHandle& hItem)
{
  ItemHandle2IdListNode* node = handle2Id_.Find(hItem);
  if (node != 0) {
    return node->Value();
  }
  return "";
}

void Inventory::DeleteItemId(const String& sItem)
{
  ApHandle hItem = GetItemHandle(sItem);
  if (ApIsHandle(hItem)) {
    handle2Id_.Unset(hItem);
    id2Handle_.Unset(sItem);
  }
}

void Inventory::DeleteItemHandle(const ApHandle& hItem)
{
  String sItem = GetItemId(hItem);
  if (!sItem.empty()) {
    handle2Id_.Unset(hItem);
    id2Handle_.Unset(sItem);
  }
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

//-------------------------

void Inventory::BuildPanels()
{
  PurgeModel();
  SendGetPanelsRequest();
}

void Inventory::SendGetPanelsRequest()
{
  ApHandle h = Apollo::newHandle();

  GetPanelsRequest* pRequest = new GetPanelsRequest(this);
  if (pRequest != 0) {
    AddRequest(h, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = h;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_GetItemIdsAndValuesByProperty);
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
    SendGetPanelItemsRequest(sPanelId_);
  }
}

void Inventory::SendGetPanelItemsRequest(const String& sPanel)
{
  ApHandle h = Apollo::newHandle();

  GetPanelItemsRequest* pRequest = new GetPanelItemsRequest(this, sPanel);
  if (pRequest != 0) {
    AddRequest(h, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = h;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_GetProperties);
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("nItem", sPanelId_);
  msg.srpc.set("vlKeys", Item_PropertyId_Name " " Item_PropertyId_Nickname " " Item_PropertyId_PanelOrder " " Item_PropertyId_Slots " " Item_PropertyId_Contains);

  if (!msg.Request()) {
    DeleteRequest(h);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetPanelDetails;
}

void Inventory::GetPanelItemsResponse(const String& sPanel, Apollo::SrpcMessage& kvProperties)
{
  String sName = kvProperties.getString(Item_PropertyId_Name);

  String sNickname = kvProperties.getString(Item_PropertyId_Nickname);
  
  int nPanelOrder = kvProperties.getInt(Item_PropertyId_PanelOrder);
  
  int nSlots = kvProperties.getInt(Item_PropertyId_Slots);
  
  String sContains = kvProperties.getString(Item_PropertyId_Contains);

  // -------------------------------

  sName_ = sNickname;
  if (sName_.empty()) { sName_ = sName; }

  nOrder_ = nPanelOrder;

  nSlots_ = nSlots;

  // -------------------------------

  if (!sContains.empty()) {
    SendGetItemsPropertiesResquest(sContains);
  }

}

void Inventory::SendGetItemsPropertiesResquest(const String& sContains)
{
  ApHandle h = Apollo::newHandle();

  GetItemsPropertiesRequest* pRequest = new GetItemsPropertiesRequest(this, sPanelId_);
  if (pRequest != 0) {
    AddRequest(h, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = h;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_GetMultiItemProperties);
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("vlItems", sContains);
  msg.srpc.set("vlKeys", Item_PropertyId_Id " " Item_PropertyId_Name " " Item_PropertyId_Nickname " " Item_PropertyId_Icon32Url " " Item_PropertyId_Image100Url " " Item_PropertyId_Stacksize " " Item_PropertyId_Slot);

  if (!msg.Request()) {
    DeleteRequest(h);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetItemDetails;
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
  if (ApIsHandle(hDragItemView_)) {
    EndDragItem();
  }

  sDragItemId_ = sItemId;
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

  hDragItemView_ = h;
}

void Inventory::EndDragItem()
{
  if (ApIsHandle(hDragItemView_)) {
    Msg_WebView_Destroy::_(hDragItemView_);
    hDragItemView_ = ApNoHandle;
  }
}

void Inventory::OnDragItemReady(const ApHandle& hView)
{
  if (hDragItemView_ == hView) {
    //if (!Msg_WebView_ViewCall::_(hView, "Start")) { throw ApException(LOG_CONTEXT, "Msg_WebView_ViewCall 'Start' failed"); }
    String sUrl;
    ItemListNode* pNode = items_.Find(sDragItemId_);
    if (pNode != 0) {
      Item* pItem = pNode->Value();
      if (pItem != 0) {
        sUrl = (*pItem)[Item_PropertyId_Image100Url].getString();
        if (!sUrl) {
          sUrl = (*pItem)[Item_PropertyId_Icon32Url].getString();
        }
      }
    }
    List lArgs;
    lArgs.AddLast(sUrl);
    lArgs.AddLast(String::from(nDragOffsetX_));
    lArgs.AddLast(String::from(nDragOffsetY_));
    String sResult = Msg_WebView_CallScriptFunction::_(hView, "", "Start", lArgs);
  }
}

//---------------------------------------------------
// Tests

String Inventory::Test_CreateItemHandle()
{
  String s;
  Inventory i;
  ApHandle h = i.CreateItemHandle("1"); 
  if (i.GetItemHandle("1") != h) { s = String::from(__LINE__); }
  return s;
}

String Inventory::Test_MultipleCreateItemHandle()
{
  String s;
  Inventory i;
  ApHandle h1 = i.CreateItemHandle("1");
  ApHandle h3 = i.CreateItemHandle("3");
  ApHandle h2 = i.CreateItemHandle("2");
  if (!s) { if (i.GetItemHandle("1") != h1) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("2") != h2) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("3") != h3) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h1) != "1") { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h2) != "2") { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h3) != "3") { s = String::from(__LINE__); }}
  return s;
}

String Inventory::Test_DeleteItemId()
{
  String s;
  Inventory i;
  ApHandle h1 = i.CreateItemHandle("1");
  ApHandle h3 = i.CreateItemHandle("3");
  ApHandle h2 = i.CreateItemHandle("2");
  i.DeleteItemId("2");
  if (!s) { if (i.GetItemHandle("1") != h1) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("2") != ApNoHandle) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("3") != h3) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h1) != "1") { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h2) != "") { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h3) != "3") { s = String::from(__LINE__); }}
  return s;
}

String Inventory::Test_DeleteItemHandle()
{
  String s;
  Inventory i;
  ApHandle h1 = i.CreateItemHandle("1");
  ApHandle h3 = i.CreateItemHandle("3");
  ApHandle h2 = i.CreateItemHandle("2");
  i.DeleteItemHandle(h2);
  if (!s) { if (i.GetItemHandle("1") != h1) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("2") != ApNoHandle) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("3") != h3) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h1) != "1") { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h2) != "") { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemId(h3) != "3") { s = String::from(__LINE__); }}
  return s;
}

String Inventory::Test_GetOrCreateItemHandle()
{
  String s;
  Inventory i;
  ApHandle h3 = i.CreateItemHandle("3");
  ApHandle h2 = i.CreateItemHandle("2");
  ApHandle h1 = i.CreateItemHandle("1");
  ApHandle h2b = i.GetOrCreateItemHandle("2");
  if (!s) { if (i.GetItemHandle("2") != h2) { s = String::from(__LINE__); }}
  if (!s) { if (i.GetItemHandle("2") != h2b) { s = String::from(__LINE__); }}
  if (!s) { if (h2 != h2b) { s = String::from(__LINE__); }}
  return s;
}

String Inventory::Test_CreateItemHandleDuplicateEntryException()
{
  String s;
  try {
    Inventory i;
    ApHandle h = i.CreateItemHandle("1"); 
    i.CreateItemHandle("1"); 
    s = String::from(__LINE__);
  } catch (ApException& ex) {
  }
  return s;
}

String Inventory::TestItemId2HandleMapper()
{
  String s;

  if (!s) { s = Test_CreateItemHandle(); }
  if (!s) { s = Test_MultipleCreateItemHandle(); }
  if (!s) { s = Test_DeleteItemHandle(); }
  if (!s) { s = Test_DeleteItemId(); }
  if (!s) { s = Test_GetOrCreateItemHandle(); }
  if (!s) { s = Test_CreateItemHandleDuplicateEntryException(); }

  return s;
}

