// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgHTTPServer_h_INCLUDED)
#define MsgHTTPServer_h_INCLUDED

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

// server ->
class Msg_Server_HttpRequest: public ApRequestMessage
{
public:
  Msg_Server_HttpRequest() : ApRequestMessage("Server_HttpRequest") {}
  ApIN ApHandle hConnection;
  ApIN String sMethod;
  ApIN String sUri;
  ApIN String sProtocol;
  ApIN String sRemoteAddress;
  ApIN Apollo::KeyValueList kvHeader;
  ApIN Buffer sbBody;
};

// -> server
class Msg_Server_HttpResponse: public ApRequestMessage
{
public:
  Msg_Server_HttpResponse() : ApRequestMessage("Server_HttpResponse"), nStatus(0) {}
  ApIN ApHandle hConnection;
  ApIN int nStatus;
  ApIN String sMessage;
  ApIN String sProtocol;
  ApIN Apollo::KeyValueList kvHeader;
  ApIN Buffer sbBody;
};

#endif // !defined(MsgHTTPServer_h_INCLUDED)
