// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Participant_h_INCLUDED)
#define Participant_h_INCLUDED

#include "Apollo.h"

class Client;
class Room;
class Stanza;

class Participant: public Elem
{
public:
  Participant(const char* szNickname, const ApHandle& hParticipant, Client* pClient, Room* pRoom);
  virtual ~Participant();

  inline ApHandle apHandle() { return hAp_; }
  
  int presence(Stanza& stanza);

#if defined(AP_TEST)
  static String Participant::test_presence();
#endif // #if defined(AP_TEST)

protected:
  ApHandle hAp_;
  Client* pClient_;
  Room* pRoom_;
  String sNickname_;

protected:
  friend class ListT<Participant, Elem>;
  Participant() {}
};

#endif // !defined(Participant_h_INCLUDED)
