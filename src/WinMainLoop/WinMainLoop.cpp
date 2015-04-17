// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "WinMainLoop.h"
#include "ApLog.h"
#include "ApOS.h"
#include "MsgLog.h"
#include "MsgOS.h"
#include "MsgConfig.h"
#include "MsgMainLoop.h"
#include "MsgSystem.h"
#include "MsgSetup.h"
#include "MsgLogWindow.h"

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

#define MODULE_NAME "WinMainLoop"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

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

//----------------------------

class MainLoopModule
{
public:
  MainLoopModule()
    :hAppInstance_(NULL)
    ,hWnd_(NULL)
    ,nWM_APMSG_(WM_APP + 1)
    ,nWaitFinishingModules_(0)
    ,nFinishedModules_(0)
    ,wParam_WM_CLOSE_(0)
    ,lParam_WM_CLOSE_(0)
  {}

  void On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg);
  void On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg);
  void On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg);
  void On_Win32_GetInstance(Msg_Win32_GetInstance* pMsg);
  void On_Win32_GetMainWindow(Msg_Win32_GetMainWindow* pMsg);
  void On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg);
  void On_OSTimer_Event(Msg_OSTimer_Event* pMsg);
  void On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg);

  LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:

  HINSTANCE hAppInstance_;
  HWND hWnd_;
  UINT nWM_APMSG_;
  ApHandle hQuitTimer_;
  int nWaitFinishingModules_;
  int nFinishedModules_;
  WPARAM wParam_WM_CLOSE_;
  LPARAM lParam_WM_CLOSE_;
};

//----------------------------------------------------------

typedef ApModuleSingleton<MainLoopModule> MainLoopModuleInstance;

//----------------------------------------------------------

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
    ApMessage* pMsg = (ApMessage*) wParam;

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
      //{ Msg_ConfigDialog_Open msg; msg.Request(); }
      break;

    case ID_TOOLS_TEST:
      {
        ApSRPCMessage msg("TestDialog_Srpc");
        msg.srpc.set(Srpc::Key::Method, "Open");
        (void) msg.Call();
      }
      break;

    case ID_TOOLS_DISPLAY:
      { Msg_Setup_Open msg; msg.Request(); }
      break;

    case ID_TOOLS_LOG:
      { Msg_LogWindow_Open msg; msg.Request(); }
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

  case WM_TIMER:
    break;

  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    RECT rt;
    GetClientRect(hWnd, &rt);
    //DrawText(hdc, "Apollo", _tcslen("Apollo"), &rt, DT_CENTER);
    EndPaint(hWnd, &ps);
    break;

  case WM_CLOSE:
    {
      wParam_WM_CLOSE_ = wParam;
      lParam_WM_CLOSE_ = lParam;

      int nDelay = Apollo::getModuleConfig(MODULE_NAME, "ShutdownDelay", 2);
      {
        Msg_MainLoop_EventLoopDelayEnd msg;
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

      {
        hQuitTimer_ = Apollo::newHandle();
        Msg_OSTimer_Start msg; 
        msg.hTimer = hQuitTimer_;
        msg.nSec = nDelay;
        if (!msg.Request()) {
          hQuitTimer_ = ApNoHandle;
        }
      }

    }
    break;

  case WM_DESTROY:
    {
      PostQuitMessage(0);
    }
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

static LRESULT CALLBACK MainLoopModule_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult = 0;
  MainLoopModule* pMainLoopModule = 0;

	if (message == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
    SetWindowLong(hWnd, GWL_USERDATA, (LONG) lpcs->lpCreateParams);
    pMainLoopModule = (MainLoopModule*) lpcs->lpCreateParams;
  } else {
    pMainLoopModule = (MainLoopModule*) GetWindowLong(hWnd, GWL_USERDATA);
  }

  if (pMainLoopModule != 0) {
    //nResult = pMainLoopModule->WndProc(hWnd, message, wParam, lParam);
    if (message == WM_TIMER) {
      int x = 0;
    }
    Msg_Win32_WndProcMessage msg;
    msg.hWnd = hWnd;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.Request();
    lResult = msg.lResult;
  } else {
		lResult = DefWindowProc(hWnd, message, wParam, lParam);
  }

  return lResult;
}

void MainLoopModule::On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg)
{
  int ok = 1;

  String sClass = Apollo::getModuleConfig(MODULE_NAME, "WindowClass", "ApolloWinMainLoopClass");
  String sTitle = Apollo::getModuleConfig(MODULE_NAME, "WindowTitle", "ApolloWinMainLoopTitle");

  if (Apollo::getModuleConfig(MODULE_NAME, "Singleton", 1)) {
    HWND hWnd = FindWindow(sClass, sTitle);
    if (hWnd != NULL) {
      apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Terminating because of Singleton requirement, found window class=%s title=%s", _sz(sClass), _sz(sTitle)));
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

    HACCEL hAccelTable = ::LoadAccelerators(g_hDllInstance, (LPCTSTR) IDC_WIN32APP);
    MSG windowsMessage;

    { Msg_MainLoop_EventLoopBegin msg; msg.Send(); }

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
      msgLL->sContext = LOG_CONTEXT;
      msgLL->sMessage = "Unknown exception";
      msgLL.Post();

    }
    if (bReLoop) { goto loop; }

    { Msg_MainLoop_EventLoopEnd msg; msg.Send(); }

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

void MainLoopModule::On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg)
{
  pMsg->lResult = WndProc(pMsg->hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
  pMsg->apStatus = ApMessage::Ok;
}

void MainLoopModule::On_OSTimer_Event(Msg_OSTimer_Event* pMsg)
{
  if (pMsg->hTimer == hQuitTimer_) {
    DefWindowProc(hWnd_, WM_CLOSE, wParam_WM_CLOSE_, lParam_WM_CLOSE_);
  }
}

void MainLoopModule::On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg)
{
  nFinishedModules_++;

  if (nFinishedModules_ >= nWaitFinishingModules_) {
    if (ApIsHandle(hQuitTimer_)) {
      Msg_OSTimer_Cancel msg;
      msg.hTimer = hQuitTimer_;
      msg.Request();
    }
    {
      hQuitTimer_ = Apollo::newHandle();
      Msg_OSTimer_Start msg;
      msg.hTimer = hQuitTimer_;
      msg.nSec = 0;
      msg.Request();
    }
  }
}

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_Win32Loop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_EndLoop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_ModuleFinished)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetInstance)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetMainWindow)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_WndProcMessage)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, OSTimer_Event)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, System_ThreadMessage)

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
  { Msg_Win32_WndProcMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_WndProcMessage), MainLoopModuleInstance::Get(), ApCallbackPosLate); }
  { Msg_OSTimer_Event msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, OSTimer_Event), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_System_ThreadMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }

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
  { Msg_Win32_WndProcMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_WndProcMessage), MainLoopModuleInstance::Get()); }
  { Msg_OSTimer_Event msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, OSTimer_Event), MainLoopModuleInstance::Get()); }
  { Msg_System_ThreadMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get()); }

  MainLoopModuleInstance::Delete();

  return 1;
}
