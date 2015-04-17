// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Buddy_h_INCLUDED)
#define Buddy_h_INCLUDED

#include "Apollo.h"
#include "Stanza.h"

class Client;
class Stanza;

class Buddy: public Elem
{
public:
  Buddy(Client* pClient, const char* szJid, const ApHandle& hBuddy);
  virtual ~Buddy();

  int presenceAvailable(Stanza& stanza);
  int presenceUnavailable(Stanza& stanza);
  int presenceError(Stanza& stanza);

public:
  ApHandle hBuddy_;

protected:
  Client* pClient_;
};

#endif // !defined(Buddy_h_INCLUDED)
