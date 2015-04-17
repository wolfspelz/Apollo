// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SrpcStreamParser_h_INCLUDED)
#define SrpcStreamParser_h_INCLUDED

#include "Apollo.h"
#include "ApExports.h"
#include "SrpcMessage.h"

typedef ListT<Apollo::SrpcMessage, Elem> SrpcMessageList;

AP_NAMESPACE_BEGIN

class APOLLO_API SrpcStreamParser
{
public:
  SrpcStreamParser()
    :pCurrentMessage_(0)
  {}
  virtual ~SrpcStreamParser();

public:
  int parse(unsigned char* pData, size_t nLen);
  int parse(const String& sData);
  Apollo::SrpcMessage* getNextMessage();

protected:
  Buffer sbData_;
  SrpcMessageList lMessages_;
  Apollo::SrpcMessage* pCurrentMessage_;
};

AP_NAMESPACE_END

#endif // !defined(SrpcStreamParser_h_INCLUDED)
