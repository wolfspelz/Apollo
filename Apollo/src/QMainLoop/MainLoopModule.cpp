// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MainLoopModule.h"

MainLoopModule::~MainLoopModule()
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

  //connect(this, SIGNAL(AsyncMessage(void*)), this, SLOT(Async2SyncMessage(void*)));
  connect(this, SIGNAL(AsyncMessage(void*)), this, SLOT(Async2SyncMessage(void*)), Qt::QueuedConnection);

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

void MainLoopModule::Async2SyncMessage(void* p)
{
  ApMessage* pMsg = (ApMessage*) p;
  Apollo::callMsg(pMsg);
  delete pMsg;
}

void MainLoopModule::On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg)
{
  emit AsyncMessage(pMsg->pMsg);
  pMsg->apStatus = ApMessage::Ok;
  pMsg->Stop();
}

//----------------------------------------------------------

void MainLoopModule::On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg)
{
  int ok = 1;
  if (_pApp != 0) {
    MainWindow mainWindow;
    mainWindow.Setup(_pApp);    
    
    mainWindow.show();    

    { Msg_MainLoop_EventLoopBegin msg; msg.Send(); }

    _pApp->exec();

    { Msg_MainLoop_EventLoopEnd msg; msg.Send(); }
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
//  if (
/* TODO
  nFinishedModules_++;

  if (nFinishedModules_ >= nWaitFinishingModules_) {
    ::KillTimer(hWnd_, QUIT_TIMER);
    nQuitTimer_ = ::SetTimer(hWnd_, QUIT_TIMER, 0, NULL);
  }
*/
}
