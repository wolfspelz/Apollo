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

void Inventory::DeleteAllItemHandles()
{
  ItemHandle2IdListNode* node = 0;
  while ((node = handle2Id_.Next(0)) != 0) {
    DeleteItemHandle(node->Key());
  }
}

//---------------------------------------------------
// Interface

void Inventory::Create()
{
  ApHandle hDialog = Apollo::newHandle();

  // Dialog -> WebView
  if (!Msg_WebView_Create::_(hDialog, Apollo::getModuleConfig(MODULE_NAME, "Left", 200), Apollo::getModuleConfig(MODULE_NAME, "Top", 200), Apollo::getModuleConfig(MODULE_NAME, "Width", 500), Apollo::getModuleConfig(MODULE_NAME, "Height", 300))) { throw ApException(LOG_CONTEXT, "Msg_WebView_Create"); }
  if (!Msg_WebView_SetScriptAccessPolicy::Allow(hDialog)) { throw ApException(LOG_CONTEXT, "Msg_WebView_SetScriptAccessPolicy"); }
  if (!Msg_WebView_Load::_(hDialog, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Inventory.html")) { throw ApException(LOG_CONTEXT, "Msg_WebView_Load"); }
  //Msg_Dialog_Create msg;
  //msg.hDialog = hDialog; 
  //msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  //msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  //msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  //msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  //msg.bVisible = 0;
  //msg.sCaption = Apollo::translate(MODULE_NAME, "", "Inventory");
  //msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  //msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Inventory.html";
  //if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  drag_.Init();

  hCandidate_ = hDialog;
}

void Inventory::Destroy()
{
  if (ApIsHandle(hDialog_)) {
    // Dialog -> WebView
    Msg_WebView_Destroy msg;
    msg.hView = hDialog_;
    //Msg_Dialog_Destroy msg;
    //msg.hDialog = hDialog_;
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
    hDialog_ = ApNoHandle;
  }
}

void Inventory::SetVisibility(int bShow)
{
  if (ApIsHandle(hDialog_)) {
    // Dialog -> WebView
    ApHandle hView = hDialog_;
    //ApHandle hView = Msg_Dialog_GetView::_(hDialog_);
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

    // Dialog -> WebView
    Msg_WebView_CallScriptFunction::_(hDialog_, "", "Start");
    //Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "Start");

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
    BeginDragItem(request.getHandle("hItem")
      ,request.getInt("nLeft")
      ,request.getInt("nTop")
      ,request.getInt("nWidth")
      ,request.getInt("nHeight")
      ,request.getInt("nMouseX")
      ,request.getInt("nMouseY")
      ,request.getInt("nPinX")
      ,request.getInt("nPinY")
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
  ApHandle hRequest = Apollo::newHandle();

  GetPanelsRequest* pRequest = new GetPanelsRequest(this);
  if (pRequest != 0) {
    AddRequest(hRequest, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = hRequest;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_GetItemIdsAndValuesByProperty);
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("sKey", "IsPanel");
  
  if (!msg.Request()) {
    DeleteRequest(hRequest);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetPanels;
}

void Inventory::GetPanelsResponse(Apollo::SrpcMessage& kvIdValues)
{
  ApHandle hPanel;

  for (Elem* e = 0; (e = kvIdValues.Next(e)); ) {
    String sId = e->getName();
    long nId = String::atol(sId);
    int bIsPanel = String::isTrue(e->getString());
    if (bIsPanel) {
      hPanel = GetOrCreateItemHandle(sId);
    }
  }

  if (ApIsHandle(hPanel)) {
    SendGetPanelItemsRequest(hPanel);
    hPanel_ = hPanel;
  }
}

//-------------------------

void Inventory::SendGetPanelItemsRequest(const ApHandle& hPanel)
{
  ApHandle hRequest = Apollo::newHandle();

  GetPanelItemsRequest* pRequest = new GetPanelItemsRequest(this, hPanel);
  if (pRequest != 0) {
    AddRequest(hRequest, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = hRequest;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_GetProperties);
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("nItem", GetItemId(hPanel));
  msg.srpc.set("vlKeys", Item_PropertyId_Name " " Item_PropertyId_Nickname " " Item_PropertyId_PanelOrder " " Item_PropertyId_Slots " " Item_PropertyId_Contains);

  if (!msg.Request()) {
    DeleteRequest(hRequest);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetPanelDetails;
}

void Inventory::GetPanelItemsResponse(const ApHandle& hPanel, Apollo::SrpcMessage& kvProperties)
{
  String sName = kvProperties.getString(Item_PropertyId_Name);

  String sNickname = kvProperties.getString(Item_PropertyId_Nickname);
  
  int nPanelOrder = kvProperties.getInt(Item_PropertyId_PanelOrder);
  
  int nSlots = kvProperties.getInt(Item_PropertyId_Slots);
  
  String sContains = kvProperties.getString(Item_PropertyId_Contains);

  sName_ = sNickname;
  if (sName_.empty()) { sName_ = sName; }

  nOrder_ = nPanelOrder;

  nSlots_ = nSlots;

  if (!sContains.empty()) {
    SendGetItemsPropertiesResquest(hPanel, sContains);
  }

}

//-------------------------

void Inventory::SendGetItemsPropertiesResquest(const ApHandle& hPanel, const String& sContains)
{
  ApHandle hRequest = Apollo::newHandle();

  GetItemsPropertiesRequest* pRequest = new GetItemsPropertiesRequest(this, hPanel);
  if (pRequest != 0) {
    AddRequest(hRequest, pRequest);
  }

  Msg_Gm_SendRequest msg;
  msg.hRequest = hRequest;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_GetMultiItemProperties);
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("vlItems", sContains);
  msg.srpc.set("vlKeys", Item_PropertyId_Id " " Item_PropertyId_Name " " Item_PropertyId_Nickname " " Item_PropertyId_Icon32Url " " Item_PropertyId_Image100Url " " Item_PropertyId_Stacksize " " Item_PropertyId_Slot);

  if (!msg.Request()) {
    DeleteRequest(hRequest);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetItemDetails;
}

void Inventory::GetItemsPropertiesResponse(const ApHandle& hPanel, Apollo::SrpcMessage& kvIdKeyValues)
{
  for (Elem* e = 0; (e = kvIdKeyValues.Next(e)); ) {
    String sId = e->getName();
    long nId = String::atol(sId);
    if (nId > 0) {
      Apollo::KeyValueList kvProperties;
      kvIdKeyValues.getKeyValueList(sId, kvProperties);

      String sName = kvProperties[Item_PropertyId_Nickname].getString();
      if (sName.empty()) {
        sName = kvProperties[Item_PropertyId_Name].getString();
      }
      String sIcon = kvProperties[Item_PropertyId_Icon32Url].getString();
      int nSlot = kvProperties[Item_PropertyId_Slot].getInt();
      int nStacksize = kvProperties[Item_PropertyId_Stacksize].getInt();

      Item* pItem = new Item(sName, sIcon, nSlot, nStacksize);
      if (pItem != 0) {
        items_.Set(GetOrCreateItemHandle(sId), pItem);
      }
    }
  }

  nState_ = StateReady;

  PlayModel();
}

//-------------------------

void Inventory::PurgeModel()
{
  nState_ = NoState;
  hPanel_ = ApNoHandle;
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

  DeleteAllItemHandles();
}

String Inventory::GetScriptFunctionName()
{
  return Apollo::getModuleConfig(MODULE_NAME, "CallScriptSrpcFunctionName", "ReceiveMessageFromModule");
}

void Inventory::PlayModel()
{
  if (nState_ == StateReady) {
    // Dialog -> WebView
    Msg_WebView_CallScriptFunction::_(hDialog_, "", "PurgePanels");
    //Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "PurgePanels");

    { // Should be for all panels
      ApHandle hPanel = hPanel_;

      {
        Apollo::SrpcMessage srpc;
        srpc.set(Srpc::Key::Method, "AddPanel");
        srpc.set("hPanel", hPanel);
        srpc.set("sName", sName_);
        srpc.set("nOrder", nOrder_);
        srpc.set("nSlots", nSlots_);
        // Dialog -> WebView
        Msg_WebView_ViewCall::_(hDialog_, GetScriptFunctionName(), srpc);
        //Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
      }
  
      ItemListNode* node = 0;
      for (ItemListIterator iter(items_); (node = iter.Next()); ) {
        ApHandle hItem = node->Key();
        Item* pItem = node->Value();
        if (pItem != 0) {
          Apollo::SrpcMessage srpc;
          srpc.set(Srpc::Key::Method, "AddItem");
          srpc.set("hPanel", hPanel);
          srpc.set("hItem", hItem);
          srpc.set("sName", pItem->sName_);
          srpc.set("sIcon", pItem->sIcon_);
          srpc.set("nSlot", pItem->nSlot_);
          srpc.set("nStacksize", pItem->nStacksize_);
          // Dialog -> WebView
          Msg_WebView_ViewCall::_(hDialog_, GetScriptFunctionName(), srpc);
          //Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
        }
      }
    }

  }
}

//-------------------------

void Inventory::BeginDragItem(const ApHandle& hItem, int nLeft, int nTop, int nWidth, int nHeight, int nMouseX, int nMouseY, int nPinX, int nPinY)
{
  if (ApIsHandle(drag_.hView_)) {
    EndDragItem();
  }

  drag_.hItem_ = hItem;
  drag_.nPinX_ = nPinX;
  drag_.nPinY_ = nPinY;

  // Dialog -> WebView
  ApHandle hDialogView = hDialog_;
  //ApHandle hDialogView = Msg_Dialog_GetView::_(hDialog_);

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
  // Dialog -> WebView
  //{
  //  Msg_Dialog_GetContentRect msg;
  //  msg.hDialog = hDialog_;
  //  if (msg.Request()) {
  //    nContentLeft = msg.nLeft;
  //    nContentTop = msg.nTop;
  //  }
  //}

  int nAbsLeft = nInventoryLeft + nContentLeft + nMouseX - nPinX;
  int nAbsTop = nInventoryTop + nContentTop + nMouseY - nPinY;

  {
    Msg_Inventory_OnDragItemBegin msg;
    msg.hItem = drag_.hItem_;
    msg.Send();
  }
}

void Inventory::EndDragItem()
{
  if (ApIsHandle(drag_.hView_)) {
    {
      Msg_Inventory_OnDragItemEnd msg;
      msg.hItem = drag_.hItem_;
      msg.Send();
    }

    Msg_WebView_Destroy::_(drag_.hView_);

    drag_.hView_ = ApNoHandle;
    drag_.hItem_ = ApNoHandle;
  }
}

void Inventory::OnDragItemReady(const ApHandle& hView)
{
  if (drag_.hView_ == hView) {
    //if (!Msg_WebView_ViewCall::_(hView, "Start")) { throw ApException(LOG_CONTEXT, "Msg_WebView_ViewCall 'Start' failed"); }
    String sUrl;
    ItemListNode* pNode = items_.Find(drag_.hItem_);
    if (pNode != 0) {
      Item* pItem = pNode->Value();
      if (pItem != 0) {
        sUrl = pItem->sIcon_;
      }
    }
    List lArgs;
    lArgs.AddLast(sUrl);
    lArgs.AddLast(String::from(drag_.nPinX_));
    lArgs.AddLast(String::from(drag_.nPinY_));
    String sResult = Msg_WebView_CallScriptFunction::_(hView, "", "Start", lArgs);
  }
}

void Inventory::OnDragItemMove(const ApHandle& hView, int nLeft, int nTop, int nWidth, int nHeight)
{
  if (drag_.hView_ == hView) {
    Msg_Inventory_OnDragItemMove msg;
    msg.hItem = drag_.hItem_;
    msg.nLeft = nLeft;
    msg.nTop = nTop;
    msg.nWidth = nWidth;
    msg.nHeight = nHeight;
    msg.Send();
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

String Inventory::Test_DeleteAllItemHandles()
{
  String s;
  Inventory i;
  ApHandle h1 = i.CreateItemHandle("1");
  ApHandle h3 = i.CreateItemHandle("3");
  ApHandle h2 = i.CreateItemHandle("2");
  i.DeleteAllItemHandles();
  if (!s) { if (i.id2Handle_.Count()) { s = String::from(__LINE__); }}
  if (!s) { if (i.handle2Id_.Count()) { s = String::from(__LINE__); }}
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
  if (!s) { s = Test_DeleteAllItemHandles(); }

  return s;
}

