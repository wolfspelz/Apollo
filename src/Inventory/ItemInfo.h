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
  ItemInfo(Inventory& inventory, ApHandle hItem)
    :inventory_(inventory)
    ,hItem_(hItem)
    ,bOpen_(0)
  {}
  virtual ~ItemInfo();

  void Create(const String& sTitle, int nX, int nY, int nWidth, int nHeight);
  void Destroy();
  void OnOpened();
  void OnClosed();
  void OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);
  void BringToFront();
  
  ApHandle& GetItem() { return hItem_; }
  ApHandle& GetDialog() { return hDialog_; }

protected:
  Inventory& inventory_;
  ApHandle hItem_;
  ApHandle hDialog_;
  int bOpen_;
};

#endif // ItemInfo_H_INCLUDED
