// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Crypto_h_INCLUDED)
#define Crypto_h_INCLUDED

#include "SSystem.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

class APOLLO_API Crypto : public Buffer
{
public:
  int encryptBlowfish(const String sKey, Buffer& sbOut);
  int decryptBlowfish(const String sKey, const Buffer& sbIn);

  int encryptWithLoginCredentials(Buffer& sbOut);
  int decryptWithLoginCredentials(const Buffer& sbIn);
};

AP_NAMESPACE_END

#endif // !defined(Crypto_h_INCLUDED)

