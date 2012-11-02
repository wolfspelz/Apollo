// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Inventory_H_INCLUDED)
#define Inventory_H_INCLUDED

#include "Request.h"
#include "Item.h"

typedef PointerTree<String, Item*, LessThan<String> > ItemList;
typedef PointerTreeNode<String, Item*> ItemListNode;
typedef PointerTreeIterator<String, Item*, LessThan<String> > ItemListIterator;

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
  {}
  virtual ~Inventory();

  void Create();
  void Destroy();
  void Show(int bShow);

  void OnOpened(const ApHandle& hDialog);
  void OnClosed(const ApHandle& hDialog);
  void OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

  int HasCandidate(const ApHandle& hCandidate) { return hCandidate_ == hCandidate; }
  int HasDialog(const ApHandle& hDialog) { return hDialog_ == hDialog; }

protected:
  void SetVisibility(int bShow);
  void BuildGrids();
  void PurgeModel();
  void PlayModel();

  void AddRequest(const ApHandle& hRequest, Request* pRequest);
  void DeleteRequest(const ApHandle& hRequest);
  friend class InventoryModule;
  int ConsumeResponse(const ApHandle& hRequest, Apollo::SrpcMessage& response);

  friend class GetGridsRequest;
  void GetGridsResponse(Apollo::SrpcMessage& kvIdValues);

  friend class GetGridItemsRequest;
  void GetGridItemsResponse(long nGrid, Apollo::SrpcMessage& kvProperties);

  friend class GetItemsPropertiesRequest;
  void GetItemsPropertiesResponse(long nGrid, Apollo::SrpcMessage& kvIdKeyValues);

protected:
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;

  ApHandleTree<Request*> requests_;

  enum { NoState = 0
    ,StateGetGrids = 1
    ,StateGetGridDetails = 2
    ,StateGetItemDetails = 3
    ,StateReady = 4
  };

  int nState_;

  String sGridId_;
  String sName_;
  int nOrder_;
  int nSlots_;
  ItemList items_;

  ApHandle hCandidate_;
  ApHandle hDialog_;
};

#endif // Inventory_H_INCLUDED
