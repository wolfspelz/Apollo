// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"

#if defined(AP_TEST_Net)

#include "MsgNet.h"
#include "ApLog.h"
#include "MsgUnitTest.h"

#include "netapi/NetInterface.h"

//----------------------------------------------------------

#if defined(AP_TEST_Net_TCP)

class Test_Net_TCP_On_Data : public Elem
{
public:
  Test_Net_TCP_On_Data(const ApHandle& hAp, const char* szAddress, int nPort)
    :sAddress_(szAddress)
    ,nPort_(nPort)
    ,bConnected_(0)
    ,nDataIn_(0)
    ,bClosed_(0)
    ,hAp_(hAp)
  {}
  inline ApHandle apHandle() { return hAp_; }
  String sAddress_;
  int nPort_;
  int bConnected_;
  int nDataIn_;
  int bClosed_;
  ApHandle hAp_;
};

static List lTest_Net_TCP_On_DataList;
static int bTest_Net_TCP_Connected_Done = 0;
static int bTest_Net_TCP_Closed_Done = 0;
static int bTest_Net_TCP_DataIn_Done = 0;

Test_Net_TCP_On_Data* Test_Net_TCP_On_Data_FindByHandle(const ApHandle& h)
{
  Test_Net_TCP_On_Data* pResult = 0;
  for (Test_Net_TCP_On_Data* e = 0; (e = (Test_Net_TCP_On_Data*) lTest_Net_TCP_On_DataList.Next(e)) != 0; ) {
    if (e->apHandle() == h) {
      pResult = e;
      break;
    }
  }
  return pResult;
}

int Test_Net_TCP_On_TCP_Connected(Msg_Net_TCP_Connected* pMsg)
{
  Test_Net_TCP_On_Data* d = Test_Net_TCP_On_Data_FindByHandle(pMsg->hConnection);
  if (d != 0) {
    d->bConnected_ = 1;

    if (!pMsg->bSuccess) {
      apLog_Warning((LOG_CHANNEL, "OnTCPConnected", "Not connected " ApHandleFormat " %s: %s", ApHandleType(pMsg->hConnection), StringType(d->sAddress_), StringType(pMsg->sComment)));
    } else {
      const char* pBuf = "GET / HTTP/1.0\r\nConnection: close\r\n\r\n";
      Msg_Net_TCP_DataOut msg;
      msg.hConnection = pMsg->hConnection;
      msg.sbData.SetData((const unsigned char*) pBuf, ::strlen(pBuf));
      msg.Request();
    }

    {
      unsigned int n = 0;
      for (Test_Net_TCP_On_Data* e = 0; (e = (Test_Net_TCP_On_Data*) lTest_Net_TCP_On_DataList.Next(e)) != 0; ) {
        if (e->bConnected_) {
          n++;
        }
      }
      if (n == lTest_Net_TCP_On_DataList.length() && !bTest_Net_TCP_Connected_Done) {
        AP_UNITTEST_SUCCESS(Test_Net_TCP_Connected);
        bTest_Net_TCP_Connected_Done = 1;
      }
    }
  }

  return 1;
}

int Test_Net_TCP_On_TCP_Closed(Msg_Net_TCP_Closed* pMsg)
{
  Test_Net_TCP_On_Data* d = Test_Net_TCP_On_Data_FindByHandle(pMsg->hConnection);
  if (d != 0) {
    d->bClosed_ = 1;

    {
      unsigned int n = 0;
      for (Test_Net_TCP_On_Data* e = 0; (e = (Test_Net_TCP_On_Data*) lTest_Net_TCP_On_DataList.Next(e)) != 0; ) {
        if (e->bClosed_) {
          n++;
        }
      }
      if (n == lTest_Net_TCP_On_DataList.length() && !bTest_Net_TCP_Closed_Done) {
        AP_UNITTEST_SUCCESS(Test_Net_TCP_Closed);
        bTest_Net_TCP_Closed_Done = 1;
      }
    }
  }

  return 1;
}

int Test_Net_TCP_On_TCP_DataIn(Msg_Net_TCP_DataIn* pMsg)
{
  Test_Net_TCP_On_Data* d = Test_Net_TCP_On_Data_FindByHandle(pMsg->hConnection);
  if (d != 0) {
    d->nDataIn_ += pMsg->sbData.Length();

    {
      unsigned int n = 0;
      for (Test_Net_TCP_On_Data* e = 0; (e = (Test_Net_TCP_On_Data*) lTest_Net_TCP_On_DataList.Next(e)) != 0; ) {
        if (e->nDataIn_ > 0) {
          n++;
        }
      }
      if (n == lTest_Net_TCP_On_DataList.length() && !bTest_Net_TCP_DataIn_Done) {
        AP_UNITTEST_SUCCESS(Test_Net_TCP_DataIn);
        bTest_Net_TCP_DataIn_Done = 1;
      }
    }
  }

  return 1;
}

int Test_Begin_Net_TCP()
{
  int ok = 1;

  { Msg_Net_TCP_Connected msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_TCP_On_TCP_Connected, 0, 0); }
  { Msg_Net_TCP_DataIn msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_TCP_On_TCP_DataIn, 0, 0); }
  { Msg_Net_TCP_Closed msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_TCP_On_TCP_Closed, 0, 0); }

  lTest_Net_TCP_On_DataList.AddLast(new Test_Net_TCP_On_Data(Apollo::newHandle(), "www.heise.de", 80));
  lTest_Net_TCP_On_DataList.AddLast(new Test_Net_TCP_On_Data(Apollo::newHandle(), "www.virtual-presence.org", 80));
  lTest_Net_TCP_On_DataList.AddLast(new Test_Net_TCP_On_Data(Apollo::newHandle(), "blog.wolfspelz.de", 80));
  lTest_Net_TCP_On_DataList.AddLast(new Test_Net_TCP_On_Data(Apollo::newHandle(), "www.google.de", 80));
  //lTest_Net_TCP_On_DataList.AddLast(new Test_Net_TCP_On_Data(Apollo::newHandle(), "localhost", 80));

  for (Test_Net_TCP_On_Data* e = 0; (e = (Test_Net_TCP_On_Data*) lTest_Net_TCP_On_DataList.Next(e)) != 0; ) {
    Msg_Net_TCP_Connect msg;
    msg.sHost = e->sAddress_;
    msg.nPort = e->nPort_;
    msg.hConnection = e->apHandle();
    msg.Request();
  }

  return ok;
}

int Test_End_Net_TCP()
{
  int ok = 1;

  Test_Net_TCP_On_Data* d = 0;
  while ((d = (Test_Net_TCP_On_Data*) lTest_Net_TCP_On_DataList.First()) != 0) {
    lTest_Net_TCP_On_DataList.Remove(d);
    delete d;
  }

  { Msg_Net_TCP_Connected msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_TCP_On_TCP_Connected, 0); }
  { Msg_Net_TCP_DataIn msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_TCP_On_TCP_DataIn, 0); }
  { Msg_Net_TCP_Closed msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_TCP_On_TCP_Closed, 0); }

  return ok;
}

//----------------------------
// ClientServer test using net with messages

static ApHandle g_Test_Net_TCP_ClientServer_Simple_hServer = Apollo::newHandle();
static ApHandle g_Test_Net_TCP_ClientServer_Simple_hServerClient = ApNoHandle;
static ApHandle g_Test_Net_TCP_ClientServer_Simple_hClient = Apollo::newHandle();

static const String g_Test_Net_TCP_ClientServer_Simple_sAddress = "localhost";
static const int g_Test_Net_TCP_ClientServer_Simple_nPort = 9753;

static const String g_Test_Net_TCP_ClientServer_Simple_sExpectedData = "Test data in for ClientServer \n TCP Unittest \r\n\r\n";
static String g_Test_Net_TCP_ClientServer_Simple_sReceivedData = "INVALIDDATA";

static String g_Test_Net_TCP_ClientServer_Simple_sError = "";

static void Test_Net_TCP_ClientServer_Simple_ServerStarted(Msg_Net_TCP_Listening* pMsg)
{
  if (pMsg->hServer == g_Test_Net_TCP_ClientServer_Simple_hServer) {
    if (g_Test_Net_TCP_ClientServer_Simple_sError.empty()) {
      if (pMsg->bSuccess == 0) {
        g_Test_Net_TCP_ClientServer_Simple_sError = "Failed to start server / Test_Net_TCP_ClientServer_Simple_ServerStarted";
        g_Test_Net_TCP_ClientServer_Simple_hClient = ApNoHandle;
        { Msg_Net_TCP_ListenStop msg; msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer; msg.Request(); }
      } else {
        Msg_Net_TCP_Connect msg; 
        msg.hConnection = g_Test_Net_TCP_ClientServer_Simple_hClient;
        msg.sHost = g_Test_Net_TCP_ClientServer_Simple_sAddress;
        msg.nPort = g_Test_Net_TCP_ClientServer_Simple_nPort;
        if (!msg.Request()) {
          g_Test_Net_TCP_ClientServer_Simple_sError = "Failed to connect to server / Test_Net_TCP_ClientServer_Simple_ServerStarted";
          g_Test_Net_TCP_ClientServer_Simple_hClient = ApNoHandle;
          { Msg_Net_TCP_ListenStop msg; msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer; msg.Request(); }
        }
      }
    } else {
      Msg_Net_TCP_ListenStop msg; msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer; msg.Request();
    }
  }
}

static void Test_Net_TCP_ClientServer_Simple_ConnectionAccepted(Msg_Net_TCP_ConnectionAccepted* pMsg)
{
  if (pMsg->hServer == g_Test_Net_TCP_ClientServer_Simple_hServer) {
    g_Test_Net_TCP_ClientServer_Simple_hServerClient = pMsg->hConnection;
    int ok = 1;
    if (ok) { 
      Msg_Net_TCP_DataOut msg; 
      msg.hConnection = pMsg->hConnection; 
      msg.sbData.SetData((String)g_Test_Net_TCP_ClientServer_Simple_sExpectedData); 
      ok = msg.Request();
    }
    if (!ok) {
      g_Test_Net_TCP_ClientServer_Simple_sError = "Failed to send data / Test_Net_TCP_ClientServer_Simple_ConnectionAccepted";
      { Msg_Net_TCP_Close msg; msg.hConnection = pMsg->hConnection; msg.Request(); }
    }
  }
}

static void Test_Net_TCP_ClientServer_Simple_Connected(Msg_Net_TCP_Connected* pMsg)
{
  if (pMsg->hConnection == g_Test_Net_TCP_ClientServer_Simple_hClient) {
    if (g_Test_Net_TCP_ClientServer_Simple_sError.empty()) {
      if (pMsg->bSuccess) {
        // Ok
      } else {
        g_Test_Net_TCP_ClientServer_Simple_sError = "Failed to connect to server / Test_Net_TCP_ClientServer_Simple_Connected";
        g_Test_Net_TCP_ClientServer_Simple_hClient = ApNoHandle;
      }
    } else {
      Msg_Net_TCP_Close msg; pMsg->hConnection = pMsg->hConnection; msg.Request();
    }
  }
}

static void Test_Net_TCP_ClientServer_Simple_DataIn(Msg_Net_TCP_DataIn* pMsg)
{
  if (pMsg->hConnection == g_Test_Net_TCP_ClientServer_Simple_hClient) {
    (void) pMsg->sbData.GetString(g_Test_Net_TCP_ClientServer_Simple_sReceivedData);
    { Msg_Net_TCP_Close msg; msg.hConnection = pMsg->hConnection; msg.Request(); }
  }
}

static void Test_Net_TCP_ClientServer_Simple_Disconnected(Msg_Net_TCP_Closed* pMsg)
{
  int bOwnedConnection = 0;
  if (0) {
  } else if (pMsg->hConnection == g_Test_Net_TCP_ClientServer_Simple_hServerClient) {
    g_Test_Net_TCP_ClientServer_Simple_hServerClient = ApNoHandle;
    bOwnedConnection = 1;
  } else if (pMsg->hConnection == g_Test_Net_TCP_ClientServer_Simple_hClient) {
    g_Test_Net_TCP_ClientServer_Simple_hClient = ApNoHandle;
    bOwnedConnection = 1;
  }
  if (bOwnedConnection) {
    if (!ApIsHandle(g_Test_Net_TCP_ClientServer_Simple_hClient) && !ApIsHandle(g_Test_Net_TCP_ClientServer_Simple_hServerClient)) {
      Msg_Net_TCP_ListenStop msg; msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer; msg.Request();
    }
  }
}

static void Test_Net_TCP_ClientServer_Simple_ServerStopped(Msg_Net_TCP_ListenStopped* pMsg)
{
  if (pMsg->hServer == g_Test_Net_TCP_ClientServer_Simple_hServer) {
    g_Test_Net_TCP_ClientServer_Simple_hServer = ApNoHandle;
    int ok = 0;
    if (g_Test_Net_TCP_ClientServer_Simple_sError.empty()) {
      ok = ((String) g_Test_Net_TCP_ClientServer_Simple_sExpectedData == (const char*) (String) g_Test_Net_TCP_ClientServer_Simple_sReceivedData);
      if (!ok) {
        g_Test_Net_TCP_ClientServer_Simple_sError = "Received data mismatch / Test_Net_TCP_ClientServer_Simple_ServerStopped";
      }
    }
    if (ok) {
      AP_UNITTEST_SUCCESS(Test_Net_TCP_ClientServer_Simple);
    } else {
      AP_UNITTEST_FAILED(Test_Net_TCP_ClientServer_Simple, ((const char*) g_Test_Net_TCP_ClientServer_Simple_sError));
    }
  }
}

int Test_Begin_Net_TCP_ClientServer_Simple()
{
  int ok = 1;
  
  { Msg_Net_TCP_Listening msg; msg.Hook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_ServerStarted, 0, 0); }
  { Msg_Net_TCP_Connected msg; msg.Hook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_Connected, 0, 0); }
  { Msg_Net_TCP_ConnectionAccepted msg; msg.Hook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_ConnectionAccepted, 0, 0); }
  { Msg_Net_TCP_DataIn msg; msg.Hook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_DataIn, 0, 0); }
  { Msg_Net_TCP_Closed msg; msg.Hook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_Disconnected, 0, 0); }
  { Msg_Net_TCP_ListenStopped msg; msg.Hook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_ServerStopped, 0, 0); }

  if (ok) {
    Msg_Net_TCP_Listen msg;
    msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer;
    msg.sAddress = g_Test_Net_TCP_ClientServer_Simple_sAddress;
    msg.nPort = g_Test_Net_TCP_ClientServer_Simple_nPort;
    if (!msg.Request()) {
      g_Test_Net_TCP_ClientServer_Simple_sError = "Failed to start server / Test_Net_TCP_ClientServer_Simple_ServerStarted";
      g_Test_Net_TCP_ClientServer_Simple_hClient = ApNoHandle;
      { Msg_Net_TCP_ListenStop msg; msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer; msg.Request(); }
    }
  }

  return ok;
}

int Test_End_Net_TCP_ClientServer_Simple()
{
  int ok = 1;
  
  if (ApIsHandle(g_Test_Net_TCP_ClientServer_Simple_hClient)) {
    AP_DEBUG_BREAK();
    { Msg_Net_TCP_Close msg; msg.hConnection = g_Test_Net_TCP_ClientServer_Simple_hClient; msg.Request(); }
  }
  if (ApIsHandle(g_Test_Net_TCP_ClientServer_Simple_hServerClient)) {
    AP_DEBUG_BREAK();
    { Msg_Net_TCP_Close msg; msg.hConnection = g_Test_Net_TCP_ClientServer_Simple_hServerClient; msg.Request(); }
  }
  if (ApIsHandle(g_Test_Net_TCP_ClientServer_Simple_hServer)) {
    AP_DEBUG_BREAK();
    { Msg_Net_TCP_ListenStop msg; msg.hServer = g_Test_Net_TCP_ClientServer_Simple_hServer; msg.Request(); }
  }

  { Msg_Net_TCP_Listening msg; msg.UnHook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_ServerStarted, 0); }
  { Msg_Net_TCP_Connected msg; msg.UnHook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_Connected, 0); }
  { Msg_Net_TCP_ConnectionAccepted msg; msg.UnHook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_ConnectionAccepted, 0); }
  { Msg_Net_TCP_DataIn msg; msg.UnHook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_DataIn, 0); }
  { Msg_Net_TCP_Closed msg; msg.UnHook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_Disconnected, 0); }
  { Msg_Net_TCP_ListenStopped msg; msg.UnHook("Test_Net_TCP_ClientServer_Simple", (ApCallback) Test_Net_TCP_ClientServer_Simple_ServerStopped, 0); }

  return ok;
}

//----------------------------
// ClientServer test using netapi:

static const String g_Test_Net_TCP_ClientServer_sExpectedData = "\n \n Test_Net_TCP_ClientServer_sExpectedData \r\n \r\n";
static String g_Test_Net_TCP_ClientServer_sError = "";

static const String g_Test_Net_TCP_ClientServer_sAddress = "localhost";
static const int g_Test_Net_TCP_ClientServer_nPort = 9754;

//----------------------------

class Test_Net_TCP_ClientServer_ClientConnection;
class Test_Net_TCP_ClientServer_ServerConnection;

class Test_Net_TCP_ClientServer_TCPServer : public Apollo::TCPServer
{
public:
  static Test_Net_TCP_ClientServer_TCPServer* Get();
  static void Delete();
protected:
  Test_Net_TCP_ClientServer_TCPServer() : TCPServer(), pClientConnection_(NULL), pServerConnection_(NULL), bDidStart_(0)
    , bClientConnected_(0), bClientConnectionAccepted_(0), bClientClosed_(0), bClientConnectionClosed_(0), bClientDeleted_(0)
    , bClientConnectionDeleted_(0) 
  {}
  virtual ~Test_Net_TCP_ClientServer_TCPServer() 
  {
    if (pClientConnection_ != 0) { delete pClientConnection_; pClientConnection_ = 0; }
    if (pServerConnection_ != 0) { delete pServerConnection_; pServerConnection_ = 0; }
  }
public:
  void OnClientConnected() { bClientConnected_ = 1; }
  void OnClientConnectionAccepted() { bClientConnectionAccepted_ = 1; }
  void OnClientDataIn(String& sData) { sReceivedData_ = sData; }
  void OnClientClosed() { bClientClosed_ = 1; }
  void OnClientConnectionClosed() { bClientConnectionClosed_ = 1; }
  void OnClientDeleted() { bClientDeleted_ = 1; pClientConnection_ = NULL; }
  void OnClientConnectionDeleted() { bClientConnectionDeleted_ = 1; pServerConnection_ = 0; }
protected:
  virtual Apollo::TCPConnection* OnConnectionAccepted(String& sName, const ApHandle& hConnection, String& sClientAddress, int nClientPort);
  virtual int OnStarted();
  virtual int OnStopped();
protected:
  int bDidStart_;

  String sReceivedData_;
  int bClientConnected_;
  int bClientConnectionAccepted_;
  int bClientClosed_;
  int bClientConnectionClosed_;
  int bClientDeleted_;
  int bClientConnectionDeleted_;

protected:
  Test_Net_TCP_ClientServer_ClientConnection* pClientConnection_;
  Test_Net_TCP_ClientServer_ServerConnection* pServerConnection_;
  static Test_Net_TCP_ClientServer_TCPServer* pInstance_;
};

class Test_Net_TCP_ClientServer_ServerConnection : public Apollo::TCPConnection
{
public:
  Test_Net_TCP_ClientServer_ServerConnection(const char* szName, const ApHandle& hServerConnection);
  virtual ~Test_Net_TCP_ClientServer_ServerConnection();

protected:
  int OnConnected();
  int OnDataIn(unsigned char *pData, size_t nLen);
  int OnClosed();

};

class Test_Net_TCP_ClientServer_ClientConnection : public Apollo::TCPConnection
{
public:
  Test_Net_TCP_ClientServer_ClientConnection(const char* szName);
  virtual ~Test_Net_TCP_ClientServer_ClientConnection();
protected:
  virtual int OnConnected();
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();
};


Test_Net_TCP_ClientServer_TCPServer* Test_Net_TCP_ClientServer_TCPServer::pInstance_ = NULL;

Test_Net_TCP_ClientServer_TCPServer* Test_Net_TCP_ClientServer_TCPServer::Get()
{
  if (pInstance_ == NULL) {
    pInstance_ = new Test_Net_TCP_ClientServer_TCPServer();
  }
  return pInstance_;
}

void Test_Net_TCP_ClientServer_TCPServer::Delete()
{
  if (pInstance_ != 0) {
    delete pInstance_; pInstance_ = 0;
  }
}

Apollo::TCPConnection* Test_Net_TCP_ClientServer_TCPServer::OnConnectionAccepted(String& sName
                                                                      , const ApHandle& hConnection
                                                                      , String& sClientAddress
                                                                      , int nClientPort)
{
  if (pServerConnection_ != 0) {
    AP_DEBUG_BREAK();
    return NULL;
  }
  pServerConnection_ = new Test_Net_TCP_ClientServer_ServerConnection(sName, hConnection);
  return pServerConnection_;
}

int Test_Net_TCP_ClientServer_TCPServer::OnStarted()
{
  int ok = 1;

  bDidStart_ = 1;

  if (pClientConnection_ != 0) {
    ok = 0;
    AP_DEBUG_BREAK();
  }

  if (ok) {
    ok = 0;
    pClientConnection_ = new Test_Net_TCP_ClientServer_ClientConnection("Test_Net_TCP_ClientServer_ClientConnection");
    if (pClientConnection_ != 0) {
      ok = pClientConnection_->Connect(sAddress_, nPort_);
    }
  }

  return ok;
}

int Test_Net_TCP_ClientServer_TCPServer::OnStopped()
{
  int ok = 1;

  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bDidStart_) { g_Test_Net_TCP_ClientServer_sError = "Failed to start server"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bClientConnected_) { g_Test_Net_TCP_ClientServer_sError = "Missing client connected"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bClientConnectionAccepted_) { g_Test_Net_TCP_ClientServer_sError = "Missing client connection accepted"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if ((String)g_Test_Net_TCP_ClientServer_sExpectedData != sReceivedData_) { g_Test_Net_TCP_ClientServer_sError = "Incoming data mismatch"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bClientClosed_) { g_Test_Net_TCP_ClientServer_sError = "Missing client closed"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bClientConnectionClosed_) { g_Test_Net_TCP_ClientServer_sError = "Missing clientconnection closed"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bClientDeleted_) { g_Test_Net_TCP_ClientServer_sError = "Missing client deleted"; }
  }
  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    if (!bClientConnectionDeleted_) { g_Test_Net_TCP_ClientServer_sError = "Missing clientconnection deleted"; }
  }

  // -- Evaluate

  if (g_Test_Net_TCP_ClientServer_sError.empty()) {
    AP_UNITTEST_SUCCESS(Test_Net_TCP_ClientServer);
  } else {
    AP_UNITTEST_FAILED(Test_Net_TCP_ClientServer, (const char*)g_Test_Net_TCP_ClientServer_sError);
  }

  return ok;
}


Test_Net_TCP_ClientServer_ClientConnection::Test_Net_TCP_ClientServer_ClientConnection(const char* szName) 
: Apollo::TCPConnection(szName) 
{
}
  
Test_Net_TCP_ClientServer_ClientConnection::~Test_Net_TCP_ClientServer_ClientConnection() 
{
  Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientDeleted();
}

int Test_Net_TCP_ClientServer_ClientConnection::OnConnected()
{
  int ok = 1;

  Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientConnected();
  
  return ok;
}

int Test_Net_TCP_ClientServer_ClientConnection::OnDataIn(unsigned char *pData, size_t nLen)
{
  int ok = 1;
  
  String sData; sData.append((const char*)pData, nLen, 1);
  Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientDataIn(sData);

  ok = Close();

  return ok;
}

int Test_Net_TCP_ClientServer_ClientConnection::OnClosed()
{
  int ok = 1;

  Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientClosed();
  AutoDelete(1);

  return ok;
}

Test_Net_TCP_ClientServer_ServerConnection::Test_Net_TCP_ClientServer_ServerConnection(const char* szName
                                                                                     , const ApHandle& hServerConnection)

: Apollo::TCPConnection(szName, hServerConnection)
{
}

Test_Net_TCP_ClientServer_ServerConnection::~Test_Net_TCP_ClientServer_ServerConnection()
{
  Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientConnectionDeleted();
}

int Test_Net_TCP_ClientServer_ServerConnection::OnConnected()
{
  int ok = 1;

  if (ok) {
    Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientConnectionAccepted();
    String sData = g_Test_Net_TCP_ClientServer_sExpectedData;
    unsigned char* pData = (unsigned char*) (const char*) sData;
    size_t nDataLen = ((String)g_Test_Net_TCP_ClientServer_sExpectedData).bytes();
    ok = DataOut(pData, nDataLen);
  }

  // We don't close here, this way we test connection close by client
  //Close();

  return ok;
}

int Test_Net_TCP_ClientServer_ServerConnection::OnDataIn(unsigned char *pData, size_t nLen) 
{
  int ok = 1;
  return ok; 
}

int Test_Net_TCP_ClientServer_ServerConnection::OnClosed()
{
  int ok = 1;

  Test_Net_TCP_ClientServer_TCPServer::Get()->OnClientConnectionClosed();
  Test_Net_TCP_ClientServer_TCPServer::Get()->Stop();
  AutoDelete(1);

  return ok;
}

int Test_Begin_Net_TCP_ClientServer()
{
  int ok = 1;

  Test_Net_TCP_ClientServer_TCPServer* pServer = Test_Net_TCP_ClientServer_TCPServer::Get();
  if (pServer != 0) {
    if (! pServer->Start(g_Test_Net_TCP_ClientServer_sAddress, g_Test_Net_TCP_ClientServer_nPort)) {
      AP_UNITTEST_FAILED(Test_Net_TCP_ClientServer, "Failed to request server start");
    }
  } else {
    AP_UNITTEST_FAILED(Test_Net_TCP_ClientServer, "new server failed");
  }

  return ok;
}

int Test_End_Net_TCP_ClientServer()
{
  int ok = 1;

  if (Test_Net_TCP_ClientServer_TCPServer::Get() != 0) {
    Test_Net_TCP_ClientServer_TCPServer::Delete();
  }

  return ok;
}


//----------------------------
// Test TCP Simple

static ApHandle hTest_Net_TCP_Simple = ApNoHandle;
static int bTest_Net_TCP_Simple_Connected = 0;
static int bTest_Net_TCP_Simple_Closed = 0;
static int bTest_Net_TCP_Simple_DataIn = 0;

int Test_Net_TCP_Simple_On_TCP_Connected(Msg_Net_TCP_Connected* pMsg)
{
  if (pMsg->hConnection == hTest_Net_TCP_Simple) {
    bTest_Net_TCP_Simple_Connected = 1;
    AP_UNITTEST_SUCCESS(Test_Net_TCP_Simple_Connected);

    if (!pMsg->bSuccess) {
      apLog_Warning((LOG_CHANNEL, "Test_Net_TCP_Simple_On_TCP_Connected", "Not connected " ApHandleFormat ": %s", ApHandleType(pMsg->hConnection), StringType(pMsg->sComment)));
    } else {
      const char* pBuf = "GET / HTTP/1.0\r\nConnection: close\r\n\r\n";
      Msg_Net_TCP_DataOut msg;
      msg.hConnection = pMsg->hConnection;
      msg.sbData.SetData((const unsigned char*) pBuf, ::strlen(pBuf));
      msg.Request();
    }
  }

  return 1;
}

int Test_Net_TCP_Simple_On_TCP_Closed(Msg_Net_TCP_Closed* pMsg)
{
  if (pMsg->hConnection == hTest_Net_TCP_Simple) {
    bTest_Net_TCP_Simple_Closed = 1;
    AP_UNITTEST_SUCCESS(Test_Net_TCP_Simple_Closed);
  }

  return 1;
}

int Test_Net_TCP_Simple_On_TCP_DataIn(Msg_Net_TCP_DataIn* pMsg)
{
  if (pMsg->hConnection == hTest_Net_TCP_Simple) {
    bTest_Net_TCP_Simple_DataIn = 1;
    AP_UNITTEST_SUCCESS(Test_Net_TCP_Simple_DataIn);
  }

  return 1;
}

int Test_Begin_Net_TCP_Simple()
{
  int ok = 1;

  { Msg_Net_TCP_Connected msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_TCP_Simple_On_TCP_Connected, 0, 0); }
  { Msg_Net_TCP_DataIn msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_TCP_Simple_On_TCP_DataIn, 0, 0); }
  { Msg_Net_TCP_Closed msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_TCP_Simple_On_TCP_Closed, 0, 0); }

  {
    Msg_Net_TCP_Connect msg;
    msg.sHost = "www.heise.de";
    msg.nPort = 80;
    msg.hConnection = hTest_Net_TCP_Simple = Apollo::newHandle();
    msg.Request();
  }

  return ok;
}

int Test_End_Net_TCP_Simple()
{
  int ok = 1;

  { Msg_Net_TCP_Connected msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_TCP_Simple_On_TCP_Connected, 0); }
  { Msg_Net_TCP_DataIn msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_TCP_Simple_On_TCP_DataIn, 0); }
  { Msg_Net_TCP_Closed msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_TCP_Simple_On_TCP_Closed, 0); }

  return ok;
}

//----------------------------------------------------------
// TCPConnection SendRecvCloseAutoDelete

class Test_TCPConnection_SendRecvCloseAutoDelete : public Apollo::TCPConnection
{
public:
  Test_TCPConnection_SendRecvCloseAutoDelete()
    :Apollo::TCPConnection("Test_TCPConnection_SendRecvCloseAutoDelete")
  {}
  virtual ~Test_TCPConnection_SendRecvCloseAutoDelete();
  virtual int OnConnected();
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();
};

int Test_TCPConnection_SendRecvCloseAutoDelete::OnConnected()
{
  const char* pBuf = "GET /xx HTTP/1.0\r\nConnection: close\r\n\r\n";
  DataOut((unsigned char*) pBuf, ::strlen(pBuf));
  return 1;
}

int Test_TCPConnection_SendRecvCloseAutoDelete::OnDataIn(unsigned char* pData, size_t nLen)
{
  AP_UNUSED_ARG(pData);
  AP_UNUSED_ARG(nLen);
  Close();
  return 1;
}

static int g_bTest_TCPConnection_SendRecvCloseAutoDelete_Closed = 0;
int Test_TCPConnection_SendRecvCloseAutoDelete::OnClosed()
{
  g_bTest_TCPConnection_SendRecvCloseAutoDelete_Closed = 1;
  AutoDelete(1);
  return 1;
}

Test_TCPConnection_SendRecvCloseAutoDelete::~Test_TCPConnection_SendRecvCloseAutoDelete()
{
  if (g_bTest_TCPConnection_SendRecvCloseAutoDelete_Closed) {
    AP_UNITTEST_SUCCESS(Test_Net_TCPConnection_SendRecvCloseAutoDelete);
  }
}

int Test_Begin_Net_TCPConnection_SendRecvCloseAutoDelete()
{
  int ok = 0;
  Test_TCPConnection_SendRecvCloseAutoDelete* p = new Test_TCPConnection_SendRecvCloseAutoDelete();
  if (p != 0) {
    ok = p->Connect("www.virtual-presence.org", 80);
  }
  return ok;
}

//----------------------------------------------------------
// TCPConnection ConnectedCloseAutoDelete

class Test_TCPConnection_ConnectedCloseAutoDelete : public Apollo::TCPConnection
{
public:
  Test_TCPConnection_ConnectedCloseAutoDelete()
    :Apollo::TCPConnection("Test_TCPConnection_ConnectedCloseAutoDelete")
  {}
  virtual ~Test_TCPConnection_ConnectedCloseAutoDelete();
  virtual int OnConnected();
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();
};

int Test_TCPConnection_ConnectedCloseAutoDelete::OnConnected()
{
  Close();
  return 1;
}

int Test_TCPConnection_ConnectedCloseAutoDelete::OnDataIn(unsigned char* pData, size_t nLen)
{
  AP_UNUSED_ARG(pData);
  AP_UNUSED_ARG(nLen);
  return 1;
}

static int g_bTest_TCPConnection_ConnectedCloseAutoDelete_Closed = 0;
int Test_TCPConnection_ConnectedCloseAutoDelete::OnClosed()
{
  g_bTest_TCPConnection_ConnectedCloseAutoDelete_Closed = 1;
  AutoDelete(1);
  return 1;
}

Test_TCPConnection_ConnectedCloseAutoDelete::~Test_TCPConnection_ConnectedCloseAutoDelete()
{
  if (g_bTest_TCPConnection_ConnectedCloseAutoDelete_Closed) {
    AP_UNITTEST_SUCCESS(Test_Net_TCPConnection_CloseAutoDelete);
  }
}

int Test_Begin_Net_TCPConnection_CloseAutoDelete()
{
  int ok = 0;
  Test_TCPConnection_ConnectedCloseAutoDelete* p = new Test_TCPConnection_ConnectedCloseAutoDelete();
  if (p != 0) {
    ok = p->Connect("www.virtual-presence.org", 80);
  }
  return ok;
}

#endif // defined(AP_TEST_Net_TCP)

//----------------------------------------------------------
// TCPConnection CheckIncomingData

#if defined(AP_TEST_Net_TCP) && defined(AP_TEST_Net_HTTP)

class Test_CompareTCPHTTPData_TCPConnection : public Apollo::TCPConnection
{
public:
  Test_CompareTCPHTTPData_TCPConnection()
    :Apollo::TCPConnection("Test_CompareTCPHTTPData_TCPConnection")
  {}
  virtual int OnConnected();
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();
};

class Test_CompareTCPHTTPData_HTTPClient : public Apollo::HTTPClient
{
public:
  Test_CompareTCPHTTPData_HTTPClient() : Apollo::HTTPClient("Test_CompareTCPHTTPData_HTTPClient") {}
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnClosed();
};

int Test_CompareTCPHTTPData_TCPConnection::OnConnected()
{
  const char* pBuf = "GET / HTTP/1.0\r\nHost: www.virtual-presence.org\r\nConnection: close\r\n\r\n";
  DataOut((unsigned char*) pBuf, ::strlen(pBuf));
  return 1;
}

static int g_bTest_CompareTCPHTTPData_TCPConnection_Closed = 0;
static int g_bTest_CompareTCPHTTPData_HTTPClient_Closed = 0;
static Buffer g_sbTest_CompareTCPHTTPData_TCPConnection;
static Buffer g_sbTest_CompareTCPHTTPData_HTTPClient;

int Test_CompareTCPHTTPData_TCPConnection::OnDataIn(unsigned char* pData, size_t nLen)
{
  g_sbTest_CompareTCPHTTPData_TCPConnection.Append(pData, nLen);
  return 1;
}

int Test_CompareTCPHTTPData_HTTPClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  g_sbTest_CompareTCPHTTPData_HTTPClient.Append(pData, nLen);
  return 1;
}

static int Test_CompareTCPHTTPData_BothClosed()
{
  int ok = 0;

  size_t nTCP = 0;
  size_t nHTTP = 0;

  const unsigned char* pTCP = g_sbTest_CompareTCPHTTPData_TCPConnection.Data();
  if (pTCP != 0) {
    pTCP = (const unsigned char*) ::strstr((const char *) pTCP, "<html>");
    nTCP = g_sbTest_CompareTCPHTTPData_TCPConnection.Length() - (pTCP - g_sbTest_CompareTCPHTTPData_TCPConnection.Data());
  }

  const unsigned char* pHTTP = g_sbTest_CompareTCPHTTPData_HTTPClient.Data();
  nHTTP = g_sbTest_CompareTCPHTTPData_HTTPClient.Length();

  String s;
  if (pTCP != 0 && pHTTP != 0 && nTCP != 0 && nHTTP != 0 && nTCP == nHTTP) {
    ok = 1;
    for (unsigned int i = 0; i < nTCP; i++) {
      ok = pTCP[i] == pHTTP[i];
      if (!ok) {
        break;
      }
    }
  }

  if (!ok && pTCP != 0 && pHTTP != 0) {
    int nTCPLen = nTCP < 100 ? nTCP : 100;
    int nHTTPLen = nHTTP < 100 ? nHTTP : 100;
    String sTCP1; sTCP1.set((const char*) pTCP, nTCPLen);
    String sHTTP1; sHTTP1.set((const char*) pHTTP, nHTTPLen);
    String sTCP2; sTCP2.set((const char*) pTCP + nTCP - nTCPLen, nTCPLen);
    String sHTTP2; sHTTP2.set((const char*) pHTTP + nHTTP - nHTTPLen, nHTTPLen);
    s.appendf("TCP=%s...%s HTTP=%s...%s", StringType(sTCP1), StringType(sTCP2), StringType(sHTTP1), StringType(sHTTP2));
  } else {
    s.appendf("pTCP=%8x pHTTP=%8x nTCP=%d nHTTP=%d", pTCP, pHTTP, nTCP, nHTTP);
  }

  AP_UNITTEST_RESULT(Test_Net_CompareTCPHTTPData, ok, s);

  return 1;
}

int Test_CompareTCPHTTPData_TCPConnection::OnClosed()
{
  g_bTest_CompareTCPHTTPData_TCPConnection_Closed = 1;
  AutoDelete(1);
  if (g_bTest_CompareTCPHTTPData_TCPConnection_Closed && g_bTest_CompareTCPHTTPData_HTTPClient_Closed) {
    Test_CompareTCPHTTPData_BothClosed();
  }
  return 1;
}

int Test_CompareTCPHTTPData_HTTPClient::OnClosed()
{
  g_bTest_CompareTCPHTTPData_HTTPClient_Closed = 1;
  AutoDelete(1);
  if (g_bTest_CompareTCPHTTPData_TCPConnection_Closed && g_bTest_CompareTCPHTTPData_HTTPClient_Closed) {
    Test_CompareTCPHTTPData_BothClosed();
  }
  return 1;
}

int Test_Begin_Net_CompareTCPHTTPData()
{
  {
    int ok = 0;
    Test_CompareTCPHTTPData_TCPConnection* p = new Test_CompareTCPHTTPData_TCPConnection();
    if (p != 0) {
      ok = p->Connect("www.virtual-presence.org", 80);
    }
  }
  {
    int ok = 0;
    Test_CompareTCPHTTPData_HTTPClient* p = new Test_CompareTCPHTTPData_HTTPClient();
    if (p != 0) {
      ok = p->Get("http://www.virtual-presence.org/");
    }
  }
  return 1;
}

#endif // defined(AP_TEST_Net_HTTP) && defined(AP_TEST_Net_TCP)

//----------------------------------------------------------

#if defined(AP_TEST_Net_HTTP)

class Test_Net_HTTP_Data : public Elem
{
public:
  Test_Net_HTTP_Data(const ApHandle& hAp, const char* szUrl)
    :sUrl_(szUrl)
    ,bConnected_(0)
    ,nHeader_(0)
    ,nDataIn_(0)
    ,bClosed_(0)
    ,bFailed_(0)
    ,hAp_(hAp)
  {}
  inline ApHandle apHandle() { return hAp_; }
  String sUrl_;
  int bConnected_;
  int nHeader_;
  int nDataIn_;
  int bClosed_;
  int bFailed_;
  ApHandle hAp_;
};

static List lTest_Net_HTTP_DataList;
static int bTest_Net_HTTP_Failed_Done = 0;

Test_Net_HTTP_Data* Test_Net_HTTP_Data_FindByHandle(const ApHandle& h)
{
  Test_Net_HTTP_Data* pResult = 0;
  for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
    if (e->apHandle() == h) {
      pResult = e;
      break;
    }
  }
  return pResult;
}

int Test_Net_HTTP_OnConnected(Msg_Net_HTTP_Connected* pMsg)
{
  Test_Net_HTTP_Data* d = Test_Net_HTTP_Data_FindByHandle(pMsg->hClient);
  if (d != 0) {
    d->bConnected_ = 1;

    {
      unsigned int n = 0;
      for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
        if (e->bConnected_) {
          n++;
        }
      }
      if (n == lTest_Net_HTTP_DataList.length()) AP_UNITTEST_SUCCESS(Test_Net_HTTP_Connected);
    }
  }

  return 1;
}

int Test_Net_HTTP_OnHeader(Msg_Net_HTTP_Header* pMsg)
{
  Test_Net_HTTP_Data* d = Test_Net_HTTP_Data_FindByHandle(pMsg->hClient);
  if (d != 0) {
    d->nHeader_ = pMsg->kvHeader.length();

    {
      unsigned int n = 0;
      for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
        if (e->nHeader_) {
          n++;
        }
      }
      if (n == lTest_Net_HTTP_DataList.length()) AP_UNITTEST_SUCCESS(Test_Net_HTTP_Header);
    }
  }

  return 1;
}

int Test_Net_HTTP_OnDataIn(Msg_Net_HTTP_DataIn* pMsg)
{
  Test_Net_HTTP_Data* d = Test_Net_HTTP_Data_FindByHandle(pMsg->hClient);
  if (d != 0) {
    d->nDataIn_ += pMsg->sbData.Length();

    {
      unsigned int n = 0;
      for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
        if (e->nDataIn_ > 0) {
          n++;
        }
      }
      if (n == lTest_Net_HTTP_DataList.length()) AP_UNITTEST_SUCCESS(Test_Net_HTTP_DataIn);
    }
  }

  return 1;
}

int Test_Net_HTTP_OnFailed(Msg_Net_HTTP_Failed* pMsg)
{
  Test_Net_HTTP_Data* d = Test_Net_HTTP_Data_FindByHandle(pMsg->hClient);
  if (d != 0) {
    d->bFailed_ = 1;
  }

  return 1;
}

int Test_Net_HTTP_OnClosed(Msg_Net_HTTP_Closed* pMsg)
{
  Test_Net_HTTP_Data* d = Test_Net_HTTP_Data_FindByHandle(pMsg->hClient);
  if (d != 0) {
    d->bClosed_ = 1;

    {
      unsigned int n = 0;
      for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
        if (e->bClosed_) {
          n++;
        }
      }
      if (n == lTest_Net_HTTP_DataList.length()) AP_UNITTEST_SUCCESS(Test_Net_HTTP_Closed);
    }

    {
      unsigned int n = 0;
      for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
        if (!e->bFailed_) {
          n++;
        }
      }
      if (n == lTest_Net_HTTP_DataList.length() && !bTest_Net_HTTP_Failed_Done) {
        AP_UNITTEST_SUCCESS(Test_Net_HTTP_Failed);
        bTest_Net_HTTP_Failed_Done = 1;
      }
    }
  }

  return 1;
}

int Test_Begin_Net_HTTP()
{
  int ok = 1;

  { Msg_Net_HTTP_Connected msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnConnected, 0, 0); }
  { Msg_Net_HTTP_Header msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnHeader, 0, 0); }
  { Msg_Net_HTTP_DataIn msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnDataIn, 0, 0); }
  { Msg_Net_HTTP_Failed msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnFailed, 0, 0); }
  { Msg_Net_HTTP_Closed msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnClosed, 0, 0); }

  lTest_Net_HTTP_DataList.AddLast(new Test_Net_HTTP_Data(Apollo::newHandle(), "http://www.heise.de/"));
  lTest_Net_HTTP_DataList.AddLast(new Test_Net_HTTP_Data(Apollo::newHandle(), "http://www.heise.de/_vpi.xml"));
  lTest_Net_HTTP_DataList.AddLast(new Test_Net_HTTP_Data(Apollo::newHandle(), "http://www.virtual-presence.org/"));
  lTest_Net_HTTP_DataList.AddLast(new Test_Net_HTTP_Data(Apollo::newHandle(), "http://blog.wolfspelz.de/"));
  lTest_Net_HTTP_DataList.AddLast(new Test_Net_HTTP_Data(Apollo::newHandle(), "https://ssl.google-analytics.com/urchin.js"));

  // sometimes google does a redirect to https based on my cookie
  //lTest_Net_HTTP_DataList.AddLast(new Test_Net_HTTP_Data(Apollo::newHandle(), "http://www.google.de/ig?hl=de"));

  for (Test_Net_HTTP_Data* e = 0; (e = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.Next(e)) != 0; ) {
    Msg_Net_HTTP_Request msg;
    msg.hClient = e->apHandle();
    msg.sUrl = e->sUrl_;
    msg.Request();
  }

  //Msg_Net_HTTP_CancelAll msg;
  //msg.Request();

  return ok;
}

int Test_End_Net_HTTP()
{
  int ok = 1;

  Test_Net_HTTP_Data* d = 0;
  while ((d = (Test_Net_HTTP_Data*) lTest_Net_HTTP_DataList.First()) != 0) {
    lTest_Net_HTTP_DataList.Remove(d);
    delete d;
  }

  { Msg_Net_HTTP_Connected msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnConnected, 0); }
  { Msg_Net_HTTP_Header msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnHeader, 0); }
  { Msg_Net_HTTP_DataIn msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnDataIn, 0); }
  { Msg_Net_HTTP_Failed msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnFailed, 0); }
  { Msg_Net_HTTP_Closed msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Net_HTTP_OnClosed, 0); }

  return ok;
}

//----------------------------------------------------------
// HTTPClient

class Test_HTTPClient : public Apollo::HTTPClient
{
public:
  Test_HTTPClient()
    :Apollo::HTTPClient("Test_HTTPClient") 
    ,nStatus_(0)
    ,bFailed_(0)
  {}
  virtual int OnConnected();
  virtual int OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();

  int nStatus_;
  Buffer sbData_;
  List lHeader_;
  int bFailed_;
};

class Test_HTTPClient_GET : public Test_HTTPClient
{
public:
};

class Test_HTTPClient_POST : public Test_HTTPClient
{
public:
};

class Test_HTTPClient_Cancel1GET : public Test_HTTPClient
{
public:
  virtual int OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders);
};

int Test_HTTPClient_Cancel1GET::OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders)
{
  AP_UNUSED_ARG(kvHeaders);
  nStatus_ = nStatus;
  return 1;
}

class Test_HTTPClient_Cancel2GET : public Test_HTTPClient
{
public:
  Test_HTTPClient_Cancel2GET()
    :nFirstCallbackLen_(0)
  {}
  size_t nFirstCallbackLen_;
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
};

int Test_HTTPClient_Cancel2GET::OnDataIn(unsigned char* pData, size_t nLen)
{
  if (nFirstCallbackLen_ == 0) {
    nFirstCallbackLen_ = nLen;
  }
  sbData_.Append(pData, nLen);
  Cancel();
  return 1;
}

static Test_HTTPClient_GET* pTest_Net_HTTPClient_GET = 0;
static Test_HTTPClient_POST* pTest_Net_HTTPClient_POST = 0;
static Test_HTTPClient_Cancel1GET* pTest_Net_HTTPClient_Cancel1GET = 0;
static Test_HTTPClient_Cancel2GET* pTest_Net_HTTPClient_Cancel2GET = 0;

int Test_HTTPClient::OnConnected()
{
  return 1;
}

int Test_HTTPClient::OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders)
{
  nStatus_ = nStatus;

  for (Apollo::KeyValueElem* e = 0; (e = kvHeaders.nextElem(e)) != 0; ) {
    lHeader_.AddLast(String::toLower(e->getKey()), e->getString());
  }

  return 1;
}

int Test_HTTPClient::OnDataIn(unsigned char* pData, size_t nLen)
{
  sbData_.Append(pData, nLen);
  return 1;
}

int Test_HTTPClient::OnFailed(const char* szMessage)
{
  String sMessage = szMessage;
  bFailed_ = 1;
  return 1;
}

int Test_HTTPClient::OnClosed()
{
  if (this == pTest_Net_HTTPClient_GET) {
    int ok = 1;
    if (ok) ok = (pTest_Net_HTTPClient_GET->nStatus_ == 200);
    if (ok) ok = (pTest_Net_HTTPClient_GET->lHeader_.length() > 0);
    if (ok) ok = (pTest_Net_HTTPClient_GET->lHeader_["content-type"].getString().contains("text/html"));
    if (ok) ok = (pTest_Net_HTTPClient_GET->sbData_.Length() > 0);
    if (ok) ok = (pTest_Net_HTTPClient_GET->bFailed_ == 0);
    if (ok) AP_UNITTEST_SUCCESS(Test_Net_HTTPClient_GET);
  }

  if (this == pTest_Net_HTTPClient_POST) {
    int ok = 1;
    if (ok) ok = (pTest_Net_HTTPClient_POST->nStatus_ == 200);
    if (ok) ok = (pTest_Net_HTTPClient_POST->lHeader_.length() > 0);
    if (ok) ok = (pTest_Net_HTTPClient_POST->lHeader_["content-type"].getString().contains("text/html"));
    if (ok) ok = (pTest_Net_HTTPClient_POST->sbData_.Length() > 0);
    if (ok) ok = (pTest_Net_HTTPClient_POST->bFailed_ == 0);
    if (ok) AP_UNITTEST_SUCCESS(Test_Net_HTTPClient_POST);
  }

  if (this == pTest_Net_HTTPClient_Cancel1GET) {
    int ok = 1;
    if (ok) ok = (pTest_Net_HTTPClient_Cancel1GET->nStatus_ == 0);
    if (ok) ok = (pTest_Net_HTTPClient_Cancel1GET->sbData_.Length() == 0);
    if (ok) AP_UNITTEST_SUCCESS(Test_Net_HTTPClient_Cancel1GET);
  }

  if (this == pTest_Net_HTTPClient_Cancel2GET) {
    int ok = 1;
    if (ok) ok = (pTest_Net_HTTPClient_Cancel2GET->nFirstCallbackLen_ == pTest_Net_HTTPClient_Cancel2GET->sbData_.Length());
    if (ok) AP_UNITTEST_SUCCESS(Test_Net_HTTPClient_Cancel2GET);
  }

  return 1;
}

// ------------------------------

class Test_HTTPClient_AutoDelete : public Apollo::HTTPClient
{
public:
  Test_HTTPClient_AutoDelete() : Apollo::HTTPClient("Test_HTTPClient_AutoDelete") {}
  virtual ~Test_HTTPClient_AutoDelete();
  virtual int OnConnected();
  virtual int OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();
};

int Test_HTTPClient_AutoDelete::OnConnected()
{
  int ok = 1;
  return ok;
}

int Test_HTTPClient_AutoDelete::OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders)
{
  AP_UNUSED_ARG(nStatus);
  AP_UNUSED_ARG(kvHeaders);
  int ok = 1;
  return ok;
}

int Test_HTTPClient_AutoDelete::OnDataIn(unsigned char* pData, size_t nLen)
{
  AP_UNUSED_ARG(pData);
  AP_UNUSED_ARG(nLen);
  int ok = 1;
  return ok;
}

int Test_HTTPClient_AutoDelete::OnFailed(const char* szMessage)
{
  int ok = 1;
  String sMessage = szMessage;
  return ok;
}

static int g_bTest_HTTPClient_AutoDelete_Closed = 0;
int Test_HTTPClient_AutoDelete::OnClosed()
{
  int ok = 1;
  g_bTest_HTTPClient_AutoDelete_Closed = 1;
  AutoDelete(1);
  return ok;
}

Test_HTTPClient_AutoDelete::~Test_HTTPClient_AutoDelete()
{
  if (g_bTest_HTTPClient_AutoDelete_Closed) {
    AP_UNITTEST_SUCCESS(Test_Net_HTTPClient_AutoDelete);
  }
}

class Test_HTTPClient_CancelAutoDelete : public Apollo::HTTPClient
{
public:
  Test_HTTPClient_CancelAutoDelete() : Apollo::HTTPClient("Test_HTTPClient") {}
  virtual ~Test_HTTPClient_CancelAutoDelete();
  virtual int OnConnected();
  virtual int OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();
};

int Test_HTTPClient_CancelAutoDelete::OnConnected()
{
  int ok = 1;
  return ok;
}

int Test_HTTPClient_CancelAutoDelete::OnHeader(int nStatus, Apollo::KeyValueList& kvHeaders)
{
  AP_UNUSED_ARG(nStatus);
  AP_UNUSED_ARG(kvHeaders);
  int ok = 1;
  Cancel();
  return ok;
}

int Test_HTTPClient_CancelAutoDelete::OnDataIn(unsigned char* pData, size_t nLen)
{
  AP_UNUSED_ARG(pData);
  AP_UNUSED_ARG(nLen);
  int ok = 1;
  return ok;
}

int Test_HTTPClient_CancelAutoDelete::OnFailed(const char* szMessage)
{
  int ok = 1;
  String sMessage = szMessage;
  return ok;
}

static int g_bTest_HTTPClient_CancelAutoDelete_Closed = 0;
int Test_HTTPClient_CancelAutoDelete::OnClosed()
{
  int ok = 1;
  g_bTest_HTTPClient_CancelAutoDelete_Closed = 1;
  AutoDelete(1);
  return ok;
}

Test_HTTPClient_CancelAutoDelete::~Test_HTTPClient_CancelAutoDelete()
{
  if (g_bTest_HTTPClient_CancelAutoDelete_Closed) {
    AP_UNITTEST_SUCCESS(Test_Net_HTTPClient_CancelAutoDelete);
  }
}

// ------------------------------

String Test_NetApi_BasicAuthorization()
{
  String s;

  Apollo::HTTPClient* c = new Apollo::HTTPClient("Test_NetApi_BasicAuthorization");
  if (c) {
    Apollo::KeyValueList kvHeaders;
    c->SetBasicAuthorization("User", "Password", kvHeaders);
    Apollo::KeyValueElem* e = kvHeaders.find("Authorization");
    if (e) {
      String sAuthorization = e->getString();
      if (sAuthorization != "Basic VXNlcjpQYXNzd29yZA==") { s = String::from(__LINE__); }
    } else {
      s = "Authorization header not found";
    }
  } else {
    s = "new Test_NetApi_BasicAuthorization_HTTPClient() failed";
  }

  return s;
}

int Test_Begin_Net_HTTPClient()
{
  int ok = 0;

  pTest_Net_HTTPClient_GET = new Test_HTTPClient_GET();
  if (pTest_Net_HTTPClient_GET != 0) {
    ok = pTest_Net_HTTPClient_GET->Get("http://www.virtual-presence.org/");
  }

  pTest_Net_HTTPClient_POST = new Test_HTTPClient_POST();
  if (pTest_Net_HTTPClient_POST != 0) {
    String sPostData = "a=b&b=c+d%20e";
    Buffer sbPostData;
    ok = pTest_Net_HTTPClient_POST->Post("http://www.virtual-presence.org/", (const unsigned char*) sPostData.c_str(), sPostData.bytes());
  }

  pTest_Net_HTTPClient_Cancel1GET = new Test_HTTPClient_Cancel1GET();
  if (pTest_Net_HTTPClient_Cancel1GET != 0) {
    ok = pTest_Net_HTTPClient_Cancel1GET->Get("http://www.virtual-presence.org/");
    if (ok) {
      pTest_Net_HTTPClient_Cancel1GET->Cancel();
    }
  }

  pTest_Net_HTTPClient_Cancel2GET = new Test_HTTPClient_Cancel2GET();
  if (pTest_Net_HTTPClient_Cancel2GET != 0) {
    ok = pTest_Net_HTTPClient_Cancel2GET->Get("http://www.virtual-presence.org/");
  }

  {
    Test_HTTPClient_AutoDelete* p = new Test_HTTPClient_AutoDelete();
    if (p != 0) {
      ok = p->Get("http://www.virtual-presence.org/");
    }
  }

  {
    Test_HTTPClient_CancelAutoDelete* p = new Test_HTTPClient_CancelAutoDelete();
    if (p != 0) {
      ok = p->Get("http://www.virtual-presence.org/");
    }
  }

  return ok;
}

int Test_End_Net_HTTPClient()
{
  int ok = 0;

  if (pTest_Net_HTTPClient_GET != 0) {
    delete pTest_Net_HTTPClient_GET;
    pTest_Net_HTTPClient_GET = 0;
  }
  if (pTest_Net_HTTPClient_POST != 0) {
    delete pTest_Net_HTTPClient_POST;
    pTest_Net_HTTPClient_POST = 0;
  }
  if (pTest_Net_HTTPClient_Cancel1GET != 0) {
    delete pTest_Net_HTTPClient_Cancel1GET;
    pTest_Net_HTTPClient_Cancel1GET = 0;
  }
  if (pTest_Net_HTTPClient_Cancel2GET != 0) {
    delete pTest_Net_HTTPClient_Cancel2GET;
    pTest_Net_HTTPClient_Cancel2GET = 0;
  }

  return ok;
}

#endif // AP_TEST_Net_HTTP

//----------------------------------------------------------

static int g_bTest_Vpi_Online = 0;

#endif // AP_TEST_Net

void Test_Net_Register()
{
#if defined(AP_TEST_Net)
  if (Apollo::isLoadedModule("Net")) {
    Msg_Net_IsOnline msg;
    if (msg.Request()) {
      if (msg.bIsOnline) {
        g_bTest_Vpi_Online = 1;
      }
    }
  }

  if (Apollo::isLoadedModule("Net") && g_bTest_Vpi_Online) {
    #if defined(AP_TEST_Net_TCP)
  	  #if defined(AP_TEST_Net_TCP_Advanced)
        AP_UNITTEST_REGISTER(Test_Net_TCPConnection_SendRecvCloseAutoDelete);
        AP_UNITTEST_REGISTER(Test_Net_TCPConnection_CloseAutoDelete);
        AP_UNITTEST_REGISTER(Test_Net_CompareTCPHTTPData);
        AP_UNITTEST_REGISTER(Test_Net_TCP_Connected);
        AP_UNITTEST_REGISTER(Test_Net_TCP_DataIn);
        AP_UNITTEST_REGISTER(Test_Net_TCP_Closed);
        AP_UNITTEST_REGISTER(Test_Net_TCP_ClientServer_Simple);
        AP_UNITTEST_REGISTER(Test_Net_TCP_ClientServer);
      #else
        AP_UNITTEST_REGISTER(Test_Net_TCP_Simple_Connected);
        AP_UNITTEST_REGISTER(Test_Net_TCP_Simple_Closed);
        AP_UNITTEST_REGISTER(Test_Net_TCP_Simple_DataIn);
      #endif
    #endif

    #if defined(AP_TEST_Net_HTTP)
      AP_UNITTEST_REGISTER(Test_Net_HTTP_Connected);
      AP_UNITTEST_REGISTER(Test_Net_HTTP_Header);
      AP_UNITTEST_REGISTER(Test_Net_HTTP_DataIn);
      AP_UNITTEST_REGISTER(Test_Net_HTTP_Closed);
      AP_UNITTEST_REGISTER(Test_Net_HTTP_Failed);

      AP_UNITTEST_REGISTER(Test_Net_HTTPClient_GET);
      AP_UNITTEST_REGISTER(Test_Net_HTTPClient_POST);
      AP_UNITTEST_REGISTER(Test_Net_HTTPClient_Cancel1GET);
      AP_UNITTEST_REGISTER(Test_Net_HTTPClient_Cancel2GET);
      AP_UNITTEST_REGISTER(Test_Net_HTTPClient_AutoDelete);
      AP_UNITTEST_REGISTER(Test_Net_HTTPClient_CancelAutoDelete);

      AP_UNITTEST_REGISTER(Test_NetApi_BasicAuthorization);
    #endif
  }
#endif // AP_TEST_Net
}

void Test_Net_Begin()
{
#if defined(AP_TEST_Net)
  if (Apollo::isLoadedModule("Net") && g_bTest_Vpi_Online) {
    #if defined(AP_TEST_Net_TCP)
  	  #if defined(AP_TEST_Net_TCP_Advanced)
        Test_Begin_Net_TCP();
        Test_Begin_Net_TCPConnection_SendRecvCloseAutoDelete();
        Test_Begin_Net_TCPConnection_CloseAutoDelete();
        Test_Begin_Net_TCP_ClientServer_Simple();
        Test_Begin_Net_TCP_ClientServer();
      #else
        Test_Begin_Net_TCP_Simple();
      #endif
    #endif
    #if defined(AP_TEST_Net_TCP) && defined(AP_TEST_Net_HTTP)
      Test_Begin_Net_CompareTCPHTTPData();
    #endif
    #if defined(AP_TEST_Net_HTTP)
      Test_Begin_Net_HTTP();
      Test_Begin_Net_HTTPClient();

      AP_UNITTEST_EXECUTE(Test_NetApi_BasicAuthorization);
    #endif
  }
#endif // AP_TEST_Net
}

void Test_Net_End()
{
#if defined(AP_TEST_Net)
  if (Apollo::isLoadedModule("Net") && g_bTest_Vpi_Online) {
    #if defined(AP_TEST_Net_TCP)
  	  #if defined(AP_TEST_Net_TCP_Advanced)
        Test_End_Net_TCP();
        Test_End_Net_TCP_ClientServer_Simple();
        Test_End_Net_TCP_ClientServer();
      #else
        Test_End_Net_TCP_Simple();
      #endif
    #endif

    #if defined(AP_TEST_Net_HTTP)
      Test_End_Net_HTTP();
      Test_End_Net_HTTPClient();
    #endif
  }
#endif // AP_TEST_Net
}
