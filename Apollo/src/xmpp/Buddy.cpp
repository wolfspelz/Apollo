// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Client.h"
#include "Buddy.h"
#include "Stanza.h"

Buddy::Buddy(Client* pClient, const char* szJid, ApHandle hBuddy)
:Elem(szJid)
,pClient_(pClient)
,hBuddy_(hBuddy)
{
}

Buddy::~Buddy()
{
}

int Buddy::presenceAvailable(Stanza& stanza)
{
  AP_UNUSED_ARG(stanza);

  int ok = 1;

  

  return ok;
}

int Buddy::presenceUnavailable(Stanza& stanza)
{
  AP_UNUSED_ARG(stanza);
  
  int ok = 1;

  return ok;
}

int Buddy::presenceError(Stanza& stanza)
{
  AP_UNUSED_ARG(stanza);
  
  int ok = 1;

  

  return ok;
}
