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
  Item::Item(const String& sName, const String& sIcon, int nSlot, int nStacksize)
    :sName_(sName)
    ,sIcon_(sIcon)
    ,nSlot_(nSlot)
    ,nStacksize_(nStacksize)
  {}
  virtual ~Item() {}

  String sName_;
  String sIcon_;
  int nSlot_;
  int nStacksize_;

protected:
};

#endif // Item_H_INCLUDED
