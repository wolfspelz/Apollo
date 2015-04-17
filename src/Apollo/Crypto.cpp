// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Crypto.h"

#include "blowfish/blowfish.h"

AP_NAMESPACE_BEGIN

static unsigned char cbc_iv[8] = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

int Crypto::encryptBlowfish(const String sKey, Buffer& sbOut)
{
  int ok = 0;

  unsigned char iv[8];
  BF_KEY key;
  int len = Length();
  int last_len = len % 8;
  int cbc_len = len - last_len;
	unsigned char* cbc_in = Data();
	Flexbuf<unsigned char, 8192> cbc_out(len + 8);

  BF_set_key(&key, sKey.bytes(), (unsigned char*) (const char*) sKey);
	memcpy(iv, cbc_iv, 8);

  // crypt complete blocks
  if (cbc_len > 0) {
    BF_cbc_encrypt(cbc_in, cbc_out, cbc_len, &key, iv, BF_ENCRYPT);
  }

  // crypt 0-7 oversize bytes
  for (int i = 0; i < last_len; i++) {
    cbc_out[cbc_len + i] = iv[i] ^ cbc_in[cbc_len + i];
  }

  sbOut.SetData(cbc_out, len);
  ok = 1;

  return ok;
}

int Crypto::decryptBlowfish(const String sKey, const Buffer& sbIn)
{
  int ok= 0;

	unsigned char iv[8];
  BF_KEY key;
  int len = sbIn.Length();
  int last_len = len % 8;
  int cbc_len = len - last_len;
	unsigned char* cbc_in = (unsigned char *) sbIn.Data();
	Flexbuf<unsigned char, 8192> cbc_out(len + 8);

  BF_set_key(&key, sKey.bytes(), (unsigned char*) (const char*) sKey);
	memcpy(iv, cbc_iv, 8);

  // decrypt complete blocks
  if (cbc_len > 0) {
	  BF_cbc_encrypt(cbc_in, cbc_out, cbc_len, &key, iv, BF_DECRYPT);
  }

  // decrypt 0-7 oversize bytes
  for (int i = 0; i < last_len; i++) {
    cbc_out[cbc_len + i] = iv[i] ^ cbc_in[cbc_len + i];
  }

  SetData(cbc_out, len);
  ok = 1;

  return ok;
}

#if defined(WIN32)
  #include <Wincrypt.h>
#endif 

int Crypto::encryptWithLoginCredentials(Buffer& sbOut)
{
  int ok = 0;

#if defined(WIN32)
	DATA_BLOB dataIn, dataOut;
	dataIn.pbData = Data();
	dataIn.cbData = Length();

  BOOL bOk = CryptProtectData(&dataIn, L"Apollo", NULL, NULL, NULL, 0, &dataOut);
  if (!bOk) {
    // Sometimes CryptProtectData fails, but works with CRYPTPROTECT_LOCAL_MACHINE.
    // Weak security hole: anyone on the PC can decrypt it, no just the user.
    // Not necessary to apply the flag for decryption. The encrypted data knows it all.
    bOk = CryptProtectData(&dataIn, L"Apollo", NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE, &dataOut);
  }
	if (bOk) {
    sbOut.SetData(dataOut.pbData, dataOut.cbData);
    ::LocalFree(dataOut.pbData);
    ok = 1;
	}

#elif defined(LINUX)
  #error Not implemented for this OS
#else
  #error Not implemented for this OS
#endif 

  return ok;
}

int Crypto::decryptWithLoginCredentials(const Buffer& sbIn)
{
  int ok= 0;

#if defined(WIN32)
	DATA_BLOB dataIn, dataOut;
	dataIn.pbData = (unsigned char*) sbIn.Data();
	dataIn.cbData = sbIn.Length();

	BOOL bOk = CryptUnprotectData(&dataIn, NULL, NULL, NULL, NULL, 0, &dataOut);
	if (bOk) {
    SetData(dataOut.pbData, dataOut.cbData);
    ::LocalFree(dataOut.pbData);
    ok = 1;
	}

#elif defined(LINUX)
  #error Not implemented for this OS
#else
  #error Not implemented for this OS
#endif 

  return ok;
}

AP_NAMESPACE_END
