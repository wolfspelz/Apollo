// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SrpcServer_h_INCLUDED)
#define SrpcServer_h_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

class SrpcServer: public Apollo::TCPServer
{
public:
  SrpcServer() {}
  virtual ~SrpcServer() {}

protected:
  virtual Apollo::TCPConnection* OnConnectionAccepted(String& sName, ApHandle hConnection, String& sClientAddress, int nClientPort);
  virtual int OnStarted();
  virtual int OnStopped();
};

#endif // !defined(SrpcServer_h_INCLUDED)
