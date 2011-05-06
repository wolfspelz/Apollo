// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NetAPIModule_h_INCLUDED)
#define NetAPIModule_h_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgNet.h"
#include "MsgSystem.h"
#include "NetInterface.h"

class NetAPIModule
{
public:
  NetAPIModule()
    :nValue_(42)
    ,lTCPConnections_("TCPConnections")
    ,lHTTPClients_("HTTPClients")
    {}
  virtual ~NetAPIModule() {}

  int Init();
  int Exit();

  void On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg);

  void On_Net_TCP_Connected(Msg_Net_TCP_Connected* pMsg);
  void On_Net_TCP_DataIn(Msg_Net_TCP_DataIn* pMsg);
  void On_Net_TCP_Closed(Msg_Net_TCP_Closed* pMsg);
  
  void On_Net_TCP_Listening(Msg_Net_TCP_Listening* pMsg);
  void On_Net_TCP_ConnectionAccepted(Msg_Net_TCP_ConnectionAccepted* pMsg);
  void On_Net_TCP_ListenStopped(Msg_Net_TCP_ListenStopped* pMsg);

  void On_Net_HTTP_Connected(Msg_Net_HTTP_Connected* pMsg);
  void On_Net_HTTP_Header(Msg_Net_HTTP_Header* pMsg);
  void On_Net_HTTP_DataIn(Msg_Net_HTTP_DataIn* pMsg);
  void On_Net_HTTP_Failed(Msg_Net_HTTP_Failed* pMsg);
  void On_Net_HTTP_Closed(Msg_Net_HTTP_Closed* pMsg);

  Apollo::TCPServer* FindTCPServer(const ApHandle& hAp);
  void AddTCPServer(Apollo::TCPServer* pServer);
  void RemoveTCPServer(Apollo::TCPServer* pServer);

  Apollo::TCPConnection* FindTCPConnection(const ApHandle& hAp);
  void AddTCPConnection(Apollo::TCPConnection* pConnection);
  void RemoveTCPConnection(Apollo::TCPConnection* pConnection);

  Apollo::HTTPClient* FindHTTPClient(const ApHandle& hAp);
  void AddHTTPClient(Apollo::HTTPClient* pClient);
  void RemoveHTTPClient(Apollo::HTTPClient* pClient);

protected:
  int nValue_;

protected:
  ApHandleTree<Apollo::TCPServer*> tcpServers_;
  ListT<Apollo::TCPConnection, Elem> lTCPConnections_;
  ListT<Apollo::HTTPClient, Elem> lHTTPClients_;
};

typedef ApModuleSingleton<NetAPIModule> NetAPIModuleInstance;

#endif // !defined(NetAPIModule_h_INCLUDED)
