// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgVpView_h_INCLUDED)
#define MsgVpView_h_INCLUDED

#include "ApMessage.h"

//----------------------------------------------------------
// Notification: vp -> displays

// input -> VP/Arena
class Msg_VpView_ContextCreated: public ApNotificationMessage
{
public:
  Msg_VpView_ContextCreated() : ApNotificationMessage("VpView_ContextCreated") {}
  ApIN ApHandle hContext;
};

// input -> VP/Arena
class Msg_VpView_ContextDestroyed: public ApNotificationMessage
{
public:
  Msg_VpView_ContextDestroyed() : ApNotificationMessage("VpView_ContextDestroyed") {}
  ApIN ApHandle hContext;
};

// input -> VP/Arena
class Msg_VpView_ContextVisibility: public ApNotificationMessage
{
public:
  Msg_VpView_ContextVisibility() : ApNotificationMessage("VpView_ContextVisibility"), bVisible(1) {}
  ApIN ApHandle hContext;
  ApIN int bVisible;
};

// input -> VP/Arena
class Msg_VpView_ContextPosition: public ApNotificationMessage
{
public:
  Msg_VpView_ContextPosition() : ApNotificationMessage("VpView_ContextPosition"), nX(0), nY(0) {}
  ApIN ApHandle hContext;
  ApIN int nX;
  ApIN int nY;
};

// input -> VP/Arena
class Msg_VpView_ContextSize: public ApNotificationMessage
{
public:
  Msg_VpView_ContextSize() : ApNotificationMessage("VpView_ContextSize"), nWidth(0), nHeight(0) {}
  ApIN ApHandle hContext;
  ApIN int nWidth;
  ApIN int nHeight;
};

// -------------------------------

// -> input
class Msg_VpView_GetContextVisibility: public ApRequestMessage
{
public:
  Msg_VpView_GetContextVisibility() : ApRequestMessage("VpView_GetContextVisibility"), bVisible(1) {}
  ApIN ApHandle hContext;
  ApOUT int bVisible;
};

// -> input
class Msg_VpView_GetContextPosition: public ApRequestMessage
{
public:
  Msg_VpView_GetContextPosition() : ApRequestMessage("VpView_GetContextPosition"), nX(0), nY(0) {}
  ApIN ApHandle hContext;
  ApOUT int nX;
  ApOUT int nY;
};

// -> input
class Msg_VpView_GetContextSize: public ApRequestMessage
{
public:
  Msg_VpView_GetContextSize() : ApRequestMessage("VpView_GetContextSize"), nWidth(0), nHeight(0) {}
  ApIN ApHandle hContext;
  ApOUT int nWidth;
  ApOUT int nHeight;
};

//--------------------------
// Location events

class Msg_VpView_LocationsChanged: public ApNotificationMessage
{
public:
  Msg_VpView_LocationsChanged() : ApNotificationMessage("VpView_LocationsChanged") {}
};

class Msg_VpView_EnterLocationRequested: public ApNotificationMessage
{
public:
  Msg_VpView_EnterLocationRequested() : ApNotificationMessage("VpView_EnterLocationRequested") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_EnterLocationBegin: public ApNotificationMessage
{
public:
  Msg_VpView_EnterLocationBegin() : ApNotificationMessage("VpView_EnterLocationBegin") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_EnterLocationComplete: public ApNotificationMessage
{
public:
  Msg_VpView_EnterLocationComplete() : ApNotificationMessage("VpView_EnterLocationComplete") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_LeaveLocationRequested: public ApNotificationMessage
{
public:
  Msg_VpView_LeaveLocationRequested() : ApNotificationMessage("VpView_LeaveLocationRequested") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_LeaveLocationBegin: public ApNotificationMessage
{
public:
  Msg_VpView_LeaveLocationBegin() : ApNotificationMessage("VpView_LeaveLocationBegin") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_LeaveLocationComplete: public ApNotificationMessage
{
public:
  Msg_VpView_LeaveLocationComplete() : ApNotificationMessage("VpView_LeaveLocationComplete") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_LocationContextsChanged: public ApNotificationMessage
{
public:
  Msg_VpView_LocationContextsChanged() : ApNotificationMessage("VpView_LocationContextsChanged") {}
  ApIN ApHandle hLocation;
};

class Msg_VpView_ContextLocationAssigned: public ApNotificationMessage
{
public:
  Msg_VpView_ContextLocationAssigned() : ApNotificationMessage("VpView_ContextLocationAssigned") {}
  ApIN ApHandle hContext;
  ApIN ApHandle hLocation;
};

class Msg_VpView_ContextLocationUnassigned: public ApNotificationMessage
{
public:
  Msg_VpView_ContextLocationUnassigned() : ApNotificationMessage("VpView_ContextLocationUnassigned") {}
  ApIN ApHandle hContext;
  ApIN ApHandle hLocation;
};

class Msg_VpView_ParticipantsChanged: public ApNotificationMessage
{
public:
  Msg_VpView_ParticipantsChanged() : ApNotificationMessage("VpView_ParticipantsChanged"), nCount(0) {}
  ApIN ApHandle hLocation;
  ApIN int nCount;
};

//--------------------------
// Participant events

class Msg_VpView_ParticipantAdded: public ApNotificationMessage
{
public:
  Msg_VpView_ParticipantAdded() : ApNotificationMessage("VpView_ParticipantAdded"), bSelf(0) {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hParticipant;
  ApIN int bSelf;
};

class Msg_VpView_ParticipantRemoved: public ApNotificationMessage
{
public:
  Msg_VpView_ParticipantRemoved() : ApNotificationMessage("VpView_ParticipantRemoved") {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hParticipant;
};

class Msg_VpView_ParticipantDetailsChanged: public ApNotificationMessage
{
public:
  Msg_VpView_ParticipantDetailsChanged() : ApNotificationMessage("VpView_ParticipantDetailsChanged") {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hParticipant;
  Apollo::ValueList vlKeys;
};

class Msg_VpView_IdentityContainerAvailable: public ApNotificationMessage
{
public:
  Msg_VpView_IdentityContainerAvailable() : ApNotificationMessage("VpView_IdentityContainerAvailable") {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hParticipant;
  String sUrl;
};

//--------------------------
// Context inspection

#define Msg_VpView_ContextDetail_DocumentUrl "DocumentUrl"
#define Msg_VpView_ContextDetail_LocationUrl "LocationUrl"
#define Msg_VpView_ContextDetail_Visibility "Visibility"
#define Msg_VpView_ContextDetail_Position "Position"
  #define Msg_VpView_ContextDetail_Position_X "nX"
  #define Msg_VpView_ContextDetail_Position_Y "nY"
#define Msg_VpView_ContextDetail_Size "Size"
  #define Msg_VpView_ContextDetail_Size_Width "nWidth"
  #define Msg_VpView_ContextDetail_Size_Height "nHeight"

class Msg_VpView_GetContextDetail: public ApRequestMessage
{
public:
  Msg_VpView_GetContextDetail() : ApRequestMessage("VpView_GetContextDetail") {}
  ApIN ApHandle hContext;
  ApIN String sKey;
  ApOUT String sValue;
  ApOUT String sMimeType;
};

class Msg_VpView_SubscribeContextDetail: public ApRequestMessage
{
public:
  Msg_VpView_SubscribeContextDetail() : ApRequestMessage("VpView_SubscribeContextDetail") {}
  ApIN ApHandle hContext;
  ApIN String sKey;
  ApOUT String sValue;
  ApOUT String sMimeType;
};

class Msg_VpView_UnsubscribeContextDetail: public ApRequestMessage
{
public:
  Msg_VpView_UnsubscribeContextDetail() : ApRequestMessage("VpView_UnsubscribeContextDetail") {}
  ApIN ApHandle hContext;
  ApIN String sKey;
  ApOUT String sMimeType;
};

class Msg_VpView_ContextDetailsChanged: public ApNotificationMessage
{
public:
  Msg_VpView_ContextDetailsChanged() : ApNotificationMessage("VpView_ContextDetailsChanged") {}
  ApIN ApHandle hContext;
  Apollo::ValueList vlKeys;
};

//--------------------------
// Location inspection

class Msg_VpView_GetLocations: public ApRequestMessage
{
public:
  Msg_VpView_GetLocations() : ApRequestMessage("VpView_GetLocations") {}
  ApOUT Apollo::ValueList vlLocations;
};

class Msg_VpView_GetLocationContexts: public ApRequestMessage
{
public:
  Msg_VpView_GetLocationContexts() : ApRequestMessage("VpView_GetLocationContexts") {}
  ApIN ApHandle hLocation;
  ApOUT Apollo::ValueList vlContexts;
};

class Msg_VpView_GetParticipants: public ApRequestMessage
{
public:
  Msg_VpView_GetParticipants() : ApRequestMessage("VpView_GetParticipants") {}
  ApIN ApHandle hLocation;
  ApOUT Apollo::ValueList vlParticipants;
  ApOUT ApHandle hSelf;
};

#define Msg_VpView_LocationDetail_LocationUrl "LocationUrl"
#define Msg_VpView_LocationDetail_State "State"
  #define Msg_VpView_LocationDetail_StateNotEntered "NotEntered"
  #define Msg_VpView_LocationDetail_StateEnterWaitingforOnline "EnterWaitingforOnline"
  #define Msg_VpView_LocationDetail_StateEnterRequested "EnterRequested"
  #define Msg_VpView_LocationDetail_StateEntering "Entering"
  #define Msg_VpView_LocationDetail_StateEntered "Entered"
  #define Msg_VpView_LocationDetail_StateLeaveRequested "LeaveRequested"
  #define Msg_VpView_LocationDetail_StateLeaving "Leaving"
  #define Msg_VpView_LocationDetail_StateUnknown "Unknown"

class Msg_VpView_GetLocationDetail: public ApRequestMessage
{
public:
  Msg_VpView_GetLocationDetail() : ApRequestMessage("VpView_GetLocationDetail") {}
  ApIN ApHandle hLocation;
  ApIN String sKey;
  ApOUT String sValue;
  ApOUT String sMimeType;
};

class Msg_VpView_SubscribeLocationDetail: public ApRequestMessage
{
public:
  Msg_VpView_SubscribeLocationDetail() : ApRequestMessage("VpView_SubscribeLocationDetail") {}
  ApIN ApHandle hLocation;
  ApIN String sKey;
  ApOUT String sValue;
  ApOUT String sMimeType;
};

class Msg_VpView_UnsubscribeLocationDetail: public ApRequestMessage
{
public:
  Msg_VpView_UnsubscribeLocationDetail() : ApRequestMessage("VpView_UnsubscribeLocationDetail") {}
  ApIN ApHandle hLocation;
  ApIN String sKey;
  ApOUT String sMimeType;
};

class Msg_VpView_LocationDetailsChanged: public ApNotificationMessage
{
public:
  Msg_VpView_LocationDetailsChanged() : ApNotificationMessage("VpView_LocationDetailsChanged") {}
  ApIN ApHandle hLocation;
  Apollo::ValueList vlKeys;
};

//--------------------------
// Participant inspection

#define Msg_VpView_ParticipantDetail_Nickname "Nickname"
#define Msg_VpView_ParticipantDetail_NicknameLink "NicknameLink"
#define Msg_VpView_ParticipantDetail_NicknameTitle "NicknameTitle"
#define Msg_VpView_ParticipantDetail_avatar "avatar"
#define Msg_VpView_ParticipantDetail_OnlineStatus "Status"
#define Msg_VpView_ParticipantDetail_Position "Position"
#define Msg_VpView_ParticipantDetail_Message "Message"
#define Msg_VpView_ParticipantDetail_JabberId "JabberId"
#define Msg_VpView_ParticipantDetail_MucAffiliation "MucAffiliation"
#define Msg_VpView_ParticipantDetail_MucRole "MucRole"
#define Msg_VpView_ParticipantDetail_Condition "Condition"
#define Msg_VpView_ParticipantDetail_FirebatFeatures "FirebatFeatures"
#define Msg_VpView_ParticipantDetail_PublicChat "PublicChat"
#define Msg_VpView_ParticipantDetail_PrivateChat "PrivateChat"
#define Msg_VpView_ParticipantDetail_IdentityUrl "IdentityUrl"
#define Msg_VpView_ParticipantDetail_ProfileUrl "ProfilePage"

class Msg_VpView_GetParticipantDetailString: public ApRequestMessage
{
public:
  Msg_VpView_GetParticipantDetailString() : ApRequestMessage("VpView_GetParticipantDetailString") {}
  ApIN ApHandle hParticipant;
  ApIN String sKey;
  ApIN Apollo::ValueList vlMimeTypes;
  ApOUT String sValue;
  ApOUT String sMimeType;
};

#define Msg_VpView_ParticipantDetail_SourcePrefix_IdentityItemUrl "IdentityItemUrl"
#define Msg_VpView_ParticipantDetail_SourcePrefix_IdentityItemData "IdentityItemData"
#define Msg_VpView_ParticipantDetail_SourcePrefix_Protocol "Protocol"
#define Msg_VpView_ParticipantDetail_SourcePrefix_UserDefined "UserDefined"
#define Msg_VpView_ParticipantDetail_SourceSeparator "="

class Msg_VpView_GetParticipantDetailData: public ApRequestMessage
{
public:
  Msg_VpView_GetParticipantDetailData() : ApRequestMessage("VpView_GetParticipantDetailData") {}
  ApIN ApHandle hParticipant;
  ApIN String sKey;
  ApIN Apollo::ValueList vlMimeTypes;
  ApOUT Buffer sbData;
  ApOUT String sMimeType;
  ApOUT String sSource; // e.g. "IdentityItemUrl=http://ydentiti.org/test/Tassadar/config.xml"
};

class Msg_VpView_GetParticipantDetailRef: public ApRequestMessage
{
public:
  Msg_VpView_GetParticipantDetailRef() : ApRequestMessage("VpView_GetParticipantDetailRef") {}
  ApIN ApHandle hParticipant;
  ApIN String sKey;
  ApIN Apollo::ValueList vlMimeTypes;
  ApOUT String sUrl;
  ApOUT String sMimeType;
};

class Msg_VpView_SubscribeParticipantDetail: public ApRequestMessage
{
public:
  Msg_VpView_SubscribeParticipantDetail() : ApRequestMessage("VpView_SubscribeParticipantDetail"), bAvailable(0) {}
  ApIN ApHandle hParticipant;
  ApIN String sKey;
  ApIN Apollo::ValueList vlMimeTypes;
  ApOUT int bAvailable;
};

class Msg_VpView_UnsubscribeParticipantDetail: public ApRequestMessage
{
public:
  Msg_VpView_UnsubscribeParticipantDetail() : ApRequestMessage("VpView_UnsubscribeParticipantDetail") {}
  ApIN ApHandle hParticipant;
  ApIN String sKey;
  ApIN Apollo::ValueList vlMimeTypes;
};

//--------------------------
// Display profile

class Msg_VpView_Profile_Create: public ApRequestMessage
{
public:
  Msg_VpView_Profile_Create() : ApRequestMessage("VpView_Profile_Create") {}
  ApIN ApHandle hProfile;
};

class Msg_VpView_Profile_SetParticipantDetail: public ApRequestMessage
{
public:
  Msg_VpView_Profile_SetParticipantDetail() : ApRequestMessage("VpView_Profile_SetParticipantDetail") {}
  ApIN ApHandle hProfile;
  ApIN String sKey;
  ApIN Apollo::ValueList vlMimeTypes;
  ApIN int bByRef;
};

class Msg_VpView_Profile_GetParticipantDetails: public ApRequestMessage
{
public:
  Msg_VpView_Profile_GetParticipantDetails() : ApRequestMessage("VpView_Profile_GetParticipantDetails") {}
  ApIN ApHandle hProfile;
  ApIN ApHandle hParticipant;
};

class Msg_VpView_Profile_SubscribeParticipantDetails: public ApRequestMessage
{
public:
  Msg_VpView_Profile_SubscribeParticipantDetails() : ApRequestMessage("VpView_Profile_SubscribeParticipantDetails") {}
  ApIN ApHandle hProfile;
  ApIN ApHandle hParticipant;
};

class Msg_VpView_Profile_UnsubscribeParticipantDetails: public ApRequestMessage
{
public:
  Msg_VpView_Profile_UnsubscribeParticipantDetails() : ApRequestMessage("VpView_Profile_UnsubscribeParticipantDetails") {}
  ApIN ApHandle hProfile;
  ApIN ApHandle hParticipant;
};

class Msg_VpView_Profile_Delete: public ApRequestMessage
{
public:
  Msg_VpView_Profile_Delete() : ApRequestMessage("VpView_Profile_Delete") {}
  ApIN ApHandle hProfile;
};

//--------------------------
// Participant activity

// VPA -> 
class Msg_VpView_LocationPublicChat: public ApNotificationMessage
{
public:
  Msg_VpView_LocationPublicChat() : ApNotificationMessage("VpView_LocationPublicChat"), nSec(0), nMicroSec(0) {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hParticipant;
  ApIN ApHandle hChat;
  ApIN String sNickname;
  ApIN String sText;
  ApIN int nSec;
  ApIN int nMicroSec;
};

// -> VPA
class Msg_VpView_ReplayLocationPublicChat: public ApRequestMessage
{
public:
  Msg_VpView_ReplayLocationPublicChat() : ApRequestMessage("VpView_ReplayLocationPublicChat"), nMaxAge(0), nMaxLines(0), nMaxData(0) {}
  ApIN ApHandle hLocation;
  ApIN int nMaxAge;
  ApIN int nMaxLines;
  ApIN int nMaxData;
  ApOUT Apollo::ValueList vlParticipants;
};

#endif // !defined(MsgVpView_h_INCLUDED)
