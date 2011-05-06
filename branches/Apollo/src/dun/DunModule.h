// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(DunModule_H_INCLUDED)
#define DunModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgCore.h"

class DunModule
{
public:
  DunModule()
    :nTheAnswer_(42)
    {}

  int init();
  void exit();

  void On_Core_ModuleLoaded(Msg_Core_ModuleLoaded* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  int nTheAnswer_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<DunModule> DunModuleInstance;

#endif // DunModule_H_INCLUDED
