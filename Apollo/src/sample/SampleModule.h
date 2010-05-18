// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SampleModule_H_INCLUDED)
#define SampleModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgSample.h"

class SampleModule
{
public:
  SampleModule()
    :nTheAnswer_(42)
    {}

  int init();
  void exit();

  void On_Sample_Get(Msg_Sample_Get* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  int nTheAnswer_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<SampleModule> SampleModuleInstance;

#endif // SampleModule_H_INCLUDED
