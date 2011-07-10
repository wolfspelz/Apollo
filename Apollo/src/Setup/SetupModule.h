// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SetupModule_H_INCLUDED)
#define SetupModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"

class SetupModule
{
public:
  SetupModule()
    :bInSendRunLevelNormal_(0)
    ,bInterseptedRunLevelNormal_(0)
    {}

  int Init();
  void Exit();

  void On_Setup_Open(Msg_Setup_Open* pMsg);
  void On_Setup_Close(Msg_Setup_Close* pMsg);
  void On_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg);
  void On_Dialog_OnClosed(Msg_Dialog_OnClosed* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class SetupModuleTester;
#endif

protected:
  void SendRunLevelNormal();

protected:
  int bInSendRunLevelNormal_;
  int bInterseptedRunLevelNormal_;
  ApHandle hDialog_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<SetupModule> SetupModuleInstance;

//----------------------------------------------------------

#if defined(AP_TEST)

class SetupModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Dev();
};

#endif

#endif // SetupModule_H_INCLUDED
