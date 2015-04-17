// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgNet_h_INCLUDED)
#define MsgNet_h_INCLUDED

#include "ApMessage.h"
#include "Apollo.h"

// net ->
class Msg_Net_Online: public ApNotificationMessage
{
public:
  Msg_Net_Online() : ApNotificationMessage("Net_Online") {}
};

// net ->
class Msg_Net_Offline: public ApNotificationMessage
{
public:
  Msg_Net_Offline() : ApNotificationMessage("Net_Offline") {}
};

// -> net
class Msg_Net_IsOnline: public ApRequestMessage
{
public:
  Msg_Net_IsOnline() : ApRequestMessage("Net_IsOnline"), bIsOnline(0) {}
  ApOUT int bIsOnline;
  static int _()
  {
    Msg_Net_IsOnline msg;
    int bResult = 0;
    if (msg.Request()) {
      bResult = msg.bIsOnline;
    }
    return bResult;
  }
};

// ------------------------------

// -> net
#define Msg_Net_DNS_Resolve_Type_Default "" // A + CNAME
#define Msg_Net_DNS_Resolve_Type_SRV "SRV" // SRV
class Msg_Net_DNS_Resolve: public ApRequestMessage
{
public:
  Msg_Net_DNS_Resolve() : ApRequestMessage("Net_DNS_Resolve") {}
  ApIN ApHandle hResolver;
  ApIN String sName;
  ApIN String sType;
};

// net ->
class Msg_Net_DNS_Resolved: public ApNotificationMessage
{
public:
  Msg_Net_DNS_Resolved() : ApNotificationMessage("Net_DNS_Resolved"), bSuccess(1) {}
  ApIN ApHandle hResolver;
  ApIN int bSuccess;
  ApIN String sName;
  ApIN String sType;
  ApIN String sAddress;
};

// ------------------------------

class Msg_Net_TCP_Connect: public ApRequestMessage
{
public:
  Msg_Net_TCP_Connect() : ApRequestMessage("Net_TCP_Connect") {}
  ApIN ApHandle hConnection;
  ApIN String sHost;
  ApIN int nPort;
};

class Msg_Net_TCP_Connected: public ApNotificationMessage
{
public:
  Msg_Net_TCP_Connected() : ApNotificationMessage("Net_TCP_Connected"), bSuccess(1) {}
  ApIN ApHandle hConnection;
  ApIN int bSuccess;
};

class Msg_Net_TCP_Close: public ApRequestMessage
{
public:
  Msg_Net_TCP_Close() : ApRequestMessage("Net_TCP_Close") {}
  ApIN ApHandle hConnection;
};

class Msg_Net_TCP_Closed: public ApNotificationMessage
{
public:
  Msg_Net_TCP_Closed() : ApNotificationMessage("Net_TCP_Closed") {}
  ApIN ApHandle hConnection;
};

class Msg_Net_TCP_DataIn: public ApNotificationMessage
{
public:
  Msg_Net_TCP_DataIn() : ApNotificationMessage("Net_TCP_DataIn") {}
  ApIN ApHandle hConnection;
  ApIN Buffer sbData;
};

class Msg_Net_TCP_DataOut: public ApRequestMessage
{
public:
  Msg_Net_TCP_DataOut() : ApRequestMessage("Net_TCP_DataOut") {}
  ApIN ApHandle hConnection;
  ApIN Buffer sbData;
};

// ------------------------------

class Msg_Net_TCP_Listen : public ApRequestMessage
{
public:
  Msg_Net_TCP_Listen() : ApRequestMessage("Net_TCP_Listen"), hServer(ApNoHandle), nPort(0) {}
public:
  ApIN ApHandle hServer;
  ApIN String sAddress;
  ApIN int nPort;
};

class Msg_Net_TCP_Listening : public ApNotificationMessage
{
public:
  Msg_Net_TCP_Listening() : ApNotificationMessage("Net_TCP_Listening"), hServer(ApNoHandle), bSuccess(0), hCnt_(Apollo::newHandle())
  {
    ApHandle x = this->hCnt_;
  }
  ~Msg_Net_TCP_Listening()
  {
    ApHandle x = this->hCnt_;
  }
public:
  ApIN ApHandle hServer;
  ApIN int bSuccess;
  ApHandle hCnt_;
};

class Msg_Net_TCP_ListenStop : public ApRequestMessage
{
public:
  Msg_Net_TCP_ListenStop() : ApRequestMessage("Net_TCP_ListenStop") {}
public:
  ApIN ApHandle hServer;
};

class Msg_Net_TCP_ListenStopped : public ApNotificationMessage 
{
public:
  Msg_Net_TCP_ListenStopped() : ApNotificationMessage("Net_TCP_ListenStopped") {}
public:
  ApIN ApHandle hServer;
};

class Msg_Net_TCP_ConnectionAccepted : public ApNotificationMessage
{
public:
  Msg_Net_TCP_ConnectionAccepted() : ApNotificationMessage("Net_TCP_ConnectionAccepted"), hServer(ApNoHandle), nClientPort(0) {}
public:
  ApIN ApHandle hServer;
  ApIN ApHandle hConnection;
  ApIN String sClientAddress;
  ApIN int nClientPort;
};

// ------------------------------

class Msg_Net_HTTP_Request: public ApRequestMessage
{
public:
  Msg_Net_HTTP_Request() : ApRequestMessage("Net_HTTP_Request"), sMethod("GET") {}
  ApIN String sMethod;
  ApIN ApHandle hClient;
  ApIN String sUrl;
  ApIN Apollo::KeyValueList kvHeader;
  ApIN Buffer sbBody;
  ApIN String sReason; // reason for the request like "fetching the user's image"
};

class Msg_Net_HTTP_Cancel: public ApRequestMessage
{
public:
  Msg_Net_HTTP_Cancel() : ApRequestMessage("Net_HTTP_Cancel") {}
  ApIN ApHandle hClient;
};

class Msg_Net_HTTP_Connected: public ApNotificationMessage
{
public:
  Msg_Net_HTTP_Connected() : ApNotificationMessage("Net_HTTP_Connected") {}
  ApIN ApHandle hClient;
};

class Msg_Net_HTTP_Header: public ApNotificationMessage
{
public:
  Msg_Net_HTTP_Header() : ApNotificationMessage("Net_HTTP_Header") {}
  ApIN ApHandle hClient;
  ApIN Apollo::KeyValueList kvHeader;
  ApIN int nStatus;
};

class Msg_Net_HTTP_DataIn: public ApNotificationMessage
{
public:
  Msg_Net_HTTP_DataIn() : ApNotificationMessage("Net_HTTP_DataIn") {}
  ApIN ApHandle hClient;
  ApIN Buffer sbData;
};

class Msg_Net_HTTP_Failed: public ApNotificationMessage
{
public:
  Msg_Net_HTTP_Failed() : ApNotificationMessage("Net_HTTP_Failed") {}
  ApIN ApHandle hClient;
  ApIN String sMessage;
};

class Msg_Net_HTTP_Closed: public ApNotificationMessage
{
public:
  Msg_Net_HTTP_Closed() : ApNotificationMessage("Net_HTTP_Closed") {}
  ApIN ApHandle hClient;
};

class Msg_Net_HTTP_CancelAll: public ApRequestMessage
{
public:
  Msg_Net_HTTP_CancelAll() : ApRequestMessage("Net_HTTP_CancelAll") {}
};

#endif // !defined(MsgNet_h_INCLUDED)
