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

Item* Inventory::FindItem(const ApHandle& hItem)
{
  Item* pItem = 0;
  for (ItemListNode* pNode = 0; (pNode = items_.Next(pNode)) != 0; ) {
    if (pNode->Key() == hItem) {
      pItem = pNode->Value();
    }
  }
  return pItem;
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

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog; 
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 0;
  msg.sCaption = Msg_Translation_Get::_(MODULE_NAME, "", "Your Inventory");
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Inventory.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  //hw DragDropInventoryItem
  //drag_.Init();

  hCandidate_ = hDialog;
}

void Inventory::Destroy()
{
  if (ApIsHandle(hDialog_)) {
    Msg_Dialog_Destroy msg;
    msg.hDialog = hDialog_;
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
    hDialog_ = ApNoHandle;
  }

  //hw DragDropInventoryItem
  //drag_.Exit();
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

  if (0) {
  } else if (sMethod == "OnPlayModel") {
    PlayModel();

  } else if (sMethod == "OpenItemInfo") {
    OpenItemInfo(request.getHandle("hItem")
      , request.getInt("nMouseX")
      , request.getInt("nMouseY")
      );

  //hw DragDropInventoryItem
  //} else if (sMethod == "OnDragItem") {
  //  BeginDragItem(request.getHandle("hItem")
  //    ,request.getInt("nLeft")
  //    ,request.getInt("nTop")
  //    ,request.getInt("nWidth")
  //    ,request.getInt("nHeight")
  //    ,request.getInt("nMouseX")
  //    ,request.getInt("nMouseY")
  //    ,request.getInt("nPinX")
  //    ,request.getInt("nPinY")
  //    );
  //} else if (sMethod == "OnEndDrag") {
  //  EndDragItem();

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

  nState_ = StateGetPanelProperties;
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
  msg.srpc.set("vlKeys", 
    Item_PropertyId_Id " " 
    Item_PropertyId_Name " " 
    Item_PropertyId_Nickname " " 
    Item_PropertyId_Icon32Url " " 
    Item_PropertyId_Image100Url " " 
    Item_PropertyId_Slot " " 
    Item_PropertyId_Stacksize " " 
    Item_PropertyId_IsRezable " " 
    Item_PropertyId_Rezzed " "
    Item_PropertyId_RezzedDestination " "
    Item_PropertyId_RezzedJid " "
    );

  if (!msg.Request()) {
    DeleteRequest(hRequest);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }

  nState_ = StateGetItemProperties;
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

      String sImage = kvProperties[Item_PropertyId_Image100Url].getString();
      if (sImage.empty()) {
        sImage = kvProperties[Item_PropertyId_Icon32Url].getString();
      }

      Item* pItem = new Item();
      if (pItem != 0) {
        pItem->sName_ = sName; 
        pItem->sIcon_ = kvProperties[Item_PropertyId_Icon32Url].getString(); 
        pItem->sImage_ = sImage; 
        pItem->nSlot_ = kvProperties[Item_PropertyId_Slot].getInt(); 
        pItem->nStacksize_ = kvProperties[Item_PropertyId_Stacksize].getInt();
        pItem->bIsRezable_ = kvProperties[Item_PropertyId_IsRezable].getInt();
        pItem->bRezzed_ = kvProperties[Item_PropertyId_Rezzed].getInt();
        pItem->sRezzedDestination_ = kvProperties[Item_PropertyId_RezzedDestination].getString();
        pItem->sRezzedJid_ = kvProperties[Item_PropertyId_RezzedJid].getString();

        items_.Set(GetOrCreateItemHandle(sId), pItem);
      }
    }
  }

  nState_ = StateReady;

  PlayModel();
}

void Inventory::RezToLocation(const ApHandle& hItem, const ApHandle& hLocation)
{
  String sLocation;
  String sDestination;

  {
    Msg_VpView_GetLocationDetail VVGLDmsg;
    VVGLDmsg.hLocation = hLocation;
    VVGLDmsg.sKey = Msg_VpView_LocationDetail_LocationUrl;
    ApOUT String sValue;
    ApOUT String sMimeType;
    if (!VVGLDmsg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_VpView_GetLocationDetail(Msg_VpView_LocationDetail_LocationUrl) failed for loc=" ApHandleFormat " item=" ApHandleFormat "", ApHandlePrintf(hLocation), ApHandlePrintf(hItem)));
    } else {
      sLocation = VVGLDmsg.sValue;
    }
  }

  {
    Msg_VpView_GetLocationContexts msg;
    msg.hLocation = hLocation;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_VpView_GetLocationContexts failed for for loc=" ApHandleFormat " item=" ApHandleFormat "", ApHandlePrintf(hLocation), ApHandlePrintf(hItem)));
    } else {
      for (Apollo::ValueElem* e = 0; (e = msg.vlContexts.nextElem(e)) != 0; ) {
        Msg_VpView_GetContextDetail msgVVGCD;
        msgVVGCD.hContext = e->getHandle();
        msgVVGCD.sKey = Msg_VpView_ContextDetail_DocumentUrl;
        if (!msgVVGCD.Request()) {
          apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_VpView_GetContextDetail(Msg_VpView_ContextDetail_DocumentUrl) failed for for loc=" ApHandleFormat " item=" ApHandleFormat "", ApHandlePrintf(hLocation), ApHandlePrintf(hItem)));
        } else {
          if (msgVVGCD.sValue) {
            sDestination = msgVVGCD.sValue;
            break;
          }
        }
      }
    }
  }

  if (sLocation && sDestination) {
    SendRezToLocationRequest(hItem, sLocation, sDestination, 300);
  }
}

void Inventory::SendRezToLocationRequest(const ApHandle& hItem, const String& sLocation, const String& sDestination, int nX)
{
  ApHandle hRequest = Apollo::newHandle();

  RezToLocationRequest* pRequest = new RezToLocationRequest(this, hItem, sLocation, sDestination);
  if (pRequest != 0) {
    AddRequest(hRequest, pRequest);
  }
  
  Msg_Gm_SendRequest msg;
  msg.hRequest = hRequest;
  msg.srpc.set(Srpc::Key::Method, Gm_ItemProtocol_RezToLocation);
  msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
  msg.srpc.set("nItem", GetItemId(hItem));
  msg.srpc.set("sLocation", sLocation);
  msg.srpc.set("sDestination", sDestination);
  msg.srpc.set("sX", nX);

  if (!msg.Request()) {
    DeleteRequest(hRequest);
    throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed");
  }
}

void Inventory::RezToLocationResponse(const ApHandle& hItem, const String& sLocation, const String& sDestination, Apollo::SrpcMessage& kvIdKeyValues)
{
  Item* pItem = FindItem(hItem);
  if (pItem != 0) {
    pItem->bRezzing_ = true;
  }
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
    Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), "PurgePanels");

    { // Should be for all panels
      ApHandle hPanel = hPanel_;

      {
        Apollo::SrpcMessage srpc;
        srpc.set(Srpc::Key::Method, "AddPanel");
        srpc.set("hPanel", hPanel);
        srpc.set("sName", sName_);
        srpc.set("nOrder", nOrder_);
        srpc.set("nSlots", nSlots_);

        Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
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
          pItem->AddFieldsForDisplay(srpc);

          Msg_Dialog_ContentCall::_(hDialog_, GetScriptFunctionName(), srpc);
        }
      }
    }

  }
}

//-------------------------

ItemInfo* Inventory::FindItemInfoByDialog(const ApHandle& hDialog)
{
  ItemInfo* p = 0;

  for (ItemInfoListNode* pNode = 0; (pNode = itemInfos_.Next(pNode)) != 0; ) {
    ItemInfo* pItemInfo = pNode->Value();
    if (pItemInfo->GetDialog() == hDialog) {
      p = pItemInfo;
    }
  }

  return p;
}

int Inventory::HasItemInfo(const ApHandle& hDialog)
{
  return (FindItemInfoByDialog(hDialog) != 0);
}

void Inventory::OpenItemInfo(const ApHandle& hItem, int nX, int nY)
{
  ItemInfoListNode* pNode = itemInfos_.Find(hItem);
  if (pNode != 0) {
    ItemInfo* pItemInfo = pNode->Value();
    pItemInfo->BringToFront();
  } else {

    Msg_Dialog_GetContentRect msgDGCR;
    msgDGCR.hDialog = hDialog_;
    msgDGCR.Request();

    Msg_WebView_GetPosition msgWGP;
    msgWGP.hView = Msg_Dialog_GetView::_(hDialog_);
    msgWGP.Request();

    ItemInfo* pItemInfo = new ItemInfo(*this, hItem);
    if (pItemInfo != 0) {

      String sTitle = Msg_Translation_Get::_(MODULE_NAME, "", "Item");
      int nLeft = msgWGP.nLeft + msgDGCR.nLeft + nX;
      int nTop = msgWGP.nTop + msgDGCR.nTop + nY;
      int nWidth = 0; // Create will default 
      int nHeight = 0; // Create will default
      pItemInfo->Create(sTitle, nLeft, nTop, nWidth, nHeight);
      itemInfos_.Set(hItem, pItemInfo);
    }

  }
}

void Inventory::CloseItemInfo(const ApHandle& hItem)
{
  ItemInfoListNode* pNode = itemInfos_.Find(hItem);
  if (pNode != 0) {
    ItemInfo* pItemInfo = pNode->Value();
    pItemInfo->Destroy();
    itemInfos_.Unset(hItem);
    delete pItemInfo;
    pItemInfo = 0;
  }
}

void Inventory::OnItemInfoOpened(const ApHandle& hDialog)
{
  ItemInfo* pItemInfo = FindItemInfoByDialog(hDialog);
  if (pItemInfo != 0) {
    pItemInfo->OnOpened();
  }
}

void Inventory::OnItemInfoClosed(const ApHandle& hDialog)
{
  ItemInfo* pItemInfo = FindItemInfoByDialog(hDialog);
  if (pItemInfo != 0) {
    pItemInfo->OnClosed();
    ApHandle hItem = pItemInfo->GetItem();
    itemInfos_.Unset(hItem);
    delete pItemInfo;
    pItemInfo = 0;
  }
}

void Inventory::OnItemInfoModuleCall(const ApHandle& hDialog, Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  ItemInfo* pItemInfo = FindItemInfoByDialog(hDialog);
  if (pItemInfo != 0) {
    pItemInfo->OnModuleCall(request, response);
  }
}

//hw DragDropInventoryItem
//void Inventory::BeginDragItem(const ApHandle& hItem, int nLeft, int nTop, int nWidth, int nHeight, int nMouseX, int nMouseY, int nPinX, int nPinY)
//{
//  drag_.SetItem(hItem);
//
//  String sImage;
//  ItemListNode* pNode = items_.Find(hItem);
//  if (pNode != 0) {
//    sImage = pNode->Value()->sIcon_;
//  }
//  drag_.SetImage(sImage);
//
//  // Dialog -> WebView
//  //ApHandle hDialogView = hDialog_;
//  ApHandle hDialogView = Msg_Dialog_GetView::_(hDialog_);
//
//  int nInventoryLeft = 0;
//  int nInventoryTop = 0;
//  {
//    Msg_WebView_GetPosition msg;
//    msg.hView = hDialogView;
//    if (msg.Request()) {
//      nInventoryLeft = msg.nLeft;
//      nInventoryTop = msg.nTop;
//    }
//  }
//
//  int nContentLeft = 0;
//  int nContentTop = 0;
//  // Dialog -> WebView
//  {
//    Msg_Dialog_GetContentRect msg;
//    msg.hDialog = hDialog_;
//    if (msg.Request()) {
//      nContentLeft = msg.nLeft;
//      nContentTop = msg.nTop;
//    }
//  }
//
//  int nAbsLeft = nInventoryLeft + nContentLeft + nMouseX - nPinX;
//  int nAbsTop = nInventoryTop + nContentTop + nMouseY - nPinY;
//
//  drag_.SetPosition(nAbsLeft, nAbsTop, nWidth, nHeight, nPinX, nPinY);
//
//  {
//    Msg_Inventory_OnDragItemBegin msg;
//    msg.hItem = hItem;
//    msg.Send();
//  }
//
//  drag_.Show();
//}
//
//void Inventory::EndDragItem()
//{
//  if (ApIsHandle(drag_.GetView())) {
//
//    if (drag_.GetInDropZone()) {
//      Msg_Inventory_OnDragItemDrop msg;
//      msg.hItem = drag_.GetItem();
//      msg.Send();
//    } else {
//      Msg_Inventory_OnDragItemCancel msg;
//      msg.hItem = drag_.GetItem();
//      msg.Send();
//    }
//
//    {
//      Msg_Inventory_OnDragItemEnd msg;
//      msg.hItem = drag_.GetItem();
//      msg.Send();
//    }
//
//    drag_.Destroy();
//    drag_.Create();
//  }
//}
//
//void Inventory::OnDragItemReady(const ApHandle& hView)
//{
//  if (drag_.GetView() == hView) {
//    String sResult = Msg_WebView_CallScriptFunction::_(hView, "", "Start");
//  }
//}
//
//void Inventory::OnDragItemMove(const ApHandle& hView, int nLeft, int nTop, int nWidth, int nHeight)
//{
//  if (drag_.GetView() == hView) {
//    Msg_Inventory_OnDragItemMove msg;
//    msg.hItem = drag_.GetItem();
//    msg.nLeft = nLeft;
//    msg.nTop = nTop;
//    msg.nWidth = nWidth;
//    msg.nHeight = nHeight;
//    msg.Send();
//  }
//}
//
//void Inventory::OnDragItemLostFocus(const ApHandle& hView)
//{
//  if (drag_.GetView() == hView) {
//    EndDragItem();
//  }
//}

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

