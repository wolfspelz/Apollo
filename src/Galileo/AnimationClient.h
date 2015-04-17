// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(AnimationClient_H_INCLUDED)
#define AnimationClient_H_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

using namespace Apollo;

class AnimationClient: public HTTPClient
{
public:
  AnimationClient(const ApHandle& hRequest)
    :HTTPClient("AnimationClient")
    ,hRequest_(hRequest)
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
  int nStatus_;
  Buffer sbData_;
  List lHeader_;
  int bFailed_;
  String sContentType_;
};

#endif // AnimationClient_H_INCLUDED
