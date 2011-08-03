// ============================================================================
//
// Apollo
//
// ============================================================================

#include "MessageDigest.h"

#include "sha1/sha1.h"
#include "md5/md5.h"

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

// ---------------------------------------

void MessageDigest::doMD5()
{
  md5_state_t state;
	md5_byte_t digest[16];

  md5_init(&state);
	md5_append(&state, (const md5_byte_t *) sbMessage_.Data(), sbMessage_.Length());
	md5_finish(&state, digest);

  sbMD5_.SetData(digest, sizeof(digest));
  bMD5_ = 1;
}

String MessageDigest::getMD5Hex()
{
  String sResult;

  if (!bMD5_) { doMD5(); }
  sbMD5_.encodeBinhex(sResult);

  return sResult;
}

Buffer& MessageDigest::getMD5()
{
  if (!bMD5_) { doMD5(); }
  return sbMD5_;
}

AP_NAMESPACE_END
