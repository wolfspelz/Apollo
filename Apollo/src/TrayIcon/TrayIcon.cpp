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

//----------------------------------------------------------

static void SendConnectionStatus()
{
  if (ApIsHandle(g_hConnection)) {
    Msg_TcpServer_SendSrpc msg;
    msg.hConnection = g_hConnection;
    msg.srpc.set("Method" , "TrayIcon_ConnectionStatus");
    msg.srpc.set("nConnections" , g_nConnections);
    msg.Request();
  }
}

static void Handle_TcpServer_SrpcRequest(Msg_TcpServer_SrpcRequest* pMsg)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (sMethod == "TrayIcon_Hello") {
    if (ApIsHandle(g_hConnection) && g_hConnection != pMsg->hConnection) {
      apLog_Warning((LOG_CHANNEL, "Handle_TcpServer_SrpcRequest", "Received %s for conn=" ApHandleFormat " but already have conn=" ApHandleFormat ": replaced", StringType(sMethod), ApHandleType(pMsg->hConnection), ApHandleType(g_hConnection)));
    }
    g_hConnection = pMsg->hConnection;

    pMsg->response.createResponse(pMsg->srpc);

    SendConnectionStatus();

    pMsg->apStatus = ApMessage::Ok;
  }
}

static void Handle_TcpServer_Disconnected(Msg_TcpServer_Disconnected* pMsg)
{
  if (g_hConnection == pMsg->hConnection) {
    g_hConnection = ApNoHandle;
  }
}

static void Handle_Xmpp_Connected(Msg_Xmpp_Connected* pMsg)
{
  g_nConnections++;
  SendConnectionStatus();
}

static void Handle_Xmpp_Disconnected(Msg_Xmpp_Disconnected* pMsg)
{
  g_nConnections--;
  SendConnectionStatus();
}

//----------------------------------------------------------

TRAYICON_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  Apollo::hookMsg("TcpServer_SrpcRequest", "TrayIcon", (ApCallback) Handle_TcpServer_SrpcRequest, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  Apollo::hookMsg("TcpServer_Disconnected", "TrayIcon", (ApCallback) Handle_TcpServer_Disconnected, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  Apollo::hookMsg("Xmpp_Connected", "TrayIcon", (ApCallback) Handle_Xmpp_Connected, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  Apollo::hookMsg("Xmpp_Disconnected", "TrayIcon", (ApCallback) Handle_Xmpp_Disconnected, 0, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));

  return 1;
}

TRAYICON_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  Apollo::unhookMsg("TcpServer_SrpcRequest", "TrayIcon", (ApCallback) Handle_TcpServer_SrpcRequest, 0);
  Apollo::unhookMsg("TcpServer_Disconnected", "TrayIcon", (ApCallback) Handle_TcpServer_Disconnected, 0);
  Apollo::unhookMsg("Xmpp_Connected", "TrayIcon", (ApCallback) Handle_Xmpp_Connected, 0);
  Apollo::unhookMsg("Xmpp_Disconnected", "TrayIcon", (ApCallback) Handle_Xmpp_Disconnected, 0);

  return 1;
}
