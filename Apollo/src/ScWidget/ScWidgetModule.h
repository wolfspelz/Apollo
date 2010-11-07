// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ScWidgetModule_H_INCLUDED)
#define ScWidgetModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgScWidget.h"
#include "ScWidgetModuleTester.h"

class ScWidgetModule
{
public:
  ScWidgetModule()
    {}

  int Init();
  void Exit();

  //void On_ScWidget_Create(Msg_ScWidget_Create* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:

public:

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class ScWidgetModuleTester;
#endif
};

typedef ApModuleSingleton<ScWidgetModule> ScWidgetModuleInstance;

#endif // ScWidgetModule_H_INCLUDED
