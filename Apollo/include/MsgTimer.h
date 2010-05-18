// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgTimer_h_INCLUDED)
#define MsgTimer_h_INCLUDED

// -> timer source
class APOLLO_API Msg_Timer_Start: public ApRequestMessage
{
public:
  Msg_Timer_Start() : ApRequestMessage("Timer_Start"), nSec(0), nMicroSec(0), nCount(0) {}
  ApIN ApHandle hTimer;
  ApIN int nSec;
  ApIN int nMicroSec;
  ApIN int nCount;
};

// -> timer source
class APOLLO_API Msg_Timer_Cancel: public ApRequestMessage
{
public:
  Msg_Timer_Cancel() : ApRequestMessage("Timer_Cancel") {}
  ApIN ApHandle hTimer;
};

// timer source -> 
class APOLLO_API Msg_Timer_Event: public ApNotificationMessage
{
public:
  Msg_Timer_Event() : ApNotificationMessage("Timer_Event") {}
  ApIN ApHandle hTimer;
};

// -> timer source
class APOLLO_API Msg_Timer_ClearAll: public ApRequestMessage
{
public:
  Msg_Timer_ClearAll() : ApRequestMessage("Timer_ClearAll") {}
};

#endif // !defined(MsgTimer_h_INCLUDED)
