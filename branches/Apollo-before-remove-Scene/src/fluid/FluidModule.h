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

  Display* findDisplay(const ApHandle& h);
  Display* findDestroyedDisplay(const ApHandle& h);
  Display* findDisplayById(const String& sId);
  Connection* findConnection(const ApHandle& h);
  int addConnection(const ApHandle& hConnection, Connection* pConnection);
  int removeConnection(const ApHandle& hConnection);

  void setConnectionDisplayMapping(const ApHandle& hConnection, const ApHandle& hDisplay);
  void unsetConnectionDisplayMapping(const ApHandle& hConnection, const ApHandle& hDisplay);
  ApHandle findDisplayHandleByConnectionHandle(const ApHandle& hConnection);
  ApHandle findConnectionHandleByDisplayHandle(const ApHandle& hDisplay);

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
