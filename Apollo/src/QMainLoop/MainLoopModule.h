// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MainLoopModule_H_INCLUDED)
#define MainLoopModule_H_INCLUDED

#include "Local.h"
#include "MainWindow.h"

class MainLoopModule : public QObject
{
  Q_OBJECT

public:
  MainLoopModule()
    :_pApp(0)
    ,_pMainWindow(0)
  {}

  int Init();
  void Exit();

  virtual ~MainLoopModule();

  void On_MainLoop_Win32Loop(Msg_MainLoop_Win32Loop* pMsg);
  void On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg);
  void On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg);
  void On_Win32_GetInstance(Msg_Win32_GetInstance* pMsg);
  void On_Win32_GetMainWindow(Msg_Win32_GetMainWindow* pMsg);
  void On_Win32_WndProcMessage(Msg_Win32_WndProcMessage* pMsg);
  void On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg);

signals:
  void AsyncMessage(void* p);

private slots:
  void Async2SyncMessage(void* p);

protected:
  QApplication* _pApp;
  MainWindow* _pMainWindow;

};

#endif // MainLoopModule_H_INCLUDED
