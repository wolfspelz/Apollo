// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Item_H_INCLUDED)
#define Item_H_INCLUDED

class Item
{
public:
  Item()
    :nSlot_(0)
    ,nStacksize_(0)
    ,bIsRezable_(0)
    ,bRezzed_(0)
  {}
  virtual ~Item() {}

  void AddFieldsForDisplay(Apollo::SrpcMessage& srpc);

  String sName_;
  String sIcon_;
  int nSlot_;
  int nStacksize_;
  int bIsRezable_;
  int bRezzed_;

protected:
};

#endif // Item_H_INCLUDED
