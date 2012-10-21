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
};

// -> Inventory
class Msg_Inventory_Destroy: public ApRequestMessage
{
public:
  Msg_Inventory_Destroy() : ApRequestMessage("Inventory_Destroy") {}
};

// -> Inventory
class Msg_Inventory_Show: public ApRequestMessage
{
public:
  Msg_Inventory_Show() : ApRequestMessage("Inventory_Show"), bShow(1) {}
  ApIN int bShow;

  static int _(int bShow)
  {
    Msg_Inventory_Show msg;
    msg.bShow = bShow;
    return msg.Request();
  }
};

#endif // !defined(MsgInventory_h_INCLUDED)
