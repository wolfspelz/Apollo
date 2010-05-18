// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSrpcGate_h_INCLUDED)
#define MsgSrpcGate_h_INCLUDED

#include "ApMessage.h"

typedef void (*ApSRPCCallback) (ApSRPCMessage* pMsg);

// -> msggate
class Msg_SrpcGate_Register: public ApRequestMessage
{
public:
  Msg_SrpcGate_Register() : ApRequestMessage("SrpcGate_Register"), fnHandler(0), nRef(0) {}
  ApIN String sCallName;
  ApIN ApSRPCCallback fnHandler;
  ApIN ApCallbackRef nRef;
};

// -> msggate
class Msg_SrpcGate_Unregister: public ApRequestMessage
{
public:
  Msg_SrpcGate_Unregister() : ApRequestMessage("SrpcGate_Unregister") {}
  ApIN String sCallName;
};

#endif // !defined(MsgSrpcGate_h_INCLUDED)
