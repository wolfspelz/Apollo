// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ProtocolTasks_h_INCLUDED)
#define ProtocolTasks_h_INCLUDED

#include "XMLProcessor.h"

class XmppProtocol;

// -------------------------------------------------------------------

class XmppStanza : public Apollo::XMLNode
{
public:
  XmppStanza(const char* szName);
};

class XmppIQStanza : public XmppStanza
{
public:
  XmppIQStanza();
  Apollo::XMLNode& AddQuery(const char* szNameSpace);
};

class XmppGetStanza : public XmppIQStanza
{
public:
  XmppGetStanza(int nId, const char* szTo);
};

class XmppSetStanza : public XmppIQStanza
{
public:
  XmppSetStanza(int nId, const char* szTo);
};

// ----------------------------------

class XmppPresenceStanza : public XmppStanza
{
public:
  XmppPresenceStanza(const char* szType);
};

// ----------------------------------

class XmppMessageStanza : public XmppStanza
{
public:
  XmppMessageStanza(const char* szType);
};

// -------------------------------------------------------------------

class XmppProtocolTask : public SElem
{
public:
  XmppProtocolTask()
    :bDone_(0)
  {}
  virtual int OnStanza(Apollo::XMLNode* pStanza);

  int Done() { return bDone_; }

protected:
  int bDone_;
};

// -------------------------------------------------------------------

class LoginTask : public XmppProtocolTask
{
public:
  LoginTask(XmppProtocol* pProtocol)
    :pProtocol_(pProtocol)
    ,nPhase_(StreamOpen)
    ,nId_(0)
  {}
  int OnStanza(Apollo::XMLNode* pStanza);

  int SendAuthMechQuery();
  int SendDigestAuth();
  int SendPasswordAuth();

  typedef enum LoginPhase_ {
    StreamOpen,
    AuthMechanisms,
    DigestAuth,
    PasswordAuth,
    LoggedIn
  } LoginPhase;

  XmppProtocol* pProtocol_;
  LoginPhase nPhase_;
  SString sStreamId_;
  int nId_;
};

// -------------------------------------------------------------------

class OnlinePresenceTask : public XmppProtocolTask
{
public:
  OnlinePresenceTask(XmppProtocol* pProtocol)
    :pProtocol_(pProtocol)
  {}
  int OnStanza(Apollo::XMLNode* pStanza);

  XmppProtocol* pProtocol_;
};

// -------------------------------------------------------------------

class RoomPresenceTask : public XmppProtocolTask
{
public:
  RoomPresenceTask(XmppProtocol* pProtocol)
    :pProtocol_(pProtocol)
  {}
  int OnStanza(Apollo::XMLNode* pStanza);

  XmppProtocol* pProtocol_;
};

// -------------------------------------------------------------------

class GroupChatTask : public XmppProtocolTask
{
public:
  GroupChatTask(XmppProtocol* pProtocol)
    :pProtocol_(pProtocol)
  {}
  int OnStanza(Apollo::XMLNode* pStanza);

  XmppProtocol* pProtocol_;
};

#endif // !defined(ProtocolTasks_h_INCLUDED)
