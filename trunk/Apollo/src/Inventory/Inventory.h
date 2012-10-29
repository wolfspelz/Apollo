// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Inventory_H_INCLUDED)
#define Inventory_H_INCLUDED

#include "Request.h"

class Inventory
{
public:
  Inventory::Inventory()
    :bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
    ,nGrid_(0)
  {}
  virtual ~Inventory();

  void Create();
  void Destroy();
  void Show(int bShow);

  void OnOpened(const ApHandle& hDialog);
  void OnClosed(const ApHandle& hDialog);

  void SetVisibility(int bShow);
  int ConsumeResponse(const ApHandle& hRequest, Apollo::SrpcMessage& response);
  void Purge();
  void BuildPanes(Apollo::KeyValueList& kvValues);

protected:
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;

  ApHandleTree<Request*> requests_;
  ApHandle hCandidate_;
  ApHandle hDialog_;
  long nGrid_;
};

#endif // Inventory_H_INCLUDED
