// ============================================================================
//
// Apollo
//
// ============================================================================

#include "NetModule.h"
#include "NetOS.h"

int PostResolveConnectTask::Execute()
{
  int ok = 1;

  Msg_Net_TCP_Connect msg;
  msg.hConnection = hConnection_;
  msg.sHost = sAddress_;
  msg.nPort = nPort_;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "PostResolveConnectTask::Execute", "Msg_Net_TCP_Connect failed " ApHandleFormat " %s %d", ApHandleType(hConnection_), StringType(sAddress_), nPort_));
  }

  return ok;
}

int PostResolveListenTask::Execute()
{
  int ok = 1;

  Msg_Net_TCP_Listen msg;
  msg.hServer = hServer_;
  msg.sAddress = sAddress_;
  msg.nPort = nPort_;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "PostResolveListenTask::Execute", "Msg_Net_TCP_Listen failed " ApHandleFormat " %s %d", ApHandleType(hServer_), StringType(sAddress_), nPort_));
  }

  return ok;
}

//----------------------------------------------------------

String NetModule::ip4_LongtoStr(unsigned long ip)
{
	String sAddress;

  sAddress.appendf("%d.%d.%d.%d", 
		(ip & 0x000000ff) >> 0,
		(ip & 0x0000ff00) >> 8,
		(ip & 0x00ff0000) >> 16,
		(ip & 0xff000000) >> 24
  );

	return sAddress;
}

void NetModule::checkOnlineChange(int bIPOnline)
{
  if (bIPOnline_ != bIPOnline) {
    bIPOnline_ = bIPOnline;
    bIPOnlineValid_ = 1;

    if (bIPOnline) {
      Msg_Net_Online msg; 
      msg.Send();
    } else {
      Msg_Net_Offline msg; 
      msg.Send();
    }
  }
}

//----------------------------------------------------------

void NetModule::On_Net_IsOnline(Msg_Net_IsOnline* pMsg)
{
  if (!bIPOnlineValid_) {
    int bIPOnline = NetOS::CheckIPConnection();
    checkOnlineChange(bIPOnline);
    bIPOnline_ = bIPOnline;
    bIPOnlineValid_ = 1;
    if (bIPOnline_) {
      Msg_Net_Online msg; 
      msg.Send();
    } else {
      Msg_Net_Offline msg; 
      msg.Send();
      
    }
  }

  pMsg->bIsOnline = bIPOnline_;
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

void NetModule::On_Net_DNS_Resolve(Msg_Net_DNS_Resolve* pMsg)
{
  int ok = 0;

  apLog_VeryVerbose((LOG_CHANNEL, "On_Net_DNS_Resolve", ApHandleFormat " %s", ApHandleType(pMsg->hResolver), StringType(pMsg->sName)));

  DNSResolveTask* pTask = new DNSResolveTask(pMsg->hResolver, pMsg->sName);
  if (pTask != 0) {
    ok = 1;
    oResolver_.AddTask(pTask);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_Net_DNS_Resolved(Msg_Net_DNS_Resolved* pMsg)
{
  int ok = 0;
  int bHandled = 0;

  apLog_VeryVerbose((LOG_CHANNEL, "On_Net_DNS_Resolved", ApHandleFormat " %s", ApHandleType(pMsg->hResolver), StringType(pMsg->sName)));

  if (0) {
  } else if (ApHandleTreeNode<PostResolveConnectTask>* pNode = postResolveConnectTasks_.Find(pMsg->hResolver)) {
    bHandled = 1;
    PostResolveConnectTask& task = pNode->Value();
    if (pMsg->bSuccess) {

      task.setAddress(pMsg->sAddress);
      ok = task.Execute();
      pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;

    } else {

      Msg_Net_TCP_Connected msg;
      msg.hConnection = task.getConnection();
      msg.bSuccess = 0;
      msg.sComment = pMsg->sComment;
      msg.Send();
      ok = 1;
    }

    postResolveConnectTasks_.Unset(pMsg->hResolver);
  } else if (ApHandleTreeNode<PostResolveListenTask>* pNode = postResolveListenTasks_.Find(pMsg->hResolver)) {
    bHandled = 1;
    PostResolveListenTask& task = pNode->Value();
    if (pMsg->bSuccess) {
      task.setAddress(pMsg->sAddress);
      ok = task.Execute();
      pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
    } else {
      Msg_Net_TCP_Listening msg;
      msg.hServer = task.getServer();
      msg.bSuccess = 0;
      msg.sComment = pMsg->sComment;
      msg.Send();
      ok = 1;
    }

    postResolveListenTasks_.Unset(pMsg->hResolver);
  }

  if (bHandled) {
    pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  }
}


//----------------------------------------------------------

void NetModule::On_Net_TCP_Connect(Msg_Net_TCP_Connect* pMsg)
{
  int ok = 0;

  apLog_Verbose((LOG_CHANNEL, "On_Net_TCP_Connect", ApHandleFormat " %s %d", ApHandleType(pMsg->hConnection), StringType(pMsg->sHost), pMsg->nPort));

  if (SocketAddress::isAddress(pMsg->sHost)) {
    // If it's an address, then connect directly
    SocketAddress saAddress(pMsg->sHost, pMsg->nPort);
    TCPConnectTask* pTask = new TCPConnectTask(pMsg->hConnection, saAddress);
    if (pTask != 0) {
      ok = 1;
      oSocketIO_.AddTask(pTask);
    }

  } else {
    // If it's a name, then resolve and connect later
    ApHandle hResolver = pMsg->hConnection;

    PostResolveConnectTask task(pMsg->hConnection, pMsg->sHost, pMsg->nPort);
    postResolveConnectTasks_.Set(hResolver, task);

    Msg_Net_DNS_Resolve msg;
    msg.hResolver = hResolver;
    msg.sName = pMsg->sHost;
    msg.sType = Msg_Net_DNS_Resolve_Type_Default;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "On_Net_TCP_Connect", "Msg_Net_DNS_Resolve failed " ApHandleFormat " %s %d", ApHandleType(pMsg->hConnection), StringType(pMsg->sHost), pMsg->nPort));
    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_Net_TCP_DataOut(Msg_Net_TCP_DataOut* pMsg)
{
  int ok = 0;

  SendDataTask* pTask = new SendDataTask(pMsg->hConnection, pMsg->sbData.Data(), pMsg->sbData.Length());
  if (pTask != 0) {
    oSocketIO_.AddTask(pTask);
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_Net_TCP_Close(Msg_Net_TCP_Close* pMsg)
{
  int ok = 0;

  CloseSocketTask* pTask = new CloseSocketTask(pMsg->hConnection);
  if (pTask != 0) {
    oSocketIO_.AddTask(pTask);
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

void NetModule::On_Net_TCP_Listen(Msg_Net_TCP_Listen* pMsg)
{
  int ok = 0;

  apLog_Verbose((LOG_CHANNEL, "On_Net_TCP_Listen", ApHandleFormat " %s %d", ApHandleType(pMsg->hServer), StringType(pMsg->sAddress), pMsg->nPort));

  if (SocketAddress::isAddress(pMsg->sAddress)) {
    SocketAddress saAddress(pMsg->sAddress, pMsg->nPort);
    TCPListenTask* pTask = new TCPListenTask(pMsg->hServer, saAddress);
    if (pTask != 0) {
      ok = 1;
      oSocketIO_.AddTask(pTask);
    }

  } else {
    ApHandle hResolver = pMsg->hServer;

    PostResolveListenTask task(pMsg->hServer, pMsg->sAddress, pMsg->nPort);
    postResolveListenTasks_.Set(hResolver, task);

    Msg_Net_DNS_Resolve msg;
    msg.hResolver = hResolver;
    msg.sName = pMsg->sAddress;
    msg.sType = Msg_Net_DNS_Resolve_Type_Default;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "On_Net_TCP_Listen", "Msg_Net_DNS_Resolve failed " ApHandleFormat " %s %d", ApHandleType(pMsg->hServer), StringType(pMsg->sAddress), pMsg->nPort));
    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_Net_TCP_ListenStop(Msg_Net_TCP_ListenStop* pMsg)
{
  int ok = 0;

  CloseSocketTask* pTask = new CloseSocketTask(pMsg->hServer);
  if (pTask != 0) {
    oSocketIO_.AddTask(pTask);
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

void NetModule::On_Net_HTTP_Request(Msg_Net_HTTP_Request* pMsg)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "On_Net_HTTP_Request", ApHandleFormat " %s %s", ApHandleType(pMsg->hClient), StringType(pMsg->sMethod), StringType(pMsg->sUrl)));

  if (pMsg->sUrl.empty()) {
    apLog_Warning((LOG_CHANNEL, "On_Net_HTTP_Request", ApHandleFormat " %s empty URL: %s", ApHandleType(pMsg->hClient), StringType(pMsg->sMethod), StringType(pMsg->sUrl)));
  }

  HTTPRequestTask* pTask = new HTTPRequestTask();
  if (pTask != 0) {
    pTask->hAp_ = pMsg->hClient;
    pTask->sUrl_ = pMsg->sUrl;
    pTask->sMethod_ = pMsg->sMethod;
    for (Apollo::KeyValueElem* e = 0; (e = pMsg->kvHeader.nextElem(e)) != 0; ) { pTask->lHeader_.AddLast(e->getKey(), e->getString()); }
    pTask->sbBody_.SetData(pMsg->sbBody.Data(), pMsg->sbBody.Length());
    pTask->sIndexFile_ = Apollo::getModuleConfig(MODULE_NAME, "HTTP/IndexFile", "");
    pTask->sReason_ = pMsg->sReason;
    oHttp_.AddTask(pTask);
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_Net_HTTP_Cancel(Msg_Net_HTTP_Cancel* pMsg)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "On_Net_HTTP_Cancel", ApHandleFormat, ApHandleType(pMsg->hClient)));

  ok = oHttp_.CancelRequest(pMsg->hClient);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_Net_HTTP_CancelAll(Msg_Net_HTTP_CancelAll* pMsg)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "On_Net_HTTP_CancelAll", ""));

  ok = oHttp_.CancelAllRequests();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void NetModule::On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg)
{
  apLog_Verbose((LOG_CHANNEL, "On_MainLoop_EventLoopBeforeEnd", ""));

  pMsg->nWaitCount++;

  bShutdown_ = 1;

  oResolver_.CloseAll();
  oSocketIO_.CloseAll();
  oHttp_.CloseAll();
}

void NetModule::On_System_SecTimer(Msg_System_SecTimer* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (inShutdown()) {
    if (1 
      && oResolver_.IsFinished()
      && oSocketIO_.IsFinished()
      && oHttp_.IsFinished()
      ) {
      Msg_MainLoop_ModuleFinished msg; 
      msg.Send();
    }
  }
}

void NetModule::On_System_3SecTimer(Msg_System_3SecTimer* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  int bIPOnline = NetOS::CheckIPConnection();
  checkOnlineChange(bIPOnline);
  bIPOnline_ = bIPOnline;
  bIPOnlineValid_ = 1;
}

void NetModule::On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  NetModuleInstance::Get()->Exit();
}

#if defined(AP_TEST)

static String Test_SocketAddress_isAddress_1(const char* szAddress, int bIsAddress, int bIsIP4Address)
{
  String s;

  if (s.empty()) {
    if (SocketAddress::isAddress(szAddress) != bIsAddress) {
      if (bIsAddress) {
        s.appendf("%s is no address, although it should be", StringType(szAddress));
      } else {
        s.appendf("%s is an address, although it should not be", StringType(szAddress));
      }
    }
  }

  if (s.empty()) {
    if (SocketAddress::isIP4Address(szAddress) != bIsIP4Address) {
      if (bIsIP4Address) {
        s.appendf("%s is no IP4 address, although it should be", StringType(szAddress));
      } else {
        s.appendf("%s is an IP4 address, although it should not be", StringType(szAddress));
      }
    }
  }

  return s;
}

static String Test_SocketAddress()
{
  String s;

  if (s.empty()) { s = Test_SocketAddress_isAddress_1("1.1.1.1", 1, 1); }
  if (s.empty()) { s = Test_SocketAddress_isAddress_1("1.1.1.1.1", 1, 0); }
  if (s.empty()) { s = Test_SocketAddress_isAddress_1("1.1.1", 1, 0); }
  if (s.empty()) { s = Test_SocketAddress_isAddress_1("193.99.144.85", 1, 1); }
  if (s.empty()) { s = Test_SocketAddress_isAddress_1("www.heise.de", 0, 0); }
  if (s.empty()) { s = Test_SocketAddress_isAddress_1("193.99.144.com", 0, 0); }

  return s;
}

void NetModule::On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Net", 0)) {
    AP_UNITTEST_REGISTER(Test_SocketAddress);
  }
}

void NetModule::On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Net", 0)) {
    AP_UNITTEST_EXECUTE(Test_SocketAddress);
  }
}

void NetModule::On_UnitTest_End(Msg_UnitTest_End* pMsg)
{
  AP_UNUSED_ARG(pMsg);
}
#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int NetModule::Init()
{
  int ok = 1;

  if (ok) {
    oSocketIO_.Run();
    oResolver_.Run();
  }

  if (ok) {
    ok = oHttp_.Init("Apollo");
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Load", "oHttp.Init failed"));
    } else {
      oHttp_.Run();
    }
  }

  if (ok) {
    ok = NetOS::PlatformAPI_Init();
  }

  return ok;
}

int NetModule::Exit()
{
  int ok = 1;

  oHttp_.Stop();
  oHttp_.Exit();

  oResolver_.Stop();
  oSocketIO_.Stop();

  (void)NetOS::PlatformAPI_Shutdown();

  return ok;
}
