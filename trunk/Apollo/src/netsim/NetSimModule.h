// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NetSimModule_H_INCLUDED)
#define NetSimModule_H_INCLUDED

#include "Apollo.h"
#include "ApContainer.h"
#include "MsgMainLoop.h"
#include "MsgNet.h"
#include "MsgNetSim.h"
#include "Http.h"
#include "Xmpp.h"

class NetSimModule
{
public:
  NetSimModule()
    //:bConnected_(0)
    //,hConnection_(ApNoHandle)
    {}

  int init();
  void exit();

  void On_NetSim_Engage(Msg_NetSim_Engage* pMsg);
  void On_NetSim_Disengage(Msg_NetSim_Disengage* pMsg);
  void On_NetSim_ContinueHttp(Msg_NetSim_ContinueHttp* pMsg);
  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopDelayEnd(Msg_MainLoop_EventLoopDelayEnd* pMsg);
  
  void On_Net_IsOnline(Msg_Net_IsOnline* pMsg);
  void On_Net_DNS_Resolve(Msg_Net_DNS_Resolve* pMsg);
  void On_Net_TCP_Close(Msg_Net_TCP_Close* pMsg);
  void On_Net_TCP_DataOut(Msg_Net_TCP_DataOut* pMsg);
  void On_Net_TCP_Connect(Msg_Net_TCP_Connect* pMsg);
  void On_Net_HTTP_Request(Msg_Net_HTTP_Request* pMsg);
  void On_Net_HTTP_Cancel(Msg_Net_HTTP_Cancel* pMsg);
  void On_Net_HTTP_CancelAll(Msg_Net_HTTP_CancelAll* pMsg);

  void On_Net_Online(Msg_Net_Online* pMsg);
  void On_Net_Offline(Msg_Net_Offline* pMsg);

protected:
  void writeHttpLog(const String& s);
  //void closeConnection();
  //void sendData(const unsigned char* pData, size_t nLen);

protected:
  //int bConnected_;
  //ApHandle hConnection_;

  ApHandlePointerTree<Http*> http_;

  ApMsgHandlerRegistrantList simMsgRegistry_;
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<NetSimModule> NetSimInstance;

#endif // NetSimModule_H_INCLUDED
