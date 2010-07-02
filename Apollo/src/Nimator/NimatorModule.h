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
#include "MsgTimer.h"
#include "MsgAnimation.h"
#include "Item.h"

class AnimationRequest
{
public:
  AnimationRequest(const String& sUrl)
    :sUrl_(sUrl)
    ,tvCreated_(Apollo::TimeValue::getTime())
  {}

  Apollo::TimeValue CreationTime() { return tvCreated_; }

protected:
  String sUrl_;
  Apollo::TimeValue tvCreated_;
};

typedef StringTree<AnimationRequest> AnimationRequestList;
typedef StringTreeNode<AnimationRequest> AnimationRequestListNode;
typedef StringTreeIterator<AnimationRequest> AnimationRequestListIterator;

// ---------------------------------------------------

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
  void On_Animation_SetCondition(Msg_Animation_SetCondition* pMsg);
  void On_Animation_Event(Msg_Animation_Event* pMsg);
  void On_Animation_SetPosition(Msg_Animation_SetPosition* pMsg);
  void On_Animation_MoveTo(Msg_Animation_MoveTo* pMsg);
  void On_Animation_GetPosition(Msg_Animation_GetPosition* pMsg);
  void On_Timer_Event(Msg_Timer_Event* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class NimatorModuleTester;
#endif

protected:
  Item* GetItemByTimer(ApHandle hTimer);

  int AnimationIsRequested(const String& sUrl);
  void RequestAnimation(const String& sUrl);
  void AnimationRequestComplete(const String& sUrl, Buffer& sbData);

public:
  ItemList items_;
  AnimationRequestList requestedAnimations_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<NimatorModule> NimatorModuleInstance;

#endif // NimatorModule_H_INCLUDED
