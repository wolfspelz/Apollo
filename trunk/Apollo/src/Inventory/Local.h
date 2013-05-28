// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Local_H_INCLUDED)
#define Local_H_INCLUDED

#include "ApLog.h"
#include "MsgInventory.h"
#include "MsgTranslation.h"
#include "MsgGm.h"
#include "MsgWebView.h"
#include "MsgDialog.h"
#include "InventoryModule.h"

#define MODULE_NAME "Inventory"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

#define Item_PropertyId_Id "Id"
#define Item_PropertyId_Name "Name"
#define Item_PropertyId_Nickname "Nickname"
#define Item_PropertyId_Icon32Url "Icon32Url"
#define Item_PropertyId_Image100Url "Image100Url"
#define Item_PropertyId_Stacksize "Stacksize"
#define Item_PropertyId_Slot "Slot"
#define Item_PropertyId_Slots "Slots"
#define Item_PropertyId_PanelOrder "PanelOrder"
#define Item_PropertyId_IsPanel "IsPanel"
#define Item_PropertyId_Contains "Contains"

#define Gm_ItemProtocol_GetItemIdsAndValuesByProperty "Item.GetItemIdsAndValuesByProperty"
#define Gm_ItemProtocol_GetProperties "Item.GetProperties"
#define Gm_ItemProtocol_GetMultiItemProperties "Item.GetMultiItemProperties"

#endif // Local_H_INCLUDED
