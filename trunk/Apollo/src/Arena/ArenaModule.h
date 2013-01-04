// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ArenaModule_H_INCLUDED)
#define ArenaModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "MsgAnimation.h"
#include "MsgTimer.h"
#include "Display.h"
//#include "Location.h"

typedef ApHandlePointerTree<Display*> DisplayList;
typedef ApHandlePointerTreeNode<Display*> DisplayListNode;
typedef ApHandlePointerTreeIterator<Display*> DisplayListIterator;

typedef ApHandleTree<int> ContextHandleList;
typedef ApHandleTreeNode<int> ContextHandleNode;
typedef ApHandleTree<ContextHandleList> LocationContextHandleList;
typedef ApHandleTreeNode<ContextHandleList> LocationContextHandleNode;

class ArenaModule
{
public:
  ArenaModule()
  {}

  int Init();
  void Exit();

  void On_VpView_ContextCreated(Msg_VpView_ContextCreated* pMsg);
  void On_VpView_ContextDestroyed(Msg_VpView_ContextDestroyed* pMsg);
  void On_VpView_ContextVisibility(Msg_VpView_ContextVisibility* pMsg);
  void On_VpView_ContextPosition(Msg_VpView_ContextPosition* pMsg);
  void On_VpView_ContextSize(Msg_VpView_ContextSize* pMsg);
  void On_VpView_LocationsChanged(Msg_VpView_LocationsChanged* pMsg);
  void On_VpView_ContextLocationAssigned(Msg_VpView_ContextLocationAssigned* pMsg);
  void On_VpView_ContextLocationUnassigned(Msg_VpView_ContextLocationUnassigned* pMsg);
  void On_VpView_EnterLocationRequested(Msg_VpView_EnterLocationRequested* pMsg);
  void On_VpView_EnterLocationBegin(Msg_VpView_EnterLocationBegin* pMsg);
  void On_VpView_EnterLocationComplete(Msg_VpView_EnterLocationComplete* pMsg);
  void On_VpView_LocationContextsChanged(Msg_VpView_LocationContextsChanged* pMsg);
  void On_VpView_ParticipantsChanged(Msg_VpView_ParticipantsChanged* pMsg);
  void On_VpView_LocationPublicChat(Msg_VpView_LocationPublicChat* pMsg);
  void On_VpView_LocationPublicAction(Msg_VpView_LocationPublicAction* pMsg);
  void On_VpView_LocationDetailsChanged(Msg_VpView_LocationDetailsChanged* pMsg);
  void On_VpView_ContextDetailsChanged(Msg_VpView_ContextDetailsChanged* pMsg);
  void On_VpView_ParticipantDetailsChanged(Msg_VpView_ParticipantDetailsChanged* pMsg);
  void On_VpView_LeaveLocationRequested(Msg_VpView_LeaveLocationRequested* pMsg);
  void On_VpView_LeaveLocationBegin(Msg_VpView_LeaveLocationBegin* pMsg);
  void On_VpView_LeaveLocationComplete(Msg_VpView_LeaveLocationComplete* pMsg);
  void On_VpView_ParticipantAdded(Msg_VpView_ParticipantAdded* pMsg);
  void On_VpView_ParticipantRemoved(Msg_VpView_ParticipantRemoved* pMsg);
  void On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg);
  void On_WebView_Event_Closing(Msg_WebView_Event_Closing* pMsg);
  void On_Inventory_OnDragItemMove(Msg_Inventory_OnDragItemMove* pMsg);
  void On_Animation_SequenceBegin(Msg_Animation_SequenceBegin* pMsg);
  //void On_Animation_Frame(Msg_Animation_Frame* pMsg);
  //void On_Animation_SequenceEnd(Msg_Animation_SequenceEnd* pMsg);
  void On_WebView_ModuleCall(Msg_WebView_ModuleCall* pMsg);
  void On_Navigator_CallDisplay(ApSRPCMessage* pMsg);
  #if defined(WIN32)
    void On_BrowserInfo_GetContextWin32Window(Msg_BrowserInfo_GetContextWin32Window* pMsg);
  #endif // defined(WIN32)

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

  void SetContextOfLocation(const ApHandle& hLocation, const ApHandle& hContext);
  void DeleteContextOfLocation(const ApHandle& hLocation, const ApHandle& hContext);
  ContextHandleList& GetContextsOfLocation(const ApHandle& hLocation);

  void SetContextOfHandle(const ApHandle& h, const ApHandle& hContext);
  void DeleteContextOfHandle(const ApHandle& h, const ApHandle& hContext);
  ApHandle GetContextOfHandle(const ApHandle& h);
  Display* GetDisplayOfHandle(const ApHandle& h);

  void SetParticipantOfAnimation(const ApHandle& hAnimation, const ApHandle& hParticipant);
  void DeleteParticipantOfAnimation(const ApHandle& hAnimation, const ApHandle& hParticipant);
  ApHandle GetParticipantOfAnimation(const ApHandle& hAnimation);

protected:
  Display* CreateDisplay(const ApHandle& hContext);
  Display* FindDisplay(const ApHandle& hContext);
  void DeleteDisplay(const ApHandle& hContext);

protected:
  DisplayList displays_;
  LocationContextHandleList contextsOfLocation_;
  ApHandleTree<ApHandle> contextOfHandle_;
  ApHandleTree<ApHandle> participantOfAnimation_;

public:
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ArenaModule> ArenaModuleInstance;

#endif // ArenaModule_H_INCLUDED
