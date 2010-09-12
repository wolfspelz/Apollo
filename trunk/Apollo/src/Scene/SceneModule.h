// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SceneModule_H_INCLUDED)
#define SceneModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgScene.h"

class SceneModule
{
public:
  SceneModule()
    {}

  int Init();
  void Exit();

  void On_Scene_Create(Msg_Scene_Create* pMsg);
  void On_Scene_Destroy(Msg_Scene_Destroy* pMsg);
  void On_Scene_Position(Msg_Scene_Position* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<SceneModule> SceneModuleInstance;

#endif // SceneModule_H_INCLUDED
