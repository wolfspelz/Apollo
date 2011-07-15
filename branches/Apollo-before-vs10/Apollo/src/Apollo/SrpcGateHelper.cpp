// ============================================================================
//
// Apollo
//
// ============================================================================

#include "SrpcGateHelper.h"

AP_NAMESPACE_BEGIN

SrpcGateHandlerRegistrant::SrpcGateHandlerRegistrant(const char* szCallName, ApSRPCCallback fnHandler)
:Elem(szCallName)
,fnHandler_(fnHandler)
{
  Msg_SrpcGate_Register msg;
  msg.sCallName = szCallName;
  msg.fnHandler = fnHandler;
  (void) msg.Request();
}

SrpcGateHandlerRegistrant::~SrpcGateHandlerRegistrant()
{
  Msg_SrpcGate_Unregister msg;
  msg.sCallName = getName();
  (void) msg.Request();
}

void SrpcGateHandlerRegistry::add(const char* szCallName, ApSRPCCallback fnHandler)
{
  SrpcGateHandlerRegistrant* p = new SrpcGateHandlerRegistrant(szCallName, fnHandler);
  if (p != 0) {
    AddFirst(p);
  }
}

AP_NAMESPACE_END
