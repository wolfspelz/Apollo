// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgTcpServer_h_INCLUDED)
#define MsgTcpServer_h_INCLUDED

// server ->
class Msg_TcpServer_Connected: public ApNotificationMessage
{
public:
  Msg_TcpServer_Connected() : ApNotificationMessage("TcpServer_Connected") {}
  ApIN ApHandle hConnection;
};

// server ->
class Msg_TcpServer_SrpcRequest: public ApSRPCMessage
{
public:
  Msg_TcpServer_SrpcRequest() : ApSRPCMessage("TcpServer_SrpcRequest") {}
  ApIN ApHandle hConnection;
};

// server ->
class Msg_TcpServer_Disconnected: public ApNotificationMessage
{
public:
  Msg_TcpServer_Disconnected() : ApNotificationMessage("TcpServer_Disconnected") {}
  ApIN ApHandle hConnection;
};

#endif // !defined(MsgTcpServer_h_INCLUDED)
