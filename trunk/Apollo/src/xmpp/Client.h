// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Client_h_INCLUDED)
#define Client_h_INCLUDED

#include "netapi/NetInterface.h"
#include "XMLProcessor.h"
#include "JabberId.h"
#include "Task.h"
#include "Room.h"
#include "Buddy.h"

class Connection;
class Parser;

class Client: public Elem
{
public:
  Client(ApHandle hAp)
    :hAp_(hAp)
    ,nPort_(0)
    ,bOnline_(0)
    ,bDefault_(0)
    ,nState_(ClientState_Unknown)
    ,bLoggedIn_(0)
    ,bReconnectAfterDisconnect_(0)
    ,bConnectImmediately_(1)
    ,nNextConnectDelay_(0)
    ,nNextConnectBase_(0)
    ,nStanzaId_(1)
    ,tLastTraffic_(0)
    ,nHeartbeatDelay_(60)
    ,pConnection_(0)
    ,pParser_(0)
    {}

  int start();
  int stop();
  int idle();

  int connect();
  int connectNow();
  int connectDeferred(int nDelaySec);
  int disconnect();
  int sendHeartbeat();

  int dataOut(unsigned char* pData, size_t nLen);
  int dataIn(unsigned char* pData, size_t nLen);

  // Called by Msg_Xmpp_StanzaIn, parses the data
  int stanzaIn(const String& sData);
  // Called from client and protocol classes, sends Msg_Xmpp_StanzaOut
  int sendStanza(Apollo::XMLNode& stanza);
  // Called by Msg_Xmpp_StanzaOut handler, sends to connection
  int stanzaOut(const String& sData);

  int isDefault();
  void setDefault(int bDefault);
  int isLoggedIn() { return bLoggedIn_; }

  int netOnline(int bOnline);

  int selfPresenceAvailable(Stanza& stanza);
  int selfPresenceUnavailable(Stanza& stanza);
  int selfPresenceError(Stanza& stanza);

  Buddy* findBuddy(const String& szJid);

  Room* findRoom(const String& szJid);
  Room* findRoom(ApHandle hRoom);
  ApHandle getRoomHandle(String& sJid);

  int enterRoom(String& sJid, String& sNickname, ApHandle hRoom);
  int enterRoomComplete(ApHandle hRoom);
  int leaveRoom(ApHandle hRoom);
  int leaveRoomComplete(ApHandle hRoom);

  int sendGroupchat(ApHandle hRoom, String& sText);
  int sendRoomState(ApHandle hRoom);

  // --------------------------------
  // Iternal events:

  int onProtocolStart();
  int onProtocolStartFailed();
  int onProtocolLogin();
  int onProtocolLoginFailed();

  int onConnectionConnected();
  int onConnectionDataIn(unsigned char* pData, size_t nLen);
  int onConnectionDisconnected();

  int onParserBegin(Apollo::XMLNode* pNode);
  int onParserStanza(Apollo::XMLNode* pStanza);
  int onParserEnd(const char* szName);

  void setJabberId(String& sJabberId);
  JabberId& getJabberId();
  void setPassword(String& sPassword);
  String& getPassword();
  void setResource(String& sResource);
  String& getResource();
  void setHost(String& sHost);
  String& getHost();
  void setPort(int nPort);
  int getPort();

  ApHandle apHandle() { return hAp_; }
  int getNextStanzaId();

  typedef enum _ClientState { ClientState_Unknown
    ,ClientState_NotConnected
    ,ClientState_ConnectDelay
    ,ClientState_Connecting
    ,ClientState_Connected
    ,ClientState_Disconnecting
  } ClientState;

  ApHandle hAp_;

protected:
  String getConnectionName();
  int getConnectDelay();

protected:
  JabberId jid_;
  String sPassword_;
  String sResource_;
  String sHost_;
  int nPort_;
  int bOnline_;
  int bDefault_;
  ClientState nState_;
  int bLoggedIn_;
  int bReconnectAfterDisconnect_;
  int bConnectImmediately_;
  int nNextConnectDelay_;
  time_t nNextConnectBase_;
  int nStanzaId_;
  time_t tLastTraffic_;
  int nHeartbeatDelay_;

protected:
  Connection* pConnection_;
  Parser* pParser_;
  SafeListT<Task> lTasks_;
  ListT<Room, Elem> lRooms_;
  ListT<Buddy, Elem> lBuddys_;

protected:
  const char* LogId();
  String sLogId_;

};

#endif // !defined(Client_h_INCLUDED)
