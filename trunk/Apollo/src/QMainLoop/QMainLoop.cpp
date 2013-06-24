// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MainWindow.h"

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

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "QMainLoop",
  "QT based MainLoop Module",
  "1",
  "The module implements the main application loop.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

QMAINLOOP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------

class MainLoopModule
{
public:
  MainLoopModule()
    :_pApp(0)
    //:hAppInstance_(NULL)
    //,hWnd_(NULL)
    //,bFirstMessage_(true)
    //,nWM_APMSG_(WM_APP + 1)
    //,nQuitTimer_(-1)
    //,nWaitFinishingModules_(0)
    //,nFinishedModules_(0)
    //,wParam_WM_CLOSE_(0)
    //,lParam_WM_CLOSE_(0)
  {}

  int Init();
  void Exit();

  virtual ~MainLoopModule()
  {
    if (_pApp == 0) {
      delete _pApp;
      _pApp = 0;
    }
  }

  void On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg);
  void On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg);
  void On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg);
  void On_Win32_GetInstance(Msg_Win32_GetInstance* pMsg);
  void On_Win32_GetMainWindow(Msg_Win32_GetMainWindow* pMsg);
  void On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg);
  void On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg);

//  LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:

  //enum TimerID {
  //  NO_TIMER = 0xAFFE,
  //  SEC_TIMER,
  //  TIMERQUEUE_TIMER,
  //  QUIT_TIMER
  //};

  //HINSTANCE hAppInstance_;
  //HWND hWnd_;
  //bool bFirstMessage_;
  //UINT nWM_APMSG_;
  //UINT nQuitTimer_;
  //int nWaitFinishingModules_;
  //int nFinishedModules_;
  //WPARAM wParam_WM_CLOSE_;
  //LPARAM lParam_WM_CLOSE_;

  QApplication* _pApp;
};

//----------------------------------------------------------

typedef ApModuleSingleton<MainLoopModule> MainLoopModuleInstance;

int MainLoopModule::Init()
{
  int ok = 0;

  Exit();

  if (_pApp == 0) {
    int argcc = 0;
    char* argvv = "";
    _pApp = new QApplication(argcc, &argvv);
    if (_pApp != 0) {
      ok = 1;
    }
  }

  return ok;
}

void MainLoopModule::Exit()
{
  if (_pApp != 0) {
    delete _pApp;
    _pApp = 0;
  }
}

//----------------------------------------------------------

void MainLoopModule::On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg)
{
  int ok = 0;
/* TODO
  MainLoopModule* self = (MainLoopModule*) pMsg->Ref();
  if (self != 0) {
    if (self->hWnd_ != NULL) {
      ok = (TRUE == ::PostMessage(self->hWnd_, self->nWM_APMSG_, (LPARAM) pMsg->pMsg, 0));
    }
  }
*/
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  pMsg->Stop();
}

//----------------------------------------------------------

/*
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
    {
      switch (wParam) {

      case QUIT_TIMER:
        ::KillTimer(hWnd, QUIT_TIMER);
        DefWindowProc(hWnd, WM_CLOSE, wParam_WM_CLOSE_, lParam_WM_CLOSE_);
        break;

      }
    }
    break;

  case WM_CLOSE:
    {
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
    }
    break;

  case WM_DESTROY:
    {
      PostQuitMessage(0);
      { Msg_MainLoop_EventLoopEnd msg; msg.Send(); }
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
*/

/* TODO
void MainLoopModule::On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg)
{
  int ok = 1;
  String sClass = Apollo::getModuleConfig(MODULE_NAME, "WindowClass", "ApolloQMainLoopClass");
  String sTitle = Apollo::getModuleConfig(MODULE_NAME, "WindowTitle", "ApolloQMainLoopTitle");

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

    { Msg_MainLoop_EventLoopBeforeBegin msg; msg.Send(); }

    HACCEL hAccelTable = ::LoadAccelerators(g_hDllInstance, (LPCTSTR) IDC_WIN32APP);
    MSG windowsMessage;

    int bReLoop = 0;
loop:
    bReLoop = 0;
    try {

      while (::GetMessage(&windowsMessage, NULL, 0, 0)) {

        if (bFirstMessage_) {
          bFirstMessage_ = false;
          { Msg_MainLoop_EventLoopBegin msg; msg.Send(); }
        }

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

    pMsg->wParam = windowsMessage.wParam;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}
*/

void MainLoopModule::On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg)
{
  int ok = 1;
  if (_pApp != 0) {
    MainWindow mainWindow;
    mainWindow.Setup(_pApp);    
    mainWindow.show();    

    _pApp->exec();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void MainLoopModule::On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg)
{
  int ok = 1;

/* TODO
  if (ok) {
    ::PostMessage(hWnd_, WM_CLOSE, (WPARAM)0, (LPARAM)0);
  }
*/
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void MainLoopModule::On_Win32_GetInstance(Msg_Win32_GetInstance* pMsg)
{
  // TODO
  //pMsg->hInstance = hAppInstance_;
  //pMsg->apStatus = ApMessage::Ok;
}

void MainLoopModule::On_Win32_GetMainWindow(Msg_Win32_GetMainWindow* pMsg)
{
  // TODO
  //pMsg->hWnd = hWnd_;
  //pMsg->apStatus = ApMessage::Ok;
}

void MainLoopModule::On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg)
{
  // TODO
  //pMsg->lResult = WndProc(pMsg->hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
  //pMsg->apStatus = ApMessage::Ok;
}

void MainLoopModule::On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg)
{
/* TODO
  nFinishedModules_++;

  if (nFinishedModules_ >= nWaitFinishingModules_) {
    ::KillTimer(hWnd_, QUIT_TIMER);
    nQuitTimer_ = ::SetTimer(hWnd_, QUIT_TIMER, 0, NULL);
  }
*/
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_Win32Loop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_EndLoop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_ModuleFinished)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetInstance)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetMainWindow)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_WndProcMessage)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, System_ThreadMessage)

//----------------------------------------------------------


QMAINLOOP_API int Load(AP_MODULE_CALL* pModuleData)
{
  int ok = 0;

  AP_UNUSED_ARG(pModuleData);
  MainLoopModuleInstance::Delete();

  { Msg_MainLoop_Win32Loop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_EndLoop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_ModuleFinished msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_GetInstance msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetInstance), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_GetMainWindow msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetMainWindow), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_WndProcMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_WndProcMessage), MainLoopModuleInstance::Get(), ApCallbackPosLate); }
  { Msg_System_ThreadMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }

  MainLoopModule* pModule = MainLoopModuleInstance::Get();
  if (pModule != 0) {
    ok = pModule->Init();
  }

  return ok;
}

QMAINLOOP_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_MainLoop_Win32Loop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get()); }
  { Msg_MainLoop_EndLoop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get()); }
  { Msg_MainLoop_ModuleFinished msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get()); }
  { Msg_Win32_GetInstance msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetInstance), MainLoopModuleInstance::Get()); }
  { Msg_Win32_GetMainWindow msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetMainWindow), MainLoopModuleInstance::Get()); }
  { Msg_Win32_WndProcMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_WndProcMessage), MainLoopModuleInstance::Get()); }
  { Msg_System_ThreadMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get()); }

  MainLoopModule* pModule = MainLoopModuleInstance::Get();
  if (pModule != 0) {
    pModule->Exit();
  }

  MainLoopModuleInstance::Delete();

  return 1;
}
