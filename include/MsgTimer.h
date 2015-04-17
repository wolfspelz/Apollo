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
  static ApHandle Interval(int nSec, int nMicroSec, const String& sName)
  {
    ApHandle hTimer = Apollo::newHandle();
    Msg_Timer_Start msg;
    msg.hTimer = hTimer;
    msg.nSec = nSec;
    msg.nMicroSec = nMicroSec;
    msg.nCount = 0; // 0 means infinite
    msg.sName = sName;
    if (!msg.Request()) {
      hTimer = ApNoHandle;
    }
    return hTimer;
  }
  static ApHandle Timeout(int nSec, int nMicroSec, const String& sName)
  {
    ApHandle hTimer = Apollo::newHandle();
    Msg_Timer_Start msg;
    msg.hTimer = hTimer;
    msg.nSec = nSec;
    msg.nMicroSec = nMicroSec;
    msg.nCount = 1; // only once
    msg.sName = sName;
    if (!msg.Request()) {
      hTimer = ApNoHandle;
    }
    return hTimer;
  }
public:
  ApIN ApHandle hTimer;
  ApIN int nSec;
  ApIN int nMicroSec;
  ApIN int nCount;
  ApIN String sName; // optional
};

// -> timer source
class APOLLO_API Msg_Timer_Cancel: public ApRequestMessage
{
public:
  Msg_Timer_Cancel() : ApRequestMessage("Timer_Cancel") {}
  static int Timeout(const ApHandle& hTimer)
  {
    int ok = 0;
    Msg_Timer_Cancel msg;
    msg.hTimer = hTimer;
    msg.Request();
    return msg.Request();
  }
  static int Interval(const ApHandle& hTimer) { return Timeout(hTimer); }
public:
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
