// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "TrayIcon.h"
#include "apLog.h"
#include "MsgTcpServer.h"
#include "MsgXmpp.h"
#include "MsgUnittest.h"
#include "MsgTrayIcon.h"

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

#define MODULE_NAME "TrayIcon"
#define LOG_CHANNEL MODULE_NAME

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME, // szName
  "", // szServices
  "TrayIcon Module", // szLongName
  "1", // szVersion
  "Counterpart for tray icon exe. Feeds info to the external tray icon app.", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

TRAYICON_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

static ApHandle g_hConnection;
static int g_nConnections = 0;
static int g_nLogins = 0;
static Msg_TrayIcon_Hello msgTIH;

//----------------------------------------------------------

static void SendStatus()
{
  Msg_TrayIcon_Status msg;
  msg.nConnections = g_nConnections;
  msg.nLogins = g_nLogins;
  msg.Send();
}

static void Handle_TcpServer_SrpcRequest(Msg_TcpServer_SrpcRequest* pMsg)
{
  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);
  if (sMethod == msgTIH.Type()) {
    if (ApIsHandle(g_hConnection) && g_hConnection != pMsg->hConnection) {
      apLog_Warning((LOG_CHANNEL, "Handle_TcpServer_SrpcRequest", "Received %s for conn=" ApHandleFormat " but already have conn=" ApHandleFormat ": replaced", StringType(sMethod), ApHandleType(pMsg->hConnection), ApHandleType(g_hConnection)));
    }
    g_hConnection = pMsg->hConnection;

    if (0) {
      // Either return a response in pMsg->response ...
      pMsg->response.createResponse(pMsg->srpc);
    } else {
      // ... or leave pMsg->response empty and send your own response
      Msg_TcpServer_SendSrpc msg;
      msg.hConnection = g_hConnection;
      msg.srpc.set(Srpc::Key::Status , 1);
      msg.Request();
    }

    SendStatus();

    pMsg->apStatus = ApMessage::Ok;
  }
}

static void Handle_TcpServer_Disconnected(Msg_TcpServer_Disconnected* pMsg)
{
  if (g_hConnection == pMsg->hConnection) {
    g_hConnection = ApNoHandle;
  }
}

static void Handle_TrayIcon_Status(Msg_TrayIcon_Status* pMsg)
{
  if (ApIsHandle(g_hConnection)) {
    Msg_TcpServer_SendSrpc msg;
    msg.hConnection = g_hConnection;
    msg.srpc.set(Srpc::Key::Method , pMsg->Type());
    msg.srpc.set("nConnections" , pMsg->nConnections);
    msg.srpc.set("nLogins" , pMsg->nLogins);
    msg.Request();
  }
}

static void Handle_Xmpp_Connected(Msg_Xmpp_Connected* pMsg)
{
  g_nConnections++;
  SendStatus();
}

static void Handle_Xmpp_Disconnected(Msg_Xmpp_Disconnected* pMsg)
{
  g_nConnections--;
  SendStatus();
}

static void Handle_Xmpp_LoggedIn(Msg_Xmpp_LoggedIn* pMsg)
{
  g_nLogins++;
  SendStatus();
}

static void Handle_Xmpp_LoggedOut(Msg_Xmpp_LoggedOut* pMsg)
{
  g_nLogins--;
  SendStatus();
}

//----------------------------------------------------------

#if defined(AP_TEST)

int Test_HelloXmppUpDownDisconnect_nCnt_0_Connections = 0;
int Test_HelloXmppUpDownDisconnect_nCnt_1_Connections = 0;

static void Test_HelloXmppUpDownDisconnect_Handle_TcpServer_SendSrpc(Msg_TcpServer_SendSrpc* pMsg)
{
  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);

  if (sMethod == "TrayIcon_Status") {
    String sConnections = pMsg->srpc.getString("nConnections");
    if (sConnections == "0") {
      Test_HelloXmppUpDownDisconnect_nCnt_0_Connections++;
    }
    if (sConnections == "1") {
      Test_HelloXmppUpDownDisconnect_nCnt_1_Connections++;
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

static String Test_HelloXmppUpDownDisconnect()
{
  String s;

  { Msg_TcpServer_SendSrpc msg; msg.Hook(MODULE_NAME, (ApCallback) Test_HelloXmppUpDownDisconnect_Handle_TcpServer_SendSrpc, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  ApHandle hConn = Apollo::newHandle();

  if (!s) {
    Msg_TcpServer_SrpcRequest msg;
    msg.hConnection = hConn;
    msg.srpc.set(Srpc::Key::Method, "TrayIcon_Hello");
    if (!msg.Request()) {
      s.appendf("%s(%s) failed", StringType(msg.Type()), StringType(msg.srpc.getString(Srpc::Key::Method)));
    }
  }

  if (!s) { if (g_hConnection != hConn) { s.appendf("Expected g_hConnection == hConn"); }}
  if (!s) { if (Test_HelloXmppUpDownDisconnect_nCnt_0_Connections != 1) { s.appendf("Expected 1x nConnections=0"); }}
  if (!s) { Msg_Xmpp_Connected msg; msg.hClient = ApHandle(1,1); msg.Send(); }
  if (!s) { if (Test_HelloXmppUpDownDisconnect_nCnt_1_Connections != 1) { s.appendf("Expected 1x nConnections=1"); }}
  if (!s) { Msg_Xmpp_Disconnected msg; msg.hClient = ApHandle(1,1); msg.Send(); }
  if (!s) { if (Test_HelloXmppUpDownDisconnect_nCnt_0_Connections != 2) { s.appendf("Expected 2x nConnections=0"); }}
  if (!s) { Msg_TcpServer_Disconnected msg; msg.hConnection = hConn; msg.Send(); }
  if (!s) { if (ApIsHandle(g_hConnection)) { s.appendf("Expected g_hConnection == ApNoHandle"); }}

  { Msg_TcpServer_SendSrpc msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_HelloXmppUpDownDisconnect_Handle_TcpServer_SendSrpc, 0); }

  return s;
}

static void Handle_UnitTest_Begin(Msg_UnitTest_Begin* pMsg)
{
  if (Apollo::getConfig("Test/TrayIcon", 0)) {
    AP_UNITTEST_REGISTER(Test_HelloXmppUpDownDisconnect);
  }
}

static void Handle_UnitTest_Execute(Msg_UnitTest_Execute* pMsg)
{
  if (Apollo::getConfig("Test/TrayIcon", 0)) {
    AP_UNITTEST_EXECUTE(Test_HelloXmppUpDownDisconnect);
  }
}

static void Handle_UnitTest_End(Msg_UnitTest_End* pMsg)
{
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

TRAYICON_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_TcpServer_SrpcRequest msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_TcpServer_SrpcRequest, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  { Msg_TcpServer_Disconnected msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_TcpServer_Disconnected, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  { Msg_TrayIcon_Status msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_TrayIcon_Status, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  { Msg_Xmpp_Connected msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_Xmpp_Connected, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  { Msg_Xmpp_Disconnected msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_Xmpp_Disconnected, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  { Msg_Xmpp_LoggedIn msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_Xmpp_LoggedIn, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  { Msg_Xmpp_LoggedOut msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_Xmpp_LoggedOut, 0, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }

#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_UnitTest_Begin, 0, ApCallbackPosNormal); }
  { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_UnitTest_Execute, 0, ApCallbackPosNormal); }
  { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, (ApCallback) Handle_UnitTest_End, 0, ApCallbackPosNormal); }
#endif // #if defined(AP_TEST)

  return 1;
}

TRAYICON_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_TcpServer_SrpcRequest msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_TcpServer_SrpcRequest, 0); }
  { Msg_TcpServer_Disconnected msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_TcpServer_Disconnected, 0); }
  { Msg_TrayIcon_Status msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_TrayIcon_Status, 0); }
  { Msg_Xmpp_Connected msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_Xmpp_Connected, 0); }
  { Msg_Xmpp_Disconnected msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_Xmpp_Disconnected, 0); }
  { Msg_Xmpp_LoggedIn msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_Xmpp_LoggedIn, 0); }
  { Msg_Xmpp_LoggedOut msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_Xmpp_LoggedOut, 0); }

#if defined(AP_TEST)
  { Msg_UnitTest_Begin msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_UnitTest_Begin, 0); }
  { Msg_UnitTest_Execute msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_UnitTest_Execute, 0); }
  { Msg_UnitTest_End msg; msg.Unhook(MODULE_NAME, (ApCallback) Handle_UnitTest_End, 0); }
#endif // #if defined(AP_TEST)

  return 1;
}
