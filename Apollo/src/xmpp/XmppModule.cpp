// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "XmppModule.h"
#include "JabberId.h"
#include "Room.h"

XmppModule::XmppModule()
:bNetOnline_(0)
{
}

XmppModule::~XmppModule()
{
  void Exit();
}

Client* XmppModule::FindClient(const ApHandle& h)
{
  Client* pResult = 0; 

  if (!ApIsHandle(h)) {

    // If no client given, then use the default client
    ClientNode* node = 0;
    for (ClientIterator iter(clients_); (node = iter.Next()); ) {
      Client* pClient = node->Value();
      if (pClient != 0) {
        if (pClient->isDefault()) {
          pResult = pClient;
        }
      }
    }

  } else {

    // Otherwise search for the handle
    ClientNode* pNode = clients_.Find(h);
    if (pNode != 0) {
      pResult = pNode->Value();
    }
  }

  return pResult; 
}

String XmppModule::getNickname()
{
  String sNickname = Apollo::getModuleConfig(MODULE_NAME, "Room/Nickname", Apollo::getRandomString(10));
  if (sNickname.empty()) {
    JabberId jid = Apollo::getModuleConfig(MODULE_NAME, "Connection/JabberId", "");
    if (!jid.empty()) {
      sNickname = jid.user();
    }
  }
  if (sNickname.empty()) {
    sNickname = "NoName";
  }
  return sNickname;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_GetStatus)
{
  pMsg->apStatus = ApMessage::Ok;
}

// This is the default handler. Can be overidden by other modules.
AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_GetAutoConnect)
{
  int ok = 1;

  pMsg->bConnect = Apollo::getModuleConfig(MODULE_NAME, "Connection/AutoConnect", 1);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_StartClient)
{
  int ok = 0;

  if (pMsg->sJabberId.empty()) { throw ApException("XmppModule::Xmpp_StartClient sJabberId empty"); }
  if (pMsg->sPassword.empty()) { throw ApException("XmppModule::Xmpp_StartClient sPassword empty"); }

  Client* pClient = FindClient(pMsg->hClient);
  if (pClient != 0) {
    apLog_Warning((LOG_CHANNEL, "XmppModule::On_Xmpp_StartClient", "Client " ApHandleFormat " already exists", ApHandleType(pMsg->hClient)));
  } else {

    pClient = new Client(pMsg->hClient);
    if (pClient != 0) {
      pClient->setJabberId(pMsg->sJabberId);
      pClient->setPassword(pMsg->sPassword);

      String sResource = pMsg->sResource;
      if (sResource.empty()) {
        sResource = Apollo::getUniqueId();
      }
      pClient->setResource(sResource);

      pClient->netOnline(bNetOnline_);
      ok = pClient->start();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "XmppModule::On_Xmpp_StartClient", "Client " ApHandleFormat " Client::Start() failed, jid=%s", ApHandleType(pMsg->hClient), StringType(pMsg->sJabberId)));
        delete pClient;
        pClient = 0;
      } else {
        clients_.Set(pMsg->hClient, pClient);
      }

      if (clients_.Count() == 1) {
        Msg_Xmpp_DefaultClient msg;
        msg.hClient = pMsg->hClient;
        if (!msg.Request()) {
          apLog_Error((LOG_CHANNEL, "XmppModule::On_Xmpp_StartClient", "Msg_Xmpp_DefaultClient failed " ApHandleFormat "", ApHandleType(pMsg->hClient)));
        }
      }
    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_StopClient)
{
  int ok = 0;

  Client* pClient = FindClient(pMsg->hClient);
  if (pClient == 0) {
    apLog_Warning((LOG_CHANNEL, "XmppModule::On_Xmpp_StopClient", "Client " ApHandleFormat " not found", ApHandleType(pMsg->hClient)));
  } else {
    (void) pClient->stop();
    clients_.Unset(pMsg->hClient);
    delete pClient;
    pClient = 0;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_DefaultClient)
{
  int ok = 0;

  ClientNode* node = 0;
  for (ClientIterator iter(clients_); (node = iter.Next()); ) {
    Client* pClient = node->Value();
    if (pClient != 0) {
      pClient->setDefault(0);
    }
  }

  Client* pClient = FindClient(pMsg->hClient);
  if (pClient == 0) {
    apLog_Warning((LOG_CHANNEL, "XmppModule::On_Xmpp_DefaultClient", "Client " ApHandleFormat " not found", ApHandleType(pMsg->hClient)));
  } else {
    pClient->setDefault(1);
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------

#define FORWARD_TO_CLIENT_PRE(_method_) \
  Client* pClient = FindClient(pMsg->hClient); \
  if (pClient == 0) { \
    apLog_Warning((LOG_CHANNEL, _method_, "Client " ApHandleFormat " not found", ApHandleType(pMsg->hClient))); \
  } else {

#define FORWARD_TO_CLIENT_POST(_method_) \
    if (!ok) { \
      apLog_Error((LOG_CHANNEL, _method_, "Client " ApHandleFormat " client call failed", ApHandleType(pMsg->hClient))); \
    } \
  }

//----------------------

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Connect)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_Connect");
  if (pMsg->nDelaySec == 0) {
    ok = pClient->connectNow();
  } else {
    ok = pClient->connectDeferred(pMsg->nDelaySec);
  }
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_Connect");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Disconnect)
{
  int ok = 1;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_Disconnect");
  ok = pClient->disconnect();
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_Disconnect");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_DataIn)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_DataIn");
  ok = pClient->dataIn(pMsg->sbData.Data(), pMsg->sbData.Length());
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_DataIn");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_DataOut)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_DataOut");
  ok = pClient->dataOut(pMsg->sbData.Data(), pMsg->sbData.Length());
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_DataOut");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_StanzaIn)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_StanzaIn");
  ok = pClient->stanzaIn(pMsg->sData);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_StanzaIn");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_StanzaOut)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_StanzaOut");
  ok = pClient->stanzaOut(pMsg->sData);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_StanzaOut");
  
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_EnterRoom)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_EnterRoom");
  ok = pClient->enterRoom(pMsg->sRoomJid, pMsg->sNickname, pMsg->hRoom);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_EnterRoom");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_LeaveRoom)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_LeaveRoom");
  ok = pClient->leaveRoom(pMsg->hRoom);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_LeaveRoom");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_GetRoomHandle)
{
  int ok = 1;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_PublicChat");
  pMsg->hRoom = pClient->getRoomHandle(pMsg->sRoomJid);
  if (!ApIsHandle(pMsg->hRoom)) {
    ok = 0;
  }
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_PublicChat");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_SendGroupchat)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_SendGroupchat");
  ok = pClient->sendGroupchat(pMsg->hRoom, pMsg->sText);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_SendGroupchat");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_SendRoomState)
{
  int ok = 0;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_SendRoomState");
  ok = pClient->sendRoomState(pMsg->hRoom);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_SendRoomState");

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//---------------------------

AP_MSG_HANDLER_METHOD(XmppModule, Protocol_IsOnline)
{
  if (pMsg->sProtocol == PROTOCOL_NAME) {
    int ok = 0;

    // ignore feature, just check for loggedin-ness
    Client* pClient = FindClient(ApNoHandle);
    if (pClient == 0) {
      apLog_Warning((LOG_CHANNEL, "XmppModule::On_Protocol_IsOnline", "Default client not found"));
    } else {

      pMsg->bOnline = pClient->isLoggedIn();
      ok = 1;
    
    }

    pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, Protocol_EnterRoom)
{
  if (pMsg->sProtocol == PROTOCOL_NAME) {
    int ok = 0;

    // Forward the request if xmpp protocol
    Msg_Xmpp_EnterRoom msg;
    msg.hClient = ApNoHandle; // default client
    msg.hRoom = pMsg->hRoom;
    msg.sRoomJid = pMsg->sRoom;
    msg.sNickname = getNickname();
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "XmppModule::On_Protocol_EnterRoom", "Msg_Xmpp_EnterRoom failed, room=" ApHandleFormat " jid=%s", ApHandleType(pMsg->hRoom), StringType(pMsg->sRoom)));
    }

    pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  }
}

//---------------------------

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_Begin)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_Begin msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_Status)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_Status msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sStatus = pMsg->sStatus;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_StatusMessage)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_StatusMessage msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sMessage = pMsg->sMessage;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_JabberId)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_JabberId msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sJabberId = pMsg->sJabberId;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_MucUser)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_MucUser msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sJabberId = pMsg->sJabberId;
  msg.sAffiliation = pMsg->sAffiliation;
  msg.sRole = pMsg->sRole;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_FirebatFeatures)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_FirebatFeatures msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sFeatures = pMsg->sFeatures;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_Participant_End)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Participant_End msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.Send();
}

//---------------------------

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_ReceiveGroupchat)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_ReceivePublicChat msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sText = pMsg->sText;
  msg.nSec = pMsg->nSec;
  msg.nMicroSec = pMsg->nMicroSec;
  msg.Send();
}

//---------------------------

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_EnteringRoom)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_EnteringRoom msg; 
  msg.sProtocol = PROTOCOL_NAME; 
  msg.hRoom = pMsg->hRoom; 
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_EnterRoomComplete)
{
  int ok = 1;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_LeaveRoomComplete");
  ok = pClient->enterRoomComplete(pMsg->hRoom);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_LeaveRoomComplete");

  // Forward the notification in a protocol independent way
  Msg_Protocol_EnterRoomComplete msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Protocol_LeaveRoom)
{
  if (pMsg->sProtocol == PROTOCOL_NAME) {
    int ok = 0;

    // Forward the request if xmpp protocol
    Msg_Xmpp_LeaveRoom msg;
    msg.hClient = ApNoHandle; // default client
    msg.hRoom = pMsg->hRoom;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "XmppModule::On_Protocol_LeaveRoom", "Msg_Xmpp_LeaveRoom failed, room=" ApHandleFormat "", ApHandleType(pMsg->hRoom)));
    }

    pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_LeavingRoom)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_LeavingRoom msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_LeaveRoomComplete)
{
  int ok = 1;

  FORWARD_TO_CLIENT_PRE("XmppModule::On_Xmpp_LeaveRoomComplete");
  ok = pClient->leaveRoomComplete(pMsg->hRoom);
  FORWARD_TO_CLIENT_POST("XmppModule::On_Xmpp_LeaveRoomComplete");

  // Forward the notification in a protocol independent way
  Msg_Protocol_LeaveRoomComplete msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.Send();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_ParticipantEntered)
{
  int ok = 1;

  // Forward the notification in a protocol independent way
  Msg_Protocol_ParticipantEntered msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.sNickname = pMsg->sNickname;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_ParticipantLeft)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_ParticipantLeft msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_ParticipantSelf)
{
  int ok = 1;

  // Forward the notification in a protocol independent way
  Msg_Protocol_ParticipantSelf msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.hRoom = pMsg->hRoom;
  msg.hParticipant = pMsg->hParticipant;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_LoggedIn)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Online msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Xmpp_LoggedOut)
{
  // Forward the notification in a protocol independent way
  Msg_Protocol_Offline msg;
  msg.sProtocol = PROTOCOL_NAME;
  msg.Send();
}

AP_MSG_HANDLER_METHOD(XmppModule, Protocol_SendPublicChat)
{
  if (pMsg->sProtocol == PROTOCOL_NAME) {
    int ok = 0;

    // Forward the request if xmpp protocol
    Msg_Xmpp_SendGroupchat msg;
    msg.hClient = ApNoHandle; // default client
    msg.hRoom = pMsg->hRoom;
    msg.sText = pMsg->sText;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "XmppModule::On_Protocol_SendPublicChat", "Msg_Xmpp_SendGroupchat failed, room=" ApHandleFormat "", ApHandleType(pMsg->hRoom)));
    }

    pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, Protocol_Room_CommitState)
{
  if (pMsg->sProtocol == PROTOCOL_NAME) {
    int ok = 0;

    // Forward the request if xmpp protocol
    Msg_Xmpp_SendRoomState msg;
    msg.hClient = ApNoHandle; // default client
    msg.hRoom = pMsg->hRoom;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "XmppModule::On_Protocol_Room_CommitState", "Msg_Xmpp_SendRoomState failed, room=" ApHandleFormat "", ApHandleType(pMsg->hRoom)));
    }

    pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  }
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(XmppModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    if (Apollo::getModuleConfig(MODULE_NAME, "StartOnRunLevelNormal", 1)) {
      Msg_Xmpp_StartClient msg;
      msg.hClient = Apollo::newHandle();
      msg.sJabberId = Apollo::getModuleConfig(MODULE_NAME, "Connection/JabberId", "");
      msg.sPassword = Apollo::getModuleConfig(MODULE_NAME, "Connection/Password", "");
      msg.sResource = Apollo::getModuleConfig(MODULE_NAME, "Connection/Resource", "");
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "XmppModule::On_System_RunLevel", "Failed to start client"));
      }
    }

  } else if (pMsg->sLevel == Msg_System_RunLevel_Shutdown) {
    ClientNode* node = 0;
    for (ClientIterator iter(clients_); (node = iter.Next()); ) {
      Client* pClient = node->Value();
      if (pClient != 0) {
        pClient->stop();
      }
    }

  }
}

AP_MSG_HANDLER_METHOD(XmppModule, Net_Online)
{
  AP_UNUSED_ARG(pMsg);

  int bChanged = 0;
  if (!bNetOnline_) {
    bChanged = 1;
    bNetOnline_ = 1;
  }

  if (bChanged) {
    ClientNode* node = 0;
    for (ClientIterator iter(clients_); (node = iter.Next()); ) {
      Client* pClient = node->Value();
      if (!pClient->netOnline(1)) {
        apLog_Error((LOG_CHANNEL, "XmppModule::On_Net_Online", "pClient->netOnline() failed"));
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, Net_Offline)
{
  AP_UNUSED_ARG(pMsg);

  int bChanged = 0;
  if (bNetOnline_) {
    bChanged = 1;
    bNetOnline_ = 0;
  }

  if (bChanged) {
    ClientNode* node = 0;
    for (ClientIterator iter(clients_); (node = iter.Next()); ) {
      Client* pClient = node->Value();
      if (!pClient->netOnline(0)) {
        apLog_Error((LOG_CHANNEL, "XmppModule::On_Net_Online", "pClient->netOnline() failed"));
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, System_SecTimer)
{
  AP_UNUSED_ARG(pMsg);

  ClientNode* node = 0;
  for (ClientIterator iter(clients_); (node = iter.Next()); ) {
    Client* pClient = node->Value();
    pClient->idle();
  }
}

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(XmppModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Xmpp", 0)) {
    AP_UNITTEST_REGISTER(JabberId::Test);
    AP_UNITTEST_REGISTER(Room::test_nextNickname);
    AP_UNITTEST_REGISTER(Room::test_parseDelay);
    AP_UNITTEST_REGISTER(Participant::test_presence);
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Xmpp", 0)) {
    AP_UNITTEST_EXECUTE(JabberId::Test);
    AP_UNITTEST_EXECUTE(Room::test_nextNickname);
    AP_UNITTEST_EXECUTE(Room::test_parseDelay);
    AP_UNITTEST_EXECUTE(Participant::test_presence);
  }
}

AP_MSG_HANDLER_METHOD(XmppModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int XmppModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_GetStatus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_StartClient, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_StopClient, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_DefaultClient, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Connect, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Disconnect, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_GetAutoConnect, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_DataIn, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_DataOut, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_StanzaIn, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_StanzaOut, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_EnterRoom, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_LeaveRoom, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_GetRoomHandle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_SendGroupchat, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_SendRoomState, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Protocol_IsOnline, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Protocol_EnterRoom, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Protocol_LeaveRoom, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_EnteringRoom, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_EnterRoomComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_LeavingRoom, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_LeaveRoomComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_ParticipantEntered, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_ParticipantLeft, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_ParticipantSelf, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_LoggedIn, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_LoggedOut, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Protocol_SendPublicChat, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Protocol_Room_CommitState, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_Status, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_StatusMessage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_JabberId, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_MucUser, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_FirebatFeatures, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_Participant_End, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Xmpp_ReceiveGroupchat, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, System_RunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Net_Online, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, Net_Offline, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, System_SecTimer, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, XmppModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

int XmppModule::Exit()
{
  int ok = 1;

  AP_MSG_REGISTRY_FINISH;

  return ok;
}
