// ============================================================================
//
// Apollo
//
// ============================================================================

#include "SSystem.h"

const char* Elem::szEmpty_= "";
Elem Elem_eEmpty("", "");

void Elem::zero()
{
  pLink_ = 0;
  nInt_ = 0;
  pPtr_ = 0;
  nLen_ = 0;
  nSize_ = 0;
  pData_ = 0;
}

Elem::Elem()
{
  zero();
  //if (this == (Elem*) 0x00B6C600) { __asm { int 3 } }
}

Elem::Elem(const String& sName)
{
  zero();
  sName_ = sName;
}

Elem::Elem(const String& sName, int nValue)
{
  zero();
  sName_ = sName;
  nInt_ = nValue;
}

Elem::Elem(const String& sName, const String& sStr)
{
  zero();
  sName_ = sName;
  sString_ = sStr;
}

Elem::Elem(const String& sName, int nValue, const String& sStr)
{
  zero();
  sName_ = sName;
  sString_ = sStr;
  nInt_ = nValue;
}

Elem::Elem(int nValue)
{
  zero();
  nInt_ = nValue;
}

Elem::~Elem()
{
  if (pData_ != 0) { SSystem::free(pData_); }
  pData_= 0;
  nSize_= 0;
  nLen_= 0;
}

// ============================================================================
// data

String& Elem::getName()
{
  return sName_;
}

const String& Elem::getName() const
{
  return sName_;
}

void Elem::setName(const String& sName)
{
  sName_ = sName;
}

String& Elem::getString()
{
  return sString_;
}

const String& Elem::getString() const
{
  return sString_;
}

void Elem::setString(const String& sStr)
{
  sString_ = sStr;
}

void Elem::operator=(Elem &elem)
{
  sName_ = elem.sName_;
  sString_ = elem.sString_;
  nInt_ = elem.nInt_;
  pPtr_ = elem.pPtr_;

  size_t nDataLen = 0;
  char* pDataPtr = 0;
  if (elem.readData(&nDataLen, &pDataPtr)) {
    writeData(nDataLen, pDataPtr);
  }
}

int Elem::getInt() const
{
  return nInt_;
}

int Elem::setInt(int nValue)
{
  return nInt_ = nValue;
}

void* Elem::getPtr()
{
  return pPtr_;
}

const void* Elem::getPtr() const
{
  return pPtr_;
}


void Elem::setPtr(void* pPtr)
{
  pPtr_ = pPtr;
}

bool Elem::readData(size_t *nDataLen, char* *pDataPtr)
{
  if (nDataLen != 0) *nDataLen = nLen_;
  if (pDataPtr != 0) *pDataPtr = pData_;
  return true;
}

bool Elem::readData(size_t *nDataLen, const char* *pDataPtr) const
{
  if (nDataLen != 0) *nDataLen = nLen_;
  if (pDataPtr != 0) *pDataPtr = pData_;
  return true;
}

bool Elem::writeData(size_t nDataLen, char* pDataPtr)
{
  bool ok= false;
  
  if (pDataPtr != pData_) {
    if (pData_ != 0) {
      SSystem::free(pData_);
      nLen_ = 0;
      nSize_ = 0;
      pData_ = 0;
    }
    if (pDataPtr != 0) {
      pData_ = (char*) SSystem::malloc(nDataLen);
      if (pData_ != 0) {
        nSize_= nDataLen;
        memcpy(pData_, pDataPtr, nDataLen);
        nLen_ = nDataLen;
        ok= true;
      }
    }
  }

  return ok;
}

// ============================================================================
// filter

bool Elem_FindByObjectRefFilter(void* refCon, void* e)
{
  return(e == refCon);
}

bool Elem_FindByIntFilter(void* refCon, void* e)
{
  if ((int) refCon == ((Elem *)e)->getInt()) {
    return true;
  } else {
    return false;
  }
}

bool Elem_FindByPtrFilter(void* refCon, void* e)
{
  if (refCon == ((Elem *)e)->getPtr()) {
    return true;
  } else {
    return false;
  }
}

bool Elem_FindByStringFilter(void* refCon, void* e)
{
  if ((refCon == 0) || (((Elem *)e)->getString().empty()) ) {
    return false;
  } else {
    if (String::strcmp((char*) refCon,((Elem *)e)->getString())) {
      return false;
    } else {
      return true;
    }
  }
}

bool Elem_FindByNameFilter(void* refCon, void* e)
{
  if ((refCon == 0) || (((Elem *)e)->getName().empty()) ) {
    return false;
  } else {
    if (String::strcmp((char*) refCon,((Elem *)e)->getName())) {
      return false;
    } else {
      return true;
    }
  }
}

bool Elem_FindByNameCaseFilter(void* refCon, void* e)
{
  if ((refCon == 0) || (((Elem *)e)->getName().empty()) ) {
    return false;
  } else {
    if (String::stricmp((char*) refCon,((Elem *)e)->getName())) {
      return false;
    } else {
      return true;
    }
  }
}

int Elem_CompareNameFilter(void* refCon, void* e)
{
  if ((refCon == 0) || (((Elem *)e)->getName().empty()) ) {
    return 0;
  } else {
    return String::strcmp((char*) refCon,((Elem *)e)->getName());
  }
}

// ============================================================================
// Binary buffer wrapper for Elem

bool Buffer::SetData(const unsigned char* pDataPtr, size_t nDataLen)
{
  return writeData(nDataLen, (char*) pDataPtr);
}

int Buffer::GetString(String& _data) const
{
  int ok= 1;

  if (Length() > 0) {
    Flexbuf<char> buf(Length()+1);
    if (buf) {
      memcpy((char*) buf, Data(), Length());
      buf[Length()]= '\0';
      _data = (char*) buf;
    } else {
      ok= 0;
    }
  }

  return ok;
}

unsigned char* Buffer::Data(void)
{
  size_t nDataLen = 0;
  char* pDataPtr = 0;
  readData(&nDataLen, &pDataPtr);
  return (unsigned char*) pDataPtr;
}

const unsigned char* Buffer::Data(void) const
{
  size_t nDataLen = 0;
  const char* pDataPtr = 0;
  readData(&nDataLen, &pDataPtr);
  return (const unsigned char*) pDataPtr;
}

size_t Buffer::Length(void) const
{
  size_t nDataLen = 0;
  const char* pDataPtr = 0;
  readData(&nDataLen, &pDataPtr);
  return nDataLen;
}

bool Buffer::Append(unsigned char* pDataPtr, size_t nDataLen)
{
  bool retval= false;
  size_t old_dataLen= 0, new_dataLen= 0; 
  char* old_dataPtr= 0; unsigned char*new_dataPtr= 0; 

  if (pDataPtr != 0 && nDataLen != 0) {
    if (readData(&old_dataLen, &old_dataPtr)) {
      if (old_dataLen == 0 || old_dataPtr == 0) {
        new_dataLen= nDataLen;
        new_dataPtr= pDataPtr;
      } else {
        new_dataLen= old_dataLen + nDataLen;
        new_dataPtr= (unsigned char*) SSystem::malloc(new_dataLen);
        if (new_dataPtr != 0) {
          memcpy(new_dataPtr, old_dataPtr, old_dataLen);
          memcpy(&(new_dataPtr[old_dataLen]), pDataPtr, nDataLen);
        }
      }
      if (new_dataPtr != 0 && new_dataLen != 0) {
        retval= writeData(new_dataLen, (char*) new_dataPtr);
      }
      if (new_dataPtr != 0 && new_dataPtr != pDataPtr) {
        SSystem::free(new_dataPtr);
      }
    }
  }

  return retval;
}

bool Buffer::Discard(size_t usedLen)
{
  bool retval= false;

  if (usedLen < Length()) {
    Buffer tmp;
    tmp.SetData(Data() + usedLen, Length() - usedLen);
    retval= SetData(tmp.Data(), tmp.Length());
  } else if (usedLen == Length()) {
    Empty();
  }

  return retval;
}

int Buffer::dump_encode(String& _string_out)
{
  int ok= 0;
/*
  unsigned char b[]= {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
  };
  SetData(b, sizeof(b));
*/
  if (Length() > 0) {
    Flexbuf<char> encoded(Length()*3+1);
    if (encoded) {
      ok= 1;

      int escape_cnt= 0;
      char* p= (char*) Data();
      size_t len= Length();
      size_t j= 0;
      for (unsigned int i= 0; i<len; i++) {
        if (isprint(*p) && isascii(*p)) {
          ((char*) encoded)[j++]= *p;
        } else {
          escape_cnt++;
          char s[3];
          sprintf(s, "%02x", *((unsigned char*) p));
          ((char*) encoded)[j++]= '%';
          ((char*) encoded)[j++]= s[0];
          ((char*) encoded)[j++]= s[1];
        }
        p++;
      }

      ((char*) encoded)[j++]= '\0';
      _string_out= (char*) encoded;
    }
  } else {
    _string_out= "";
    ok= 1;
  }

  return ok;
}

int Buffer::encodeBase64(String& _string_out)
{
  int ok= 0;

  if (Length() > 0) {

    // Make the buffer at least 2 bytes longer, 1 byte must be 0 if Length() % 3 == 0 and the stupid algorithm sometimes reads 2 more
    Flexbuf<unsigned char, 8192> resized(Length() + 3);
    if (resized) {
      unsigned char* pResized = resized;
      memcpy(pResized, Data(), Length());
      pResized[Length()] = '\0';

      Flexbuf<unsigned char> encoded(Length() << 1); // should be about 4/3 + 2, x2 is enough
      if (encoded) {
        int outlen = HTUU_encode(pResized, Length(), (char*) (unsigned char*) encoded);
        if (outlen > 0) {
          _string_out = (char*) (unsigned char*) encoded;
          ok= 1;
        } // if (outlen > 0)
      } // if (encoded)
    } // if (resized)
    
  } else {
    _string_out= "";
    ok= 1;
  }

  return ok;
}

void Buffer::encodeBinhex(String& _string_out)
{
  Flexbuf<unsigned char> buf(Length() * 2);
  const unsigned char* p = Data();
  unsigned char* q = buf.get();
  int nCnt = 0;

  for (int i = 0; i < Length(); i++) {
    int n1 = (*p & 0xf0) >> 4;
    int n2 = *p & 0x0f;
    p++;

    if (n1 >= 0 && n1 <= 9) { *q++ = '0' + n1; }
    else if (n1 >= 10 && n1 <= 15) { *q++ = 'a' + (n1 - 10); }
     
    if (n2 >= 0 && n2 <= 9) { *q++ = '0' + n2; }
    else if (n2 >= 10 && n2 <= 15) { *q++ = 'a' + (n2 - 10); }

    nCnt+=2;
  }

  _string_out.set((const char*) buf.get(), nCnt);
}

void Buffer::decodeBinhex(const String& _string_in)
{
  Flexbuf<unsigned char> buf(_string_in.bytes() / 2 + 1);
  const unsigned char* p = (const unsigned char*) (const char*) _string_in;
  unsigned char* q = buf.get();
  int nCnt = 0;

  for (int i = 0; i < _string_in.bytes(); i += 2) {
    char c1 = p[i];
    char c2 = p[i+1];
    int n = 0;

    if (c1 >= '0' && c1 <= '9') { n += (c1 - '0') << 4; }
    else if (c1 >= 'A' && c1 <= 'F') { n += (c1 - 'A' + 10) << 4; }
    else if (c1 >= 'a' && c1 <= 'f') { n += (c1 - 'a' + 10) << 4; }

    if (c2 >= '0' && c2 <= '9') { n += (c2 - '0'); }
    else if (c2 >= 'A' && c2 <= 'F') { n += (c2 - 'A' + 10); }
    else if (c2 >= 'a' && c2 <= 'f') { n += (c2 - 'a' + 10); }

    *q++ = n;
    nCnt++;
  }

  SetData(buf.get(), nCnt);
}

int Buffer::decodeBase64(const String& _string_in)
{
  int ok= 0;
  Flexbuf<char> decoded(_string_in.bytes()+1);

  if (decoded) {
    if (_string_in.bytes() > 0) {
      int outlen = HTUU_decode(_string_in.c_str(), (unsigned char*) (char*) decoded, decoded.length());
      if (outlen > 0) {
        ok= SetData((char*) decoded, outlen);
      }
    } else {
      Empty();
    }
  }


  return ok;
}

static char six2pr[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/'
};

static unsigned char pr2six[256];


/*--- function HTUU_encode -----------------------------------------------
 *
 *   Encode a single line of binary data to a standard format that
 *   uses only printing ASCII characters (but takes up 33% more bytes).
 *
 *    Entry    bufin    points to a buffer of bytes.  If nbytes is not
 *                      a multiple of three, then the byte just beyond
 *                      the last byte in the buffer must be 0.
 *             nbytes   is the number of bytes in that buffer.
 *                      This cannot be more than 48.
 *             bufcoded points to an output buffer.  Be sure that this
 *                      can hold at least 1 + (4*nbytes)/3 characters.
 *
 *    Exit     bufcoded contains the coded line.  The first 4*nbytes/3 bytes
 *                      contain printing ASCII characters representing
 *                      those binary bytes. This may include one or
 *                      two '=' characters used as padding at the end.
 *                      The last byte is a zero byte.
 *             Returns the number of ASCII characters in "bufcoded".
 */
int Buffer::HTUU_encode (unsigned char* bufin, unsigned int nbytes, char* bufcoded)
{
/* ENC is the basic 1 character encoding function to make a char printing */
#define ENC(c) six2pr[c]

   register char* outptr = bufcoded;
   unsigned int i;

   for (i=0; i<nbytes; i += 3) {
      *(outptr++) = ENC(*bufin >> 2);            /* c1 */
      *(outptr++) = ENC(((*bufin << 4) & 060) | ((bufin[1] >> 4) & 017)); /*c2*/
      *(outptr++) = ENC(((bufin[1] << 2) & 074) | ((bufin[2] >> 6) & 03));/*c3*/
      *(outptr++) = ENC(bufin[2] & 077);         /* c4 */

      bufin += 3;
   }

   /* If nbytes was not a multiple of 3, then we have encoded too
    * many characters.  Adjust appropriately.
    */
   if(i == nbytes+1) {
      /* There were only 2 bytes in that last group */
      outptr[-1] = '=';
   } else if(i == nbytes+2) {
      /* There was only 1 byte in that last group */
      outptr[-1] = '=';
      outptr[-2] = '=';
   }
   *outptr = '\0';
   return(outptr - bufcoded);
}


/*--- function HTUU_decode ------------------------------------------------
 *
 *  Decode an ASCII-encoded buffer back to its original binary form.
 *
 *    Entry    bufcoded    points to a uuencoded string.  It is 
 *                         terminated by any character not in
 *                         the printable character table six2pr, but
 *                         leading whitespace is stripped.
 *             bufplain    points to the output buffer; must be big
 *                         enough to hold the decoded string (generally
 *                         shorter than the encoded string) plus
 *                         as many as two extra bytes used during
 *                         the decoding process.
 *             outbufsize  is the maximum number of bytes that
 *                         can fit in bufplain.
 *
 *    Exit     Returns the number of binary bytes decoded.
 *             bufplain    contains these bytes.
 */
int Buffer::HTUU_decode(const char* bufcoded, unsigned char* bufplain, int outbufsize)
{
/* single character decode */
#define DEC(c) pr2six[(int)c]
#define MAXVAL 63

   static int first = 1;

   int nbytesdecoded, j;
   register const char* bufin = bufcoded;
   register unsigned char* bufout = bufplain;
   register int nprbytes;

   /* If this is the first call, initialize the mapping table.
    * This code should work even on non-ASCII machines.
    */
   if(first) {
      first = 0;
      for(j=0; j<256; j++) pr2six[j] = MAXVAL+1;

      for(j=0; j<64; j++) pr2six[(int)six2pr[j]] = (unsigned char) j;
#if 0
      pr2six['A']= 0; pr2six['B']= 1; pr2six['C']= 2; pr2six['D']= 3; 
      pr2six['E']= 4; pr2six['F']= 5; pr2six['G']= 6; pr2six['H']= 7; 
      pr2six['I']= 8; pr2six['J']= 9; pr2six['K']=10; pr2six['L']=11; 
      pr2six['M']=12; pr2six['N']=13; pr2six['O']=14; pr2six['P']=15; 
      pr2six['Q']=16; pr2six['R']=17; pr2six['S']=18; pr2six['T']=19; 
      pr2six['U']=20; pr2six['V']=21; pr2six['W']=22; pr2six['X']=23; 
      pr2six['Y']=24; pr2six['Z']=25; pr2six['a']=26; pr2six['b']=27; 
      pr2six['c']=28; pr2six['d']=29; pr2six['e']=30; pr2six['f']=31; 
      pr2six['g']=32; pr2six['h']=33; pr2six['i']=34; pr2six['j']=35; 
      pr2six['k']=36; pr2six['l']=37; pr2six['m']=38; pr2six['n']=39; 
      pr2six['o']=40; pr2six['p']=41; pr2six['q']=42; pr2six['r']=43; 
      pr2six['s']=44; pr2six['t']=45; pr2six['u']=46; pr2six['v']=47; 
      pr2six['w']=48; pr2six['x']=49; pr2six['y']=50; pr2six['z']=51; 
      pr2six['0']=52; pr2six['1']=53; pr2six['2']=54; pr2six['3']=55; 
      pr2six['4']=56; pr2six['5']=57; pr2six['6']=58; pr2six['7']=59; 
      pr2six['8']=60; pr2six['9']=61; pr2six['+']=62; pr2six['/']=63;
#endif
   }

   /* Strip leading whitespace. */

   while(*bufcoded==' ' || *bufcoded == '\t') bufcoded++;

   /* Figure out how many characters are in the input buffer.
    * If this would decode into more bytes than would fit into
    * the output buffer, adjust the number of input bytes downwards.
    */
   bufin = bufcoded;
   while(pr2six[(int)*(bufin++)] <= MAXVAL);
   nprbytes = bufin - bufcoded - 1;
   nbytesdecoded = ((nprbytes+3)/4) * 3;
   if(nbytesdecoded > outbufsize) {
      nprbytes = (outbufsize*4)/3;
   }

   bufin = bufcoded;
   
   while (nprbytes > 0) {
      *(bufout++) = (unsigned char) (DEC(*bufin) << 2 | DEC(bufin[1]) >> 4);
      *(bufout++) = (unsigned char) (DEC(bufin[1]) << 4 | DEC(bufin[2]) >> 2);
      *(bufout++) = (unsigned char) (DEC(bufin[2]) << 6 | DEC(bufin[3]));
      bufin += 4;
      nprbytes -= 4;
   }
   
   if(nprbytes & 03) {
      if(pr2six[(int)bufin[-2]] > MAXVAL) {
         nbytesdecoded -= 2;
      } else {
         nbytesdecoded -= 1;
      }
   }

   return(nbytesdecoded);
}

// ============================================================================

#include <sys/stat.h>
#if !defined (EEXIST)
#  define EEXIST 17
#endif

int xFile::Exists(void)
{
  int ok= 1;

#if defined(WIN32) && defined(_UNICODE)
  FILE* f= ::_wfopen(Path(), _T("rb"));
#else
  FILE* f= ::fopen(Path(), "rb");
#endif
  if (f == 0) {
    ok= 0;
  } else {
    ::fclose(f);
  }

  return ok;
}

int xFile::Delete(void)
{
  int ok= 1;

#if defined(WIN32) && defined(_UNICODE)
  int res= _wunlink(Path());
#else
  int res= unlink(Path());
#endif
  if (res != 0) {
    ok= 0;
  }

  return ok;
}

int xFile::Age(void)
{
  int age= -1;

#if defined(WIN32) && defined(_UNICODE)
  struct _stat statbuf;
  if (_wstat(Path(), &statbuf) == 0) {
#else
  struct stat statbuf;
  if (stat(Path(), &statbuf) == 0) {
#endif
    age= time(0) - statbuf.st_ctime;
  }

  return age;
}

int xFile::Load()
{
  int ok= 1;

#if defined(WIN32) && defined(_UNICODE)
  FILE* f= ::_wfopen(Path(), _T("rb"));
#else
  FILE* f= ::fopen(Path(), "rb");
#endif
  if (f == 0) {
    ok= 0;
  } else {
    ok= 1; // revert ok logic
    unsigned char buf[16384];

    int done= 0;
    while (!done && ok) {
      int n= ::fread(buf, sizeof( char ), sizeof(buf), f );
      if (n > 0) {
        ok= this->Append(buf, n);
      } else {
        done= 1;
        if (! feof(f) || ferror(f)) ok= 0;
      }
    }
    ::fclose(f);
  }

  return ok;
}

int xFile::CreatePath()
{
  int ok = 1;

  int first = 1;
  int done = 0;
  String path = Path();
  String dirpath;
  String part;
  while (!done && path.nextToken("\\/", part)) {
    if (first && part.empty()) {
      done = 1;
    } else {
      first = 0;
      if (! part.empty()) {
        if (path.empty()) {
          done = 1;
        } else {
          dirpath += part;
        
          int res = SSystem::mkdir(dirpath.t_str());
          if (res == -1) {
            if (errno != EEXIST) {
              ok = 0;
              done = 1;
            }
          }
          dirpath += "/";

        }
      }
    }
  }

  return ok;
}

int xFile::Save(int nFlags)
{
  int ok= 1;

  if (nFlags && SaveFlag_CreatePath) {
    ok= CreatePath();
  }

  if (ok) {
#if defined(WIN32) && defined(_UNICODE)
    FILE* f= ::_wfopen(Path(), _T("wb"));
#else
    FILE* f= ::fopen(Path(), "wb");
#endif
    if (f == 0) {
      ok= 0;
    } else {
      ok= 1; // revert ok logic

      int n= ::fwrite( this->Data(), 1, this->Length(), f );
      if (n < this->Length()) {
        ok = 0;
      }

      ::fclose(f);
    }
  }

  return ok;
}

int xFile::Rename(String& sNewPath)
{
  int ok = 1;
  
  String sOldPath = Path();
  if (ok) {
    Path(sNewPath);
    if (Exists()) { 
      ok = Delete(); 
    }
  }
  if (ok) {
#if defined(WIN32) && defined(_UNICODE)
    ok = (0 == ::_wrename((const wchar_t*)sOldPath, (const wchar_t*)Path()));
#else
    ok = (0 == ::rename((const char*) sOldPath, (const char*) Path()));
#endif
  }
  
  return ok;
}

int xFile::GetData(Buffer& sbData)
{
  int ok= 1;

  if (this->Data() == 0) {
    ok= this->Load();
  }
  if (ok) {
    sbData.SetData(this->Data(), this->Length());
  }

  return ok;
}

int xFile::GetData(String& sData)
{
  // String from file data: append trailing 0

  Flexbuf<char> buf(Length() + 1);
  if (buf) {
    ::memcpy((char*) buf, Data(), Length());
    ((char*) buf)[Length()] = 0;
    sData = (char*) buf;
    return 1;
  }

  return 0;
}

int xFile::SetData(Buffer& sbData)
{
  return super::SetData(sbData.Data(), sbData.Length());
}

int xFile::AppendToFile(unsigned char* pData, size_t nLen, int nFlags)
{
  int ok= 1;

  if (nFlags && SaveFlag_CreatePath) {
    ok= CreatePath();
  }

  if (ok) {
#if defined(WIN32) && defined(_UNICODE)
    FILE* f= ::_wfopen(Path(), _T("ab"));
#else
    FILE* f= ::fopen(Path(), "ab");
#endif
    if (f == 0) {
      ok= 0;
    } else {
      ok= 1; // revert ok logic

      size_t n= ::fwrite(pData, 1, nLen, f);
      if (n < nLen) {
        ok= 0;
      }

      ::fclose(f);
    }
  }

  return ok;
}

// ============================================================================

List::List()
{
  ClearList();
}

List::List(const String& sName)
{
  ClearList();
  setName(sName);
}

List::List(List& l)
: Elem()
{
  *this = l;
}

List::~List()
{
  Empty();
}

void List::ClearList()
{
  pHead_ = 0;
  nElem_ = 0;
}

// --------------------------------
// Add/Remove

Elem* List::Add(Elem* e)
{
  if (e != 0) {
    e->pLink_ = pHead_;
    pHead_ = e;
    nElem_++;
    return e;
  }
  return e;
}

Elem* List::Add(const String& sName)
{
  Elem* e = new Elem();
  if (e != 0) {
    e->setName(sName);
  	return Add(e);
  }
  return e;
}

Elem* List::Add(const String& sName, const String& sStr)
{
  Elem* e = new Elem();
  if (e != 0) {
    e->setName(sName);
    e->setString(sStr);
  	return Add(e);
  }
  return e;
}

Elem* List::Add(const String& sName, int nValue)
{
  Elem* e = new Elem();
  if (e != 0) {
    e->setName(sName);
    e->setInt(nValue);
  	return Add(e);
  }
  return e;
}

Elem* List::AddLast(const String& sName)
{
  Elem* e = new Elem();
  if (e != 0) {
    e->setName(sName);
  	AddLast(e);
  }
  return e;
}

Elem* List::AddLast(const String& sName, const String& sStr)
{
  Elem* e = new Elem();
  if (e != 0) {
    e->setName(sName);
    e->setString(sStr);
  	return AddLast(e);
  }
  return e;
}

Elem* List::AddLast(const String& sName, int nValue)
{
  Elem* e = new Elem();
  if (e != 0) {
    e->setName(sName);
    e->setInt(nValue);
  	return AddLast(e);
  }
  return e;
}

Elem* List::AddFirst(Elem* e)
{
  (void) Add(e);
  return e;
}

Elem* List::AddFirst(const String& sName)
{
  return Add(sName);
}

Elem* List::AddLast(Elem* e)
{
  Elem** pReferer = 0;
  bool bFound = false;
  
  if (e != 0) {
    pReferer = &(pHead_);
    while (! bFound) {
      if (*pReferer == 0) {
        bFound = true;
        *pReferer = e;
        e->pLink_ = 0;
        nElem_++;
      } else {
        pReferer = (Elem**) &((*pReferer)->pLink_) ;
      }
    }
  }
  return e;
}

Elem* List::AddAfter(Elem* ePrevious, Elem* e)
{
  if (ePrevious == 0) {
    pHead_ = e;
    e->pLink_ = 0;
  } else {
    e->pLink_ = ePrevious->pLink_;
    ePrevious->pLink_ = e;
  }
  nElem_++;
  return e;
}

Elem* List::Insert(Elem* ePrevious, Elem* e)
{
  e->pLink_= ePrevious->pLink_;
  ePrevious->pLink_= e;
  nElem_++;
  return e;
}

void List::Remove(Elem* e)
{
  FindElemCore(&Elem_FindByObjectRefFilter, (void*) e, true);
}  

// --------------------------------
// Use

String List::Collapse(const char* szSep)
{
  String sResult;
  for (Elem* e = 0; (e = Next(e)) != 0; ) {
    if (!sResult.empty()) { sResult += szSep; }
    sResult += e->getName();
  }
  return sResult;
}

void List::Empty()
{
  Elem* eSub = 0;
  while (pHead_ != 0) {
    eSub = pHead_;
    Remove(eSub);
    delete eSub;
  }
}

Elem* List::Next(Elem* e)
{
  Elem* eResult= 0;
  
  if (e== 0) {
    eResult = pHead_;
  } else {
    eResult = (Elem*) (e->pLink_);
  }
  return eResult;
}

Elem* List::First()
{
  return Next(0);
}
  
long List::NumElem()
{
  return nElem_;
}

bool List::IsEmpty()
{
  return (nElem_ == 0);
}

Elem* List::Last()
{
  Elem* eResult = 0;
  Elem* eTmp = pHead_;
  while (eTmp != 0) {
    eResult= eTmp;
    eTmp= (Elem*) eTmp->pLink_;
  }
  return eResult;
}

// --------------------------------
// Find

Elem* List::FindElemCore(ElemFindFilterProc fpFilter, void* nRefCon, bool dqFlag)
{
  Elem* eResult = 0;
  Elem** pReferer = 0;
  bool bFound = false;
  
  if (pHead_ != 0) {
    pReferer = &(pHead_);
    while (((*pReferer) != 0) && ! bFound) {
      if ((*fpFilter) (nRefCon, *pReferer)) {
        bFound = true;
      } else {
        pReferer =  (Elem**) &((*pReferer)->pLink_);
      }
    }
  }
  
  if (bFound) {
    eResult = *pReferer;
    if (dqFlag) {
      *pReferer = (Elem*) (*pReferer)->pLink_;
      nElem_--;
    }
  }
  
  return eResult;
}

Elem* List::FindByString(const String& sString)
{
	return FindElemCore(&Elem_FindByStringFilter, (void*) (const char*) sString, false);
}

Elem* List::FindByName(const String& sName)
{
  return FindElemCore(&Elem_FindByNameFilter, (void*) (const char*) sName, false);
}

Elem* List::FindByNameCase(const String& sName)
{
  return FindElemCore(&Elem_FindByNameCaseFilter, (void*) (const char*) sName, false);
}

Elem* List::FindByInt(int nValue)
{
  return FindElemCore(&Elem_FindByIntFilter, (void*) nValue, false);
}

Elem* List::FindByPtr(void* ptr)
{
  return FindElemCore(&Elem_FindByPtrFilter, ptr, false);
}

Elem* List::Find(ElemFindFilterProc fpFilter, void* p)
{
  return FindElemCore(fpFilter, p, false);
}

Elem* List::Find(Elem* e)
{
  return FindElemCore(&Elem_FindByObjectRefFilter, (void*) e, false);
}

void List::SortByName()
{
  Elem** a = new Elem*[length()];
  if (a != 0) {
    int nCnt = 0;
    while (First() != 0) {
      Elem* e = First();
      Remove(e);
      a[nCnt++] = e;
    }

    qsort((void*) a, (size_t) nCnt, sizeof(Elem*), List_SortByName_compare);

    for (int i = 0; i < nCnt; i++) {
      AddLast(a[i]);
    }

    delete [] a;
  }
}

Elem& List::operator[](const String& sName)
{
  Elem* e = FindByName(sName);
  if (e == 0) {
    e = new Elem(sName);
    if (e != 0) {
      Add(e);
    }
  }
  return *e;
}

Elem& List::operator[](int nIndex)
{
  Elem* e = FindByInt(nIndex);
  if (e == 0) {
    e = new Elem();
    if (e != 0) {
      e->setInt(nIndex);
      Add(e);
    }
  }
  return *e;
}

void List::operator=(List& l)
{
  ClearList();

  Elem::operator=(l);

  Elem* e = 0;
  while ((e= l.Next(e)) != 0) {
    Elem* eDuplicate = new Elem();
    *eDuplicate = *e;

    AddLast(eDuplicate);
  }
}

// ============================================================================

SExport void KeyValueBlob2List(const char* _text, List &_list, const char* _linesep, const char* _fieldsep, const char* _escape)
{
  if (_text != 0) {
    int fieldsep_len = String::strlen(_fieldsep);
    int len_in_bytes = (String::strlen(_text) + 1) * sizeof(char);
    char* p = (char*) SSystem::malloc(len_in_bytes);
    if (p != 0) {
      memcpy(p, _text, len_in_bytes);
      char* q= strtok(p, _linesep);
      while (q != 0) {
        char* r= 0;

        for (int i = 0; i < fieldsep_len; ++i) {
          char* rx= strchr(q, _fieldsep[i]);
          if (r == 0 && rx != 0) r = rx;
          if (rx != 0 && rx < r) r = rx;
        }

        if (r != 0) {
          *r++= 0;
          while (strchr(_fieldsep, *r) && *r != '\0') r++;
        } else {
          r = (char*)"";
        }

        _list.AddLast(q, r);
        q= strtok(0, _linesep);
      }
    }
    SSystem::free(p);
  }
}

SExport void KeyValueLfBlob2List(const char* _text, List &_list) { KeyValueBlob2List((const char* ) _text, _list, "\r\n", "=: \t", ""); }

SExport int List_SortByName_compare( const void *arg1, const void *arg2 )
{
  if (arg1 != 0 && arg2 != 0) {
    Elem** p1 = (Elem**) arg1;
    Elem** p2 = (Elem**) arg2;
    Elem* e1 = *p1;
    Elem* e2 = *p2;

    if (e1->getName() < e2->getName()) {
      return -1;
    } else if (e1->getName() > e2->getName()) { 
      return 1; 
    }
  }
  return 0;
}
