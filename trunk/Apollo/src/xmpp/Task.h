// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Task_h_INCLUDED)
#define Task_h_INCLUDED

class Client;
class Stanza;

class StanzaHandlerResult
{
public:
  StanzaHandlerResult()
    :bStanzaHandled_(0)
    ,bStanzaConsumed_(0)
    ,bTaskFinished_(0)
    ,bConnectionFinished_(0)
  {}

  int stanzaHandled() { return bStanzaHandled_; }
  int stanzaConsumed() { return bStanzaConsumed_; }
  int taskFinished() { return bTaskFinished_; }
  int connectionFinished() { return bConnectionFinished_; }

  void stanzaHandled(int b) { bStanzaHandled_ = b; }
  void stanzaConsumed(int b) { bStanzaConsumed_ = b; }
  void taskFinished(int b) { bTaskFinished_ = b; }
  void connectionFinished(int b) { bConnectionFinished_ = b; }

protected:
  int bStanzaHandled_;
  int bStanzaConsumed_;
  int bTaskFinished_;
  int bConnectionFinished_;
};

class Task : public Elem
{
public:
  Task(Client* pClient)
    :pClient_(pClient)
  {}

  virtual int handleStanza(Stanza& stanza, StanzaHandlerResult& result);

protected:
  Client* pClient_;
};

// -------------------------------------------------------------------

class LoginTask: public Task
{
public:
  LoginTask(Client* pClient)
    :Task(pClient)
    ,nPhase_(StreamOpen)
  {}

  int handleStanza(Stanza& stanza, StanzaHandlerResult& result);

  int sendAuthMechQuery();
  int sendDigestAuth();
  int sendPasswordAuth();

  typedef enum LoginPhase_ {
    StreamOpen,
    AuthMechanisms,
    DigestAuth,
    PasswordAuth,
    LoggedIn,
    LoginFailed
  } LoginPhase;

  LoginPhase nPhase_;
  String sStreamId_;
  String sId_;
};

// -------------------------------------------------------------------

class PresenceTask: public Task
{
public:
  PresenceTask(Client* pClient)
    :Task(pClient)
  {}

  int handleStanza(Stanza& stanza, StanzaHandlerResult& result);
};

// -------------------------------------------------------------------

class RoomPresenceTask: public Task
{
public:
  RoomPresenceTask(Client* pClient)
    :Task(pClient)
  {}

  int handleStanza(Stanza& stanza, StanzaHandlerResult& result);
};

// -------------------------------------------------------------------

class StreamErrorTask: public Task
{
public:
  StreamErrorTask(Client* pClient)
    :Task(pClient)
  {}

  int handleStanza(Stanza& stanza, StanzaHandlerResult& result);
};

// -------------------------------------------------------------------

class VersionTask: public Task
{
public:
  VersionTask(Client* pClient)
    :Task(pClient)
  {}

  int handleStanza(Stanza& stanza, StanzaHandlerResult& result);
  int sendResponse(Stanza& stanza);
};

// -------------------------------------------------------------------

class GroupchatTask: public Task
{
public:
  GroupchatTask(Client* pClient)
    :Task(pClient)
  {}

  int handleStanza(Stanza& stanza, StanzaHandlerResult& result);
};

#endif // !defined(Task_h_INCLUDED)
