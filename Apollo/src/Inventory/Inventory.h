// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Inventory_H_INCLUDED)
#define Inventory_H_INCLUDED

class Inventory
{
public:
  Inventory::Inventory(const ApHandle& hInventory)
    :hAp_(hInventory)
    ,bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
  {}
  virtual ~Inventory();

  inline ApHandle apHandle() { return hAp_; }

  void Create();
  void Destroy();
  void Show(int bShow);

protected:
  ApHandle hAp_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;
};

#endif // Inventory_H_INCLUDED
