// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgServer_h_INCLUDED)
#define MsgServer_h_INCLUDED

// -> server
class Msg_Server_StartHttp: public ApRequestMessage
{
public:
  Msg_Server_StartHttp() : ApRequestMessage("Server_StartHttp") {}
};

// -> server
class Msg_Server_StopHttp: public ApRequestMessage
{
public:
  Msg_Server_StopHttp() : ApRequestMessage("Server_StopHttp") {}
};

// ------------------------------------------------------

// -> server
class Msg_Server_StartTCP: public ApRequestMessage
{
public:
  Msg_Server_StartTCP() : ApRequestMessage("Server_StartTCP") {}
};

// -> server
class Msg_Server_StopTCP: public ApRequestMessage
{
public:
  Msg_Server_StopTCP() : ApRequestMessage("Server_StopTCP") {}
};

// ------------------------------------------------------

// -> server
class Msg_Server_StartWebSocket: public ApRequestMessage
{
public:
  Msg_Server_StartWebSocket() : ApRequestMessage("Server_StartWebSocket") {}
};

// -> server
class Msg_Server_StopWebSocket: public ApRequestMessage
{
public:
  Msg_Server_StopWebSocket() : ApRequestMessage("Server_StopWebSocket") {}
};

#endif // !defined(MsgServer_h_INCLUDED)
