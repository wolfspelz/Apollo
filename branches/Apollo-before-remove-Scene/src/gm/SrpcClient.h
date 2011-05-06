// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SrpcClient_H_INCLUDED)
#define SrpcClient_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

using namespace Apollo;

class GmModule;

class SrpcClient: public HTTPClient
{
public:
  SrpcClient(const String& sReason, GmModule* pModule)
    :HTTPClient(sReason) 
    ,nStatus_(0)
    ,bFailed_(0)
    ,bContentTypeOk_(0)
    ,pModule_(pModule)
  {}

  int Post(const String& sUrl, SrpcMessage& srpc);
  
  virtual int OnConnected();
  virtual int OnHeader(int nStatus, KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();

  virtual void OnResult(SrpcMessage& srpc) = 0;
  virtual void OnError(const String sError) = 0;

  int nStatus_;
  Buffer sbData_;
  List lHeader_;
  int bFailed_;
  String sError_;

  String sContentType_;
  int bContentTypeOk_;

  GmModule* pModule_;
};

#endif // SrpcClient_H_INCLUDED
