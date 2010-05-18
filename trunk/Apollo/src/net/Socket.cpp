// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"

#include "Socket.h"
#include "NetModule.h"

SocketAddress::SocketAddress(struct sockaddr* saddr)
{
  saddr_ = *saddr;
}

SocketAddress::SocketAddress(const char* szAddress, int nPort)
{
  if (isIP4Address(szAddress)) {
    struct sockaddr_in sai;
    sai.sin_family = AF_INET;
    sai.sin_port = htons(nPort);
#if defined(WIN32)
    unsigned long nAddress = inet_addr(szAddress);
    if (nAddress == INADDR_NONE) {
      apLog_Error((LOG_CHANNEL, "SocketAddress::SocketAddress", "Address convertion to long failed: %s", StringType(szAddress)));
    } else {
      memcpy(&sai.sin_addr, &nAddress, sizeof(nAddress));
    }
#else
    if (!inet_aton(szAddress, &sai.sin_addr)) {
      apLog_Error((LOG_CHANNEL, "SocketAddress::SocketAddress", "Address convertion to long failed: %s", StringType(szAddress)));
    }
#endif

    saddr_ = *((struct sockaddr*) &sai);
  } else {
    apLog_Error((LOG_CHANNEL, "SocketAddress::SocketAddress", "Address family not supported: %s", StringType(szAddress)));
  }
}

int SocketAddress::isAddress(const char* szAddress)
{
  int is = 1;

  String sAddress = szAddress;
  while (sAddress.chars() > 0) {
    String c = sAddress.subString(0, 1);
    sAddress = sAddress.subString(1);
    if (String::isDigit(c) || String::isPunct(c)) {
      // ok
    } else {
      is = 0;
      break;
    }
  }

  return is;
}

int SocketAddress::isIP4Address(const char* szAddress)
{
  int is = 1;

  String sAddress = szAddress;
  int nCnt = 0;
  while (sAddress.chars() > 0) {
    String c = sAddress.subString(0, 1);
    sAddress = sAddress.subString(1);
    if (String::isDigit(c)) {
      // ok
    } else if (c == ".") {
      nCnt++;
      if (nCnt > 3) {
        is = 0;
        break;
      }
    } else {
      is = 0;
      break;
    }
  }

  if (nCnt < 3) {
    is = 0;
  }

  return is;
}

//----------------------------------------------------------
// Action

int Socket::Open()
{
  if (!AssertThread()) { return 0; }

  int ok = 0;
  nSock_ = NetOS::TCPSocketCreate(hAp_);
  if (nSock_ != INVALID_SOCKET) {
    if (NetOS::SocketSetNonblock(nSock_, hAp_) != SOCKET_ERROR) { ok = 1; }
    else { (void)NetOS::SocketClose(nSock_, hAp_); nSock_ = INVALID_SOCKET; }
  }
  return ok;
}

int Socket::Close()
{
  if (!AssertThread()) { return 0; }

  int ok = 0;

  if (NetOS::SocketClose(nSock_, hAp_) != SOCKET_ERROR) {
    ok= 1;
  }

  nSock_ = INVALID_SOCKET;
  nState_ = SocketState_Closed;

  return ok;
}

int Socket::Connect(SocketAddress& saAddress)
{
  if (!AssertThread()) { return 0; }

  int ok = 0;

  if (::connect(nSock_, (struct sockaddr*) saAddress, sizeof(struct sockaddr)) == SOCKET_ERROR) {
#if defined(WIN32)
    int nErr = WSAGetLastError();
    if (nErr == WSAEWOULDBLOCK) {
#elif defined(LINUX) || defined(MAC)
    int nErr = errno;
    if (nErr == EWOULDBLOCK || nErr == EAGAIN || nErr == EINPROGRESS) {
#else
  #error Not implemented for this OS
#endif
      ok= 1;
    } else {
      apLog_Error((LOG_CHANNEL, "Socket::Connect", "::connect failed " ApHandleFormat " %d", ApHandleType(hAp_), nErr));
    }
  } else {
    ok= 1;
  }

  if (ok) {
    nState_ = SocketState_Connecting;
  }

  return ok;
}

int Socket::Listen(SocketAddress& saAddress)
{
  if (!AssertThread()) { return 0; }

  int ok = 0;

  if (::bind(nSock_, (struct sockaddr*)saAddress, sizeof(struct sockaddr)) == SOCKET_ERROR) {
#if defined(WIN32)
  int nErr = ::WSAGetLastError();
  int bAddrInUse = (nErr == WSAEADDRINUSE);
#elif defined(LINUX) || defined(MAC)
  int nErr = errno;
  int bAddrInUse = (nErr == EADDRINUSE);
#else
#error Not implemented for this platform
#endif
    if (bAddrInUse) {
      apLog_Error((LOG_CHANNEL, "Socket::Listen", "::bind failed: hAp=" ApHandleFormat " err=\"Address in use\" (%d)", ApHandleType(hAp_), nErr));
    } else {
      apLog_Error((LOG_CHANNEL, "Socket::Listen", "::bind failed: hAp=" ApHandleFormat " err=%d", ApHandleType(hAp_), nErr));
    }
  } else {
    if (::listen(nSock_, SOMAXCONN) == SOCKET_ERROR) {
#if defined(WIN32)
      int nErr = ::WSAGetLastError();
      int bAddrInUse = (nErr == WSAEADDRINUSE);
#elif defined(LINUX) || defined(MAC)
      int nErr = errno;
      int bAddrInUse = (nErr == EADDRINUSE);
#else
#error Not implemented for this platform
#endif
      if (bAddrInUse) {
        apLog_Error((LOG_CHANNEL, "Socket::Listen", "::listen() failed: hAp=" ApHandleFormat " err=%d (address in use)", ApHandleType(hAp_), nErr));
      } else {
        apLog_Error((LOG_CHANNEL, "Socket::Listen", "::listen() failed: hAp=" ApHandleFormat " err=%d", ApHandleType(hAp_), nErr));
      }
    } else {
      nState_ = SocketState_Listening;
      ok = 1;
    }
  }

  return ok;
}

int Socket::DoSendQueue()
{
  if (!AssertThread()) { return 0; }

  int ok = 1;

  while (lSendQueue_.length() > 0 && ok && bWritable_) {
    ok = 0;
    Buffer* sb = (Buffer*) lSendQueue_.First();
    int nFlags = 0;
#if defined(LINUX)
    nFlags |= MSG_NOSIGNAL;
#elif defined(MAC)
    nFlags |= SO_NOSIGPIPE;
#endif
    int nResult = ::send(nSock_, (const char *) sb->Data(), sb->Length(), nFlags);
    if (nResult >= 0) {
      size_t nReceived = nResult;
      if (nReceived == sb->Length()) {
        ok= 1;
        lSendQueue_.Remove(sb);
        delete sb;
      } else {
        ok= 1;
        sb->Discard(nReceived);
      }
    } else {
#if defined(WIN32)
      int nErr = WSAGetLastError();
      if (nErr == WSAEWOULDBLOCK) {
#elif defined(LINUX) || defined(MAC)
      int nErr = errno;
      if ((nErr == EWOULDBLOCK) || (nErr == EAGAIN)) {
#else
  #error Not implemented for this OS
#endif
        bWritable_ = 0;
        ok= 1;
#if defined(LINUX) || defined(MAC)
      } else if ((nErr = ENOTCONN) || (nErr == EPIPE) || (nErr == ECONNRESET)) {
        OnClose();
#endif
      } else {
        apLog_Error((LOG_CHANNEL, "Socket::DoSendQueue", "::send failed " ApHandleFormat " %d", ApHandleType(hAp_), nErr));
      }
    }
  }

  return ok;
}

int Socket::Send(unsigned char* pData, size_t nLen)
{
  if (!AssertThread()) { return 0; }

  int ok = 0;

  Buffer* sb = new Buffer();
  if (sb != 0) {
    sb->SetData(pData, nLen);
    lSendQueue_.AddLast(sb);
    ok = 1;
  }

  DoSendQueue();

  return ok;
}

//----------------------------------------------------------
// Reaction

void Socket::OnConnect(int bSuccess)
{
  if (!AssertThread()) { return; }

  if (bSuccess) {
    nState_ = SocketState_Connected;
    bWritable_ = 1;
  } else {
    Close();
  }

  ApAsyncMessage<Msg_Net_TCP_Connected> msg;
  msg->hConnection = hAp_;
  msg->bSuccess = bSuccess;
  msg->sComment = sError_;
  msg.Post();
}

void Socket::OnAccept()
{
  if (!AssertThread()) { return; }

  int ok = 1;

  SocketAddress saClientAddress;

  int nSASize = sizeof(struct sockaddr_in);
  SOCKET nClientSocket = ::accept(nSock_, (struct sockaddr*)saClientAddress, &nSASize);
  
  if (nClientSocket == SOCKET_ERROR) {
    ok = 0;
#if defined(WIN32)
    int nErr = ::WSAGetLastError();
    if (nErr == WSAEWOULDBLOCK) { ok = 1; }
#elif defined(LINUX) || defined(MAC)
    int nErr = errno;
    if (nErr == EWOULDBLOCK || nErr = EAGAIN) { ok = 1; }
#else
#error Not implemented for this OS
#endif
    if (ok) {
      apLog_Warning((LOG_CHANNEL, "Socket::OnAccept", "Got accept but no incoming connection: socket=" ApHandleFormat "", ApHandleType(hAp_)));
    } else {
      Error(nErr);
      apLog_Warning((LOG_CHANNEL, "Socket::OnAccept", "accept failed: socket=" ApHandleFormat " error=%s", ApHandleType(hAp_), StringType(sError_)));
    }
  } else {
    ApHandle hAp = Apollo::newHandle();
    Socket* s = new Socket(hAp);
    if (s != 0) {
      s->nSock_ = nClientSocket;
      s->nState_ = SocketState_Connected;

      String sClientAddr;
      int nClientPort = 0;

      if (0) {
      } else if (nSASize == sizeof(struct sockaddr_in)) {
        struct sockaddr_in* saiClient = (struct sockaddr_in*) (struct sockaddr*) saClientAddress;
        sClientAddr = ::inet_ntoa(saiClient->sin_addr);
        nClientPort = ::ntohs(saiClient->sin_port);
      } else {
        ok = 0;
        apLog_Warning((LOG_CHANNEL, "Socket::OnAccept", "Unknown or unsupported address type of client, disconnecting"));
        (void) s->Close();
        delete s; s = NULL;
      }
      
      if (ok) {
        apLog_Verbose((LOG_CHANNEL, "Socket::OnAccept", "Accepted client connection: hAp=" ApHandleFormat " addr=%s port=%d", ApHandleType(hAp), StringType(sClientAddr), nClientPort));
        
        {
          ApAsyncMessage<Msg_Net_TCP_ConnectionAccepted> msg;
          msg->hServer = hAp_; 
          msg->hConnection = hAp; 
          msg->sClientAddress = sClientAddr;
          msg->nClientPort = nClientPort;
          msg.Post();
        }
        NetModuleInstance::Get()->oSocketIO_.AddSocket(s);
      }
    } else {
      apLog_Error((LOG_CHANNEL, "Socket::OnAccept", "new Socket() failed, closing connection"));
      NetOS::SocketClose(nClientSocket, hAp);
    }
  }
}

void Socket::OnRead()
{
  if (!AssertThread()) { return; }

  int ok = 1;
  int nCount = 0;
  int bWouldBlock = 0;
  while (!bWouldBlock && ok) {
    ok = 0;
    nCount++;
    Flexbuf<unsigned char> buf(Flexbuf_DefaultLength);
    int nResult = ::recv(nSock_, (char*) (unsigned char*) buf, buf.length(), 0);
    if (nResult > 0) {
      ok = 1;
      // Now we finally got data. Send it to the framework.

      ApAsyncMessage<Msg_Net_TCP_DataIn> msg;
      msg->hConnection = hAp_;
      msg->sbData.SetData((unsigned char*) buf, nResult);
      msg.Post();

    } else if (nResult == 0) {
      // Connection has been gracefully closed
      bWritable_ = 0;
#if defined(LINUX) || defined(MAC)
      OnClose();
#endif
    } else {
#if defined(WIN32)
      int nErr = WSAGetLastError();
      if (nErr == WSAEWOULDBLOCK) {
#elif defined(LINUX) || defined(MAC)
      int nErr = errno;
      if (nErr == EWOULDBLOCK || nErr == EAGAIN) {
#else
  #error Not implemented for this OS
#endif
        bWouldBlock = 1;
        if (nCount == 1) {
          apLog_Warning((LOG_CHANNEL, "Socket::OnRead", "Got read state but no data on first recv " ApHandleFormat " %d", ApHandleType(hAp_), nErr));
        } else {
          ok = 1;
        }
      } else {
        apLog_Error((LOG_CHANNEL, "Socket::OnRead", "::recv failed " ApHandleFormat " %d", ApHandleType(hAp_), nErr));
      }
    }
  }
}

void Socket::OnWrite()
{
  if (!AssertThread()) { return; }

  bWritable_ = 1;
  DoSendQueue();
}

void Socket::OnClose()
{
  if (!AssertThread()) { return; }

  bWritable_ = 0;
  
  int bListening = Listening();

  int bClose = !bClosed_;
  bClosed_ = 1;
  if (bClose) {
    Close();

    if (bListening) {
      ApAsyncMessage<Msg_Net_TCP_ListenStopped> msg;
      msg->hServer = hAp_;
      msg.Post();
    } else {
      ApAsyncMessage<Msg_Net_TCP_Closed> msg;
      msg->hConnection = hAp_;
      msg.Post();
    }

    DeleteSocketTask* pTask = new DeleteSocketTask(this);
    if (pTask != 0) {
      NetModuleInstance::Get()->oSocketIO_.AddTask(pTask);
    }
  }
}

//----------------------------------------------------------
// Utils

unsigned int Socket::GetFlags()
{
  unsigned int nFlags = 0;
  switch (nState_) { 
  case SocketState_None:
    break;
  case SocketState_Resolving:
    break;
  case SocketState_Connecting:
    nFlags |= FD_CONNECT;
    break;
  case SocketState_Connected:
    nFlags |= FD_READ;
    nFlags |= FD_CLOSE;
    if (lSendQueue_.length() > 0) {
      nFlags |= FD_WRITE;
    }
    break;
  case SocketState_Listening:
    nFlags |= FD_ACCEPT;
    break;
  case SocketState_Closed:
    break;
  default: ;
  }
  return nFlags;
}

void Socket::Error(int nError)
{
  nError_ = nError;
  sError_ = "";

#if defined(WIN32)
  TCHAR* lpMsgBuf = 0;
  int result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM,
    0,
    nError,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (TCHAR*) &lpMsgBuf,
    0,
    0);
  if (result) {
    sError_ = lpMsgBuf;
  } else {
    sError_ = ::strerror(nError);
  }
  ::LocalFree(lpMsgBuf);
#else
  sError_ = (const char*) ::strerror(nError);
#endif
}

int Socket::AssertThread()
{
  return NetModuleInstance::Get()->oSocketIO_.AssertThread();
}
