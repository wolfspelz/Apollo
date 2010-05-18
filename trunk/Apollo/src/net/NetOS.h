// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NetOS_h_INCLUDED)
#define NetOS_h_INCLUDED

#if defined(LINUX)
#define AP_HTTP_USE_CURL 1
#elif defined(WIN32)
#define AP_HTTP_USE_CURL 0
#elif defined(MAC)
#define AP_HTTP_USE_CURL 1
#else
#define AP_HTTP_USE_CURL 0
#endif

#if !defined(WIN32) && (AP_HTTP_USE_CURL != 1)
#error HTTP not implemented
#endif

#include "URL.h"

#if defined(WIN32)
  #include "MibAccess.h"
  #include <WinInet.h>
  #include "ws2tcpip.h"
#elif defined(LINUX) || defined(MAC)
  #include <unistd.h>
  #include <sys/socket.h>
  #include <fcntl.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

#if AP_HTTP_USE_CURL
  #include <curl/curl.h>
#endif

#if !defined(WIN32)
  #if !defined(FD_CONNECT)
    #define FD_CONNECT 1
  #endif
  #if !defined(FD_READ)
    #define FD_READ 2
  #endif
  #if !defined(FD_WRITE)
    #define FD_WRITE 4
  #endif
  #if !defined(FD_CLOSE)
    #define FD_CLOSE 8
  #endif
  #if !defined(FD_ACCEPT)
    #define FD_ACCEPT 16
  #endif
  #if !defined(INVALID_SOCKET)
    #define INVALID_SOCKET (-1)
  #endif
  #if !defined(SOCKET_ERROR)
    #define SOCKET_ERROR (-1)
  #endif
  #if !defined(SOMAXCONN)
    #define SOMAXCONN (5)
  #endif
#endif

#if !defined(HCONNECTION) && AP_HTTP_USE_CURL
  typedef CURL* HCONNECTION;
#elif defined(WIN32)
  #define HCONNECTION HINTERNET
#endif

// Forward decls
class HTTPRequestTask;
class HttpManager;

namespace NetOS 
{
  // Platform TCP Api:
#if defined(WIN32)
  static MibII g_oMib;
#endif

  int PlatformAPI_Init();
  int PlatformAPI_Shutdown();

  int GetLocalIPAddressList(List &slList);
  int CheckIPConnection();
  
  void SocketIOThreadProc(UINT nThreadId);
  
  SOCKET TCPSocketCreate(ApHandle hAp);
  int SocketSetNonblock(SOCKET nSock, ApHandle hAp);
  int SocketClose(SOCKET nSock, ApHandle hAp);
    
// Platform HTTP Api:
  String HTTP_ErrorMessage(int nErr);
  int HTTP_Manager_Init(String& sClientName);
  void HTTP_Manager_Shutdown(HttpManager& oHttp);
  void HTTP_CloseConnection(HCONNECTION hConnection);
  void HTTP_InterruptTaskAndCloseConnection(HTTPRequestTask* pTask);
  int HTTP_PerformRequest(int bDoCall, Apollo::URL url, String& sMethod, int bSSL, String& sError, HTTPRequestTask* self);
  
}; // NS:NetOS

#endif // NetOS_h_INCLUDED

