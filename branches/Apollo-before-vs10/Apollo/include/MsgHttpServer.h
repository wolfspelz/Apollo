// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgHttpServer_h_INCLUDED)
#define MsgHttpServer_h_INCLUDED

// server ->
class Msg_HttpServer_Request: public ApRequestMessage
{
public:
  Msg_HttpServer_Request() : ApRequestMessage("HttpServer_Request") {}
  ApIN ApHandle hConnection;
  ApIN String sMethod;
  ApIN String sUri;
  ApIN String sProtocol;
  ApIN String sRemoteAddress;
  ApIN Apollo::KeyValueList kvHeader;
  ApIN Buffer sbBody;
};

// -> server
class Msg_HttpServer_SendResponse: public ApRequestMessage
{
public:
  Msg_HttpServer_SendResponse() : ApRequestMessage("HttpServer_SendResponse"), nStatus(0) {}
  ApIN ApHandle hConnection;
  ApIN int nStatus;
  ApIN String sMessage;
  ApIN String sProtocol;
  ApIN Apollo::KeyValueList kvHeader;
  ApIN Buffer sbBody;
};

#endif // !defined(MsgHttpServer_h_INCLUDED)
