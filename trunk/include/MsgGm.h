// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgGm_h_INCLUDED)
#define MsgGm_h_INCLUDED

#include "ApMessage.h"

// -> Gm
class Msg_Gm_Start: public ApRequestMessage
{
public:
  Msg_Gm_Start() : ApRequestMessage("Gm_Start") {}
};

// -> Gm
class Msg_Gm_Stop: public ApRequestMessage
{
public:
  Msg_Gm_Stop() : ApRequestMessage("Gm_Stop") {}
};

// -> Gm
class Msg_Gm_StartXmpp: public ApRequestMessage
{
public:
  Msg_Gm_StartXmpp() : ApRequestMessage("Gm_StartXmpp") {}
};

// -> Gm
class Msg_Gm_Activate: public ApRequestMessage
{
public:
  Msg_Gm_Activate() : ApRequestMessage("Gm_Activate"), bActive(0) {}
  ApIN int bActive;
};

// -> Gm
class Msg_Gm_SendRequest: public ApRequestMessage
{
public:
  Msg_Gm_SendRequest() : ApRequestMessage("Gm_SendRequest") {}
  ApIN ApHandle hRequest;
  ApIN Apollo::SrpcMessage srpc;
};

// Gm ->
class Msg_Gm_ReceiveResponse: public ApNotificationMessage
{
public:
  Msg_Gm_ReceiveResponse() : ApNotificationMessage("Gm_ReceiveResponse") {}
  ApIN ApHandle hRequest;
  ApIN Apollo::SrpcMessage srpc;
};

// Gm ->
class Msg_Gm_ReceiveRequest: public ApNotificationMessage
{
public:
  Msg_Gm_ReceiveRequest() : ApNotificationMessage("Gm_ReceiveRequest") {}
  ApIN String sReference;
  ApIN Apollo::SrpcMessage srpc;
};

// -> Gm
class Msg_Gm_SendResponse: public ApRequestMessage
{
public:
  Msg_Gm_SendResponse() : ApRequestMessage("Gm_SendResponse") {}
  ApIN String sReference;
  ApIN Apollo::SrpcMessage srpc;
};

#endif // !defined(MsgGm_h_INCLUDED)
