// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "wintimer.h"
#include "ApLog.h"
#include "MsgUnitTest.h"
#include "MsgTimer.h"
#include "MsgSystem.h"
#include "MsgMainLoop.h"

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

#define MODULE_NAME "wintimer"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME, // szName
  "Timer", // szServices
  "Timer Module", // szLongName
  "1", // szVersion
  "Windows Timer based timer queue and timer event implementation", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

WINTIMER_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class TimerListElem: public Elem
{
public:
  TimerListElem(const ApHandle& hTimer_, Apollo::TimeValue& tvTime)
    :hTimer_(hTimer_)
    ,tv_(tvTime)
    ,nCount_(1)
    ,nIntervalSec_(0)
    ,nIntervalMicroSec_(0)
  {}

  ApHandle hTimer_;
  Apollo::TimeValue tv_;
  int nCount_; // 0 = infinite
  int nIntervalSec_;
  int nIntervalMicroSec_;
};

//----------------------------------------------------------

class WinTimerModule
{
public:
  WinTimerModule()
    :hWnd_(NULL)
    ,bFirstTimer_(true)
    ,nTimerQueueTimer_(-1)
  {}

  void On_Timer_ClearAll(Msg_Timer_ClearAll* pMsg);
  void On_Timer_Start(Msg_Timer_Start* pMsg);
  void On_Timer_Cancel(Msg_Timer_Cancel* pMsg);
  void On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg);
  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopEnd(Msg_MainLoop_EventLoopEnd* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  String Test_Timer_Queue();
  static String Test_Timer_Basic();
  static String Test_Timer_Interval();
  static String Test_System_SecTimer();
  static void UnitTest_TokenEnd();
  static void UnitTest_Token(Msg_UnitTest_Token* pMsg);
#endif

  int InsertTimer(TimerListElem* eTimer);
  void ProcessNextTimer();
  void ScheduleNextTimer();

protected:
	HWND GetHWnd();

  enum TimerID {
    NO_TIMER = 0xAFFE,
    SEC_TIMER,
    TIMERQUEUE_TIMER,
    QUIT_TIMER
  };

  HWND hWnd_;

  bool bFirstTimer_;

  List lTimer_;
  UINT nTimerQueueTimer_;
};

typedef ApModuleSingleton<WinTimerModule> WinTimerModuleInstance;

//----------------------------------------------------------

HWND WinTimerModule::GetHWnd()
{
  if (hWnd_ == NULL) {
    hWnd_ = Msg_Win32_GetMainWindow::_();
  }

  return hWnd_;
}

//----------------------------------------------------------

int WinTimerModule::InsertTimer(TimerListElem* eTimer)
{
  int ok = 0;
  int bReSchedule = 0;

  if (lTimer_.length() == 0) {
    // no timer active
    lTimer_.Add(eTimer);
    bReSchedule = 1;
    ok = 1;
  } else {
    TimerListElem* ePrev = 0;
    TimerListElem* eNext = 0;
    while ((eNext = (TimerListElem*) lTimer_.Next(ePrev))) {
      if (eTimer->tv_ < eNext->tv_) {
        if (ePrev == 0) {
          lTimer_.AddFirst(eTimer);
          bReSchedule = 1;
        } else {
          lTimer_.Insert(ePrev, eTimer);
        }
        break;
      } else {
        ePrev = eNext;
      }
    }
    if (eNext == 0) {
      lTimer_.AddLast(eTimer);
    }
    ok = 1;
  }

  if (bReSchedule) {
    ScheduleNextTimer();
  }

  return ok;
}

void WinTimerModule::ScheduleNextTimer()
{
  ::KillTimer(GetHWnd(), TIMERQUEUE_TIMER);

  TimerListElem* eFirst = (TimerListElem*) lTimer_.First();
  if (eFirst != 0) {
    Apollo::TimeValue tvBase = Apollo::TimeValue::getTime();
    Apollo::TimeValue tvNext = eFirst->tv_;
    Apollo::TimeValue tvDiff = tvNext - tvBase;
    int nDelayMS_ = tvDiff.Sec() * 1000 + tvDiff.MicroSec() / 1000;
    if (nDelayMS_ <= 0) {
      nDelayMS_ = 1;
    }
    nTimerQueueTimer_ = ::SetTimer(GetHWnd(), TIMERQUEUE_TIMER, nDelayMS_, NULL);
  }
}

void WinTimerModule::ProcessNextTimer()
{
  Apollo::TimeValue tvNow = Apollo::TimeValue::getTime();

  int bDone = 0;
  while (!bDone) {
    bDone = 1;
    TimerListElem* eNext = (TimerListElem*) lTimer_.First();
    if (eNext) {
      Apollo::TimeValue tvExpires = eNext->tv_;
      if (tvNow >= tvExpires) {
        bDone = 0;
        lTimer_.Remove(eNext);
        
        {
          ApAsyncMessage<Msg_Timer_Event> msg;
          msg->hTimer = eNext->hTimer_;
          msg.Post();
        }

        if (eNext->nCount_ > 0) {
          eNext->nCount_--;
          if (eNext->nCount_ == 0) {
            delete eNext;
            eNext = 0;
          }
        }

        if (eNext) {
          eNext->tv_ = tvNow + Apollo::TimeValue(eNext->nIntervalSec_, eNext->nIntervalMicroSec_);
          InsertTimer(eNext);
        }

      }
    }
  }

  ScheduleNextTimer();
}

//----------------------------------------------------------

void WinTimerModule::On_Timer_ClearAll(Msg_Timer_ClearAll* pMsg)
{
  int ok = 1;

  ::KillTimer(GetHWnd(), TIMERQUEUE_TIMER);

  TimerListElem* e = 0;
  while ((e = (TimerListElem*) lTimer_.First())) {
    lTimer_.Remove(e);
    delete e;
    e = 0;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void WinTimerModule::On_Timer_Start(Msg_Timer_Start* pMsg)
{
  int ok = 0;

  if (ApIsHandle(pMsg->hTimer) && (pMsg->nSec != 0 || pMsg->nMicroSec != 0)) {
    int bReSchedule = 0;

    Apollo::TimeValue tvNow = Apollo::TimeValue::getTime();
    Apollo::TimeValue tvExpires = tvNow + Apollo::TimeValue(pMsg->nSec, pMsg->nMicroSec);

    TimerListElem* eNew = new TimerListElem(pMsg->hTimer, tvExpires);
    if (eNew != 0) {

      if (pMsg->nCount != 1 && (pMsg->nSec != 0 || pMsg->nMicroSec != 0)) {
        eNew->nCount_ = pMsg->nCount;
        eNew->nIntervalSec_ = pMsg->nSec;
        eNew->nIntervalMicroSec_ = pMsg->nMicroSec;
      }

      ok = InsertTimer(eNew);
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void WinTimerModule::On_Timer_Cancel(Msg_Timer_Cancel* pMsg)
{
  for (TimerListElem* eTimer = 0; (eTimer = (TimerListElem*) lTimer_.Next(eTimer)) != 0; ) {
    if (eTimer->hTimer_ == pMsg->hTimer) {
      lTimer_.Remove(eTimer);
      delete eTimer;
      eTimer = 0;
      break;
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

void WinTimerModule::On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg)
{
  if (pMsg->message == WM_TIMER) {

    switch (pMsg->wParam) {

      case SEC_TIMER:
        {
          Msg_System_SecTimer msg;
          msg.Send();
          pMsg->apStatus = ApMessage::Ok;
        }
        break;

      case TIMERQUEUE_TIMER:
        {
          ProcessNextTimer();
          pMsg->apStatus = ApMessage::Ok;
        }
        break;
    } // switch
  } //  if WM_TIMER
}

void WinTimerModule::On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg)
{
  ::SetTimer(GetHWnd(), SEC_TIMER, 1000, NULL);
}

void WinTimerModule::On_MainLoop_EventLoopEnd(Msg_MainLoop_EventLoopEnd* pMsg)
{
  ::KillTimer(GetHWnd(), SEC_TIMER);
}

//----------------------------------------------------------

#if defined(AP_TEST)

class Test_Timer_Elem: public Elem
{
public:
  ApHandle hExpected_;
};

static String Test_Timer_QueueAddOne(ListT<Test_Timer_Elem, Elem>& l, int nSec, int nUSec, int nPos)
{
  String err;

  ApHandle hAp = Msg_Timer_Start::Timeout(nSec, nUSec);
  if (!ApIsHandle(hAp)) { 
    err.appendf("Msg_Timer_Start::Timeout(%d, %d) failed", nSec, nUSec);
  } else {
    Test_Timer_Elem* e = new Test_Timer_Elem();
    e->setInt(nPos);
    e->hExpected_ = hAp;
    l.AddLast(e); 
  }

  return err;
}

String WinTimerModule::Test_Timer_Queue()
{
  String err;

  if (err.empty()) {
    Msg_Timer_ClearAll msg;
    if (!msg.Request()) {
      err = "Msg_Timer_ClearAll failed";
    }
  }

  ListT<Test_Timer_Elem, Elem> lExpectedOrder;

  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 1, 0     , 3); }
  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 2, 0     , 5); }
  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 2, 0     , 6); }
  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 3, 0     , 7); }
  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 1, 500000, 4); }
  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 0, 500000, 1); }
  if (err.empty()) { Test_Timer_QueueAddOne(lExpectedOrder, 0, 500000, 2); }

  int nCnt = 0;
  for (TimerListElem* tle = 0; (tle = (TimerListElem*) lTimer_.Next(tle)); ) {
    nCnt++;
    Test_Timer_Elem& e = lExpectedOrder[nCnt];
    ApHandle hApExpected = e.hExpected_;
    ApHandle hAp = tle->hTimer_;
    if (hAp != hApExpected) {
      err.appendf("Position %d has " ApHandleFormat " expected " ApHandleFormat, nCnt, ApHandlePrintf(hAp), ApHandlePrintf(hApExpected));
      break;
    }
  }

  { Msg_Timer_ClearAll msg; msg.Request(); }

  return err;
}

//---------------

static ApHandle hTest_Timer_Basic_On_Timer_Event = ApNoHandle;

static void Test_Timer_Basic_On_Timer_Event(Msg_Timer_Event* pMsg)
{
  if (pMsg->hTimer == hTest_Timer_Basic_On_Timer_Event) {
    String err;
    if (!Msg_Timer_Cancel::Interval(hTest_Timer_Basic_On_Timer_Event)) {
      err = "Msg_Timer_Cancel::Timeout() failed";
    }
    AP_UNITTEST_RESULT(WinTimerModule::Test_Timer_Basic_Complete, err.empty(), err);
    { Msg_Timer_Event msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Timer_Basic_On_Timer_Event, 0); }  

    AP_UNITTEST_EXECUTE(WinTimerModule::Test_Timer_Interval);
  }
}

String WinTimerModule::Test_Timer_Basic()
{
  String err;

  { Msg_Timer_Event msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Timer_Basic_On_Timer_Event, 0, ApCallbackPosNormal); }

  if (err.empty()) {
    hTest_Timer_Basic_On_Timer_Event = Msg_Timer_Start::Interval(0, 10000);
    if (!ApIsHandle(hTest_Timer_Basic_On_Timer_Event)) {
      err = "Msg_Timer_Start::Intertval() failed";
    }
  }
  
  return err;
}

//---------------

static ApHandle hTest_Timer_Interval_On_Timer_Event = ApNoHandle;
static int nTest_Timer_Interval_On_Timer_Event = 0;

static void Test_Timer_Interval_On_Timer_Event(Msg_Timer_Event* pMsg)
{
  if (pMsg->hTimer == hTest_Timer_Interval_On_Timer_Event) {
    nTest_Timer_Interval_On_Timer_Event++;
    Apollo::TimeValue tv = Apollo::TimeValue::getTime();
    apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "%d %s", nTest_Timer_Interval_On_Timer_Event, _sz(tv.toString())));
    if (nTest_Timer_Interval_On_Timer_Event == 10) {
      String err;
      if (!Msg_Timer_Cancel::Interval(hTest_Timer_Interval_On_Timer_Event)) {
        err = "Msg_Timer_Cancel::Interval() failed";
      }
      AP_UNITTEST_RESULT(WinTimerModule::Test_Timer_Interval_Complete, err.empty(), err);
      { Msg_Timer_Event msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Timer_Interval_On_Timer_Event, 0); }  
      
      AP_UNITTEST_EXECUTE(WinTimerModule::Test_System_SecTimer);
      //WinTimerModule::UnitTest_TokenEnd();
    }
  }
}

String WinTimerModule::Test_Timer_Interval()
{
  String err;

  { Msg_Timer_Event msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Timer_Interval_On_Timer_Event, 0, ApCallbackPosNormal); }

  if (err.empty()) {
    hTest_Timer_Interval_On_Timer_Event = Msg_Timer_Start::Interval(0, 10000);
    if (!ApIsHandle(hTest_Timer_Interval_On_Timer_Event)) {
      err = "Msg_Timer_Start::Interval() failed";
    }
  }
  
  return err;
}

//---------------

static int nTest_System_SecTimer_On_System_SecTimer = 0;

static void Test_System_SecTimer_On_System_SecTimer(Msg_System_SecTimer* pMsg)
{
  String err;

  nTest_System_SecTimer_On_System_SecTimer++;
  if (nTest_System_SecTimer_On_System_SecTimer == 2) {
    AP_UNITTEST_RESULT(WinTimerModule::Test_System_SecTimer_Complete, err.empty(), err);
    { Msg_System_SecTimer msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_System_SecTimer_On_System_SecTimer, 0); }  
      
    WinTimerModule::UnitTest_TokenEnd();
  }
}

String WinTimerModule::Test_System_SecTimer()
{
  String err;

  { Msg_System_SecTimer msg; msg.Hook(MODULE_NAME, (ApCallback) Test_System_SecTimer_On_System_SecTimer, 0, ApCallbackPosNormal); }

  return err;
}

//---------------

void WinTimerModule::UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Finished Test/Timer"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

void WinTimerModule::UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, (ApCallback) WinTimerModule::UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Starting Test/Timer"));
  int bTokenEndNow = 0;

  AP_UNITTEST_EXECUTE(WinTimerModule::Test_Timer_Basic);

  if (bTokenEndNow) { UnitTest_TokenEnd(); }
}

//---------------

void WinTimerModule::On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Timer", 0)) {
    AP_UNITTEST_REGISTER(Test_Timer_Queue);
    AP_UNITTEST_REGISTER(WinTimerModule::Test_Timer_Basic);
    AP_UNITTEST_REGISTER(WinTimerModule::Test_Timer_Basic_Complete);
    AP_UNITTEST_REGISTER(WinTimerModule::Test_Timer_Interval);
    AP_UNITTEST_REGISTER(WinTimerModule::Test_Timer_Interval_Complete);
    AP_UNITTEST_REGISTER(WinTimerModule::Test_System_SecTimer);
    AP_UNITTEST_REGISTER(WinTimerModule::Test_System_SecTimer_Complete);
  }
}

void WinTimerModule::On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Timer", 0)) {
    AP_UNITTEST_EXECUTE(Test_Timer_Queue);
    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) WinTimerModule::UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
}

void WinTimerModule::On_UnitTest_End(Msg_UnitTest_End* pMsg)
{
  AP_UNUSED_ARG(pMsg);
}
#endif // #if defined(AP_TEST)

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, Timer_ClearAll)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, Timer_Start)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, Timer_Cancel)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, Win32_WndProcMessage)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, MainLoop_EventLoopBegin)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, MainLoop_EventLoopEnd)
#if defined(AP_TEST)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, UnitTest_Begin)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, UnitTest_Execute)
AP_REFINSTANCE_MSG_HANDLER(WinTimerModule, UnitTest_End)
#endif

//----------------------------------------------------------

WINTIMER_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  WinTimerModuleInstance::Delete();

  { Msg_Timer_ClearAll msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Timer_ClearAll), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Timer_Start msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Timer_Start), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Timer_Cancel msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Timer_Cancel), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_WndProcMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Win32_WndProcMessage), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_EventLoopBegin msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, MainLoop_EventLoopBegin), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_EventLoopEnd msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, MainLoop_EventLoopEnd), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, UnitTest_Begin), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, UnitTest_Execute), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, UnitTest_End), WinTimerModuleInstance::Get(), ApCallbackPosNormal); }
#endif

  return WinTimerModuleInstance::Get() != 0;
}

WINTIMER_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_Timer_ClearAll msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Timer_ClearAll), WinTimerModuleInstance::Get()); }
  { Msg_Timer_Start msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Timer_Start), WinTimerModuleInstance::Get()); }
  { Msg_Timer_Cancel msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Timer_Cancel), WinTimerModuleInstance::Get()); }
  { Msg_Win32_WndProcMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, Win32_WndProcMessage), WinTimerModuleInstance::Get()); }
  { Msg_MainLoop_EventLoopBegin msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, MainLoop_EventLoopBegin), WinTimerModuleInstance::Get()); }
  { Msg_MainLoop_EventLoopEnd msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, MainLoop_EventLoopEnd), WinTimerModuleInstance::Get()); }
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, UnitTest_Begin), WinTimerModuleInstance::Get()); }
  { Msg_UnitTest_Execute msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, UnitTest_Execute), WinTimerModuleInstance::Get()); }
  { Msg_UnitTest_End msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(WinTimerModule, UnitTest_End), WinTimerModuleInstance::Get()); }
#endif

  WinTimerModuleInstance::Delete();

  return 1;
}
