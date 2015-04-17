// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(HttpManager_h_INCLUDED)
#define HttpManager_h_INCLUDED

#include "Apollo.h"
#include "ApOS.h"
#include "NetOS.h"

//----------------------------------------------------------

class HTTPRequestTask: public Apollo::Task
{
public:
  HTTPRequestTask()
    :hAp_(ApNoHandle)
    ,sUrl_("")
    ,sMethod_("")
    ,nState_(RequestState_None)
    ,nRequestStatusCode_(0)
    ,hConnect_(0)
#if AP_HTTP_USE_CURL
    ,bCancelled_(0)
#endif
  {}

  void Execute();
  void ExecuteHTTP();
  void ExecuteFile(String& sDataFilename);

  ApHandle hAp_;
  String sUrl_;
  String sMethod_;
  List lHeader_;
  Buffer sbBody_;
  String sReason_; // something that tells why the request was started

  typedef enum _RequestState { 
     RequestState_None
    ,RequestState_Connecting
    ,RequestState_Connected
    ,RequestState_Header
    ,RequestState_DataIn
    ,RequestState_Closed
  } RequestState;
  RequestState nState_;

  // File based simulator 
  // Format:
  // [URL Path\n]*
  //
  // Example:
  // http://www.heise.de heise/index.html
  // http://www.virtual-presence.org/about.html virtual-presence.org/about.html
  // http://www.virtual-presence.org/systems.html virtual-presence.org/systems.html
  String sIndexFile_;

public:
  HCONNECTION GetHandle() { return hConnect_; }
  List lResponseHeaders_;
  int nRequestStatusCode_;
protected:
  void SetHandle(HCONNECTION hConnect) { hConnect_ = hConnect;  }
  void CloseHandle() {
    NetOS::HTTP_CloseConnection(hConnect_);
    hConnect_ = 0;
  }
  // --------
  friend void NetOS::HTTP_CloseConnection(HCONNECTION hConnection);
  friend int NetOS::HTTP_PerformRequest(int bDoCall, Apollo::URL url, String& sMethod, int bSSL, String& sError, HTTPRequestTask* self);
  friend void NetOS::HTTP_InterruptTaskAndCloseConnection(HTTPRequestTask* pTask);
  //friend class HttpManager;
private:
  HCONNECTION hConnect_;
  // ----------
#if AP_HTTP_USE_CURL
protected:
  void Cancel() { Apollo::MutexLocker lock(&oCancelMutex_); bCancelled_ = 1; }
  int Cancelled() { Apollo::MutexLocker lock(&oCancelMutex_); return bCancelled_; }
  friend int NetOS_HTTP_CurlProgressCallback(void*, double, double, double, double);
private:
  int bCancelled_;
  Apollo::Mutex oCancelMutex_;
#endif
};

//----------------------------------------------------------

class HttpManager: public Apollo::Thread
{
public:
  HttpManager()
  : Thread("HttpManager")
  , pCurrentTask_(0)
#if defined(WIN32)
  ,hWinInet_(NULL)
#endif
  {}

  int Init(const char* szClientName);
  void Exit();

  int CancelRequest(const ApHandle& h);
  int CancelAllRequests();

  void CloseAll();

  String sHttpLog_;

public:
  int IsFinished() { return bFinished_; }
protected:
  int bFinished_;
protected:
  friend int NetOS::HTTP_Manager_Init(String& sClientName);
public:
  HTTPRequestTask* GetCurrentTask();
  void SetCurrentTask(HTTPRequestTask* t);
protected:
  HTTPRequestTask* pCurrentTask_;
  Apollo::Mutex oCurrentTaskMutex_;
#if defined(WIN32)
public:
  HINTERNET InetHandle() { return hWinInet_; };
protected:
  friend void NetOS::HTTP_Manager_Shutdown(HttpManager& oHttp);
  HINTERNET hWinInet_;
#endif
};

#endif // !defined(HttpManager_h_INCLUDED)
