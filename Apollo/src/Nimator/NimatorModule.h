// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NimatorModule_H_INCLUDED)
#define NimatorModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgAnimation.h"
#include "Item.h"

typedef ApHandlePointerTree<Item*> ItemList;
typedef ApHandlePointerTreeNode<Item*> ItemListNode;
typedef ApHandlePointerTreeIterator<Item*> ItemListIterator;

class NimatorModule
{
public:
  NimatorModule();
  virtual ~NimatorModule();

  int Init();
  void Exit();

  void On_Animation_Create(Msg_Animation_Create* pMsg);
  void On_Animation_Destroy(Msg_Animation_Destroy* pMsg);
  void On_Animation_Start(Msg_Animation_Start* pMsg);
  void On_Animation_Stop(Msg_Animation_Stop* pMsg);
  void On_Animation_SetRate(Msg_Animation_SetRate* pMsg);
  void On_Animation_SetData(Msg_Animation_SetData* pMsg);
  void On_Animation_SetStatus(Msg_Animation_SetStatus* pMsg);
  void On_Animation_Event(Msg_Animation_Event* pMsg);

#if defined(AP_TEST)
  static String Test_Parse();
  static String Test_LoadGIF();

  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  ItemList items_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<NimatorModule> NimatorModuleInstance;

#endif // NimatorModule_H_INCLUDED
