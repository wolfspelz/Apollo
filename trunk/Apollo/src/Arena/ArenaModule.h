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
#include "MsgSystem.h"
#include "Display.h"
#include "Location.h"

typedef ApHandlePointerTree<Display*> DisplayList;
typedef ApHandlePointerTreeNode<Display*> DisplayListNode;
typedef ApHandlePointerTreeIterator<Display*> DisplayListIterator;

typedef ApHandlePointerTree<Location*> LocationList;
typedef ApHandlePointerTreeNode<Location*> LocationListNode;
typedef ApHandlePointerTreeIterator<Location*> LocationListIterator;

class LocationAvatar
{
public:
  LocationAvatar(const ApHandle& hLocation, const ApHandle& hPartcipant)
    :hLocation_(hLocation)
    ,hPartcipant_(hPartcipant)
  {}

  ApHandle hLocation_;
  ApHandle hPartcipant_;
};

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
  void On_VpView_LocationDetailsChanged(Msg_VpView_LocationDetailsChanged* pMsg);
  void On_VpView_ContextDetailsChanged(Msg_VpView_ContextDetailsChanged* pMsg);
  void On_VpView_ParticipantDetailsChanged(Msg_VpView_ParticipantDetailsChanged* pMsg);
  void On_VpView_LeaveLocationRequested(Msg_VpView_LeaveLocationRequested* pMsg);
  void On_VpView_LeaveLocationBegin(Msg_VpView_LeaveLocationBegin* pMsg);
  void On_VpView_LeaveLocationComplete(Msg_VpView_LeaveLocationComplete* pMsg);
  void On_VpView_ParticipantAdded(Msg_VpView_ParticipantAdded* pMsg);
  void On_VpView_ParticipantRemoved(Msg_VpView_ParticipantRemoved* pMsg);
  void On_Animation_SequenceBegin(Msg_Animation_SequenceBegin* pMsg);
  void On_Animation_Frame(Msg_Animation_Frame* pMsg);
  void On_Animation_SequenceEnd(Msg_Animation_SequenceEnd* pMsg);
  void On_System_60SecTimer(Msg_System_60SecTimer* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

  void RegisterLocationAvatarOfAnimatedItem(const ApHandle& hItem, const ApHandle& hLocation, const ApHandle& hAvatar);
  void UnregisterLocationAvatarOfAnimatedItem(const ApHandle& hItem);
  int GetLocationAvatarOfAnimatedItem(const ApHandle& hItem, ApHandle& hLocation, ApHandle& hAvatar);

protected:
  Display* CreateDisplay(const ApHandle& hContext);
  Display* FindDisplay(const ApHandle& hContext);
  void DeleteDisplay(const ApHandle& hContext);

  Location* CreateLocation(const ApHandle& hLocation);
  Location* FindLocation(const ApHandle& hLocation);
  void DeleteLocation(const ApHandle& hLocation);
  void DeleteOldLeaveRequestedLocations();

  void SetLocationOfContext(const ApHandle& hContext, const ApHandle& hLocation);
  void DeleteLocationOfContext(const ApHandle& hContext, const ApHandle& hLocation);
  ApHandle GetLocationOfContext(const ApHandle& hContext);

protected:
  DisplayList displays_;
  LocationList locations_;
  ApHandleTree<ApHandle> locationOfContext_;
  ApHandleTree<LocationAvatar> locationAvatarOfAnimatedItem_;

public:

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ArenaModule> ArenaModuleInstance;

#endif // ArenaModule_H_INCLUDED
