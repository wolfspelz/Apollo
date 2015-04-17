// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ContainerClient_H_INCLUDED)
#define ContainerClient_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

using namespace Apollo;

class ContainerClient: public HTTPClient
{
public:
  ContainerClient(const ApHandle& hRequest, const String& sDigest)
    :HTTPClient("ContainerClient") 
    ,hRequest_(hRequest)
    ,sDigest_(sDigest)
    ,nStatus_(0)
    ,bFailed_(0)
    ,bContentTypeOk_(0)
  {}

  virtual int OnConnected();
  virtual int OnHeader(int nStatus, KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();

protected:
  ApHandle hRequest_;
  String sDigest_;

  int nStatus_;
  String sError_;
  Buffer sbData_;
  List lHeader_;
  int bFailed_;
  String sContentType_;
  int bContentTypeOk_;
};

#endif // ContainerClient_H_INCLUDED
