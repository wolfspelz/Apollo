// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(FluidModule_h_INCLUDED)
#define FluidModule_h_INCLUDED

#include "Apollo.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgFluid.h"

class Display;
class Connection;
class SrpcServer;

typedef ApHandlePointerTree<Display*> DisplayList;
typedef ApHandlePointerTreeNode<Display*> DisplayNode;
typedef ApHandlePointerTreeIterator<Display*> DisplayIterator;

typedef ApHandlePointerTree<Connection*> ConnectionList;
typedef ApHandlePointerTreeNode<Connection*> ConnectionNode;
typedef ApHandlePointerTreeIterator<Connection*> ConnectionIterator;

class FluidModule
{
public:
  FluidModule()
    :pServer_(0)
    {}

  int init();
  void exit();

  Display* findDisplay(ApHandle h);
  Display* findDestroyedDisplay(ApHandle h);
  Display* findDisplayById(const String& sId);
  Connection* findConnection(ApHandle h);
  int addConnection(ApHandle hConnection, Connection* pConnection);
  int removeConnection(ApHandle hConnection);

  void setConnectionDisplayMapping(ApHandle hConnection, ApHandle hDisplay);
  void unsetConnectionDisplayMapping(ApHandle hConnection, ApHandle hDisplay);
  ApHandle findDisplayHandleByConnectionHandle(ApHandle hConnection);
  ApHandle findConnectionHandleByDisplayHandle(ApHandle hDisplay);

  void On_Fluid_Create(Msg_Fluid_Create* pMsg);
  void On_Fluid_Destroy(Msg_Fluid_Destroy* pMsg);
  void On_Fluid_Send(Msg_Fluid_Send* pMsg);
  void On_Fluid_Receive(Msg_Fluid_Receive* pMsg);
  void On_Fluid_DisplayCall(Msg_Fluid_DisplayCall* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  SrpcServer* pServer_;
  DisplayList displays_;
  DisplayList destroyedDisplays_;
  ConnectionList connections_;
  ApHandleTree<ApHandle> connection2Display_;
  ApHandleTree<ApHandle> display2Connection_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<FluidModule> FluidModuleInstance;

#endif // !defined(FluidModule_h_INCLUDED)
