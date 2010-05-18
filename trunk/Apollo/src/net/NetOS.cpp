// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApTypes.h"
#include "ApLog.h"
#include "NetOS.h"
#include "Socket.h"
#include "SocketIO.h"
#include "NetModule.h"

#if defined(WIN32)
  #include "MibAccess.h"
#elif defined(LINUX) || defined(MAC)
  #if defined(_ANSI_SOURCE)
    #error ANSI CODE
  #endif
  #include <signal.h>
  #include <sys/ioctl.h>
  #include <net/if.h>
  #include <arpa/inet.h>
  #include <netinet/ip.h>
  #include <sys/types.h>
#else
#endif

#define AP_MAX(a,b) ((a>=b)?a:b) 

int NetOS::PlatformAPI_Init()
{
  int ok = 1;
#if defined(WIN32)
  if (ok) { ok = NetOS::g_oMib.Init(); }
  if (ok) {
    // Initialize Winsock
    WSADATA wsaData;
    int nResult = ::WSAStartup(MAKEWORD(2,2), &wsaData);
    if (nResult != NO_ERROR) {
      ok = 0;
      apLog_Error((MODULE_NAME, "ApCore::Init", "WSAStartup failed %d", WSAGetLastError()));
    }
  }
#elif defined(LINUX) || defined(MAC)
  if (ok) { ok = 1; }
#else
  if (ok) { ok = 0; }
#endif
  return ok;
}

int NetOS::PlatformAPI_Shutdown()
{
  int ok = 1;
#if defined(WIN32)
  if (ok) { ok = 1; }
#elif defined(LINUX) || defined(MAC)
  if (ok) { ok = 1; }
#else
  if (ok) { ok = 0; }
#endif
  return ok;
}

int NetOS::CheckIPConnection()
{
  int bOnline = 0;
#if defined(WIN32)
  DWORD dwFlag = INTERNET_CONNECTION_LAN 
               | INTERNET_CONNECTION_PROXY
               | INTERNET_CONNECTION_MODEM;
  if (::InternetGetConnectedState(&dwFlag, 0)) {
    List lAdresses;
    if (NetOS::GetLocalIPAddressList(lAdresses)) {
      if (lAdresses.length() > 0) {
        bOnline = 1;
      }
    }
  }
#elif defined(LINUX) || defined(MAC)
  List lAdresses;
  if (NetOS::GetLocalIPAddressList(lAdresses)) {
    if (lAdresses.length() > 0) {
      bOnline = 1;
    }
  }
#endif
  return bOnline;
}

int NetOS::GetLocalIPAddressList(List &lList)
{
  int ok = 1;
#if defined(WIN32)
  String sAddress;
	UINT aIPAddresses[50];
	UINT aIPAddressesSize =50;
	if (ok) { ok = NetOS::g_oMib.GetIPAddress(aIPAddresses, aIPAddressesSize); }
  if (ok) {
		for (UINT i = 0; i< aIPAddressesSize; ++i) {
			long ip = htonl(aIPAddresses[i]);
			if ((ip != 0x7F000001) && (ip != 0)) {
        String sAddress = NetModule::ip4_LongtoStr(aIPAddresses[i]);
        apLog_VeryVerbose((LOG_CHANNEL, "GetLocalIPAddressList", "IP address found: %s", StringType(sAddress)));

        String sKey = "IgnoreLocalIp/" + sAddress;
        if (Apollo::getModuleConfig(MODULE_NAME, sKey, 0)) {
          apLog_VeryVerbose((LOG_CHANNEL, "GetLocalIPAddressList", "IP address ignored by config: %s", StringType(sAddress)));
        } else {
          lList.Add(sAddress);
        }
			}
		}
	}	
#elif defined(LINUX) || defined(MAC)
  
  SOCKET nSockFd = -1;
  struct ifconf ifConf;
  struct ifreq *pIfReq = NULL;
  char pcBuf[BUFSIZ];  
  
  nSockFd = ::socket(AF_INET, SOCK_DGRAM, 0); // 0 = default proto
  if (nSockFd < 0) {
    ok = 0;
    apLog_Error(("Net", "NetOS::GetLocalIPAddressList", "Failed to create socket"));
  }
  
  if (ok) {
    ifConf.ifc_len = BUFSIZ;
    ifConf.ifc_buf = pcBuf;
    
    if (::ioctl(nSockFd, SIOCGIFCONF, &ifConf) < 0) {
      ::close(nSockFd);
      ok = 0;
      apLog_Error(("Net", "NetOS::GetLocalIPAddressList", "ioctl SIOCGIFCONF failed: errno=%d", errno));
    }
  }
  
  if (ok) {
    char* pIfConfBufIter = ifConf.ifc_buf;
    while (pIfConfBufIter < ifConf.ifc_buf + ifConf.ifc_len) { // iterate through all iface configs
      pIfReq = (struct ifreq*)pIfConfBufIter;
    #if defined(MAC)
      int nLen = AP_MAX(sizeof(struct sockaddr), pIfReq->ifr_addr.sa_len);
    #else
      int nLen = sizeof(struct sockaddr);
    #endif
      pIfConfBufIter += sizeof(pIfReq->ifr_name) + nLen; // increment loop pointer
      
      if (0) {;}
      else if (pIfReq->ifr_addr.sa_family == AF_INET) {}
      else if (pIfReq->ifr_addr.sa_family == AF_INET6) {}
      else { continue; } // Continue if address family is not interesting
      
      String sIfaceName = pIfReq->ifr_name;
      struct ifreq ifReqCopy = *pIfReq;
      if (::ioctl(nSockFd, SIOCGIFFLAGS, &ifReqCopy) < 0) { 
        apLog_Warning((LOG_CHANNEL, "NetOS::GetLocalIPAddressList", "ioctl SIOCGIFFLAGS failed for iface=%s: errno=%d", StringTypesIfaceName, errno));
        continue; 
      }
      // Only up and running interfaces, no loopback
      if ((ifReqCopy.ifr_flags & IFF_UP) == 0) { continue; }
      if ((ifReqCopy.ifr_flags & IFF_RUNNING) == 0) { continue; }
      if ((ifReqCopy.ifr_flags & IFF_LOOPBACK) != 0) { continue; } // No loopback
      
      struct sockaddr_in* pSIN = (struct sockaddr_in*)&(pIfReq->ifr_addr);
      String sAddress = ::inet_ntoa(pSIN->sin_addr);
      if (sAddress == "0.0.0.0") { continue; } // kick the zero-address too
      if (sAddress.startsWith("169.254.")) { continue; } // kick the AIPA address range
      
#if defined(_DEBUG)
/*
      String sFlags;
      #define NETFLAGS_CHECK(FLAGS, FLAG, STRING) \
        if ( FLAGS & FLAG ) { \
          if (! STRING .empty()) STRING += " | "; \
          STRING += #FLAG; \
        }
        
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_ALLMULTI , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_ALTPHYS , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_BROADCAST , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_DEBUG , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_LINK0 , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_LINK1 , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_LINK2 , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_MULTICAST , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_NOARP , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_NOTRAILERS , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_OACTIVE , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_POINTOPOINT , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_PROMISC , sFlags)
      NETFLAGS_CHECK(ifReqCopy.ifr_flags , IFF_SIMPLEX , sFlags)
      
      apLog_Debug((LOG_CHANNEL, "NetOS::GetLocalIPAddressList", "Found interface=[%s] ip=[%s] flags=[%s]", StringType(sIfaceName), StringType(sAddress), StringType(sFlags)));
*/
#endif // _DEBUG      
      
      lList.Add(sAddress);
    }
    
    ::close(nSockFd);
  }

#else
  ok = 0;
#endif
  return ok;
}

#if defined(LINUX) || defined(MAC)
void NetOS_SocketTaskQueue_SigHandler(int nSigNum)
{
  SocketIO& ioThread = NetModuleInstance::Get()->oSocketIO_;
  if ((bool)ioThread.AssertThread() && (nSigNum == SIGUSR1)) { 
    ioThread.OnSocketTaskQueueEvent(); 
  } else {
  #if defined(_DEBUG)
    AP_DEBUG_BREAK();
  #endif
  }
  // rearm the sighandler
  ::signal(SIGUSR1, NetOS_SocketTaskQueue_SigHandler);
}
#endif

void NetOS::SocketIOThreadProc(UINT nThreadId)
{
  SocketIO& ioThread = NetModuleInstance::Get()->oSocketIO_;
  if (!ioThread.AssertThread()) { AP_DEBUG_BREAK(); return; }
  if (ioThread.ThreadId() != nThreadId) { AP_DEBUG_BREAK(); return; }
  
#if defined(WIN32)
  WSAEVENT hSocketEvent = WSACreateEvent();
  while (!ioThread.SvcEndCondition()) {
    for (Socket* s = 0; (s = ioThread.lSockets_.Next(s)) != 0; ) {
      WSAEventSelect(s->Sock(), hSocketEvent, s->GetFlags());
    }
    WSAEVENT aEvents[2];
    aEvents[0]= hSocketEvent;
    aEvents[1]= ioThread.oTasks_.GetWSAEvent();
    DWORD dw = WSAWaitForMultipleEvents(2, aEvents, FALSE, 10000, FALSE);
    if (0) {
    } else if (dw == WSA_WAIT_FAILED) {
      int AP_UNUSED_VARIABLE nErr = WSAGetLastError();
    } else if (dw == WSA_WAIT_TIMEOUT) {
    } else {
      int nIndex = dw - WSA_WAIT_EVENT_0;
      if (nIndex == 0) { // hSocketEvent
        for (Socket* s = 0; (s = ioThread.lSockets_.Next(s)) != 0; ) {
          WSANETWORKEVENTS wsaEvents;
          int nErr = WSAEnumNetworkEvents(s->Sock(), hSocketEvent, &wsaEvents);
          if (nErr == 0) {
            if (wsaEvents.lNetworkEvents & FD_ACCEPT) {
              if (s->Listening()) {
                int nErr = wsaEvents.iErrorCode[FD_ACCEPT_BIT];
                if (nErr != 0) {
                  s->Error(nErr);
                } else {
                  s->OnAccept();
                }
              }
            }
            if (wsaEvents.lNetworkEvents & FD_CONNECT) {
              int nErr = wsaEvents.iErrorCode[FD_CONNECT_BIT];
              if (nErr != 0) {
                s->Error(nErr);
              }
              s->OnConnect( (nErr == 0) );
            }
            if (wsaEvents.lNetworkEvents & FD_READ) {
              s->OnRead();
            }
            if (wsaEvents.lNetworkEvents & FD_WRITE) {
              s->OnWrite();
            }
            if (wsaEvents.lNetworkEvents & FD_CLOSE) {
              s->OnClose();
            }
          }
        }

      } else if (nIndex == 1) { // hQueueEvent_
        ioThread.OnSocketTaskQueueEvent();
      } else {
        // error
      }
      ::WSAResetEvent(aEvents[nIndex]);
    }
  }
  WSACloseEvent(hSocketEvent);
#elif defined(LINUX) || defined(MAC)
  sigset_t sigMask, origSigMask; // signal masks for current process (new and previous)
  sigemptyset(&sigMask); // clear
  sigaddset(&sigMask, SIGUSR1); // setup mask to block SIGUSR1 signal outside pselect
  ::sigprocmask(SIG_BLOCK, &sigMask, &origSigMask); // apply to current process' mask
                                                     // and store original mask in origSigMask
  ::signal(SIGUSR1, NetOS_SocketTaskQueue_SigHandler); // register the taskqueue event handler
  
  while (!ioThread.SvcEndCondition()) {
    int nMaxFd = 0;
    struct timespec tsTimeout; 
    { tsTimeout.tv_sec = 10000; tsTimeout.tv_nsec = 0; }
    fd_set fdsRD, fdsWR, fdsERR;
    FD_ZERO(&fdsRD); FD_ZERO(&fdsWR); FD_ZERO(&fdsERR);
    for (Socket* s = 0; (s = ioThread.lSockets_.Next(s)) != 0; ) {
      if (s->Connecting()) {
        // on unix, reliable way to get connect event is to select on the socket being put
        // into write array
        FD_SET(s->Sock(), &fdsWR);
      } else {
        int bCheckError = 0;
        if (s->Readable() || s->Listening()) { FD_SET(s->Sock(), &fdsRD); }
        if (s->Writeable()) { FD_SET(s->Sock(), &fdsWR); }
        if (bCheckError) { FD_SET(s->Sock(), &fdsERR); }
      }
      
      if (s->Listening || s->Readable() || s->Writeable() || s->Connecting()) { nMaxFd = AP_MAX(s->Sock(), nMaxFd); }
    }
    {
      // ru: pselect atomically sets signal mask to origSigMask, executes select, and restores current mask
      // this way we only receive TaskQueue signal (SIGUSR1) inside pselect (otherwise it would be lost) :)
      // as doing this manually is not atomic, we could still get a race condition -> no task processing -> bad
      int nResult = ::pselect((nMaxFd+1), &fdsRD, &fdsWR, &fdsERR, &tsTimeout, &origSigMask);
      if (0) {
      } else if (nResult == (-1)) {
        if (errno != EINTR) {
          // error
          apLog_Error(("Net", "NetOS::SocketIOThreadProc", "pselect() error: %d", errno));
#if defined(_DEBUG)
          AP_DEBUG_BREAK();
#endif
        } // else, signal was caught, probably SIGUSR1, which was processed by our handler
        continue;
      } else if (nResult == 0) {
        // timeout
        continue;
      }
    }
    for (Socket* s = 0; (s = ioThread.lSockets_.Next(s)) != 0;) {
      if (0) {
      } else if ((s->Sock() != INVALID_SOCKET) && (s->GetFlags() != 0) && FD_ISSET(s->Sock(), &fdsERR)) {
        // error on socket, what now...
      #if defined(_DEBUG)
        AP_DEBUG_BREAK();
      #endif
      } else {
        if ((s->Sock() != INVALID_SOCKET) && (s->GetFlags() != 0) && FD_ISSET(s->Sock(), &fdsRD)) { 
          if (s->Listening()) {
            s->OnAccept();
          } else {
            s->OnRead(); 
          }
        }
        if ((s->Sock() != INVALID_SOCKET) && (s->GetFlags() != 0) && FD_ISSET(s->Sock(), &fdsWR)) { 
          if (s->Connecting()) {
            struct sockaddr addr;
            socklen_t len = 1;
            // If this succeeds (result=0), the event was connection being established 
            int nResult = ::getpeername(s->Sock(), &addr, &len);
            if (0) {
            } else if (nResult == 0) {
              s->OnConnect(1);
            } else {
              // else we have to check, if the error is "Not Connected", everythng else is
              // bad breakage :/
              if (errno == ENOTCONN) {
                char c;
                if (::read(s->Sock(), &c, 1) == SOCKET_ERROR) {
                  s->Error(errno);
                }
              }
              s->OnConnect(0);
            }
          } else {
            // Not connecting, so this was a real write event
            s->OnWrite(); 
          }
        }
      }
    }
  }
#else
  #error Not implemented for this OS
#endif
}

SOCKET NetOS::TCPSocketCreate(ApHandle hAp)
{
  SOCKET nSock = INVALID_SOCKET;
#if defined(WIN32)
  nSock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (nSock == INVALID_SOCKET) {
    apLog_Error(("Net", "NetOS::TCPSocketCreate", "::socket failed " ApHandleFormat " %d", ApHandleType(hAp), WSAGetLastError()));
  }
#elif defined(LINUX) || defined(MAC)
  nSock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (nSock == INVALID_SOCKET) {
    apLog_Error(("Net", "NetOS::TCPSocketCreate", "::socket failed " ApHandleFormat " %d", ApHandleType(hAp), errno));
  }
#endif
  return nSock;
}

int NetOS::SocketSetNonblock(SOCKET nSock, ApHandle hAp)
{
  int nResult = SOCKET_ERROR;
#if defined(WIN32)
  ULONG nNonBlock = 1;
  nResult = ::ioctlsocket(nSock, FIONBIO, &nNonBlock);
  if (nResult == SOCKET_ERROR) {
    apLog_Error(("Net", "NetOS::SocketSetNonblock", "::ioctlsocket() failed " ApHandleFormat " %d", ApHandleType(hAp), WSAGetLastError()));
  }
#elif defined(LINUX) || defined(MAC)
  nResult = ::fcntl(nSock, F_SETFL, O_NONBLOCK);
  if (nResult == SOCKET_ERROR) {
    apLog_Error(("Net", "NetOS::SocketSetNonblock", "::fcntl() on socket failed " ApHandleFormat " %d", ApHandleType(hAp), errno));
  }
#endif
  return ((nResult == SOCKET_ERROR) ? 0 : 1);
}

int NetOS::SocketClose(SOCKET nSock, ApHandle hAp)
{
  int nResult = SOCKET_ERROR;
#if defined(WIN32)
  nResult = ::closesocket(nSock);
  if (nResult == SOCKET_ERROR) {
    apLog_Error(("Net", "NetOS::SocketClose", "::closesocket failed " ApHandleFormat " %d", ApHandleType(hAp), WSAGetLastError()));
  }
#elif defined(LINUX) || defined(MAC)
  nResult = ::close(nSock); 
  if (nResult == SOCKET_ERROR) {
    apLog_Error(("Net", "NetOS::SocketClose", "::close on socket failed " ApHandleFormat " %d", ApHandleType(hAp), errno));
  }
#endif
  return nResult;
}

String NetOS::HTTP_ErrorMessage(int nErr)
{
  String sMessage;

#if AP_HTTP_USE_CURL
  { sMessage = ::curl_easy_strerror((CURLcode)nErr); }
#elif defined(WIN32)
  TCHAR* lpMsgBuf = 0;
	int nResult = ::FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_HMODULE,
    ::GetModuleHandle(_T("wininet.dll")), nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(TCHAR*) &lpMsgBuf, 0, 0 
  );		
  if (nResult) { sMessage = lpMsgBuf; } 
  else { sMessage = ::strerror(nErr); }
  if (lpMsgBuf != 0) { ::LocalFree(lpMsgBuf);  }
#else
  #error Not imlemented for this OS
  { sMessage = ::strerror(nErr); }
#endif

  String sResult;
  sResult.appendf("%d (%s)", nErr, StringType(sMessage));
  return sResult;
}

int NetOS::HTTP_Manager_Init(String& sClientName)
{
  AP_UNUSED_ARG(sClientName);
  
  int ok = 1;

#if AP_HTTP_USE_CURL
  CURLcode nCode = ::curl_global_init(CURL_GLOBAL_ALL);
  if (nCode != 0) {
    AP_DEBUG_BREAK();
    String sCurlErrorMessage = NetOS::HTTP_ErrorMessage((int)nCode);
    apLog_Error((LOG_CHANNEL, "NetOS::HTTP_Manager_Init", "InternetOpen() failed with %s", StringType(sCurlErrorMessage)));
    ok = 0;
  }
#elif defined(WIN32)
  HttpManager& oHttp = NetModuleInstance::Get()->oHttp_;
  oHttp.hWinInet_ = ::InternetOpen(sClientName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);//INTERNET_FLAG_ASYNC);
  if (oHttp.hWinInet_ == NULL) {
    String sInternetErrorMessage = NetOS::HTTP_ErrorMessage((int)::GetLastError());
    apLog_Error((LOG_CHANNEL, "NetOS::HTTP_Manager_Init", "InternetOpen() failed with %s", StringType(sInternetErrorMessage)));
    ok = 0;
  }
#else
  #error Not implemented for this OS
  ok = 0;
#endif
  return ok;
}

void NetOS::HTTP_Manager_Shutdown(HttpManager& oHttp)
{
#if AP_HTTP_USE_CURL
  AP_UNUSED_ARG(oHttp);
  ::curl_global_cleanup();
#elif defined(WIN32)
  (void)::InternetCloseHandle(oHttp.hWinInet_);
#else
  #error Not implemented for this OS
#endif
}

void NetOS::HTTP_CloseConnection(HCONNECTION hConnection)
{
  if (hConnection != NULL) {
#if AP_HTTP_USE_CURL
  ::curl_easy_cleanup(hConnection);
#elif defined(WIN32)
  ::InternetCloseHandle(hConnection);
#else
  #error Not implemented for this OS
#endif
  } else {
    //AP_DEBUG_BREAK();
  }
}

//#if defined(LINUX)

void NetOS::HTTP_InterruptTaskAndCloseConnection(HTTPRequestTask* pTask)
{
#if AP_HTTP_USE_CURL
  pTask->Cancel();
#elif defined(WIN32)
  pTask->CloseHandle();
#else
  #error Not implemented for this OS
#endif
}

#if AP_HTTP_USE_CURL
int NetOS_HTTP_CurlProgressCallback(void *context, double dlTotal, double dlNow, 
                                          double ulTotal, double ulNow)
{
  AP_UNUSED_ARG(dlTotal);
  AP_UNUSED_ARG(dlNow);
  AP_UNUSED_ARG(ulTotal);
  AP_UNUSED_ARG(ulNow);
  
  HTTPRequestTask* self = (HTTPRequestTask*)context;
  return self->Cancelled();
}

static size_t NetOS_HTTP_WriteDataFunc(void *buffer, size_t size, size_t nmemb, void *userp)
{
  HTTPRequestTask* self = (HTTPRequestTask*)userp;
  if (self->nState_ == HTTPRequestTask::RequestState_Header) {
    ::curl_easy_getinfo(self->GetHandle(), CURLINFO_RESPONSE_CODE, &(self->nRequestStatusCode_)); 
    Msg_Net_HTTP_Header* pMsg = new Msg_Net_HTTP_Header();
    if (pMsg != 0) {
      pMsg->hClient = self->hAp_;
      pMsg->nStatus = self->nRequestStatusCode_;
      pMsg->lHeader = self->lResponseHeaders_;
      pMsg->PostAsync();
    }
  }
  
  self->nState_ = HTTPRequestTask::RequestState_DataIn;
  
  int nSize = size * nmemb;
  unsigned char pBuff[nSize+1]; ::memcpy(pBuff, buffer, nSize); pBuff[nSize] = '\0';
  Msg_Net_HTTP_DataIn* pMsg = new Msg_Net_HTTP_DataIn();
  if (pMsg != 0) {
    pMsg->hClient = self->hAp_;
    pMsg->sbData.SetData(pBuff, nSize);
    pMsg->PostAsync();
  }
  return nSize;
}

static size_t NetOS_HTTP_WriteHeaderFunc(void *buffer, size_t size, size_t nmemb, void *userp)
{
  HTTPRequestTask* self = (HTTPRequestTask*)userp;
  if (self->nState_ == HTTPRequestTask::RequestState_Connecting) {
    self->nState_ = HTTPRequestTask::RequestState_Connected;
    Msg_Net_HTTP_Connected* pMsg = new Msg_Net_HTTP_Connected();
    if (pMsg != 0) {
      pMsg->hClient = self->hAp_;
      pMsg->PostAsync();
    }
  }
  self->nState_ = HTTPRequestTask::RequestState_Header;
  int nSize = size * nmemb;
  unsigned char pBuf[nSize+1]; ::memcpy(pBuf, buffer, nSize); pBuf[nSize] = '\0';
  Buffer sbHeader; sbHeader.SetData(pBuf, nSize+1);
  KeyValueLfBlob2List((const char*)pBuf, self->lResponseHeaders_);
  return nSize;
}
#endif

int NetOS::HTTP_PerformRequest(int bDoCall, Apollo::URL url, String& sMethod, int bSSL, String& sError, HTTPRequestTask* self)
{
  int ok = bDoCall;
  HCONNECTION hConnect = NULL;
#if AP_HTTP_USE_CURL
  AP_UNUSED_ARG(bSSL);
  if (ok) {
    self->nState_ = HTTPRequestTask::RequestState_Connecting;
    hConnect = ::curl_easy_init();
    if (hConnect == NULL) {
      sError.appendf("curl_easy_init() failed");
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", StringType(sError)));
      ok = 0;
    } else {
      self->SetHandle(hConnect);
    }
  }
  
  struct curl_slist *pHeaders = NULL;
  
  if (ok) {
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_NOPROGRESS, 0);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_PROGRESSFUNCTION, NetOS_HTTP_CurlProgressCallback);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_PROGRESSDATA, self);
    //::curl_easy_setopt(self->GetHandle(), CURLOPT_NOSIGNAL, 1);
    //::curl_easy_setopt(self->GetHandle(), CURLOPT_DNS_USE_GLOBAL_CACHE, 0);
    //::curl_easy_setopt(self->GetHandle(), CURLOPT_USE_SSL, CURLUSESSL_TRY);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_SSL_VERIFYPEER, 0);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_SSL_VERIFYHOST, 0);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_URL, self->sUrl_.c_str());
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_VERBOSE, 0);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_WRITEFUNCTION, NetOS_HTTP_WriteDataFunc);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_WRITEDATA, self);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_WRITEHEADER, self);
    ::curl_easy_setopt(self->GetHandle(), CURLOPT_HEADERFUNCTION, NetOS_HTTP_WriteHeaderFunc);
    if (sMethod == "POST") {
      ::curl_easy_setopt(self->GetHandle(), CURLOPT_POST, 1);
      ::curl_easy_setopt(self->GetHandle(), CURLOPT_POSTFIELDS, self->sbBody_.Data());
      ::curl_easy_setopt(self->GetHandle(), CURLOPT_POSTFIELDSIZE, self->sbBody_.Length()); 
    }
    for (Elem* e = NULL; (e = self->lHeader_.Next(e));) {
      String sHeader; sHeader.appendf("%s: %s", StringType(e->getName()), StringType(e->getString()));
      pHeaders = ::curl_slist_append(pHeaders, (const char*) sHeader);
    }
    if (pHeaders != 0) {
      ::curl_easy_setopt(self->GetHandle(), CURLOPT_HTTPHEADER, pHeaders);
    }
    
    apLog_VeryVerbose((LOG_CHANNEL, "NetOS::HTTP_PerformRequest", "Before curl_easy_perform()"));
    
    CURLcode nCode = ::curl_easy_perform(self->GetHandle());
    
    apLog_VeryVerbose((LOG_CHANNEL, "NetOS::HTTP_PerformRequest", "After curl_easy_perform()"));
    
    if (0) {
    } else if (nCode == CURLE_ABORTED_BY_CALLBACK) { 
      //::curl_easy_getinfo(self->GetHandle(), CURLINFO_RESPONSE_CODE, &(self->nRequestStatusCode_)); 
      apLog_Verbose((LOG_CHANNEL, "NetOS::HTTP_PerformRequest", "" ApHandleFormat " curl_perform aborted by progress callback", ApHandleType(self->hAp_)));
    } else if (nCode != CURLE_OK) {
      String sCurlErrorMessage = NetOS::HTTP_ErrorMessage(nCode);
      sError.appendf("curl_easy_perform() failed with %s: %s:%d%s", StringType(sCurlErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok= 0;
    }
  }
  
  self->nState_ = HTTPRequestTask::RequestState_Closed;

  if (!ok) {
    Msg_Net_HTTP_Failed* pMsg = new Msg_Net_HTTP_Failed();
    if (pMsg != 0) {
      pMsg->hClient = self->hAp_;
      pMsg->sMessage = sError;
      pMsg->PostAsync();
    }
  }

  {
    Msg_Net_HTTP_Closed* pMsg = new Msg_Net_HTTP_Closed();
    if (pMsg != 0) {
      pMsg->hClient = self->hAp_;
      pMsg->PostAsync();
    }
  }
  
  if (pHeaders != 0) { ::curl_slist_free_all(pHeaders); }
  self->CloseHandle();
#elif defined(WIN32)
  if (ok) {
    DWORD dwContext = (DWORD) self;
    self->nState_ = HTTPRequestTask::RequestState_Connecting;
    // Connect
    hConnect = ::InternetConnect(
      NetModuleInstance::Get()->oHttp_.InetHandle(),
      url.host(),
      url.portnum(),
      _T(""),
      _T(""),
      INTERNET_SERVICE_HTTP,
      0,
      dwContext
    );
    if (hConnect == NULL) {
      String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
      sError.appendf("InternetConnect failed with %s: %s:%d", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum());
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok = 0;
    } else {
      self->SetHandle(hConnect);
    }
  }
  ///////////////////////////////////////// ---------------
  HINTERNET hRequest = NULL;
  if (ok) {
    // Request the file
    hRequest = ::HttpOpenRequest(
      self->GetHandle(),
      sMethod,
      url.uri(),
      HTTP_VERSION,
      NULL,
      0,
        INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_PRAGMA_NOCACHE |
        INTERNET_FLAG_RELOAD |
        INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
        (bSSL ? INTERNET_FLAG_SECURE : 0),
      0
    );
    if (hRequest == NULL) {
      String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
      sError.appendf("HttpOpenRequest failed with %s: %s:%d%s", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok = 0;
    }
  }

  if (ok) {
    if (bSSL) {
      DWORD dwFlags;
      DWORD dwBuffLen = sizeof(dwFlags);
      InternetQueryOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
      dwFlags |= 0
        | SECURITY_FLAG_IGNORE_UNKNOWN_CA
        | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
        | SECURITY_FLAG_IGNORE_WRONG_USAGE
        ;
      InternetSetOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof (dwFlags));
    }

    // Send the request.
    String sRequestHeaders;
    for (Elem* e = 0; (e = self->lHeader_.Next(e)) != 0; ) {
      sRequestHeaders.appendf("%s: %s\r\n", StringType(e->getName()), StringType(e->getString()));
    }
    BOOL bSendRequest = ::HttpSendRequest(
      hRequest,
      sRequestHeaders,
      sRequestHeaders.bytes(),
      self->sbBody_.Data(),
      self->sbBody_.Length()
    );
    if (!bSendRequest) {
      String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
      sError.appendf("HttpSendRequest failed with %s: %s:%d%s", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok = 0;
    }
  }

  if (ok) {
    self->nState_ = HTTPRequestTask::RequestState_Connected;

    ApAsyncMessage<Msg_Net_HTTP_Connected> msg;
    msg->hClient = self->hAp_;
    msg.Post();
  }

  DWORD nStatusCode = -1;
  DWORD nStatusCodeLength = sizeof(nStatusCode);
  if (ok) {
    // Get the status code.
    DWORD nIndex= 0;
    BOOL bQuery = ::HttpQueryInfo(
      hRequest,
      HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
      &nStatusCode,
      &nStatusCodeLength,
      &nIndex
    );
    if (!bQuery) {
      String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
      sError.appendf("HttpQueryInfo(STATUS_CODE) failed with %s: %s:%d%s", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok= 0;
    }
  }
///*

  String sHeaders;
  DWORD dwRawHeaderSize = 8192;
  Flexbuf<TCHAR> pHeaders(dwRawHeaderSize - sizeof(TCHAR));
  if (ok) {
    // Get the raw header
    BOOL bQuery = ::HttpQueryInfo(
      hRequest,
      HTTP_QUERY_RAW_HEADERS_CRLF,
      (LPVOID) (TCHAR*) pHeaders,
      &dwRawHeaderSize,
      NULL
    );
    if (!bQuery) {
      String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
      sError.appendf("HttpQueryInfo(RAW_HEADERS_CRLF) failed with %s: %s:%d%s", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok= 0;
    } else {
      sHeaders.set(pHeaders, dwRawHeaderSize);
    }
  }

  if (ok) {
    self->nState_ = HTTPRequestTask::RequestState_Header;

    ApAsyncMessage<Msg_Net_HTTP_Header> msg;
    msg->hClient = self->hAp_;
    msg->nStatus = nStatusCode;
    List lHeader;
    KeyValueLfBlob2List(sHeaders, lHeader);
    msg->kvHeader.fromList(lHeader);
    msg.Post();
  }
//*/

  DWORD nContentLength = -1;

  int bDone= 0;
  if (!ok) {
    bDone= 1;
  }

  while (ok && !bDone) {
    DWORD nBytesAvailable = -1;
    if (ok) {
      //DWORD nIndex = 0;
      BOOL bQuery = ::InternetQueryDataAvailable(
        hRequest,
        &nBytesAvailable,
        0,
        0
      );
      if (!bQuery) {
        DWORD nErr = ::GetLastError();
        if (nErr == ERROR_NO_MORE_FILES) {
          bDone = 1;
        } else if (nErr == ERROR_INVALID_HANDLE) {
          bDone = 1;
        } else if (nErr == ERROR_INTERNET_OPERATION_CANCELLED) {
          bDone = 1;
        } else if (nErr == ERROR_INTERNET_CONNECTION_ABORTED) {
          bDone = 1;
        } else {
          String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
          sError.appendf("InternetQueryDataAvailable() failed with %s: %s:%d%s", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
          apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
          ok= 0;
        }
      } else {
        if (nBytesAvailable <= 0) {
          bDone= 1;
        }
      }

      if (nBytesAvailable > 0) {
        nContentLength += nBytesAvailable;
      }
    }

    unsigned char* pBuffer = 0;
    if (ok && !bDone) {
      // Allocate a buffer for the file.
      pBuffer = new unsigned char[nBytesAvailable];
      if (pBuffer == 0) {
        ok = 0;
      }
    }

    DWORD dwBytesRead = 0;
    if (ok && !bDone) {
      // Read the file into the buffer.
      BOOL bRead = ::InternetReadFile(
        hRequest,
        pBuffer,
        nBytesAvailable,
        &dwBytesRead
      );
      if (!bRead) {
        DWORD nErr = ::GetLastError();
        if (nErr == ERROR_INVALID_HANDLE) {
          bDone = 1;
        } else {
          String sInternetErrorMessage = NetOS::HTTP_ErrorMessage(::GetLastError());
          sError.appendf("InternetReadFile() failed with %s: %s:%d%s", StringType(sInternetErrorMessage), StringType(url.host()), url.portnum(), StringType(url.uri()));
          apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
          ok= 0;
        }
      } else {
        if (dwBytesRead == 0) {
          bDone = 1;
        }
      }
    }

    if (ok && !bDone) {
      self->nState_ = HTTPRequestTask::RequestState_DataIn;

      ApAsyncMessage<Msg_Net_HTTP_DataIn> msg;
      msg->hClient = self->hAp_;
      msg->sbData.SetData(pBuffer, dwBytesRead);
      msg.Post();
    }

    if (pBuffer != 0) {
      delete [] pBuffer;
      pBuffer = 0;
    }

  }

  // Close all Internet handles except the one from InternetOpen.
  if (hRequest != NULL) {
    NetOS::HTTP_CloseConnection(hRequest);
    hRequest = NULL;
  }

  self->CloseHandle();
  hConnect = 0;

  self->nState_ = HTTPRequestTask::RequestState_Closed;

  if (!ok) {
    ApAsyncMessage<Msg_Net_HTTP_Failed> msg;
    msg->hClient = self->hAp_;
    msg->sMessage = sError;
    msg.Post();
  }

  {
    ApAsyncMessage<Msg_Net_HTTP_Closed> msg;
    msg->hClient = self->hAp_;
    msg.Post();
  }
#else
  #error Not implemented for this OS
#endif
  return ok;
}
