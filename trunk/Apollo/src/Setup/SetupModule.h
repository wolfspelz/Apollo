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
    :nTheAnswer_(42)
    {}

  int Init();
  void Exit();

  void On_Setup_Get(Msg_Setup_Get* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class SetupModuleTester;
#endif

protected:
  int nTheAnswer_;

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

  static String Test1();
};

#endif

#endif // SetupModule_H_INCLUDED
