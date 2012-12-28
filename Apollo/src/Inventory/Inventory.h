// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Inventory_H_INCLUDED)
#define Inventory_H_INCLUDED

#include "Request.h"
#include "Item.h"

typedef ApHandleTree<String> ItemHandle2IdList;
typedef ApHandleTreeNode<String> ItemHandle2IdListNode;
typedef ApHandleTreeIterator<String> ItemHandle2IdListIterator;

typedef StringTree<ApHandle> ItemId2HandleList;
typedef StringTreeNode<ApHandle> ItemId2HandleListNode;
typedef StringTreeIterator<ApHandle> ItemId2HandleListIterator;

typedef ApHandlePointerTree<Item*> ItemList;
typedef ApHandlePointerTreeNode<Item*> ItemListNode;
typedef ApHandlePointerTreeIterator<Item*> ItemListIterator;

class Inventory
{
public:
  Inventory::Inventory()
    :bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
    ,nState_(NoState)
    ,nOrder_(0)
    ,nDragOffsetX_(0)
    ,nDragOffsetY_(0)
  {}
  virtual ~Inventory();

  void Create();
  void Destroy();
  void Show(int bShow);

  void OnOpened(const ApHandle& hDialog);
  void OnClosed(const ApHandle& hDialog);
  void OnDragItemReady(const ApHandle& hView);
  void OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

  int HasDialog(const ApHandle& hDialog) { return hDialog_ == hDialog || hCandidate_ == hDialog; }
  int HasDragItem(const ApHandle& hView) { return hDragItemView_ == hView; }

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

  void BeginDragItem(const ApHandle& hItem, int nLeft, int nTop, int nWidth, int nHeight, int nOffsetX, int nOffsetY);
  void EndDragItem();

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
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;

  ApHandleTree<Request*> requests_;

  enum { NoState = 0
    ,StateGetPanels = 1
    ,StateGetPanelDetails = 2
    ,StateGetItemDetails = 3
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

  ApHandle hDragItemView_;
  ApHandle hDragItem_;
  int nDragOffsetX_;
  int nDragOffsetY_;
};

#endif // Inventory_H_INCLUDED
