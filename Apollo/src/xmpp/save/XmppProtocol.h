// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XmppProtocol_h_INCLUDED)
#define XmppProtocol_h_INCLUDED

#include "netapi/NetInterface.h"
#include "XMLProcessor.h"
#include "URL.h"

#include "JabberId.h"
#include "ProtocolTasks.h"

// -------------------------------------------------------------------

class XmppProcessor;
class XmppRoom;
class Msg_Protocol_Feature;
class Msg_Protocol_EnterLocation;
class Msg_Protocol_LeaveLocation;
class Msg_Net_Online;
class Msg_Net_Offline;
class Msg_System_SecTimer;
class Msg_Protocol_SendPublicChat;
class Msg_MainLoop_EventLoopBeforeEnd;
class Msg_Net_TCP_Connected;
class Msg_Net_TCP_Closed;
class Msg_Net_TCP_DataIn;

class XmppProtocol
{
public:
  XmppProtocol();
  virtual ~XmppProtocol();

  int Init();
  int Exit();

  XmppRoom* FindRoom(const char* szJID);
  XmppRoom* FindRoom(ApHandle hLocation);

  int OnConnected();
  int OnDataIn(unsigned char* pData, size_t nLen);
  int OnClosed();

  int OnLoggedIn();

  int OnStanzaIn(Apollo::XMLNode* pStanza);
  int OnStanzaOut(Apollo::XMLNode* pStanza);

  int SendOnlinePresence();

  XmppRoom* NewRoom(const char* szRoomJID, ApHandle hLocation);
  int DeleteRoom(XmppRoom* pRoom);
  int GetStanzaError(Apollo::XMLNode* pStanza, int& nError, SString& sError);

  void On_Protocol_Feature(Msg_Protocol_Feature* pMsg);
  void On_Protocol_EnterLocation(Msg_Protocol_EnterLocation* pMsg);
  void On_Protocol_LeaveLocation(Msg_Protocol_LeaveLocation* pMsg);
  void On_Net_Online(Msg_Net_Online* pMsg);
  void On_Net_Offline(Msg_Net_Offline* pMsg);
  void On_System_SecTimer(Msg_System_SecTimer* pMsg);
  void On_Protocol_SendPublicChat(Msg_Protocol_SendPublicChat* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);

protected:
  void ConnectSoon();
  void Connect();
  int Connected() { return pConnection_ != 0; }
  int GetConnectDelay();
  int NextId();

  friend class LoginTask;

protected:
  SListT<XmppProtocolTask, SElem> slTasks_;
  ApHandle hConnHandle_;

  int bFirstConnect_;
  int bConnectSoon_;
  int bConnecting_;
  time_t tLastDisconnect_;
  int nNextConnectDelay_;
  int nRequestId_;

  Apollo::TCPConnection* pConnection_;
  XmppProcessor* pProcessor_;

  JabberId jid_;
  SString sPassword_;
  SString sResource_;
  SString sConnectedHost_;
  int nConnectedPort_;

  SList slRooms_;
};

void On_TCP_Connected(Msg_Net_TCP_Connected* pMsg);
void On_TCP_Closed(Msg_Net_TCP_Closed* pMsg);
void On_TCP_DataIn(Msg_Net_TCP_DataIn* pMsg);

#endif // !defined(XmppProtocol_h_INCLUDED)
