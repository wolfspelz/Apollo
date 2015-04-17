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
class Msg_TcpServer_SrpcRequest: public ApRequestMessage
{
public:
  Msg_TcpServer_SrpcRequest() : ApRequestMessage("TcpServer_SrpcRequest") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
  ApOUT Apollo::SrpcMessage response;
};

// -> server
class Msg_TcpServer_SendSrpc: public ApRequestMessage
{
public:
  Msg_TcpServer_SendSrpc() : ApRequestMessage("TcpServer_SendSrpc") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// server ->
class Msg_TcpServer_Disconnected: public ApNotificationMessage
{
public:
  Msg_TcpServer_Disconnected() : ApNotificationMessage("TcpServer_Disconnected") {}
  ApIN ApHandle hConnection;
};

#endif // !defined(MsgTcpServer_h_INCLUDED)
