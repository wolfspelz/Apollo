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

//hw DragDropInventoryItem
//// Item drag begins
//// Inventory -> 
//class Msg_Inventory_OnDragItemBegin: public ApNotificationMessage
//{
//public:
//  Msg_Inventory_OnDragItemBegin() : ApNotificationMessage("Inventory_OnDragItemBegin") {}
//  ApIN ApHandle hItem;
//};
//
//// Item drag move mouse event with screen coordinates
//// Inventory -> 
//class Msg_Inventory_OnDragItemMove: public ApNotificationMessage
//{
//public:
//  Msg_Inventory_OnDragItemMove() : ApNotificationMessage("Inventory_OnDragItemMove"), nLeft(0), nTop(0), nWidth(0), nHeight(0) {}
//  ApIN ApHandle hItem;
//  ApIN int nLeft;
//  ApIN int nTop;
//  ApIN int nWidth;
//  ApIN int nHeight;
//};
//
//// Item dropped at screen coordinates
//// Inventory -> 
//class Msg_Inventory_OnDragItemDrop: public ApNotificationMessage
//{
//public:
//  Msg_Inventory_OnDragItemDrop() : ApNotificationMessage("Inventory_OnDragItemDrop") {}
//  ApIN ApHandle hItem;
//  ApIN int nLeft;
//  ApIN int nTop;
//};
//
//// Item drag cancelled = not dropped
//// Inventory -> 
//class Msg_Inventory_OnDragItemCancel: public ApNotificationMessage
//{
//public:
//  Msg_Inventory_OnDragItemCancel() : ApNotificationMessage("Inventory_OnDragItemCancel") {}
//  ApIN ApHandle hItem;
//};
//
//// Item drag ends here
//// Inventory -> 
//class Msg_Inventory_OnDragItemEnd: public ApNotificationMessage
//{
//public:
//  Msg_Inventory_OnDragItemEnd() : ApNotificationMessage("Inventory_OnDragItemEnd") {}
//  ApIN ApHandle hItem;
//};

#endif // !defined(MsgInventory_h_INCLUDED)
