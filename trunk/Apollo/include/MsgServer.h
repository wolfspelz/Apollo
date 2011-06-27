// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgServer_h_INCLUDED)
#define MsgServer_h_INCLUDED

// -> server
class Msg_Server_StartHTTP: public ApRequestMessage
{
public:
  Msg_Server_StartHTTP() : ApRequestMessage("Server_StartHTTP") {}
};

// -> server
class Msg_Server_StopHTTP: public ApRequestMessage
{
public:
  Msg_Server_StopHTTP() : ApRequestMessage("Server_StopHTTP") {}
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

#endif // !defined(MsgServer_h_INCLUDED)
