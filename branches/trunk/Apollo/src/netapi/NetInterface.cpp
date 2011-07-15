// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "NetAPIModule.h"
#include "NetInterface.h"

#define LOG_CHANNEL "Net"
#define LOG_CONTEXT apLog_Context

AP_NAMESPACE_BEGIN

// -------------------------------------------------------------------

TCPServer::TCPServer()
:bRunning_(0)
,bCancelled_(0)
,hAp_(newHandle())
,nPort_(0)
{
  NetAPIModuleInstance::Get()->AddTCPServer(this);
}

TCPServer::~TCPServer()
{
  NetAPIModuleInstance::Get()->RemoveTCPServer(this);
}

int TCPServer::Start(const String& sAddress, int nPort)
{
  sAddress_ = sAddress;
  nPort_ = nPort;
  
  Msg_Net_TCP_Listen msg;
  msg.hServer = apHandle();
  msg.sAddress = sAddress_;
  msg.nPort = nPort_;

  int bOk = msg.Request();
  return bOk;
}

int TCPServer::Stop()
{
  int ok = 1;

  int bWasCancelled = bCancelled_;
  bCancelled_ = 1;

  if (bRunning_) {
    if (!bWasCancelled) {
      Msg_Net_TCP_ListenStop msg;
      msg.hServer = apHandle();
      ok = msg.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Net_TCP_ListenStop failed: " ApHandleFormat "", ApHandlePrintf(apHandle())));
      }
    }
  } else {
    Msg_Net_TCP_ListenStopped msg;
    msg.hServer = apHandle();
    msg.Send();
  }

  return ok;
}

int TCPServer::InternalListening()
{
  int nResult = OnStarted();
  bRunning_ = 1;
  return nResult;
}

int TCPServer::InternalListenStopped()
{
  bRunning_ = 0;
  return OnStopped();
}

int TCPServer::InternalConnectionAccepted(const ApHandle& hConnection, String& sClientAddress, int nClientPort)
{
  int ok = 0;

  String sName; sName.appendf("%s %d", _sz(sClientAddress), nClientPort);
  TCPConnection* pConnection = OnConnectionAccepted(sName, hConnection, sClientAddress, nClientPort);
  if (pConnection != 0) {
    ok = 1;
  } else {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "OnConnectionAccepted(" ApHandleFormat ", %s, %d) returned NULL", ApHandlePrintf(hConnection), _sz(sClientAddress), nClientPort));
  }

  if (ok) {
    pConnection->sAddress_ = sClientAddress;
    pConnection->nPort_ = nClientPort;
    ok = pConnection->InternalConnected();
  }

  return ok;
}

// -------------------------------------------------------------------

TCPConnection::TCPConnection(const char* szName)
:Elem(szName)
,hAp_(newHandle())
,nPort_(0)
,bOpen_(0)
,bCancelled_(0)
,bAutoDelete_(0)
{
  NetAPIModuleInstance::Get()->AddTCPConnection(this);
}

TCPConnection::TCPConnection(const char* szName, const ApHandle& hConnection)
:Elem(szName)
,hAp_(hConnection)
,nPort_(0)
,bOpen_(0)
,bCancelled_(0)
,bAutoDelete_(0)
{
  NetAPIModuleInstance::Get()->AddTCPConnection(this);
}

TCPConnection::~TCPConnection()
{
  NetAPIModuleInstance::Get()->RemoveTCPConnection(this);
}

int TCPConnection::Connect(const char* szAddress, int nPort)
{
  sAddress_ = szAddress;
  nPort_ = nPort;

  Msg_Net_TCP_Connect msg;
  msg.hConnection =  hAp_;
  msg.sHost = sAddress_;
  msg.nPort = nPort_;

  String sName;
  sName.appendf("%s %d", _sz(sAddress_), nPort_);
  setName(sName);

  int nHandlers = Apollo::callMsg(&msg);

  return msg.apStatus == ApMessage::Ok;
}

int TCPConnection::DataOut(unsigned char* pData, size_t nLen)
{
  Msg_Net_TCP_DataOut msg;
  msg.hConnection =  hAp_;
  msg.sbData.SetData(pData, nLen);
  int nHandlers = Apollo::callMsg(&msg);

  return msg.apStatus == ApMessage::Ok;
}

int TCPConnection::Close()
{
  int ok = 1;

  int bWasCancelled_ = bCancelled_;
  bCancelled_ = 1;

  if (bOpen_) {
    if (!bWasCancelled_) {
      Msg_Net_TCP_Close msg;
      msg.hConnection = hAp_;
      ok = Apollo::callMsg(&msg);
    }
  } else {
    Msg_Net_TCP_Closed msg;
    msg.hConnection = hAp_;
    ok = Apollo::callMsg(&msg);
  }

  return ok;
}

int TCPConnection::InternalConnected()
{
  bOpen_ = 1;
  return OnConnected();
}

int TCPConnection::InternalDataIn(unsigned char* pData, size_t nLen)
{
  return OnDataIn(pData, nLen);
}

int TCPConnection::InternalClosed()
{
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, ApHandleFormat " %s %d", ApHandlePrintf(hAp_), _sz(sAddress_), nPort_));

  bOpen_ = 0;
  int nResult = OnClosed();

  if (bAutoDelete_) {
    delete this;
  }

  return nResult;
}

// -------------------------------------------------------------------

KeyValueList HTTPClient::kvEmptyHeaders_;

HTTPClient::HTTPClient(const char* szReason)
:hAp_(newHandle())
,nMaxDataBytes_(100*1024)
,nMaxTimeSec_(100)
,bCancelled_(0)
,bAutoDelete_(0)
,sReason_(szReason)
{
  NetAPIModuleInstance::Get()->AddHTTPClient(this);
}

HTTPClient::~HTTPClient()
{
  NetAPIModuleInstance::Get()->RemoveHTTPClient(this);
}

int HTTPClient::Get(const char* szUrl, KeyValueList& kvHeaders)
{
  sUrl_ = szUrl;

  Msg_Net_HTTP_Request msg;
  msg.hClient =  hAp_;
  msg.sUrl = sUrl_;
  msg.sMethod = "GET";
  msg.kvHeader = kvHeaders;
  msg.sReason = sReason_;

  String sName;
  sName.appendf("%s %s %s", _sz(msg.sMethod), _sz(sUrl_), _sz(sReason_));
  setName(sName);

  int nHandlers = Apollo::callMsg(&msg);

  return msg.apStatus == ApMessage::Ok;
}

int HTTPClient::Post(const char* szUrl, const unsigned char* pData, size_t nLen, KeyValueList& kvHeaders)
{
  sUrl_ = szUrl;

  Msg_Net_HTTP_Request msg;
  msg.hClient = hAp_;
  msg.sUrl= sUrl_;
  msg.sMethod = "POST";
  msg.kvHeader = kvHeaders;
  msg.sbBody.SetData(pData, nLen);
  msg.sReason = sReason_;

  String sName;
  sName.appendf("%s %s %s", _sz(msg.sMethod), _sz(sUrl_), _sz(sReason_));
  setName(sName);
  
  int nHandlers = Apollo::callMsg(&msg);

  return msg.apStatus == ApMessage::Ok;
}

int HTTPClient::Cancel()
{
  bCancelled_ = 1;

  Msg_Net_HTTP_Cancel msg;
  msg.hClient = hAp_;
  int nHandlers = Apollo::callMsg(&msg);

  return msg.apStatus == ApMessage::Ok;
}

String HTTPClient::GetHeader(List& lHeader, const char* szName)
{
  Elem* e = lHeader.FindByNameCase(szName);
  if (e != 0) {
    return e->getString();
  }
  return "";
}

int HTTPClient::InternalConnected()
{
  return OnConnected();
}

int HTTPClient::InternalHeader(int nStatus, KeyValueList& kvHeaders)
{
  return OnHeader(nStatus, kvHeaders);
}

int HTTPClient::InternalDataIn(unsigned char* pData, size_t nLen)
{
  return OnDataIn(pData, nLen);
}

int HTTPClient::InternalFailed(const char* szMessage)
{
  return OnFailed(szMessage);
}

int HTTPClient::InternalClosed()
{
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, ApHandleFormat " %s", ApHandlePrintf(hAp_), _sz(sUrl_)));

  int nResult = OnClosed();

  if (bAutoDelete_) {
    delete this;
  }

  return nResult;
}

// -------------------------------------------------------------------

void HTTPClient::SetBasicAuthorization(const String& sUser, const String& sPassword, KeyValueList& kvHeaders)
{
  String sUserPass = sUser + ":" + sPassword;
  Buffer sbAuthorization;
  sbAuthorization.SetData(sUserPass);
  String sAuthorization;
  sbAuthorization.encodeBase64(sAuthorization);
  String sValue = "Basic " + sAuthorization;
  kvHeaders.add("Authorization", sValue);
}

// -------------------------------------------------------------------

AP_NAMESPACE_END
