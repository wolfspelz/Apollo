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

// Item drag begins
// Inventory -> 
class Msg_Inventory_OnDragItemBegin: public ApNotificationMessage
{
public:
  Msg_Inventory_OnDragItemBegin() : ApNotificationMessage("Inventory_OnDragItemBegin") {}
  ApIN ApHandle hDrag;
  ApIN String sItem; // Item Id
};

// Item drag move mouse event with screen coordinates
// Inventory -> 
class Msg_Inventory_OnDragItemMove: public ApNotificationMessage
{
public:
  Msg_Inventory_OnDragItemMove() : ApNotificationMessage("Inventory_OnDragItemMove"), nX(0), nY(0) {}
  ApIN ApHandle hDrag;
  ApIN int nX;
  ApIN int nY;
};

// Item dropped at screen coordinates
// Inventory -> 
class Msg_Inventory_OnDragItemDrop: public ApNotificationMessage
{
public:
  Msg_Inventory_OnDragItemDrop() : ApNotificationMessage("Inventory_OnDragItemDrop") {}
  ApIN ApHandle hDrag;
  ApIN int nX;
  ApIN int nY;
};

// Item drag cancelled = not dropped
// Inventory -> 
class Msg_Inventory_OnDragItemCancel: public ApNotificationMessage
{
public:
  Msg_Inventory_OnDragItemCancel() : ApNotificationMessage("Inventory_OnDragItemCancel") {}
  ApIN ApHandle hDrag;
};

// Item drag ends here
// Inventory -> 
class Msg_Inventory_OnDragItemEnd: public ApNotificationMessage
{
public:
  Msg_Inventory_OnDragItemEnd() : ApNotificationMessage("Inventory_OnDragItemEnd") {}
  ApIN ApHandle hDrag;
};

#endif // !defined(MsgInventory_h_INCLUDED)
