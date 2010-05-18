// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Socket_h_INCLUDED)
#define Socket_h_INCLUDED

#include "Apollo.h"
#include "NetOS.h"

class SocketAddress
{
public:
  SocketAddress(): saddr_(sockaddr()) {}
  SocketAddress(struct sockaddr* saddr);
  SocketAddress(const char* szAddress, int nPort);

  operator struct sockaddr*() { return &saddr_; }

  static int isAddress(const char* szAddress);
  static int isIP4Address(const char* szAddress);

  struct sockaddr saddr_;
};

class Socket: public Elem
{
public:
  Socket(ApHandle h)
    :hAp_(h)
    ,nSock_(INVALID_SOCKET)
    ,nState_(SocketState_None)
    ,bWritable_(0)
    ,bClosed_(0)
    ,nError_(0)
  {}

  int Open();
  int Close();

  int Connect(SocketAddress& saAddress);
  int Listen(SocketAddress& saAddress);
  int Send(unsigned char* pData, size_t nLen);
  int DoSendQueue();

  void OnConnect(int bSuccess);
  void OnAccept();
  void OnRead();
  void OnWrite();
  void OnClose();

  unsigned int GetFlags();
  int Connecting() { return (GetFlags() & FD_CONNECT)?1:0; }
  int Listening() { return (GetFlags() & FD_ACCEPT)?1:0; }
  int Readable() { return (int)(GetFlags() != 0 && !Connecting() && !Listening()); }
  int Writeable() { return (GetFlags() & FD_WRITE)?1:0; }
  
  void Error(int nError);
  int AssertThread();

  SOCKET Sock() { return nSock_; }

  typedef enum _SocketState { SocketState_None
    ,SocketState_Resolving
    ,SocketState_Connecting
    ,SocketState_Connected
    ,SocketState_Listening
    ,SocketState_Closed
  } SocketState;

  ApHandle hAp_;
protected:
  SOCKET nSock_;
  SocketState nState_;
  int bWritable_;
  List lSendQueue_;
  int bClosed_;
  
  int nError_;
  String sError_;
};

#endif // !defined(Socket_h_INCLUDED)
