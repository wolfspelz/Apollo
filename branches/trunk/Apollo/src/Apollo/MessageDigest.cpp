// ============================================================================
//
// Apollo
//
// ============================================================================

#include "MessageDigest.h"

#include "sha1/sha1.h"

AP_NAMESPACE_BEGIN

void MessageDigest::doSHA1()
{
  SHA1_ctx ctx;
  byte md[SHA1_DIGEST_BYTES];

  SHA1_Init(&ctx);
  SHA1_Update(&ctx, sbMessage_.Data(), sbMessage_.Length());
  SHA1_Final(&ctx, md);

  sbSHA1_.SetData(md, SHA1_DIGEST_BYTES);
  bSHA1_ = 1;
}

String MessageDigest::getSHA1Hex()
{
  String sResult;

  if (!bSHA1_) { doSHA1(); }
  sbSHA1_.encodeBinhex(sResult);

  return sResult;
}

Buffer& MessageDigest::getSHA1()
{
  if (!bSHA1_) { doSHA1(); }
  return sbSHA1_;
}

//String MessageDigest::getSHA1base64()
//{
//  String sResult;
//
//  if (!bSHA1_) { doSHA1(); }
//  sbSHA1_.encodeBase64(sResult);
//
//  return sResult;
//}

AP_NAMESPACE_END
