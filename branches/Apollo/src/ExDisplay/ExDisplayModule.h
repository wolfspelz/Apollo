// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(IdentityModule_H_INCLUDED)
#define IdentityModule_H_INCLUDED

#include "Apollo.h"
#include "MsgMainLoop.h"
#include "MsgSystem.h"
#include "MsgUnitTest.h"
#include "MsgFluid.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "Local.h"

class ExDisplayModule
{
public:
  ExDisplayModule()
    :hDisplay_(ApNoHandle)
  {}

  int init();
  void exit();

  void On_Fluid_Created(Msg_Fluid_Created* pMsg);
  void On_Fluid_Loaded(Msg_Fluid_Loaded* pMsg);
  void On_Fluid_Destroyed(Msg_Fluid_Destroyed* pMsg);

  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);

  void On_VpView_EnterLocationRequested(Msg_VpView_EnterLocationRequested* pMsg);
  void On_VpView_EnterLocationBegin(Msg_VpView_EnterLocationBegin* pMsg);
  void On_VpView_EnterLocationComplete(Msg_VpView_EnterLocationComplete* pMsg);
  void On_VpView_LeaveLocationRequested(Msg_VpView_LeaveLocationRequested* pMsg);
  void On_VpView_LeaveLocationBegin(Msg_VpView_LeaveLocationBegin* pMsg);
  void On_VpView_LeaveLocationComplete(Msg_VpView_LeaveLocationComplete* pMsg);

  void On_VpView_LocationsChanged(Msg_VpView_LocationsChanged* pMsg);
  void On_VpView_LocationContextsChanged(Msg_VpView_LocationContextsChanged* pMsg);
  void On_VpView_ContextLocationAssigned(Msg_VpView_ContextLocationAssigned* pMsg);
  void On_VpView_ContextLocationUnassigned(Msg_VpView_ContextLocationUnassigned* pMsg);
  void On_VpView_ParticipantsChanged(Msg_VpView_ParticipantsChanged* pMsg);
  void On_VpView_ParticipantAdded(Msg_VpView_ParticipantAdded* pMsg);
  void On_VpView_ParticipantRemoved(Msg_VpView_ParticipantRemoved* pMsg);
  void On_VpView_LocationDetailsChanged(Msg_VpView_LocationDetailsChanged* pMsg);
  void On_VpView_ContextDetailsChanged(Msg_VpView_ContextDetailsChanged* pMsg);
  void On_VpView_ParticipantDetailsChanged(Msg_VpView_ParticipantDetailsChanged* pMsg);
  void On_VpView_LocationPublicChat(Msg_VpView_LocationPublicChat* pMsg);

  void On_VpView_ContextVisibility(Msg_VpView_ContextVisibility* pMsg);
  void On_VpView_ContextPosition(Msg_VpView_ContextPosition* pMsg);
  void On_VpView_ContextSize(Msg_VpView_ContextSize* pMsg);

  void On_ExDisplay_Control(ApSRPCMessage* pMsg);
  void On_ExDisplay_IO(Msg_Fluid_HostCall* pMsg);
#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  ApHandle hDisplay_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ExDisplayModule> ExDisplayModuleInstance;

// ---------------------------------

#endif // IdentityModule_H_INCLUDED
