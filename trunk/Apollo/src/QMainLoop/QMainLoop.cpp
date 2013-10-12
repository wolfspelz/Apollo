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
    ,_pMainWindow(0)
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
    if (_pMainWindow == 0) {
      delete _pMainWindow;
      _pMainWindow = 0;
    }
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
  QApplication* _pApp;
  MainWindow* _pMainWindow;

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

void MainLoopModule::HandleAsyncMessage(void* p)
{
  ApMessage* pMsg = (ApMessage*) p;

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

void MainLoopModule::On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg)
{
  int ok = 0;

  emit AsyncMessage(pMsg);

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

void MainLoopModule::On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg)
{
  int ok = 1;
  if (_pApp != 0) {
    MainWindow mainWindow; _pMainWindow...
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
  if (
/* TODO
  nFinishedModules_++;

  if (nFinishedModules_ >= nWaitFinishingModules_) {
    ::KillTimer(hWnd_, QUIT_TIMER);
    nQuitTimer_ = ::SetTimer(hWnd_, QUIT_TIMER, 0, NULL);
  }
*/
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
