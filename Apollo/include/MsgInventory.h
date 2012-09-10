// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgInventory_h_INCLUDED)
#define MsgInventory_h_INCLUDED

#include "ApMessage.h"

// -> Inventory
class Msg_Inventory_Create: public ApRequestMessage
{
public:
  Msg_Inventory_Create() : ApRequestMessage("Inventory_Create") {}
  ApIN ApHandle hInventory;
};

// -> Inventory
class Msg_Inventory_Destroy: public ApRequestMessage
{
public:
  Msg_Inventory_Destroy() : ApRequestMessage("Inventory_Destroy") {}
  ApIN ApHandle hInventory;
};

// -> Inventory
class Msg_Inventory_Show: public ApRequestMessage
{
public:
  Msg_Inventory_Show() : ApRequestMessage("Inventory_Show"), bShow(1) {}
  ApIN ApHandle hInventory;
  ApIN int bShow;

  static int _(ApHandle hInventory, int bShow)
  {
    Msg_Inventory_Show msg;
    msg.hInventory = hInventory;
    msg.bShow = bShow;
    return msg.Request();
  }
};

#endif // !defined(MsgInventory_h_INCLUDED)
