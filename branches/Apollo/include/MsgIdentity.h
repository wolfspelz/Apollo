// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgIdentity_h_INCLUDED)
#define MsgIdentity_h_INCLUDED

#include "ApMessage.h"

// -> identity
// Load from URL (async, there will be Msg_Identity_RequestContainerComplete)
class Msg_Identity_RequestContainer: public ApRequestMessage
{
public:
  Msg_Identity_RequestContainer() : ApRequestMessage("Identity_RequestContainer") {}
  ApIN ApHandle hRequest;
  ApIN String sUrl;
  ApIN String sDigest;
};

// identity -> 
// in response to Msg_Identity_RequestContainer
class Msg_Identity_RequestContainerComplete: public ApNotificationMessage
{
public:
  Msg_Identity_RequestContainerComplete() : ApNotificationMessage("Identity_RequestContainerComplete"), bSuccess(0) {}
  ApIN ApHandle hRequest;
  ApIN int bSuccess;
  ApIN String sUrl;
  ApIN String sDigest;
  ApIN String sXml;
};

// identity -> identity
// Send identity data from HTTP client to module
class Msg_Identity_ReceiveContainer: public ApRequestMessage
{
public:
  Msg_Identity_ReceiveContainer() : ApRequestMessage("Identity_ReceiveContainer") {}
  ApIN ApHandle hRequest;
  ApIN String sUrl;
  ApIN String sDigest;
  ApIN String sMimeType;
  ApIN Buffer sbData;
};

// -> identity
class Msg_Identity_SetContainer: public ApRequestMessage
{
public:
  Msg_Identity_SetContainer() : ApRequestMessage("Identity_SetContainer") {}
  ApIN String sUrl;
  ApIN String sDigest;
  ApIN String sData;
};

// -> identity
// Save to local database
class Msg_Identity_SaveContainerToStorage: public ApRequestMessage
{
public:
  Msg_Identity_SaveContainerToStorage() : ApRequestMessage("Identity_SaveContainerToStorage") {}
  ApIN String sUrl;
};

// -> identity
class Msg_Identity_LoadContainerFromStorage: public ApRequestMessage
{
public:
  Msg_Identity_LoadContainerFromStorage() : ApRequestMessage("Identity_LoadContainerFromStorage") {}
  ApIN String sUrl;
};

// -> identity
class Msg_Identity_IsContainerAvailable: public ApRequestMessage
{
public:
  Msg_Identity_IsContainerAvailable() : ApRequestMessage("Identity_IsContainerAvailable"), bAvailable(0) {}
  ApIN String sUrl;
  ApIN String sDigest;
  ApOUT int bAvailable;
};

// -> identity
// Delete identity data from storage and cache
class Msg_Identity_DeleteContainer: public ApRequestMessage
{
public:
  Msg_Identity_DeleteContainer() : ApRequestMessage("Identity_DeleteContainer") {}
  ApIN String sUrl;
};

// -> identity
class Msg_Identity_DeleteContainerFromMemory: public ApRequestMessage
{
public:
  Msg_Identity_DeleteContainerFromMemory() : ApRequestMessage("Identity_DeleteContainerFromMemory") {}
  ApIN String sUrl;
};

// -> identity
class Msg_Identity_DeleteContainerFromStorage: public ApRequestMessage
{
public:
  Msg_Identity_DeleteContainerFromStorage() : ApRequestMessage("Identity_DeleteContainerFromStorage") {}
  ApIN String sUrl;
};

// -------------------------------------------------------------------
// Identity evaluation progress

// identity ->
class Msg_Identity_ContainerBegin: public ApNotificationMessage
{
public:
  Msg_Identity_ContainerBegin() : ApNotificationMessage("Identity_ContainerBegin") {}
  ApIN String sUrl;
};

// identity ->
class Msg_Identity_ItemAdded: public ApNotificationMessage
{
public:
  Msg_Identity_ItemAdded() : ApNotificationMessage("Identity_ItemAdded") {}
  ApIN String sUrl;
  ApIN String sId;
};

// identity ->
class Msg_Identity_ItemChanged: public ApNotificationMessage
{
public:
  Msg_Identity_ItemChanged() : ApNotificationMessage("Identity_ItemChanged") {}
  ApIN String sUrl;
  ApIN String sId;
};

// identity ->
class Msg_Identity_ItemDataAvailable: public ApNotificationMessage
{
public:
  Msg_Identity_ItemDataAvailable() : ApNotificationMessage("Identity_ItemDataAvailable") {}
  ApIN String sUrl;
  ApIN String sId;
  ApIN String sDigest;
  ApIN String sType;
  ApIN String sMimeType;
  ApIN int nOrder;
  ApIN int nSize;
  ApIN String sSrc;
  ApIN Buffer sbData;
  ApIN String sTrust;
};

// identity ->
class Msg_Identity_ItemRemoved: public ApNotificationMessage
{
public:
  Msg_Identity_ItemRemoved() : ApNotificationMessage("Identity_ItemRemoved") {}
  ApIN String sUrl;
  ApIN String sId;
};

// identity ->
class Msg_Identity_ContainerEnd: public ApNotificationMessage
{
public:
  Msg_Identity_ContainerEnd() : ApNotificationMessage("Identity_ContainerEnd") {}
  ApIN String sUrl;
};

// -------------------------------------------------------------------
// Getter

#define Msg_Identity_ItemType_Properties "properties"
#define Msg_Identity_ItemType_Avatar "avatar"

#define Msg_Identity_Trust_Unsigned "Unsigned"
#define Msg_Identity_Trust_Signed "Signed"
#define Msg_Identity_Trust_SignatureValid "SignatureValid"
#define Msg_Identity_Trust_SignatureInvalid "SignatureInvalid"
#define Msg_Identity_Trust_TrustedSignee "TrustedSignee"
#define Msg_Identity_Trust_UntrustedSignee "UntrustedSignee"

// -> identity
class Msg_Identity_HasProperty: public ApRequestMessage
{
public:
  Msg_Identity_HasProperty() : ApRequestMessage("Identity_HasProperty"), bIsProperty(0) {}
  ApIN String sUrl;
  ApIN String sKey;
  ApOUT int bIsProperty;
};

// -> identity
class Msg_Identity_GetProperty: public ApRequestMessage
{
public:
  Msg_Identity_GetProperty() : ApRequestMessage("Identity_GetProperty") {}
  ApIN String sUrl;
  ApIN String sKey;
  ApOUT String sValue;
  ApOUT String sTrust;
};

// -> identity
class Msg_Identity_GetItemIds: public ApRequestMessage
{
public:
  Msg_Identity_GetItemIds() : ApRequestMessage("Identity_GetItemIds"), nMax(0) {}
  ApIN String sUrl;
  ApIN String sType; // optional, if "" then all items
  ApIN Apollo::ValueList vlMimeTypes; // optional, if "" then all items
  ApIN int nMax; // 0 = all
  ApOUT Apollo::ValueList vlIds;
};

// -------------------------------------------------------------------
// Items

// -> identity
class Msg_Identity_GetItem: public ApRequestMessage
{
public:
  Msg_Identity_GetItem() : ApRequestMessage("Identity_GetItem"), nOrder(0), nSize(0) {}
  ApIN String sUrl;
  ApIN String sId;
  ApOUT String sDigest;
  ApOUT String sType;
  ApOUT String sMimeType;
  ApOUT int nOrder;
  ApOUT int nSize;
  ApOUT String sSrc;
  //ApOUT Buffer sbData;
  ApOUT String sTrust;
};

// -> identity
class Msg_Identity_IsItemDataAvailable: public ApRequestMessage
{
public:
  Msg_Identity_IsItemDataAvailable() : ApRequestMessage("Identity_IsItemDataAvailable"), bAvailable(0) {}
  ApIN String sUrl;
  ApIN String sId;
  ApOUT int bAvailable;
};

// -> identity
class Msg_Identity_GetItemData: public ApRequestMessage
{
public:
  Msg_Identity_GetItemData() : ApRequestMessage("Identity_GetItemData") {}
  ApIN String sUrl;
  ApIN String sId;
  ApOUT String sMimeType;
  ApOUT Buffer sbData;
  ApOUT String sTrust;
};

// -> identity
class Msg_Identity_RequestItem: public ApRequestMessage
{
public:
  Msg_Identity_RequestItem() : ApRequestMessage("Identity_RequestItem") {}
  ApIN ApHandle hRequest;
  ApIN String sUrl;
  ApIN String sId;
  ApIN String sSrc;
  ApIN String sDigest;
};

// identity -> 
// In response to Msg_Identity_RequestContainer
class Msg_Identity_RequestItemComplete: public ApNotificationMessage
{
public:
  Msg_Identity_RequestItemComplete() : ApNotificationMessage("Identity_RequestItemComplete"), bSuccess(0) {}
  ApIN ApHandle hRequest;
  ApIN int bSuccess;
  ApIN String sUrl;
  ApIN String sId;
  ApIN String sSrc;
  ApIN String sDigest;
  ApIN Buffer sbData;
};

// identity -> identity
// Send item data from HTTP client to module
class Msg_Identity_ReceiveItem: public ApRequestMessage
{
public:
  Msg_Identity_ReceiveItem() : ApRequestMessage("Identity_ReceiveItem") {}
  ApIN ApHandle hRequest;
  ApIN String sUrl;
  ApIN String sId;
  ApIN String sSrc;
  ApIN String sDigest;
  ApIN String sMimeType;
  ApIN Buffer sbData;
};

// identity -> identity
// Cache item data at the container object
class Msg_Identity_SetItemData: public ApRequestMessage
{
public:
  Msg_Identity_SetItemData() : ApRequestMessage("Identity_SetItemData") {}
  ApIN String sUrl;
  ApIN String sId;
  ApIN String sMimeType;
  ApIN Buffer sbData;
};

// identity -> identity
// Save item data to persistent storage from memory cache
class Msg_Identity_SaveItemDataToStorage: public ApRequestMessage
{
public:
  Msg_Identity_SaveItemDataToStorage() : ApRequestMessage("Identity_SaveItemDataToStorage") {}
  ApIN String sUrl;
  ApIN String sId;
};

// identity -> identity
// Load item data from persistent storage to memory cache
class Msg_Identity_LoadItemDataFromStorage: public ApRequestMessage
{
public:
  Msg_Identity_LoadItemDataFromStorage() : ApRequestMessage("Identity_LoadItemDataFromStorage") {}
  ApIN String sUrl;
  ApIN String sId;
};

// -> identity
// Delete item data from persistent storage and memory cache
class Msg_Identity_DeleteItemData: public ApRequestMessage
{
public:
  Msg_Identity_DeleteItemData() : ApRequestMessage("Identity_DeleteItemData") {}
  ApIN String sUrl;
  ApIN String sId;
};

// -> identity
// Delete item data from persistent storage and memory cache
class Msg_Identity_DeleteItemDataFromMemory: public ApRequestMessage
{
public:
  Msg_Identity_DeleteItemDataFromMemory() : ApRequestMessage("Identity_DeleteItemDataFromMemory") {}
  ApIN String sUrl;
  ApIN String sId;
};

// -> identity
// Delete item data from persistent storage and memory cache
class Msg_Identity_DeleteItemDataFromStorage: public ApRequestMessage
{
public:
  Msg_Identity_DeleteItemDataFromStorage() : ApRequestMessage("Identity_DeleteItemDataFromStorage") {}
  ApIN String sUrl;
  ApIN String sId;
};

// --------------------------------
// A bit more high level

// -> identity
// Request the item data. If necessary request the container first, then the item data
// Completed by Msg_Identity_AcquireItemDataComplete
class Msg_Identity_AcquireItemData: public ApRequestMessage
{
public:
  Msg_Identity_AcquireItemData() : ApRequestMessage("Identity_AcquireItemData") {}
  ApIN ApHandle hAcquisition;
  ApIN String sUrl;
  ApIN String sDigest;
  ApIN String sType;
  ApIN String sMimeType;
};

// -> identity
// In response to Msg_Identity_AcquireItemDataComplete
class Msg_Identity_AcquireItemDataComplete: public ApNotificationMessage
{
public:
  Msg_Identity_AcquireItemDataComplete() : ApNotificationMessage("Identity_AcquireItemDataComplete"), bSuccess(0) {}
  ApIN ApHandle hAcquisition;
  ApIN String sUrl;
  ApIN String sType;
  ApIN int bSuccess;
};

// -------------------------------------------------------------------
// Entire memory cache

// -> identity
// Clear memory cache
class Msg_Identity_ClearAllCache: public ApRequestMessage
{
public:
  Msg_Identity_ClearAllCache() : ApRequestMessage("Identity_ClearAllCache") {}
};

// -> identity
// Expire memory cache items
class Msg_Identity_ExpireAllCache: public ApRequestMessage
{
public:
  Msg_Identity_ExpireAllCache() : ApRequestMessage("Identity_ExpireAllCache"), nAge(0) {}
  ApIN int nAge;
};

// -------------------------------------------------------------------
// Persistent storage

// -> identity
// Expire memory cache items
class Msg_Identity_ClearAllStorage: public ApRequestMessage
{
public:
  Msg_Identity_ClearAllStorage() : ApRequestMessage("Identity_ClearAllStorage") {}
};

// -> identity
// Expire memory cache items
class Msg_Identity_ExpireAllStorage: public ApRequestMessage
{
public:
  Msg_Identity_ExpireAllStorage() : ApRequestMessage("Identity_ExpireAllStorage"), nAge(0) {}
  ApIN int nAge;
};

// -> identity
// Set and get the DB name
class Msg_Identity_SetStorageName: public ApRequestMessage
{
public:
  Msg_Identity_SetStorageName() : ApRequestMessage("Identity_SetStorageName") {}
  ApIN String sName;
  ApOUT String sPreviousName;
};

#endif // !defined(MsgIdentity_h_INCLUDED)
