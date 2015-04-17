// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SrpcGateHelper_h_INCLUDED)
#define SrpcGateHelper_h_INCLUDED

#include "SSystem.h"
#include "ApExports.h"
#include "MsgSrpcGate.h"

AP_NAMESPACE_BEGIN

class APOLLO_API SrpcGateHandlerRegistrant: public Elem
{
public:
  SrpcGateHandlerRegistrant(const char* szCallName, ApSRPCCallback fnHandler);
  ~SrpcGateHandlerRegistrant();

  ApSRPCCallback fnHandler_;

protected:
  friend class ListT<SrpcGateHandlerRegistrant, Elem>;
  SrpcGateHandlerRegistrant() {}
};

class APOLLO_API SrpcGateHandlerRegistry: protected ListT<SrpcGateHandlerRegistrant, Elem>
{
public:
  void add(const char* szCallName, ApSRPCCallback fnHandler);
  void finish() { Empty(); }
};

#define SRPCGATE_HANDLER_NATIVE_REQUEST(__pMsg, __msg) \
  if (!__msg.Request()) { \
    __pMsg->response.createError(__pMsg->srpc, __msg.sComment); \
    return; \
  } \
  __pMsg->response.createResponse(__pMsg->srpc);

AP_NAMESPACE_END

#endif // !defined(SrpcGateHelper_h_INCLUDED)
