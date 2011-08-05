// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgWebSocketServer_h_INCLUDED)
#define MsgWebSocketServer_h_INCLUDED

// server ->
class Msg_WebSocketServer_Connected: public ApNotificationMessage
{
public:
  Msg_WebSocketServer_Connected() : ApNotificationMessage("WebSocketServer_Connected") {}
  ApIN ApHandle hConnection;
  ApIN String sOrigin;
  ApIN String sProtocol;
};

// server ->
class Msg_WebSocketServer_ReceiveText: public ApRequestMessage
{
public:
  Msg_WebSocketServer_ReceiveText() : ApRequestMessage("WebSocketServer_ReceiveText") {}
  ApIN ApHandle hConnection;
  ApIN String sData;
};

// server ->
class Msg_WebSocketServer_SrpcRequest: public ApRequestMessage
{
public:
  Msg_WebSocketServer_SrpcRequest() : ApRequestMessage("WebSocketServer_SrpcRequest") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
  ApOUT Apollo::SrpcMessage response;
};

// -> server
class Msg_WebSocketServer_SendText: public ApRequestMessage
{
public:
  Msg_WebSocketServer_SendText() : ApRequestMessage("WebSocketServer_SendText") {}
  ApIN ApHandle hConnection;
  ApIN String sData;
};

// server ->
class Msg_WebSocketServer_Disconnected: public ApNotificationMessage
{
public:
  Msg_WebSocketServer_Disconnected() : ApNotificationMessage("WebSocketServer_Disconnected") {}
  ApIN ApHandle hConnection;
};

#endif // !defined(MsgWebSocketServer_h_INCLUDED)
