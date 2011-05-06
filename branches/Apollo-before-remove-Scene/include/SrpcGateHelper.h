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

AP_NAMESPACE_END

#endif // !defined(SrpcGateHelper_h_INCLUDED)
