// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(DialogModule_H_INCLUDED)
#define DialogModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgDialog.h"
//#include "SrpcGateHelper.h"

class DialogModule
{
public:
  DialogModule()
    :nTheAnswer_(42)
    {}

  int Init();
  void Exit();

  void On_Dialog_XX(Msg_Dialog_XX* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class DialogModuleTester;
#endif

public:
  int nTheAnswer_;

  //Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<DialogModule> DialogModuleInstance;

//----------------------------------------------------------

#if defined(AP_TEST)

class DialogModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Test1();
};

#endif

#endif // DialogModule_H_INCLUDED
