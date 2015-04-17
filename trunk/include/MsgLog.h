// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgLog_h_INCLUDED)
#define MsgLog_h_INCLUDED

#include "ApMessage.h"

class Msg_Log_Line: public ApNotificationMessage
{
public:
  Msg_Log_Line() : ApNotificationMessage("Log_Line"), nMask(0) {}
  ApIN int nMask;
  ApIN String sChannel;
  ApIN String sContext;
  ApIN String sMessage;
};

class Msg_Log_SetMask: public ApNotificationMessage
{
public:
  Msg_Log_SetMask() : ApNotificationMessage("Log_SetMask"), nMask(0)  {}
  ApIN int nMask;
  ApOUT int nOldMask;
};

#endif // !defined(MsgLog_h_INCLUDED)
