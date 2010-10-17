// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Resolver_h_INCLUDED)
#define Resolver_h_INCLUDED

#include "Apollo.h"
#include "NetOS.h"
#include "SocketIO.h"

//----------------------------------------------------------

class Resolver;

class DNSResolveTask: public Apollo::Task
{
public:
  DNSResolveTask(const ApHandle& hAp, String& sName)
    :hAp_(hAp)
    ,sName_(sName)
  {}
  void Execute();

  ApHandle hAp_;
  String sName_;
};

//----------------------------------------------------------

class Resolver: public Apollo::Thread
{
public:
  Resolver()
    : Thread("Resolver")
    , bFinished_(0)
  {}

  void CloseAll();

public:
  int IsFinished() { return bFinished_; }
protected:
  int bFinished_;

};

#endif // !defined(Resolver_h_INCLUDED)
