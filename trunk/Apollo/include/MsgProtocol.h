// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgProtocol_h_INCLUDED)
#define MsgProtocol_h_INCLUDED

#define Msg_Protocol_Feature_General ""
#define Msg_Protocol_Feature_GroupChat "GroupChat"
#define Msg_Protocol_Feature_P2PChat "P2PChat"
#define Msg_Protocol_Feature_Message "Message"
#define Msg_Protocol_Feature_OnlinePresence "OnlinePresence"
#define Msg_Protocol_Feature_BuddyList "BuddyList"
#define Msg_Protocol_Feature_ContactImage "ContactImage"
#define Msg_Protocol_Feature_FileTransfer "FileTransfer"
#define Msg_Protocol_Feature_URL "URL"

// -> protocols
// sFeature comma separated list of features. Maybe the protocol has modes which do not support a specific features
class Msg_Protocol_IsOnline: public ApRequestMessage
{
public:
  Msg_Protocol_IsOnline() : ApRequestMessage("Protocol_IsOnline"), bOnline(0) {}
  ApIN String sProtocol;
  ApIN String sFeature;
  ApOUT int bOnline;
};

// vp -> protocols
class Msg_Protocol_EnterRoom: public ApRequestMessage
{
public:
  Msg_Protocol_EnterRoom() : ApRequestMessage("Protocol_EnterRoom") {}
  ApIN String sProtocol;
  ApIN String sRoom;
  ApIN ApHandle hRoom;
};

// protocols ->
class Msg_Protocol_EnteringRoom: public ApNotificationMessage
{
public:
  Msg_Protocol_EnteringRoom() : ApNotificationMessage("Protocol_EnteringRoom") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
};

// protocols ->
class Msg_Protocol_EnterRoomComplete: public ApNotificationMessage
{
public:
  Msg_Protocol_EnterRoomComplete() : ApNotificationMessage("Protocol_EnterRoomComplete") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
};

// vp -> protocols
class Msg_Protocol_LeaveRoom: public ApRequestMessage
{
public:
  Msg_Protocol_LeaveRoom() : ApRequestMessage("Protocol_LeaveRoom") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
};

// protocols ->
class Msg_Protocol_LeavingRoom: public ApNotificationMessage
{
public:
  Msg_Protocol_LeavingRoom() : ApNotificationMessage("Protocol_LeavingRoom") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
};

// protocols ->
class Msg_Protocol_LeaveRoomComplete: public ApNotificationMessage
{
public:
  Msg_Protocol_LeaveRoomComplete() : ApNotificationMessage("Protocol_LeaveRoomComplete") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
};

// Hook this be notified when a room participant enters.
// protocols -> 
class Msg_Protocol_ParticipantEntered: public ApNotificationMessage
{
public:
  Msg_Protocol_ParticipantEntered() : ApNotificationMessage("Protocol_ParticipantEntered") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
  ApIN ApHandle hParticipant;
  ApIN String sNickname;
};

// Hook this be notified when a room participant left.
// protocols -> 
class Msg_Protocol_ParticipantLeft: public ApNotificationMessage
{
public:
  Msg_Protocol_ParticipantLeft() : ApNotificationMessage("Protocol_ParticipantLeft") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
  ApIN ApHandle hParticipant;
};

// Hook this be notified when the protocol knows which of all participants is our self.
// protocols -> 
class Msg_Protocol_ParticipantSelf: public ApNotificationMessage
{
public:
  Msg_Protocol_ParticipantSelf() : ApNotificationMessage("Protocol_ParticipantSelf") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
  ApIN ApHandle hParticipant; // if ApNoHandle, then protocol does not know (or not anymore)
};

// Hook this be notified when the protcol is available for I/O.
// protocols -> 
class Msg_Protocol_Online: public ApNotificationMessage
{
public:
  Msg_Protocol_Online() : ApNotificationMessage("Protocol_Online") {}
  ApIN String sProtocol;
};

// Hook this be notified when the protcol is not available for I/O.
// protocols -> 
class Msg_Protocol_Offline: public ApNotificationMessage
{
public:
  Msg_Protocol_Offline() : ApNotificationMessage("Protocol_Offline") {}
  ApIN String sProtocol;
};

// --------------------------------
// Incoming participant data

// A common base class
class Msg_Protocol_Participant_Notification: public ApNotificationMessage
{
public:
  Msg_Protocol_Participant_Notification(const char* szType): ApNotificationMessage(szType) {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
  ApIN ApHandle hParticipant;
};

class Msg_Protocol_Participant_Begin: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_Begin() : Msg_Protocol_Participant_Notification("Protocol_Participant_Begin") {}
};

class Msg_Protocol_Participant_Status: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_Status() : Msg_Protocol_Participant_Notification("Protocol_Participant_Status") {}
  ApIN String sStatus;
};

class Msg_Protocol_Participant_StatusMessage: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_StatusMessage() : Msg_Protocol_Participant_Notification("Protocol_Participant_StatusMessage") {}
  ApIN String sMessage;
};

class Msg_Protocol_Participant_JabberId: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_JabberId() : Msg_Protocol_Participant_Notification("Protocol_Participant_JabberId") {}
  String sJabberId;
};

class Msg_Protocol_Participant_MucUser: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_MucUser() : Msg_Protocol_Participant_Notification("Protocol_Participant_MucUser") {}
  ApIN String sJabberId;
  ApIN String sAffiliation;
  ApIN String sRole;
};

class Msg_Protocol_Participant_VpIdentity: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_VpIdentity() : Msg_Protocol_Participant_Notification("Protocol_Participant_VpIdentity") {}
  ApIN String sId;
  ApIN String sSrc;
  ApIN String sDigest;
};

class Msg_Protocol_Participant_Position: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_Position() : Msg_Protocol_Participant_Notification("Protocol_Participant_Position") {}
  ApIN Apollo::KeyValueList kvParams;
};

class Msg_Protocol_Participant_Condition: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_Condition() : Msg_Protocol_Participant_Notification("Protocol_Participant_Condition") {}
  ApIN Apollo::KeyValueList kvParams;
};

class Msg_Protocol_Participant_FirebatFeatures: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_FirebatFeatures() : Msg_Protocol_Participant_Notification("Protocol_Participant_FirebatFeatures") {}
  ApIN String sFeatures;
};

class Msg_Protocol_Participant_End: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_Participant_End() : Msg_Protocol_Participant_Notification("Protocol_Participant_End") {}
};

// --------------------------------
// Outgoing state

// A common base class
class Msg_Protocol_Room_StateRequest: public ApNotificationMessage
{
public:
  Msg_Protocol_Room_StateRequest(const char* szType): ApNotificationMessage(szType) {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
};

// vp -> 
class Msg_Protocol_Room_BeginState: public Msg_Protocol_Room_StateRequest
{
  public: Msg_Protocol_Room_BeginState() : Msg_Protocol_Room_StateRequest("Protocol_Room_BeginState") {}
};

// vp -> 
class Msg_Protocol_Room_SetStatus: public Msg_Protocol_Room_StateRequest
{
  public: Msg_Protocol_Room_SetStatus() : Msg_Protocol_Room_StateRequest("Protocol_Room_SetStatus") {}
  ApIN String sStatus;
};

// vp -> 
class Msg_Protocol_Room_SetStatusMessage: public Msg_Protocol_Room_StateRequest
{
  public: Msg_Protocol_Room_SetStatusMessage() : Msg_Protocol_Room_StateRequest("Protocol_Room_SetStatusMessage") {}
  ApIN String sStatusMessage;
};

// vp -> 
class Msg_Protocol_Room_SetPosition: public Msg_Protocol_Room_StateRequest
{
  public: Msg_Protocol_Room_SetPosition() : Msg_Protocol_Room_StateRequest("Protocol_Room_SetPosition") {}
  ApIN Apollo::KeyValueList kvParams;
};

// vp -> 
class Msg_Protocol_Room_SetCondition: public Msg_Protocol_Room_StateRequest
{
  public: Msg_Protocol_Room_SetCondition() : Msg_Protocol_Room_StateRequest("Protocol_Room_SetCondition") {}
  ApIN Apollo::KeyValueList kvParams;
};

// vp -> 
class Msg_Protocol_Room_CommitState: public Msg_Protocol_Room_StateRequest
{
  public: Msg_Protocol_Room_CommitState() : Msg_Protocol_Room_StateRequest("Protocol_Room_CommitState") {}
};

// --------------------------------
// Protocol asks for info

// protocol module -> vp
class Msg_Protocol_Room_GetIdentity: public ApFilterMessage
{
  public: Msg_Protocol_Room_GetIdentity() : ApFilterMessage("Protocol_Room_GetIdentity") {}
  ApIN ApHandle hRoom;
  ApOUT String sId;
  ApOUT String sUrl;
  ApOUT String sDigest;
};

// protocol module -> vp
class Msg_Protocol_Room_GetPosition: public ApFilterMessage
{
  public: Msg_Protocol_Room_GetPosition() : ApFilterMessage("Protocol_Room_GetPosition") {}
  ApIN ApHandle hRoom;
  ApIN Apollo::KeyValueList kvParams;
};

// protocol module -> vp
class Msg_Protocol_Room_GetCondition: public ApFilterMessage
{
  public: Msg_Protocol_Room_GetCondition() : ApFilterMessage("Protocol_Room_GetCondition") {}
  ApIN ApHandle hRoom;
  ApIN Apollo::KeyValueList kvParams;
};

// --------------------------------
// Outgoing activity in room

// vp -> protocols
class Msg_Protocol_SendPublicChat: public ApRequestMessage
{
public:
  Msg_Protocol_SendPublicChat() : ApRequestMessage("Protocol_SendPublicChat") {}
  ApIN String sProtocol;
  ApIN ApHandle hRoom;
  ApIN String sText;
};

// --------------------------------
// Incoming activity in room

class Msg_Protocol_ReceivePublicChat: public Msg_Protocol_Participant_Notification
{
  public: Msg_Protocol_ReceivePublicChat() : Msg_Protocol_Participant_Notification("Protocol_ReceivePublicChat"), nSec(0), nMicroSec(0) {}
  ApIN String sText;
  ApIN int nSec;
  ApIN int nMicroSec;
};

#endif // !defined(MsgProtocol_h_INCLUDED)
