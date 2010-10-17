// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Room_h_INCLUDED)
#define Room_h_INCLUDED

#include "Apollo.h"
#include "Stanza.h"
#include "Participant.h"

class Client;
class Stanza;

class Room: public Elem
{
public:
  Room(const char* szJid, const ApHandle& hRoom, Client* pClient);
  virtual ~Room();

  inline ApHandle apHandle() { return hAp_; }
  inline String& getJid() { return getName(); }
  inline String& getNickname() { return sNickname_; }

  int sendState();
  int enter(String& sNickname);
  int enterRetryNextNickname(String& sConflictedNickname);
  int leave();

  int presenceAvailable(Stanza& stanza);
  int presenceUnavailable(Stanza& stanza);
  int presenceError(Stanza& stanza);

  int receiveGroupchat(Stanza& stanza);
  int sendGroupchat(String& sText);

  static String nextNickname(String& sNickname);

#if defined(AP_TEST)
  static String test_nextNickname1(const char* szNickname, const char* szExpectedNextNickname);
  static String test_nextNickname();
  static String test_parseDelay1(int nLine, const char* szXml, Apollo::TimeValue tvExpected);
  static String test_parseDelay2(int nLine, const char* szXml, Apollo::TimeValue tvExpected);
  static String test_parseDelay();
#endif // #if defined(AP_TEST)

  typedef enum Phase_ { Phase_None
    ,Phase_EnterRequested
    ,Phase_EnterBegun
    ,Phase_Entered
    ,Phase_LeaveRequested
    ,Phase_Left
  } Phase;

protected:
  static int parseXEP0082DateTime(const String& sStamp, Apollo::TimeValue& tvStamp);
  static int parseXEP0091DateTime(const String& sStamp, Apollo::TimeValue& tvStamp);
  static int timeFromDelayNode(Apollo::XMLNode& node, Apollo::TimeValue& tvStamp);

protected:
  ApHandle hAp_;
  Client* pClient_;
  int nPhase_;
  ListT<Participant, Elem> lParticipants_;
  String sNickname_;
  int nNicknameConflictRetryCount_;
};

#endif // !defined(Room_h_INCLUDED)
