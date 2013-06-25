// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NetModule_h_INCLUDED)
#define NetModule_h_INCLUDED

#include "ApTypes.h"

#include "MsgNet.h"
#include "MsgMainLoop.h"
#include "MsgSystem.h"
#include "MsgUnitTest.h"
#include "ApLog.h"
#include "ApContainer.h"

#include "SocketIO.h"
#include "Resolver.h"
#include "HttpManager.h"

class PostResolveConnectTask
{
public:
  PostResolveConnectTask(const ApHandle& hConnection, String& sName, int nPort)
    :hConnection_(hConnection)
    ,sName_(sName)
    ,nPort_(nPort)
  {}

  void setAddress(String& sAddress) { sAddress_ = sAddress; }
  ApHandle getConnection() { return hConnection_; }

  int Execute();

protected:
  ApHandle hConnection_;
  String sName_;
  String sAddress_;
  int nPort_;
};

class PostResolveListenTask
{
public:
  PostResolveListenTask(const ApHandle& hServer, String& sName, int nPort)
    :hServer_(hServer)
    ,sName_(sName)
    ,nPort_(nPort)
  {}

  void setAddress(String& sAddress) { sAddress_ = sAddress; }
  ApHandle getServer() { return hServer_; }

  int Execute();

protected:
  ApHandle hServer_;
  String sName_;
  String sAddress_;
  int nPort_;
};

class NetModule
{
public:
  NetModule()
    :bShutdown_(0)
    ,bIPOnline_(0)
    ,bIPOnlineValid_(0)
  {}

  int Init();
  int Exit();

  void On_Net_IsOnline(Msg_Net_IsOnline* pMsg);
  
  void On_Net_DNS_Resolve(Msg_Net_DNS_Resolve* pMsg);
  void On_Net_DNS_Resolved(Msg_Net_DNS_Resolved* pMsg);

  void On_Net_TCP_Connect(Msg_Net_TCP_Connect* pMsg);
  void On_Net_TCP_DataOut(Msg_Net_TCP_DataOut* pMsg);
  void On_Net_TCP_Close(Msg_Net_TCP_Close* pMsg);

  void On_Net_TCP_Listen(Msg_Net_TCP_Listen* pMsg);
  void On_Net_TCP_ListenStop(Msg_Net_TCP_ListenStop* pMsg);

  void On_Net_HTTP_Request(Msg_Net_HTTP_Request* pMsg);
  void On_Net_HTTP_Cancel(Msg_Net_HTTP_Cancel* pMsg);
  void On_Net_HTTP_CancelAll(Msg_Net_HTTP_CancelAll* pMsg);
  
  void On_MainLoop_EventLoopDelayEnd(Msg_MainLoop_EventLoopDelayEnd* pMsg);
  void On_System_SecTimer(Msg_System_SecTimer* pMsg);
  void On_System_3SecTimer(Msg_System_3SecTimer* pMsg);
  void On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

  int inShutdown() { return bShutdown_; }
  void checkOnlineChange(int bIPOnline);

public:
  static String ip4_LongToStr(unsigned long ip);
  static unsigned long ip4_StrToLong(const String& sAddress);
  static int ipAddressInMask(const String& sMask, const String& sAddress);

public:
  SocketIO oSocketIO_;
  Resolver oResolver_;
  HttpManager oHttp_;

  ApHandleTree<PostResolveConnectTask> postResolveConnectTasks_;
  ApHandleTree<PostResolveListenTask> postResolveListenTasks_;

  int bShutdown_;
  int bIPOnline_;
  int bIPOnlineValid_;
};

typedef ApModuleSingleton<NetModule> NetModuleInstance;

#endif // !defined(NetModule_h_INCLUDED)
