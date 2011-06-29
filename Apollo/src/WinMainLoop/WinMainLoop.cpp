// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "WinMainLoop.h"
//#include "AutoWindowsMessage.h"
#include "ApLog.h"
#include "ApOS.h"
#include "MsgLog.h"
#include "MsgConfig.h"
#include "MsgUnitTest.h"
#include "MsgTimer.h"
#include "MsgMainLoop.h"
#include "MsgSystem.h"

#if defined(WIN32)
static HINSTANCE g_hDllInstance = NULL;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  g_hDllInstance = (HINSTANCE) hModule;
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

#define LOG_CHANNEL "WinMainLoop"
#define MODULE_NAME "WinMainLoop"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "Win32MainLoop",
  "Windows MainLoop Module",
  "1",
  "The module implements the main application loop.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

WINMAINLOOP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
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

//----------------------------

class MainLoopModule
{
public:
  MainLoopModule()
    :hAppInstance_(NULL)
    ,hWnd_(NULL)
    ,nWM_APMSG_(WM_APP + 1)
    ,nSecTimer_(-1)
    ,nSecTimerDelayMS_(1000)
    ,nQuitTimer_(-1)
    ,bFirstTimer_(1)
    ,nWaitFinishingModules_(0)
    ,nFinishedModules_(0)
    ,wParam_WM_CLOSE_(0)
    ,lParam_WM_CLOSE_(0)
    ,nTimerQueueTimer_(-1)
  {}

  int Handle_WM_COMMAND_TOOLS_OPTIONS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  int Handle_WM_COMMAND_TOOLS_TEST(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  int Handle_WM_COMMAND_TOOLS_DISPLAY(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  int Handle_WM_TIMER(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  int Handle_WM_CLOSE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  int Handle_WM_DESTROY(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  int Handle_WM_APMSG(ApMessage* pMsg);
  LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg);
  void On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg);
  void On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg);
  void On_Win32_GetInstance(Msg_Win32_GetInstance* pMsg);
  void On_Win32_GetMainWindow(Msg_Win32_GetMainWindow* pMsg);
  void On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg);
  void On_Timer_ClearAll(Msg_Timer_ClearAll* pMsg);
  void On_Timer_Start(Msg_Timer_Start* pMsg);
  void On_Timer_Cancel(Msg_Timer_Cancel* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  String Test_Timer_Queue();
  static String Test_Timer_Basic();
  static String Test_Timer_Interval();
  static void UnitTest_TokenEnd();
  static void UnitTest_Token(Msg_UnitTest_Token* pMsg);
#endif

  int InsertTimer(TimerListElem* eTimer);
  void ProcessNextTimer();
  void ScheduleNextTimer();

protected:

  enum TimerID {
    SEC_TIMER = 0xAFFE,
    TIMERQUEUE_TIMER,
    QUIT_TIMER
  };

  HINSTANCE hAppInstance_;
  HWND hWnd_;
  UINT nWM_APMSG_;
  UINT nSecTimer_;
  UINT nSecTimerDelayMS_;
  UINT nQuitTimer_;
  int bFirstTimer_;
  int nWaitFinishingModules_;
  int nFinishedModules_;
  WPARAM wParam_WM_CLOSE_;
  LPARAM lParam_WM_CLOSE_;

  List lTimer_;
  UINT nTimerQueueTimer_;
};

//----------------------------------------------------------

typedef ApModuleSingleton<MainLoopModule> MainLoopModuleInstance;

int MainLoopModule::Handle_WM_COMMAND_TOOLS_OPTIONS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(hWnd); AP_UNUSED_ARG(message); AP_UNUSED_ARG(wParam); AP_UNUSED_ARG(lParam);

  //Msg_ConfigDialog_Open msg;
  //msg.Request();

  return 1;
}

int MainLoopModule::Handle_WM_COMMAND_TOOLS_TEST(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(hWnd); AP_UNUSED_ARG(message); AP_UNUSED_ARG(wParam); AP_UNUSED_ARG(lParam);

  ApSRPCMessage msg("TestDialog_Srpc");
  msg.srpc.set(Srpc::Key::Method, "Open");
  (void) msg.Call();

  return 1;
}

int MainLoopModule::Handle_WM_COMMAND_TOOLS_DISPLAY(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(hWnd); AP_UNUSED_ARG(message); AP_UNUSED_ARG(wParam); AP_UNUSED_ARG(lParam);

  // Unused 

  return 1;
}

int MainLoopModule::Handle_WM_TIMER(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(message);
  AP_UNUSED_ARG(lParam);

  switch (wParam) {

  case SEC_TIMER:
    if (bFirstTimer_) {
      bFirstTimer_ = 0;
      Msg_MainLoop_EventLoopBegin msg;
      msg.Send();
    } else {
      Msg_System_SecTimer msg;
      msg.Send();
    }
    break;

  case TIMERQUEUE_TIMER:
    ProcessNextTimer();
    break;

  case QUIT_TIMER:
    ::KillTimer(hWnd, QUIT_TIMER);
    DefWindowProc(hWnd, WM_CLOSE, wParam_WM_CLOSE_, lParam_WM_CLOSE_);
    break;

  }

  return 1;
}

int MainLoopModule::Handle_WM_CLOSE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(message);

  wParam_WM_CLOSE_ = wParam;
  lParam_WM_CLOSE_ = lParam;

  int nDelay = Apollo::getModuleConfig(MODULE_NAME, "ShutdownDelay", 2);
  {
    Msg_MainLoop_EventLoopBeforeEnd msg;
    msg.nDelaySec = nDelay;
    msg.nWaitCount = 0;
    msg.Filter();

    nWaitFinishingModules_ = msg.nWaitCount;
    if (nWaitFinishingModules_ == 0) {
      nDelay = 0;
    } else {
      nDelay = msg.nDelaySec;
    }
  }

  nQuitTimer_ = ::SetTimer(hWnd, QUIT_TIMER, 1000 * nDelay, NULL);

  return 1;
}

int MainLoopModule::Handle_WM_DESTROY(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(hWnd);
  AP_UNUSED_ARG(message);
  AP_UNUSED_ARG(wParam);
  AP_UNUSED_ARG(lParam);

  PostQuitMessage(0);
  { Msg_MainLoop_EventLoopEnd msg; msg.Send(); }

  return 1;
}

//----------------------------------------------------------

int MainLoopModule::Handle_WM_APMSG(ApMessage* pMsg)
{
  //if (0
  //  || pMsg->getName() == "Timer_Event"
  //  || pMsg->getName() == "Animation_SequenceBegin"
  //  || pMsg->getName() == "Animation_SequenceEnd"
  //  || pMsg->getName() == "Animation_Frame"
  //  || pMsg->getName() == "UnitTest_Token"
  //  || pMsg->getName() == "Net_TCP_Listening"
  //  ) {
  //} else {
  //  int x = 1;
  //}

  Apollo::callMsg(pMsg);
  delete pMsg;

  return 1;
}

void MainLoopModule::On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg)
{
  int ok = 0;

  MainLoopModule* self = (MainLoopModule*) pMsg->Ref();
  if (self != 0) {
    if (self->hWnd_ != NULL) {
      ok = (TRUE == ::PostMessage(self->hWnd_, self->nWM_APMSG_, (LPARAM) pMsg->pMsg, 0));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  pMsg->Stop();
}

//----------------------------------------------------------

#include "res/resource.h"

LRESULT MainLoopModule::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc = NULL;

  if (0) {
  } else if (message == nWM_APMSG_) {
    (void) Handle_WM_APMSG((ApMessage*) wParam);
  }

  switch (message) {
  case WM_COMMAND:
    wmId    = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // Parse the menu selections:
    switch (wmId)
    {
    case IDM_ABOUT:
      //DialogBox(hInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;

    case ID_TOOLS_OPTIONS:
      Handle_WM_COMMAND_TOOLS_OPTIONS(hWnd, message, wParam, lParam);
      break;

    case ID_TOOLS_TEST:
      Handle_WM_COMMAND_TOOLS_TEST(hWnd, message, wParam, lParam);
      break;

    case ID_TOOLS_DISPLAY:
      Handle_WM_COMMAND_TOOLS_DISPLAY(hWnd, message, wParam, lParam);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    RECT rt;
    GetClientRect(hWnd, &rt);
    //DrawText(hdc, "Apollo", _tcslen("Apollo"), &rt, DT_CENTER);
    EndPaint(hWnd, &ps);
    break;

  case WM_TIMER:
    Handle_WM_TIMER(hWnd, message, wParam, lParam);
    break;

  case WM_CLOSE:
    Handle_WM_CLOSE(hWnd, message, wParam, lParam);
    break;

  case WM_DESTROY:
    Handle_WM_DESTROY(hWnd, message, wParam, lParam);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

static LRESULT CALLBACK MainLoopModule_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT nResult = 0;
  MainLoopModule* pMainLoopModule = 0;

	if (message == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
    SetWindowLong(hWnd, GWL_USERDATA, (LONG) lpcs->lpCreateParams);
    pMainLoopModule = (MainLoopModule*) lpcs->lpCreateParams;
  } else {
    pMainLoopModule = (MainLoopModule*) GetWindowLong(hWnd, GWL_USERDATA);
  }

  if (pMainLoopModule != 0) {
    nResult = pMainLoopModule->WndProc(hWnd, message, wParam, lParam);
  } else {
		nResult = DefWindowProc(hWnd, message, wParam, lParam);
  }

  return nResult;
}

void MainLoopModule::On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg)
{
  int ok = 1;

  String sClass = Apollo::getModuleConfig(MODULE_NAME, "WindowClass", "ApolloWinMainLoopClass");
  String sTitle = Apollo::getModuleConfig(MODULE_NAME, "WindowTitle", "ApolloWinMainLoopTitle");

  if (Apollo::getModuleConfig(MODULE_NAME, "Singleton", 1)) {
    HWND hWnd = FindWindow(sClass, sTitle);
    if (hWnd != NULL) {
      apLog_Info((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Win32Loop", "Terminating because of Singleton requirement, found window class=%s title=%s", StringType(sClass), StringType(sTitle)));
      ok = 0;
    }
  }

  if (ok) {
    hAppInstance_ = pMsg->hInstance;

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)MainLoopModule_WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(void*); // for MainLoopModule* this
    wcex.hInstance = g_hDllInstance;
    wcex.hIcon = ::LoadIcon(g_hDllInstance, (LPCTSTR) IDI_MAIN);
    wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = (LPTSTR)IDC_WIN32APP;
    wcex.lpszClassName = sClass;
    wcex.hIconSm = ::LoadIcon(g_hDllInstance, (LPCTSTR) IDI_MAIN);
    ATOM a = ::RegisterClassEx(&wcex);
    if (a == 0) {
      ok = 0;
    }
  }

  if (ok) {
    hWnd_ = ::CreateWindow(
                sClass,
                sTitle,
                WS_OVERLAPPEDWINDOW,
                0, 0, 150, 80,
                NULL,
                NULL,
                g_hDllInstance,
                this
              );
    if (hWnd_ == NULL) {
      ok = 0;
    }
  }

  if (ok) {
    if (Apollo::getModuleConfig(MODULE_NAME, "ShowWindow", 1)) {
      ::ShowWindow(hWnd_, pMsg->nCmdShow);
      ::UpdateWindow(hWnd_);
    }

    nSecTimer_ = ::SetTimer(hWnd_, SEC_TIMER, nSecTimerDelayMS_, NULL);

    HACCEL hAccelTable = ::LoadAccelerators(g_hDllInstance, (LPCTSTR) IDC_WIN32APP);
    MSG windowsMessage;

    int bReLoop = 0;
loop:
    bReLoop = 0;
    try {

      while (::GetMessage(&windowsMessage, NULL, 0, 0)) {
        if (!::TranslateAccelerator(windowsMessage.hwnd, hAccelTable, &windowsMessage)) {
          ::TranslateMessage(&windowsMessage);
          ::DispatchMessage(&windowsMessage);
        }

        //Msg_MainLoop_EventLoopIdle msg;
        //msg.Send();
      }
      // Terminate normally and exit

    } catch (...) {

      // Exception: restart loop
      bReLoop = 1;
      ApAsyncMessage<Msg_Log_Line> msgLL;
      msgLL->nMask = apLog_MaskError;
      msgLL->sChannel = MODULE_NAME;
      msgLL->sContext = "MainLoopModule::On_MainLoop_Win32Loop";
      msgLL->sMessage = "Unknown exception";
      msgLL.Post();

    }
    if (bReLoop) { goto loop; }

    ::KillTimer(hWnd_, SEC_TIMER);

    pMsg->wParam = windowsMessage.wParam;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void MainLoopModule::On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg)
{
  int ok = 1;

  if (ok) {
    ::PostMessage(hWnd_, WM_CLOSE, (WPARAM)0, (LPARAM)0);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void MainLoopModule::On_Win32_GetInstance(Msg_Win32_GetInstance* pMsg)
{
  pMsg->hInstance = hAppInstance_;
  pMsg->apStatus = ApMessage::Ok;
}

void MainLoopModule::On_Win32_GetMainWindow(Msg_Win32_GetMainWindow* pMsg)
{
  pMsg->hWnd = hWnd_;
  pMsg->apStatus = ApMessage::Ok;
}

void MainLoopModule::On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg)
{
  nFinishedModules_++;

  if (nFinishedModules_ >= nWaitFinishingModules_) {
    ::KillTimer(hWnd_, QUIT_TIMER);
    nQuitTimer_ = ::SetTimer(hWnd_, QUIT_TIMER, 0, NULL);
  }

  pMsg->apStatus = ApMessage::Ok;
}

int MainLoopModule::InsertTimer(TimerListElem* eTimer)
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

void MainLoopModule::ScheduleNextTimer()
{
  ::KillTimer(hWnd_, TIMERQUEUE_TIMER);

  TimerListElem* eFirst = (TimerListElem*) lTimer_.First();
  if (eFirst != 0) {
    Apollo::TimeValue tvBase = Apollo::TimeValue::getTime();
    Apollo::TimeValue tvNext = eFirst->tv_;
    Apollo::TimeValue tvDiff = tvNext - tvBase;
    int nDelayMS_ = tvDiff.Sec() * 1000 + tvDiff.MicroSec() / 1000;
    if (nDelayMS_ <= 0) {
      nDelayMS_ = 1;
    }
    nTimerQueueTimer_ = ::SetTimer(hWnd_, TIMERQUEUE_TIMER, nDelayMS_, NULL);
  }
}

void MainLoopModule::ProcessNextTimer()
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

void MainLoopModule::On_Timer_ClearAll(Msg_Timer_ClearAll* pMsg)
{
  int ok = 1;

  ::KillTimer(hWnd_, TIMERQUEUE_TIMER);

  TimerListElem* e = 0;
  while ((e = (TimerListElem*) lTimer_.First())) {
    lTimer_.Remove(e);
    delete e;
    e = 0;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void MainLoopModule::On_Timer_Start(Msg_Timer_Start* pMsg)
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

void MainLoopModule::On_Timer_Cancel(Msg_Timer_Cancel* pMsg)
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

#if defined(AP_TEST)

class Test_Timer_Elem: public Elem
{
public:
  ApHandle hExpected_;
};

static String Test_Timer_QueueAddOne(ListT<Test_Timer_Elem, Elem>& l, int nSec, int nUSec, int nPos)
{
  String err;

  ApHandle hAp = Apollo::startTimeout(nSec, nUSec);
  if (!ApIsHandle(hAp)) { 
    err.appendf("Apollo::startTimeout(%d, %d) failed", nSec, nUSec);
  } else {
    Test_Timer_Elem* e = new Test_Timer_Elem();
    e->setInt(nPos);
    e->hExpected_ = hAp;
    l.AddLast(e); 
  }

  return err;
}

String MainLoopModule::Test_Timer_Queue()
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
      err.appendf("Position %d has " ApHandleFormat " expected " ApHandleFormat, nCnt, ApHandleType(hAp), ApHandleType(hApExpected));
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
    if (!Apollo::cancelInterval(hTest_Timer_Basic_On_Timer_Event)) {
      err = "Apollo::cancelTimeout() failed";
    }
    AP_UNITTEST_RESULT(MainLoopModule::Test_Timer_Basic_Complete, err.empty(), err);
    { Msg_Timer_Event msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Timer_Basic_On_Timer_Event, 0); }  
    AP_UNITTEST_EXECUTE(MainLoopModule::Test_Timer_Interval);
  }
}

String MainLoopModule::Test_Timer_Basic()
{
  String err;

  { Msg_Timer_Event msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Timer_Basic_On_Timer_Event, 0, ApCallbackPosNormal); }

  if (err.empty()) {
    hTest_Timer_Basic_On_Timer_Event = Apollo::startInterval(0, 10000);
    if (!ApIsHandle(hTest_Timer_Basic_On_Timer_Event)) {
      err = "Apollo::startTimeout() failed";
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
    apLog_Info((LOG_CHANNEL, "Test_Timer_Interval_On_Timer_Event", "%d %s", nTest_Timer_Interval_On_Timer_Event, StringType(tv.toString())));
    if (nTest_Timer_Interval_On_Timer_Event == 10) {
      String err;
      if (!Apollo::cancelInterval(hTest_Timer_Interval_On_Timer_Event)) {
        err = "Apollo::cancelInterval() failed";
      }
      AP_UNITTEST_RESULT(MainLoopModule::Test_Timer_Interval_Complete, err.empty(), err);
      { Msg_Timer_Event msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Timer_Interval_On_Timer_Event, 0); }  
      MainLoopModule::UnitTest_TokenEnd();
    }
  }
}

String MainLoopModule::Test_Timer_Interval()
{
  String err;

  { Msg_Timer_Event msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Timer_Interval_On_Timer_Event, 0, ApCallbackPosNormal); }

  if (err.empty()) {
    hTest_Timer_Interval_On_Timer_Event = Apollo::startInterval(0, 10000);
    if (!ApIsHandle(hTest_Timer_Interval_On_Timer_Event)) {
      err = "Apollo::startInterval() failed";
    }
  }
  
  return err;
}

//---------------

void MainLoopModule::UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "MainLoopModule::UnitTest_TokenEnd", "Finished Test/Timer"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

void MainLoopModule::UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, (ApCallback) MainLoopModule::UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "MainLoopModule::UnitTest_Token", "Starting Test/Timer"));
  int bTokenEndNow = 0;

  AP_UNITTEST_EXECUTE(MainLoopModule::Test_Timer_Basic);

  if (bTokenEndNow) { UnitTest_TokenEnd(); }
}

//---------------

void MainLoopModule::On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Timer", 0)) {
    AP_UNITTEST_REGISTER(Test_Timer_Queue);
    AP_UNITTEST_REGISTER(MainLoopModule::Test_Timer_Basic);
    AP_UNITTEST_REGISTER(MainLoopModule::Test_Timer_Basic_Complete);
    AP_UNITTEST_REGISTER(MainLoopModule::Test_Timer_Interval);
    AP_UNITTEST_REGISTER(MainLoopModule::Test_Timer_Interval_Complete);
  }
}

void MainLoopModule::On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Timer", 0)) {
    AP_UNITTEST_EXECUTE(Test_Timer_Queue);
    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) MainLoopModule::UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
}

void MainLoopModule::On_UnitTest_End(Msg_UnitTest_End* pMsg)
{
  AP_UNUSED_ARG(pMsg);
}
#endif // #if defined(AP_TEST)

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_Win32Loop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_EndLoop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_ModuleFinished)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetInstance)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetMainWindow)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, System_ThreadMessage)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Timer_ClearAll)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Timer_Start)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Timer_Cancel)
#if defined(AP_TEST)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, UnitTest_Begin)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, UnitTest_Execute)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, UnitTest_End)
#endif

//----------------------------------------------------------


WINMAINLOOP_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  MainLoopModuleInstance::Delete();

  { Msg_MainLoop_Win32Loop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_EndLoop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_ModuleFinished msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_GetInstance msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetInstance), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_GetMainWindow msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetMainWindow), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_System_ThreadMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Timer_ClearAll msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Timer_ClearAll), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Timer_Start msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Timer_Start), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Timer_Cancel msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Timer_Cancel), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, UnitTest_Begin), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, UnitTest_Execute), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, UnitTest_End), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
#endif

//Msg_

  return MainLoopModuleInstance::Get() != 0;
}

WINMAINLOOP_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  { Msg_MainLoop_Win32Loop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get()); }
  { Msg_MainLoop_EndLoop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get()); }
  { Msg_MainLoop_ModuleFinished msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get()); }
  { Msg_Win32_GetInstance msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetInstance), MainLoopModuleInstance::Get()); }
  { Msg_Win32_GetMainWindow msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetMainWindow), MainLoopModuleInstance::Get()); }
  { Msg_System_ThreadMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get()); }
  { Msg_Timer_ClearAll msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Timer_ClearAll), MainLoopModuleInstance::Get()); }
  { Msg_Timer_Start msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Timer_Start), MainLoopModuleInstance::Get()); }
  { Msg_Timer_Cancel msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Timer_Cancel), MainLoopModuleInstance::Get()); }
#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, UnitTest_Begin), MainLoopModuleInstance::Get()); }
  { Msg_UnitTest_Execute msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, UnitTest_Execute), MainLoopModuleInstance::Get()); }
  { Msg_UnitTest_End msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, UnitTest_End), MainLoopModuleInstance::Get()); }
#endif

  MainLoopModuleInstance::Delete();

  return 1;
}
