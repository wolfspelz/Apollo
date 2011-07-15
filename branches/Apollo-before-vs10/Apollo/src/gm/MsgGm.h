// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgGm_h_INCLUDED)
#define MsgGm_h_INCLUDED

#include "ApMessage.h"

class Msg_Gm_Start: public ApRequestMessage
{
public:
  Msg_Gm_Start() : ApRequestMessage("Gm_Start") {}
};

class Msg_Gm_Stop: public ApRequestMessage
{
public:
  Msg_Gm_Stop() : ApRequestMessage("Gm_Stop") {}
};

class Msg_Gm_StartXmpp: public ApRequestMessage
{
public:
  Msg_Gm_StartXmpp() : ApRequestMessage("Gm_StartXmpp") {}
};

class Msg_Gm_Activate: public ApRequestMessage
{
public:
  Msg_Gm_Activate() : ApRequestMessage("Gm_Activate"), bActive(0) {}
  ApIN int bActive;
};

#endif // !defined(MsgGm_h_INCLUDED)
