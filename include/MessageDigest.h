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
    ,bMD5_(0)
  {}
  virtual ~MessageDigest() {}

  String getSHA1Hex();
  Buffer& getSHA1();

  String getMD5Hex();
  Buffer& getMD5();

protected:
  void doSHA1();
  void doMD5();

protected:
  Buffer sbMessage_;

  int bSHA1_;
  Buffer sbSHA1_;

  int bMD5_;
  Buffer sbMD5_;
};

AP_NAMESPACE_END

#endif // !defined(MessageDigest_h_INCLUDED)

