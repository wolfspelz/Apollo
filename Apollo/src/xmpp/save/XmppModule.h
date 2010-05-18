// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XmppModule_h_INCLUDED)
#define XmppModule_h_INCLUDED

#include "Local.h"
#include "MsgUnitTest.h"
#include "MsgNet.h"
#include "MsgSystem.h"
#include "MsgMainLoop.h"
#include "netapi/NetInterface.h"
#include "Client.h"

typedef ApHandleTree<Client*> ClientList;
typedef ApHandleTreeNode<Client*> ClientNode;
typedef ApHandleTreeIterator<Client*> ClientIterator;

class XmppModule
{
public:
  XmppModule()
    :bNetOnline_(0)
    {}

  int Init();
  int Exit();

  void On_XMPP_GetStatus(Msg_XMPP_GetStatus* pMsg);

  void On_XMPP_Client_Start(Msg_XMPP_Client_Start* pMsg);
  void On_XMPP_Client_Stop(Msg_XMPP_Client_Stop* pMsg);
  void On_XMPP_Client_Connect(Msg_XMPP_Client_Connect* pMsg);
  void On_XMPP_Client_Disconnect(Msg_XMPP_Client_Disconnect* pMsg);

  void On_XMPP_Client_GetAutoConnect(Msg_XMPP_Client_GetAutoConnect* pMsg);
  void On_XMPP_Client_ConnectDetails(Msg_XMPP_Client_ConnectDetails* pMsg);

  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);

  void On_Net_Online(Msg_Net_Online* pMsg);
  void On_Net_Offline(Msg_Net_Offline* pMsg);
  void On_System_SecTimer(Msg_System_SecTimer* pMsg);

#if defined(APOLLOTEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

  Client* FindClient(ApHandle h);

protected:
  ClientList clients_;

protected:
  int bNetOnline_;
};

typedef ApModuleSingleton<XmppModule> XmppModuleInstance;

#endif // !defined(XmppModule_h_INCLUDED)
