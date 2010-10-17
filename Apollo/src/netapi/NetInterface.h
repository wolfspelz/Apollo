// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NetInterface_h_INCLUDED)
#define NetInterface_h_INCLUDED

#include "ApExports.h"
#include "ApTypes.h"
#include "ApCompiler.h"

#if !defined(NETAPI_API)

#if defined(WIN32)
  #ifdef NETAPI_EXPORTS
    #define NETAPI_API __declspec(dllexport)
  #else
    #define NETAPI_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(NETAPI_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define NETAPI_API __attribute__ ((visibility("default")))
  #else
    #define NETAPI_API
  #endif
#else
  #define NETAPI_API
#endif

#endif // defined (NETAPI_EXPORTS)

AP_NAMESPACE_BEGIN

class TCPConnection;
class NETAPI_API TCPServer
{
public:
  TCPServer();
  virtual ~TCPServer();

public:
  virtual int Start(const String& sAddress, int nPort);
  virtual int Stop();
  int Running() { return bRunning_; }
  inline ApHandle apHandle() { return hAp_; }

protected:
  // Overwrite this method by returnin a new connection object inheriting from TCPConnection
  virtual TCPConnection* OnConnectionAccepted(String& sName, const ApHandle& hConnection, String& sClientAddress, int nClientPort) 
  { return NULL; }

protected:
  virtual int OnStarted() { return 0; }
  virtual int OnStopped() { return 0; }

public:
  int InternalListening();
  int InternalListenStopped();
  int InternalConnectionAccepted(const ApHandle& hConnection, String& sClientAddress, int nClientPort);

protected:
  int bRunning_;
  int bCancelled_;
  
  ApHandle hAp_;

  String sAddress_;
  int nPort_;
};

class NETAPI_API TCPConnection : public Elem
{
public:
  TCPConnection(const char* szName);
  TCPConnection(const char* szName, const ApHandle& hConnection);
  virtual ~TCPConnection();

  static TCPConnection* Find(const ApHandle& h);

  virtual int Connect(const char* szAddress, int nPort);
  virtual int DataOut(unsigned char* pData, size_t nLen);
  virtual int Close();

  virtual void AutoDelete(int bFlag) { bAutoDelete_ = bFlag; }
  virtual int Cancelled() { return bCancelled_; }

  int InternalConnected();
  int InternalDataIn(unsigned char* pData, size_t nLen);
  int InternalClosed();

  inline ApHandle apHandle() { return hAp_; }

protected:
  virtual int OnConnected() { return 0; }
  virtual int OnDataIn(unsigned char* pData, size_t nLen) { AP_UNUSED_ARG(pData); AP_UNUSED_ARG(nLen); return 0; }
  virtual int OnClosed() { return 0; }

protected:
  friend class TCPServer;

  ApHandle hAp_;
  String sAddress_;
  int nPort_;
  int bOpen_;
  int bCancelled_;

  int bAutoDelete_;
};

// -------------------------------------------------------------------

class NETAPI_API HTTPClient : public Elem
{
public:
  HTTPClient(const char* szReason);
  virtual ~HTTPClient();

  static void ClassInit();
  static void ClassExit();
  static HTTPClient* Find(const ApHandle& h);

  virtual int Get(const char* szUrl, KeyValueList& kvHeaders = kvEmptyHeaders_);
  virtual int Post(const char* szUrl, const unsigned char* pData, size_t nLen, KeyValueList& kvHeaders = kvEmptyHeaders_);

  void SetBasicAuthorization(const String& sUser, const String& sPassword, KeyValueList& kvHeaders);

  virtual int Cancel();
  virtual int Cancelled() { return bCancelled_; }
  virtual void AutoDelete(int bFlag) { bAutoDelete_ = bFlag; }
  virtual String GetHeader(List& lHeader, const char* szName);

  int InternalConnected();
  int InternalHeader(int nStatus, KeyValueList& kvHeaders);
  int InternalDataIn(unsigned char* pData, size_t nLen);
  int InternalFailed(const char* szMessage);
  int InternalClosed();

  inline ApHandle apHandle() { return hAp_; }

protected:
  virtual int OnConnected() { return 0; }
  virtual int OnHeader(int nStatus, KeyValueList& kvHeaders) { AP_UNUSED_ARG(nStatus); AP_UNUSED_ARG(kvHeaders); return 0; }
  virtual int OnDataIn(unsigned char* pData, size_t nLen) { AP_UNUSED_ARG(pData); AP_UNUSED_ARG(nLen); return 0; }
  virtual int OnFailed(const char* szMessage) { AP_UNUSED_ARG(szMessage); return 0; }
  virtual int OnClosed() { return 0; }

protected:
  static KeyValueList kvEmptyHeaders_;

  ApHandle hAp_;
  String sUrl_;
  size_t nMaxDataBytes_;
  size_t nMaxTimeSec_;
  int bCancelled_;

  int bAutoDelete_;
  String sReason_;
};

AP_NAMESPACE_END

#endif // !defined(NetInterface_h_INCLUDED)
