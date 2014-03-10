// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Inventory_H_INCLUDED)
#define Inventory_H_INCLUDED

#include "Request.h"
#include "Item.h"
#include "ItemInfo.h"

//hw DragDropInventoryItem
//#include "DragItem.h"

typedef ApHandleTree<String> ItemHandle2IdList;
typedef ApHandleTreeNode<String> ItemHandle2IdListNode;
typedef ApHandleTreeIterator<String> ItemHandle2IdListIterator;

typedef StringTree<ApHandle> ItemId2HandleList;
typedef StringTreeNode<ApHandle> ItemId2HandleListNode;
typedef StringTreeIterator<ApHandle> ItemId2HandleListIterator;

typedef ApHandlePointerTree<Item*> ItemList;
typedef ApHandlePointerTreeNode<Item*> ItemListNode;
typedef ApHandlePointerTreeIterator<Item*> ItemListIterator;

typedef ApHandlePointerTree<ItemInfo*> ItemInfoList;
typedef ApHandlePointerTreeNode<ItemInfo*> ItemInfoListNode;
typedef ApHandlePointerTreeIterator<ItemInfo*> ItemInfoListIterator;

class Inventory
{
public:
  Inventory()
    :bVisible_(0)
    ,nState_(NoState)
    ,nOrder_(0)
  {}
  virtual ~Inventory();

  void Create();
  void Destroy();
  void Show(int bShow);

  void OnOpened(const ApHandle& hDialog);
  void OnClosed(const ApHandle& hDialog);
  //hw DragDropInventoryItem
  //void OnDragItemReady(const ApHandle& hView);
  //void OnDragItemMove(const ApHandle& hView, int nLeft, int nTop, int nWidth, int nHeight);
  //void OnDragItemLostFocus(const ApHandle& hView);
  void OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

  int HasDialog(const ApHandle& hDialog) { return hDialog_ == hDialog || hCandidate_ == hDialog; }

  ItemInfo* FindItemInfoByDialog(const ApHandle& hDialog);
  int HasItemInfo(const ApHandle& hDialog);
  void OnItemInfoOpened(const ApHandle& hDialog);
  void OnItemInfoClosed(const ApHandle& hDialog);
  void OnItemInfoModuleCall(const ApHandle& hDialog, Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

  //hw DragDropInventoryItem
  //int HasDragItem(const ApHandle& hView) { return drag_.GetView() == hView; }

  static String TestItemId2HandleMapper();
  static String Test_CreateItemHandle();
  static String Test_MultipleCreateItemHandle();
  static String Test_DeleteItemId();
  static String Test_DeleteItemHandle();
  static String Test_GetOrCreateItemHandle();
  static String Test_CreateItemHandleDuplicateEntryException();
  static String Test_DeleteAllItemHandles();

protected:
  String GetScriptFunctionName();
  
  void SetVisibility(int bShow);
  void BuildPanels();
  void PurgeModel();
  void PlayModel();

  void AddRequest(const ApHandle& hRequest, Request* pRequest);
  void DeleteRequest(const ApHandle& hRequest);
  friend class InventoryModule;
  int ConsumeResponse(const ApHandle& hRequest, Apollo::SrpcMessage& response);

  friend class GetPanelsRequest;
  void SendGetPanelsRequest();
  void GetPanelsResponse(Apollo::SrpcMessage& kvIdValues);

  friend class GetPanelItemsRequest;
  void SendGetPanelItemsRequest(const ApHandle& hPanel);
  void GetPanelItemsResponse(const ApHandle& hPanel, Apollo::SrpcMessage& kvProperties);

  friend class GetItemsPropertiesRequest;
  void SendGetItemsPropertiesResquest(const ApHandle& hPanel, const String& sContains);
  void GetItemsPropertiesResponse(const ApHandle& hPanel, Apollo::SrpcMessage& kvIdKeyValues);

  void OpenItemInfo(const ApHandle& hItem, int nX, int nY);
  void CloseItemInfo(const ApHandle& hItem);

  //hw DragDropInventoryItem
  //void BeginDragItem(const ApHandle& hItem, int nLeft, int nTop, int nWidth, int nHeight, int nMouseX, int nMouseY, int nPinX, int nPinY);
  //void EndDragItem();

public: // for ItemInfo
  Item* FindItem(const ApHandle& hItem);
protected:
  // ItemId to Handle mapper
  ApHandle GetOrCreateItemHandle(const String& sItem);
  ApHandle CreateItemHandle(const String& sItem);
  ApHandle GetItemHandle(const String& sItem);
  String GetItemId(const ApHandle& hItem);
  void DeleteItemId(const String& sItem);
  void DeleteItemHandle(const ApHandle& hItem);
  void DeleteAllItemHandles();
  ItemHandle2IdList handle2Id_;
  ItemId2HandleList id2Handle_;

protected:
  int bVisible_;
  
  ApHandleTree<Request*> requests_;

  enum { NoState = 0
    ,StateGetPanels = 1
    ,StateGetPanelProperties = 2
    ,StateGetItemProperties = 3
    ,StateReady = 4
  };

  int nState_;

  ApHandle hPanel_;
  String sName_;
  int nOrder_;
  int nSlots_;
  ItemList items_;

  ApHandle hCandidate_;
  ApHandle hDialog_;

  ItemInfoList itemInfos_;

  //hw DragDropInventoryItem
  //DragItem drag_;
};

#endif // Inventory_H_INCLUDED
