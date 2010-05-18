// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(RoomVisitor_h_INCLUDED)
#define RoomVisitor_h_INCLUDED

#include "Apollo.h"

class XmppRoom;

class RoomVisitor : public SElem
{
public:
  RoomVisitor(XmppRoom* pRoom, ApHandle hVisitor, const char* szName);
  virtual ~RoomVisitor();

  int OnPresence(Apollo::XMLNode* pStanza);
  int OnGroupChat(Apollo::XMLNode* pStanza);

  XmppRoom* pRoom_;
  ApHandle hVisitor_;

  SString sFirebatClientJID_;
  SString sFirebatUserJID_;

  SString sMucClientJID_;
  SString sMucUserJID_;
};

#endif // !defined(RoomVisitor_h_INCLUDED)
