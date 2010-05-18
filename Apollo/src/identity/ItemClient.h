// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ItemClient_H_INCLUDED)
#define ItemClient_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

using namespace Apollo;

class ItemClient: public HTTPClient
{
public:
  ItemClient(ApHandle hRequest, const String& sContainerUrl, const String& sId, const String& sDigest)
    :HTTPClient("ItemClient") 
    ,hRequest_(hRequest)
    ,sContainerUrl_(sContainerUrl)
    ,sId_(sId)
    ,sDigest_(sDigest)
    ,nStatus_(0)
    ,bFailed_(0)
  {}

  virtual int OnConnected();
  virtual int OnHeader(int nStatus, KeyValueList& kvHeaders);
  virtual int OnDataIn(unsigned char* pData, size_t nLen);
  virtual int OnFailed(const char* szMessage);
  virtual int OnClosed();

protected:
  ApHandle hRequest_;
  String sContainerUrl_;
  String sId_;
  String sDigest_;

  int nStatus_;
  Buffer sbData_;
  List lHeader_;
  int bFailed_;
  String sContentType_;
};

#endif // ItemClient_H_INCLUDED
