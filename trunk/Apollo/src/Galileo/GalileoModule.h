// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(GalileoModule_H_INCLUDED)
#define GalileoModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgTimer.h"
#include "MsgServer.h"
#include "MsgAnimation.h"
#include "MsgGalileo.h"
#include "MsgSystem.h"
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

class GalileoModule
{
public:
  GalileoModule();

  virtual ~GalileoModule();

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
  void On_Animation_Activity(Msg_Animation_Activity* pMsg);
  //void On_Animation_Static(Msg_Animation_Static* pMsg);
  void On_Animation_GetGroups(Msg_Animation_GetGroups* pMsg);
  void On_Animation_GetGroupSequences(Msg_Animation_GetGroupSequences* pMsg);
  void On_Animation_GetSequenceInfo(Msg_Animation_GetSequenceInfo* pMsg);
  //void On_Animation_SetPosition(Msg_Animation_SetPosition* pMsg);
  //void On_Animation_MoveTo(Msg_Animation_MoveTo* pMsg);
  //void On_Animation_GetPosition(Msg_Animation_GetPosition* pMsg);

  void On_Galileo_RequestAnimation(Msg_Galileo_RequestAnimation* pMsg);
  void On_Galileo_RequestAnimationComplete(Msg_Galileo_RequestAnimationComplete* pMsg);
  void On_Galileo_SetStorageName(Msg_Galileo_SetStorageName* pMsg);
  void On_Galileo_SaveAnimationDataToStorage(Msg_Galileo_SaveAnimationDataToStorage* pMsg);
  void On_Galileo_IsAnimationDataInStorage(Msg_Galileo_IsAnimationDataInStorage* pMsg);
  void On_Galileo_LoadAnimationDataFromStorage(Msg_Galileo_LoadAnimationDataFromStorage* pMsg);
  void On_Galileo_ClearAllStorage(Msg_Galileo_ClearAllStorage* pMsg);
  void On_Galileo_ExpireAllStorage(Msg_Galileo_ExpireAllStorage* pMsg);

  void On_Timer_Event(Msg_Timer_Event* pMsg);
  void On_System_AfterLoadModules(Msg_System_AfterLoadModules* pMsg);
  void On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_Server_HttpRequest(Msg_Server_HttpRequest* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class GalileoModuleTester;
#endif

  String GetSequenceDataCacheUrl(ApHandle& hItem, const String& sGroup, const String& sName);
  int AnimationIsRequested(const String& sUrl);
  Item* GetItem(const ApHandle& hItem);

protected:
  Item* GetItemByTimer(const ApHandle& hTimer);
  //String PrepareDbKey(const String& sText) { return String::reverse(sText); }
  //String PrepareDbKey(const String& sText) { return sText; }
  String PrepareDbKey(const String& sText) { String sResult = Apollo::getShortHash(sText, 4) + " " + sText; return sResult; }

public:
  bool bInShutdown_;
  String sLocalHttpServerAddress_;
  int nLocalHttpServerPort_;
  int bLocalHttpServerActive_;
  String sDb_;

  ItemList items_;
  AnimationRequestList requestedAnimations_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<GalileoModule> GalileoModuleInstance;

#endif // GalileoModule_H_INCLUDED
