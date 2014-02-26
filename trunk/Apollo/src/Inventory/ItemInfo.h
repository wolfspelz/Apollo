// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ItemInfo_H_INCLUDED)
#define ItemInfo_H_INCLUDED

class ItemInfo
{
public:
  ItemInfo(ApHandle hItem)
    :hItem_(hItem)
  {}
  virtual ~ItemInfo();

  void Create(int nX, int nY, int nWidth, int nHeight);
  void Destroy();
  void BringToFront();
  void OnClosed();

  ApHandle& GetItem() { return hItem_; }
  ApHandle& GetDialog() { return hDialog_; }

protected:
  ApHandle hItem_;
  ApHandle hDialog_;
};

#endif // ItemInfo_H_INCLUDED
