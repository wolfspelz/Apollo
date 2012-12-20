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
class Msg_Inventory_DragItemBegin: public ApNotificationMessage
{
public:
  Msg_Inventory_DragItemBegin() : ApNotificationMessage("Inventory_DragItemBegin") {}
  ApIN ApHandle hDrag;
  ApIN String sItem; // Item Id
};

// Item drag move mouse event with screen coordinates
// Inventory -> 
class Msg_Inventory_DragItemMove: public ApNotificationMessage
{
public:
  Msg_Inventory_DragItemMove() : ApNotificationMessage("Inventory_DragItemMove"), nX(0), nY(0) {}
  ApIN ApHandle hDrag;
  ApIN int nX;
  ApIN int nY;
};

// Item dropped at screen coordinates
// Inventory -> 
class Msg_Inventory_DragItemDrop: public ApNotificationMessage
{
public:
  Msg_Inventory_DragItemDrop() : ApNotificationMessage("Inventory_DragItemDrop") {}
  ApIN ApHandle hDrag;
  ApIN int nX;
  ApIN int nY;
};

// Item drag cancelled = not dropped
// Inventory -> 
class Msg_Inventory_DragItemCancel: public ApNotificationMessage
{
public:
  Msg_Inventory_DragItemCancel() : ApNotificationMessage("Inventory_DragItemCancel") {}
  ApIN ApHandle hDrag;
};

// Item drag ends here
// Inventory -> 
class Msg_Inventory_DragItemEnd: public ApNotificationMessage
{
public:
  Msg_Inventory_DragItemEnd() : ApNotificationMessage("Inventory_DragItemEnd") {}
  ApIN ApHandle hDrag;
};

#endif // !defined(MsgInventory_h_INCLUDED)
