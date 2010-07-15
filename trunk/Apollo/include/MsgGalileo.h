// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgGalileo_h_INCLUDED)
#define MsgGalileo_h_INCLUDED

#include "ApMessage.h"

// -------------------------------------------------------------------
// Used internally, can be hooked and changed by other modules

// Galileo -> Galileo
class Msg_Galileo_RequestAnimation: public ApRequestMessage
{
public:
  Msg_Galileo_RequestAnimation() : ApRequestMessage("Galileo_RequestAnimation") {}
  ApIN ApHandle hRequest;
  ApIN String sUrl;
};

// Galileo -> Galileo
// In response to Msg_Galileo_RequestAnimation
class Msg_Galileo_RequestAnimationComplete: public ApNotificationMessage
{
public:
  Msg_Galileo_RequestAnimationComplete() : ApNotificationMessage("Galileo_RequestAnimationComplete"), bSuccess(0) {}
  ApIN ApHandle hRequest;
  ApIN int bSuccess;
  ApIN String sUrl;
  ApIN Buffer sbData;
  ApIN String sMimeType;
};

// -------------------------------------------------------------------
// Persistent storage

// -> Galileo
// Set and get the DB name
class Msg_Galileo_SetStorageName: public ApRequestMessage
{
public:
  Msg_Galileo_SetStorageName() : ApRequestMessage("Galileo_SetStorageName") {}
  ApIN String sName;
  ApOUT String sPreviousName;
};

// Galileo -> Galileo
// Save animation file to persistent storage
class Msg_Galileo_SaveAnimationDataToStorage: public ApRequestMessage
{
public:
  Msg_Galileo_SaveAnimationDataToStorage() : ApRequestMessage("Galileo_SaveAnimationDataToStorage") {}
  ApIN String sUrl;
  ApIN Buffer sbData;
  ApIN String sMimeType;
};

// Galileo -> Galileo
// Load animation file from persistent storage
class Msg_Galileo_LoadAnimationDataFromStorage: public ApRequestMessage
{
public:
  Msg_Galileo_LoadAnimationDataFromStorage() : ApRequestMessage("Galileo_LoadAnimationDataFromStorage") {}
  ApIN String sUrl;
  ApOUT Buffer sbData;
  ApOUT String sMimeType;
};

// -> Galileo
// Expire memory cache items
class Msg_Galileo_ClearAllStorage: public ApRequestMessage
{
public:
  Msg_Galileo_ClearAllStorage() : ApRequestMessage("Galileo_ClearAllStorage") {}
};

// -> Galileo
// Expire memory cache items
class Msg_Galileo_ExpireAllStorage: public ApRequestMessage
{
public:
  Msg_Galileo_ExpireAllStorage() : ApRequestMessage("Galileo_ExpireAllStorage"), nAge(0) {}
  ApIN int nAge;
};

#endif // !defined(MsgSample_h_INCLUDED)
