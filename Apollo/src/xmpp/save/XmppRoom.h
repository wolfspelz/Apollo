// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XmppRoom_h_INCLUDED)
#define XmppRoom_h_INCLUDED

#include "Apollo.h"

#include "XmppProtocol.h"
#include "ProtocolTasks.h"

class XmppRoom : public SElem
{
public:
  XmppRoom(XmppProtocol* pProtocol, const char* szJID, ApHandle hLocation);
  virtual ~XmppRoom();

  typedef enum Phase_ {
    Phase_None,
    Phase_EnterRequested,
    Phase_Entered,
    Phase_LeaveRequested,
  } Phase;

  int Enter(const char* szNickname);
  int EnterRetryNextNickname(const char* szConflictedNickname);
  int Leave();

  int SendPublicChat(const char* szText);
  int SendPrivateChat(const char* szName, const char* szText);

  int OnPresence(Apollo::XMLNode* pStanza);
  int OnGroupChat(Apollo::XMLNode* pStanza);
  int OnSystemChat(Apollo::XMLNode* pStanza);

public:
  ApHandle hLocation_;

protected:
  XmppProtocol* pProtocol_;
  Phase nPhase_;
  SString sNick_;
  SList slVisitors_;
  int nNicknameConflictRetryCount_;
};

#endif // !defined(XmppRoom_h_INCLUDED)
