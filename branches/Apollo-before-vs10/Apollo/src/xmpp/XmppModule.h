// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XmppModule_h_INCLUDED)
#define XmppModule_h_INCLUDED

#include "Local.h"
#include "MsgUnitTest.h"
#include "MsgNet.h"
#include "MsgSystem.h"
#include "MsgProtocol.h"
#include "MsgXmpp.h"
#include "ApContainer.h"
#include "netapi/NetInterface.h"
#include "Client.h"

typedef ApHandlePointerTree<Client*> ClientList;
typedef ApHandlePointerTreeNode<Client*> ClientNode;
typedef ApHandlePointerTreeIterator<Client*> ClientIterator;

class XmppModule
{
public:
  XmppModule();
  virtual ~XmppModule();

  int Init();
  int Exit();

  void On_Xmpp_GetStatus(Msg_Xmpp_GetStatus* pMsg);

  void On_Xmpp_StartClient(Msg_Xmpp_StartClient* pMsg);
  void On_Xmpp_StopClient(Msg_Xmpp_StopClient* pMsg);
  void On_Xmpp_DefaultClient(Msg_Xmpp_DefaultClient* pMsg);

  void On_Xmpp_Connect(Msg_Xmpp_Connect* pMsg);
  void On_Xmpp_Disconnect(Msg_Xmpp_Disconnect* pMsg);

  void On_Xmpp_GetAutoConnect(Msg_Xmpp_GetAutoConnect* pMsg);
  void On_Xmpp_ConnectDetails(Msg_Xmpp_ConnectDetails* pMsg);

  void On_Xmpp_DataIn(Msg_Xmpp_DataIn* pMsg);
  void On_Xmpp_DataOut(Msg_Xmpp_DataOut* pMsg);

  void On_Xmpp_StanzaIn(Msg_Xmpp_StanzaIn* pMsg);
  void On_Xmpp_StanzaOut(Msg_Xmpp_StanzaOut* pMsg);

  void On_Xmpp_EnterRoom(Msg_Xmpp_EnterRoom* pMsg);
  void On_Xmpp_LeaveRoom(Msg_Xmpp_LeaveRoom* pMsg);
  void On_Xmpp_GetRoomHandle(Msg_Xmpp_GetRoomHandle* pMsg);
  void On_Xmpp_SendGroupchat(Msg_Xmpp_SendGroupchat* pMsg);
  void On_Xmpp_SendRoomState(Msg_Xmpp_SendRoomState* pMsg);

  void On_Protocol_IsOnline(Msg_Protocol_IsOnline* pMsg);
  void On_Protocol_EnterRoom(Msg_Protocol_EnterRoom* pMsg);
  void On_Protocol_LeaveRoom(Msg_Protocol_LeaveRoom* pMsg);
  void On_Protocol_SendPublicChat(Msg_Protocol_SendPublicChat* pMsg);
  //void On_Protocol_Room_BeginState(Msg_Protocol_Room_BeginState* pMsg);
  //void On_Protocol_Room_SetPosition(Msg_Protocol_Room_SetPosition* pMsg);
  //void On_Protocol_Room_SetCondition(Msg_Protocol_Room_SetCondition* pMsg);
  void On_Protocol_Room_CommitState(Msg_Protocol_Room_CommitState* pMsg);

  void On_Xmpp_EnteringRoom(Msg_Xmpp_EnteringRoom* pMsg);
  void On_Xmpp_EnterRoomComplete(Msg_Xmpp_EnterRoomComplete* pMsg);
  void On_Xmpp_LeavingRoom(Msg_Xmpp_LeavingRoom* pMsg);
  void On_Xmpp_LeaveRoomComplete(Msg_Xmpp_LeaveRoomComplete* pMsg);
  void On_Xmpp_ParticipantEntered(Msg_Xmpp_ParticipantEntered* pMsg);
  void On_Xmpp_ParticipantLeft(Msg_Xmpp_ParticipantLeft* pMsg);
  void On_Xmpp_ParticipantSelf(Msg_Xmpp_ParticipantSelf* pMsg);
  void On_Xmpp_LoggedIn(Msg_Xmpp_LoggedIn* pMsg);
  void On_Xmpp_LoggedOut(Msg_Xmpp_LoggedOut* pMsg);

  void On_Xmpp_Participant_Begin(Msg_Xmpp_Participant_Begin* pMsg);
  void On_Xmpp_Participant_Status(Msg_Xmpp_Participant_Status* pMsg);
  void On_Xmpp_Participant_StatusMessage(Msg_Xmpp_Participant_StatusMessage* pMsg);
  void On_Xmpp_Participant_JabberId(Msg_Xmpp_Participant_JabberId* pMsg);
  void On_Xmpp_Participant_MucUser(Msg_Xmpp_Participant_MucUser* pMsg);
  void On_Xmpp_Participant_FirebatFeatures(Msg_Xmpp_Participant_FirebatFeatures* pMsg);
  void On_Xmpp_Participant_End(Msg_Xmpp_Participant_End* pMsg);

  void On_Xmpp_ReceiveGroupchat(Msg_Xmpp_ReceiveGroupchat* pMsg);

  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_Net_Online(Msg_Net_Online* pMsg);
  void On_Net_Offline(Msg_Net_Offline* pMsg);
  void On_System_SecTimer(Msg_System_SecTimer* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

  Client* FindClient(const ApHandle& h);

protected:
  String XmppModule::getNickname();

protected:
  ClientList clients_;

  int bNetOnline_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<XmppModule> XmppModuleInstance;

#endif // !defined(XmppModule_h_INCLUDED)
