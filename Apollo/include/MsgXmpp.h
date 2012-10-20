// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgXmpp_h_INCLUDED)
#define MsgXmpp_h_INCLUDED

#include "ApMessage.h"

// -> xmpp
class Msg_Xmpp_GetStatus: public ApRequestMessage
{
public:
  Msg_Xmpp_GetStatus() : ApRequestMessage("Xmpp_GetStatus") {}
  ApOUT Apollo::KeyValueList kvStatus;
};

// ---------------------------------

// -> xmpp
class Msg_Xmpp_StartClient: public ApRequestMessage
{
public:
  Msg_Xmpp_StartClient() : ApRequestMessage("Xmpp_StartClient") {}
  ApIN ApHandle hClient;
  ApIN String sJabberId;
  ApIN String sPassword;
  ApIN String sResource;
};

// -> xmpp
class Msg_Xmpp_StopClient: public ApRequestMessage
{
public:
  Msg_Xmpp_StopClient() : ApRequestMessage("Xmpp_StopClient") {}
  ApIN ApHandle hClient;
};

// -> xmpp
class Msg_Xmpp_DefaultClient: public ApRequestMessage
{
public:
  Msg_Xmpp_DefaultClient() : ApRequestMessage("Xmpp_DefaultClient") {}
  ApIN ApHandle hClient;
};

// -------------------------------------------------------------------
// Before connection

// -> xmpp
class Msg_Xmpp_Connect: public ApRequestMessage
{
public:
  Msg_Xmpp_Connect() : ApRequestMessage("Xmpp_Connect"), nDelaySec(0) {}
  ApIN ApHandle hClient;
  ApIN int nDelaySec;
};

// -> xmpp
class Msg_Xmpp_Disconnect: public ApRequestMessage
{
public:
  Msg_Xmpp_Disconnect() : ApRequestMessage("Xmpp_Disconnect") {}
  ApIN ApHandle hClient;
};

// xmpp ->
class Msg_Xmpp_GetAutoConnect: public ApRequestMessage
{
public:
  Msg_Xmpp_GetAutoConnect() : ApRequestMessage("Xmpp_GetAutoConnect"), bConnect(0) {}
  ApIN ApHandle hClient;
  ApOUT int bConnect;
};

// Hook this to change the XMPP connection data before the connect starts
// xmpp <-> 
class Msg_Xmpp_ConnectDetails: public ApFilterMessage
{
public:
  Msg_Xmpp_ConnectDetails() : ApFilterMessage("Xmpp_ConnectDetails"), nPort(5222) {}
  ApIN ApHandle hClient;
  ApINOUT String sJabberId;
  ApINOUT String sPassword;
  ApINOUT String sResource;
  ApINOUT String sHost;
  ApINOUT int nPort;
};

// --------------------------------
// Connection:

// Hook this to be notified when an XMPP connection opens.
class Msg_Xmpp_Connected: public ApNotificationMessage
{
public:
  Msg_Xmpp_Connected() : ApNotificationMessage("Xmpp_Connected") {}
  ApIN ApHandle hClient;
};

// Hook this to be notified when an XMPP connection closes.
class Msg_Xmpp_Disconnected: public ApNotificationMessage
{
public:
  Msg_Xmpp_Disconnected() : ApNotificationMessage("Xmpp_Disconnected") {}
  ApIN ApHandle hClient;
};

// --------------------------------
// Login:

// Hook this to be notified when an XMPP connection is logged in.
class Msg_Xmpp_LoggedIn: public ApNotificationMessage
{
public:
  Msg_Xmpp_LoggedIn() : ApNotificationMessage("Xmpp_LoggedIn") {}
  ApIN ApHandle hClient;
};

// Hook this to be notified when an XMPP connection is logged out.
class Msg_Xmpp_LoggedOut: public ApNotificationMessage
{
public:
  Msg_Xmpp_LoggedOut() : ApNotificationMessage("Xmpp_LoggedOut") {}
  ApIN ApHandle hClient;
};

// --------------------------------
// In:

// Hook this to change XMPP data before it processed.
class Msg_Xmpp_DataIn: public ApRequestMessage
{
public:
  Msg_Xmpp_DataIn() : ApRequestMessage("Xmpp_DataIn") {}
  ApIN ApHandle hClient;
  ApIN Buffer sbData;
};

// Hook this early to know which stanzas will be processed.
class Msg_Xmpp_StanzaIn: public ApRequestMessage
{
public:
  Msg_Xmpp_StanzaIn() : ApRequestMessage("Xmpp_StanzaIn") {}
  ApIN ApHandle hClient;
  ApIN String sData;
};

// --------------------------------
// Out:

// Hook this to change XMPP data before it sent to the connection.
// -> xmpp
class Msg_Xmpp_DataOut: public ApRequestMessage
{
public:
  Msg_Xmpp_DataOut() : ApRequestMessage("Xmpp_DataOut") {}
  ApIN ApHandle hClient;
  ApIN Buffer sbData;
};

// Hook this early to know which stanzas will be sent.
// -> xmpp
class Msg_Xmpp_StanzaOut: public ApRequestMessage
{
public:
  Msg_Xmpp_StanzaOut() : ApRequestMessage("Xmpp_StanzaOut") {}
  ApIN ApHandle hClient;
  ApIN String sData;
};

// --------------------------------

// Send a request via XMPP to a destination address
// -> xmpp
class Msg_Xmpp_SendSrpcRequest: public ApRequestMessage
{
public:
  Msg_Xmpp_SendSrpcRequest() : ApRequestMessage("Xmpp_SendSrpcRequest") {}
  ApIN ApHandle hClient;
  ApIN String sDestination;
  ApIN String sReference;
  ApIN Apollo::SrpcMessage srpc;
};

// --------------------------------
// Now some real work: enter and leave chat room

// Hook this early to change the data before it is processed.
// -> xmpp
class Msg_Xmpp_EnterRoom: public ApRequestMessage
{
public:
  Msg_Xmpp_EnterRoom() : ApRequestMessage("Xmpp_EnterRoom") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
  ApIN String sRoomJid;
  ApIN String sNickname;
};

// Hook this be notified when the entering starts.
// xmpp -> 
class Msg_Xmpp_EnteringRoom: public ApNotificationMessage
{
public:
  Msg_Xmpp_EnteringRoom() : ApNotificationMessage("Xmpp_EnteringRoom") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
};

// Hook this to be notified of the confirmation by the server.
// xmpp -> 
class Msg_Xmpp_EnterRoomComplete: public ApNotificationMessage
{
public:
  Msg_Xmpp_EnterRoomComplete() : ApNotificationMessage("Xmpp_EnterRoomComplete") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
  ApIN String sNickname;
};

// Hook this early to change the data before it is processed.
// -> xmpp
class Msg_Xmpp_LeaveRoom: public ApRequestMessage
{
public:
  Msg_Xmpp_LeaveRoom() : ApRequestMessage("Xmpp_LeaveRoom") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
};

// Hook this to be notified of the confirmation by the server.
// xmpp -> 
class Msg_Xmpp_LeavingRoom: public ApNotificationMessage
{
public:
  Msg_Xmpp_LeavingRoom() : ApNotificationMessage("Xmpp_LeavingRoom") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
};

// Hook this to be notified when leaving a room starts.
// xmpp -> 
class Msg_Xmpp_LeaveRoomComplete: public ApNotificationMessage
{
public:
  Msg_Xmpp_LeaveRoomComplete() : ApNotificationMessage("Xmpp_LeaveRoomComplete") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
};

// Get the room handle for a room jid from a client
// -> xmpp 
class Msg_Xmpp_GetRoomHandle: public ApRequestMessage
{
public:
  Msg_Xmpp_GetRoomHandle() : ApRequestMessage("Xmpp_GetRoomHandle") {}
  ApIN ApHandle hClient;
  ApIN String sRoomJid;
  ApOUT ApHandle hRoom;
};

// --------------------------------
// Outgoing activity

// Send a chat line through the room
// -> xmpp
class Msg_Xmpp_SendGroupchat: public ApRequestMessage
{
public:
  Msg_Xmpp_SendGroupchat() : ApRequestMessage("Xmpp_SendGroupchat") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
  ApIN String sText;
};

// --------------------------------
// Outgoing state

class Msg_Xmpp_SendRoomState: public ApRequestMessage
{
public:
  Msg_Xmpp_SendRoomState() : ApRequestMessage("Xmpp_SendRoomState") {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
};

// --------------------------------
// Participant enter/leave

// A common base class
class Msg_Xmpp_Participant_Notification: public ApNotificationMessage
{
public:
  Msg_Xmpp_Participant_Notification(const char* szType): ApNotificationMessage(szType) {}
  ApIN ApHandle hClient;
  ApIN ApHandle hRoom;
  ApIN ApHandle hParticipant;
};

// Hook this to be notified when a room participant enters.
// xmpp -> 
class Msg_Xmpp_ParticipantEntered: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_ParticipantEntered() : Msg_Xmpp_Participant_Notification("Xmpp_ParticipantEntered") {}
  ApIN String sNickname;
};

// Hook this to be notified when a room participant leaves. This includes a simulated leave for all participants when I leave.
// xmpp -> 
class Msg_Xmpp_ParticipantLeft: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_ParticipantLeft() : Msg_Xmpp_Participant_Notification("Xmpp_ParticipantLeft") {}
  ApIN String sNickname;
};

// Hook this to be notified when the room knows who I am.
// xmpp -> 
class Msg_Xmpp_ParticipantSelf: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_ParticipantSelf() : Msg_Xmpp_Participant_Notification("Xmpp_ParticipantSelf") {}
};

// --------------------------------
// Participant data

// Hook this to be notified when before the evaluation of the participant presence data starts.
// xmpp -> 
class Msg_Xmpp_Participant_Begin: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_Participant_Begin() : Msg_Xmpp_Participant_Notification("Xmpp_Participant_Begin") {}
};

// Hook this to be notified when before the evaluation of the participant presence data starts.
// xmpp -> 
class Msg_Xmpp_Participant_End: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_Participant_End() : Msg_Xmpp_Participant_Notification("Xmpp_Participant_End") {}
};

// Hook this to be notified of a participant status.
// This message is is enclosed by Xmpp_Participant_Status/Xmpp_Participant_End
// xmpp -> 
#define Msg_Xmpp_Participant_Status_Chat "chat"
#define Msg_Xmpp_Participant_Status_Available "available"
#define Msg_Xmpp_Participant_Status_Away "away"
#define Msg_Xmpp_Participant_Status_ExtendedAway "xa"
#define Msg_Xmpp_Participant_Status_DoNotDisturb "dnd"
class Msg_Xmpp_Participant_Status: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_Participant_Status() : Msg_Xmpp_Participant_Notification("Xmpp_Participant_Status") {}
  String sStatus;
};

// Hook this to be notified of a participant status message.
// This message is is enclosed by Xmpp_Participant_Begin/Xmpp_Participant_End
// xmpp -> 
class Msg_Xmpp_Participant_StatusMessage: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_Participant_StatusMessage() : Msg_Xmpp_Participant_Notification("Xmpp_Participant_StatusMessage") {}
  String sMessage;
};

// Hook this to be notified of a participant jabber id, if available.
// This message is is enclosed by Xmpp_Participant_Begin/Xmpp_Participant_End
// xmpp -> 
class Msg_Xmpp_Participant_JabberId: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_Participant_JabberId() : Msg_Xmpp_Participant_Notification("Xmpp_Participant_JabberId") {}
  String sJabberId;
};

// Hook this to be notified of a participant's muc user info.
// This message is is enclosed by Xmpp_Participant_Begin/Xmpp_Participant_End
// xmpp -> 
class Msg_Xmpp_Participant_MucUser: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_Participant_MucUser() : Msg_Xmpp_Participant_Notification("Xmpp_Participant_MucUser") {}
  ApIN String sJabberId;
  ApIN String sAffiliation;
  ApIN String sRole;
};

// xmpp -> 
class Msg_Xmpp_Participant_FirebatFeatures: public ApNotificationMessage
{
public:
  Msg_Xmpp_Participant_FirebatFeatures() : ApNotificationMessage("Xmpp_Participant_FirebatFeatures") {}
  ApIN ApHandle hRoom;
  ApIN ApHandle hParticipant;
  ApIN String sFeatures;
};

// --------------------------------
// Participant incoming remote activity

// receive a public chat line.
// xmpp -> 
class Msg_Xmpp_ReceiveGroupchat: public Msg_Xmpp_Participant_Notification
{
public:
  Msg_Xmpp_ReceiveGroupchat() : Msg_Xmpp_Participant_Notification("Xmpp_ReceiveGroupchat"), nSec(0), nMicroSec(0) {}
  ApIN String sText;
  ApIN int nSec;
  ApIN int nMicroSec;
};

#endif // !defined(MsgXmpp_h_INCLUDED)
