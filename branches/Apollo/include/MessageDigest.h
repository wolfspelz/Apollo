// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MessageDigest_h_INCLUDED)
#define MessageDigest_h_INCLUDED

#include "SSystem.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

class APOLLO_API MessageDigest
{
public:
  MessageDigest(unsigned char* pData, size_t nLen)
    :sbMessage_(pData, nLen)
    ,bSHA1_(0)
  {}
  virtual ~MessageDigest() {}

  String getSHA1Hex();
  //String getSHA1base64();
  Buffer& getSHA1();

protected:
  void doSHA1();

protected:
  Buffer sbMessage_;

  int bSHA1_;
  Buffer sbSHA1_;
};

AP_NAMESPACE_END

#endif // !defined(MessageDigest_h_INCLUDED)

