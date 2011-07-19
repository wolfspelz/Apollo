// ============================================================================

#include "SSystem.h"

String::String(void)
{
  init();
}

String::String(const char* szBegin)
{
  init();
  if (szBegin != 0) {
    append((char*) szBegin, countBytes(szBegin));
  }
}

#if defined(WIN32) && defined(UNICODE)
String::String(const PWSTR wzBegin)
{
  init();
  operator=(wzBegin);
}

String::String(const PWSTR wzBegin, size_t nLen)
{
  init();
  set(wzBegin, nLen);
}
#endif

String::String(String const &s)
{
  init();
  append(((String*) &s)->c_str(), ((String*) &s)->bytes());
}

String::~String(void)
{
  clear();
}

void String::init(void)
{
  szStr_ = 0;
  nAllocated_ = 0;
  nBytes_ = 0;
  nChars_ = 0;
#if defined(WIN32) && defined(UNICODE)
  wzStr_ = 0;
#endif
  //pStatic_[0] = '\0';
}

void String::clear(void)
{
  if (szStr_ != 0) {
    //if (szStr_ == pStatic_) {
    //  pStatic_[0] = '\0';
    //} else {
      SSystem::free(szStr_);
    //}
  }
  szStr_ = 0;
  nAllocated_ = 0;
  nBytes_ = 0;
  nChars_ = 0;
#if defined(WIN32) && defined(UNICODE)
  if (wzStr_ != 0) { SSystem::free(wzStr_); }
  wzStr_ = 0;
#endif
}

// ------------------------------------
// utf-8

// This table maps from the first byte of a UTF-8 character to the number
// of bytes. A value '0' indicates that the table key
// is not a legal first byte for a UTF-8 character.
static const unsigned char String_UTF8_Len[256]  = {
// 0xxxxxxx
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,

// 10wwwwww
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

// 110yyyyy
2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,

// 1110zzzz
3, 3, 3, 3, 3, 3, 3, 3,     3, 3, 3, 3, 3, 3, 3, 3,

// 11110yyy
4, 4, 4, 4, 4, 4, 4, 4,     0, 0, 0, 0, 0, 0, 0, 0,
};


unsigned int String::UTF8_CharSize(const char* c)
{
  return String_UTF8_Len[* (const unsigned char*) c];
}

String::UTF8Char String::UTF8_Char(const char* c)
{
  switch (String_UTF8_Len[* (const unsigned char*) c]) {
  case 0: return UTF8Char_Zero;
  case 1: return (UTF8Char) (  ((const unsigned char*) c)[0] );
  case 2: return (UTF8Char) ( (((const unsigned char*) c)[0] << 8) +   ((const unsigned char*) c)[1] );
  case 3: return (UTF8Char) ( (((const unsigned char*) c)[0] << 16) + (((const unsigned char*) c)[1] << 8) +   ((const unsigned char*) c)[2] );
  case 4: return (UTF8Char) ( (((const unsigned char*) c)[0] << 24) + (((const unsigned char*) c)[1] << 16) + (((const unsigned char*) c)[2] << 8) + ((const unsigned char*) c)[3] );
  default:;
  }
  return (UTF8Char) 0;
}

// p is a UTF-8 encoded unicode string. If nByte is less than zero,
// return the number of unicode characters in p up to (but not including)
// the first 0x00 byte. If nByte is not less than zero, return the
// number of unicode characters in the first nByte of p (or up to 
// the first 0x00, whichever comes first).
int String::UTF8_CharLen(const char *p, int nByte)
{
  int nChars = 0;
  const char *pEnd;
  if (nByte >= 0) {
    pEnd = &p[nByte];
  } else {
    pEnd = (const char*) (-1);
  }
  while (*p != '\0' && p < pEnd) {
    int nLen = UTF8_CharSize(p);
    if (nLen <= 0) {
      // not a UTF8 legal first byte
      break;
    } else {
      p += nLen;
      nChars++;
    }
  }
  return nChars;
}

// p is a UTF-8 encoded unicode string. If nChar is less than zero,
// return the number of bytes in p up to (but not including)
// the first 0x00 byte. If nChar is not less than zero, return the
// number of bytes in the first nChar of p (or up to 
// the first 0x00, whichever comes first).
int String::UTF8_ByteLen(const char *p, int nChar)
{
  int nBytes = 0;
  if (nChar < 0) {
    nChar = INT_MAX;
  }
  while (*p != '\0' && nChar > 0) {
    unsigned int n = UTF8_CharSize(p);
    nBytes += n;
    p += n;
    nChar--;
  }
  return nBytes;
}

// ------------------------------------
// set

String& String::operator=(const char* szBegin)
{
  if (szBegin != 0) {
    append((char*) szBegin, countBytes(szBegin), 1);
  }
  return *this;
}

String& String::operator=(const String& s)
{
  append(s.c_str(), s.bytes(), 1);
  return *this;
}

void String::set(const String& s)
{
  if (!s.empty()) {
    append(s.c_str(), s.bytes(), 1);
  }
}

void String::set(const char* szBegin, size_t nLen)
{
  if (szBegin != 0 && nLen > 0) {
    append(szBegin, nLen, 1);
  }
}

void String::append(const char* szBegin, size_t nBytes, int bClear)
{
  // Do not copy onto itself, but allow apppend to self
  if (szStr_ == szBegin && bClear) { return; }

  if (bClear) {
    clear();
  }

  if (szBegin != 0 && nBytes > 0) {
    size_t nLen1InChars = nChars_;
    size_t nLen2InChars = countCharacters(szBegin, nBytes);
    size_t nLen1InBytes = nBytes_;
    size_t nLen2InBytes = nBytes;
    size_t nLenInBytes = nLen1InBytes + nLen2InBytes;

    if (nLen2InChars > 0 && nLen2InBytes > 0) {

      if (nAllocated_ > nLenInBytes) { // ">" because of the 0
        ::memcpy(szStr_ + nLen1InBytes, szBegin, nLen2InBytes);
        szStr_[nLen1InBytes + nLen2InBytes] = '\0';

      } else {

        size_t nAllocate = (nLenInBytes > nAllocated_) ? 2 * nLenInBytes : 2 * nAllocated_;
        nAllocate = (nAllocate < 64 ? 64 : nAllocate);

        char* szTmp = (char*) SSystem::malloc(nAllocate);
        if (szTmp == 0) {
          return; // no change due to mem alloc problem
        }

        nAllocated_ = nAllocate;
        if (szStr_ != 0) {
          memcpy(szTmp, szStr_, nLen1InBytes);
        }
        memcpy(szTmp + nLen1InBytes, szBegin, nLen2InBytes);
        szTmp[nLen1InBytes + nLen2InBytes] = '\0';

        if (szStr_ != 0) { SSystem::free(szStr_); }
        szStr_ = szTmp;
      }

      nBytes_ = nLen1InBytes + nLen2InBytes;
      nChars_ = nLen1InChars + nLen2InChars;
    }
  }
}

String& String::append(const char* szBegin, const char* szEnd)
{
  if (szBegin != 0 && szEnd != 0) {
    append(szBegin, (size_t) (szEnd - szBegin));
  }
  return *this;
}

String& String::append(const char* szBegin)
{ 
  if (szBegin != 0) {
    append(szBegin, countBytes(szBegin));
  }
  return *this;
}

void String::operator+=(const char* szBegin)
{
  if (szBegin != 0) {
    append(szBegin, countBytes(szBegin), 0);
  }
}

void String::operator+=(const String& s)
{
  if (!s.empty()) {
    append(s.c_str(), s.bytes(), 0);
  }
}

#if defined(WIN32) && defined(UNICODE)
void String::append(const PWSTR wzBegin, size_t nLen, int bClear)
{
  if (wzBegin != 0 && nLen > 0) {
    unsigned int nChars = ::wcslen(wzBegin);
    if (nChars > nLen) {
      nChars = nLen;
    }
    unsigned int nBufLen = nChars * 4 + 1;
    Flexbuf<char> pBuf(nBufLen);
    unsigned int nBytes = ::WideCharToMultiByte(CP_UTF8, 0, wzBegin, nChars, (char*) pBuf, nBufLen, 0, 0);
    if (nBytes > 0) {
      append((char*) pBuf, nBytes, bClear);
    }
  }
}

void String::set(const PWSTR wzBegin, size_t nLen)
{
  if (wzBegin != 0 && nLen > 0) {
    append(wzBegin, nLen, 1);
  }
}

String& String::operator=(PWSTR wzBegin)
{
  if (wzBegin != 0) {
    unsigned int nChars = ::wcslen(wzBegin);
    unsigned int nBufLen = nChars * 4 + 1;
    Flexbuf<char> pBuf(nBufLen);
    unsigned int nBytes = ::WideCharToMultiByte(CP_UTF8, 0, wzBegin, -1, (char*) pBuf, nBufLen, 0, 0);
    if (nBytes > 0) {
      append((char*) pBuf, nBytes - 1, 1);
    }
  }
  return *this;
}
#endif

#if defined(WIN32)
  #if defined(vsnprintf)
    #undef vsnprintf
  #endif
  #define vsnprintf _vsnprintf
#endif

#include <stdarg.h>

#define String_appendf_STATIC_STRING_LENGTH 8192

int String::appendf(const char* szFmt, ...)
{
  va_list argp;
  // default buffer large enough for most tasks
  char szBuf[(String_appendf_STATIC_STRING_LENGTH+1)], *pBuf = szBuf, *pVarBuf = 0;
  int nLenInBytes = 0;
  *szBuf= '\0';

  va_start (argp, szFmt);
  nLenInBytes = vsnprintf(pBuf, String_appendf_STATIC_STRING_LENGTH, szFmt, argp);
  va_end(argp);
  // make a heap buffer, if default buffer too small
  // retry vsnprintf and increase the heap buffer until the thing fits
  if (nLenInBytes < 0) {
    int done= 0;
    size_t nAllocSizeInBytes = nBytes_ > (int) String_appendf_STATIC_STRING_LENGTH ? nBytes_ : (int) String_appendf_STATIC_STRING_LENGTH;
    while (!done) {
      if (pVarBuf != 0) {
        SSystem::free(pVarBuf);
      }
      nAllocSizeInBytes *= 2;
      pVarBuf = (char*) SSystem::malloc(nAllocSizeInBytes + 1);
      if (pVarBuf != 0) {
        pBuf = pVarBuf;
        va_start(argp, szFmt);
        nLenInBytes = vsnprintf(pBuf, nAllocSizeInBytes, szFmt, argp);
        va_end(argp);
        done = (nLenInBytes != -1);
      }
      if (done) {
        pBuf[nAllocSizeInBytes] = '\0';
      }
    }
  }

  append(pBuf, nLenInBytes);

  if (pVarBuf != 0) {
    SSystem::free(pVarBuf);
  }

  return nLenInBytes;
}

// ------------------------------------
// internal

unsigned int String::countBytes(const char* szStr)
{
  return ::strlen(szStr);
}

unsigned int String::countCharacters(const char* szStr, unsigned int nBytes)
{
  if (szStr == 0) {
    return 0;
  } else {
    if (nBytes > 0) {
      return UTF8_CharLen(szStr, nBytes);
    } else {
      return UTF8_CharLen(szStr, -1);
    }
  }
}

// ------------------------------------

int String::strcmp(const char* s1, const char* s2)
{ 
  return ::strcmp(s1, s2);
}

int String::strncmp(const char* s1, const char* s2, size_t n)
{ 
  return ::strncmp(s1, s2, n);
};

int String::strncasecmp(const char* s1, const char* s2, size_t n)
{
#if defined(WIN32)
  return ::_strnicmp(s1, s2, n);
#else
  return ::strncasecmp(s1, s2, n);
#endif
}

int String::stricmp(const char* s1, const char* s2)
{ 
#if defined(WIN32)
  return ::_stricmp(s1, s2);
#else
  return ::strcasecmp(s1, s2);
#endif
};

const char* String::strstr(const char* s1, const char* s2)
{ 
  return ::strstr(s1, s2);
};

size_t String::strlen(const char* s)
{ 
  return ::strlen(s);
};

char* String::strncpy(char* dest, const char* src, size_t bufSize)
{
  if (strlen(src) < bufSize) { // < because of trailing \0
    return ::strcpy(dest, src);
  } else {
    String sSrc(src);
    return strcpy(dest, (const char*) sSrc.subString(0, bufSize - 1));
  }
};

// ------------------------------------
// get

const char* String::c_str() const
{
  if (szStr_ == 0) {
    return "";
  } else {
    return szStr_;
  }
}

#if defined(WIN32)

#if defined(_UNICODE)
wchar_t* String::w_str() const
{
  if (szStr_ == 0) {
    return L"";
  } else {
    size_t nAllocate = (nChars_ + 2) * sizeof(wchar_t);
    if (wzStr_ != 0) {
      int x = 0;
    }
    if (wzStr_ != 0)  { wzStr_ = (wchar_t*) SSystem::realloc(wzStr_, nAllocate); } 
    else              { wzStr_ = (wchar_t*) SSystem::malloc(nAllocate); } 
    if (wzStr_ != 0) {
      ::MultiByteToWideChar(CP_UTF8, 0, szStr_, -1, wzStr_, nAllocate);
    }
  }
  return (wzStr_ == 0 ? L"" : wzStr_);
}
#endif // defined(_UNICODE)

const TCHAR* String::t_str() const
{
  #if !defined(_UNICODE)
  return c_str();
  #else
  return w_str();
  #endif
}
#endif

unsigned int String::bytes(void) const
{
  return nBytes_;
}

unsigned int String::chars(void) const
{
  return nChars_;
}

bool String::empty() const
{
  return szStr_ == 0 || *szStr_ == '\0';
}

bool String::operator==(const char* s) const
{
  if (szStr_ == 0 ) {
    if (s == 0 || *s == '\0') return 1;
    else return 0;
  } else if (s == 0) {
    if (szStr_ == 0 || *szStr_ == '\0') return 1;
    else return 0;
  } else {
    int bUnequal = String::strcmp(szStr_, s);
    return bUnequal == 0;
  }
}
bool String::operator!=(const char* s) const { return !operator==(s); }
bool String::operator==(String& s) const { return operator==((const char*) s); }
bool String::operator!=(String& s) const { return !operator==((const char*) s); }

int String::operator<(const String& s) const
{
  return (szStr_ && s.szStr_) ? String::strcmp(szStr_, s.szStr_) < 0 : ((s.szStr_) ? 1 : 0 );
}

int String::operator>(const String& s) const
{
  return ! operator<(s);
}

// ------------------------------------
// tools

String String::subString(int nFirst, int nCount) const
{
  String sSub;
  if (szStr_ != 0) {
    int nFirstLen = UTF8_ByteLen(szStr_, nFirst);
    const char* szBegin = &(szStr_[nFirstLen]);
    int nCountLen = UTF8_ByteLen(szBegin, nCount);
    sSub.append(szBegin, nCountLen);
  }
  return sSub;
}

int String::startsWith(const char* szBegin) const
{
  if (szStr_ == 0) { return 0; }
  String sPrefix = subString(0, UTF8_CharLen(szBegin));
  if (sPrefix.empty()) { return 0; }
  return sPrefix == szBegin;
}

int String::endsWith(const char* szEnd) const
{
  if (szStr_ == 0) { return 0; }
  String sSuffix = subString(chars() - UTF8_CharLen(szEnd));
  return sSuffix == szEnd;
}

int String::contains(const char* szBegin) const
{
  if (szStr_ == 0) { return 0; } 
  return (String::strstr(szStr_, szBegin) != 0);
}

const char* String::findChar(String::UTF8Char nC)
{
  const char* p = c_str();
  const char* q = 0;

  int bFound = 0;
  while (*p != '\0' && !bFound) {
    bFound = (UTF8_Char(p) == nC);
    q = p;
    p += UTF8_CharSize(p);
  }
  return bFound ? q : 0;
};

const char* String::findChar(const char* szChars)
{
  if (szStr_ == 0) { return 0; }

  const char* pPos = 0;

  const char* p = szStr_;
  String sChars = szChars;
  while (sChars.findChar(UTF8_Char(p)) == 0 && *p != '\0') {
    p += UTF8_CharSize(p);
  }
  if (*p != '\0') { pPos = p; }

  return pPos;
}

const char* String::findNotChar(const char* szChars)
{
  if (szStr_ == 0) { return 0; }

  const char* pPos = 0;

  const char* p = szStr_;
  String sChars = szChars;
  while (sChars.findChar(UTF8_Char(p)) != 0 && *p != '\0') {
    p += UTF8_CharSize(p);
  }
  if (*p != '\0') { pPos = p; }

  return pPos;
}

int String::nextToken(const char* szSep, String& sPart)
{
  if (empty()) { return 0; } // do nothing

  int bFound = 0;

  const char* pSep = findChar(szSep);
  if (pSep != 0) {
    sPart.append(szStr_, pSep - szStr_, 1);
    String sRemain;
    sRemain.append(pSep, nBytes_ - (pSep - szStr_));
    const char* pNext = sRemain.findNotChar(szSep);
    if (pNext == 0) {
      clear();
    } else {
      append(pNext, sRemain.bytes() - (pNext - sRemain.c_str()), 1);
    }
    bFound = 1;
  } else {
    if (nBytes_ > 0) {
      sPart = szStr_;
      clear();
      bFound = 1;
    } else {
      sPart.clear();
    }
  }

  return bFound;
}

void String::trim(const char* szChars)
{
  if (empty()) { return; } // do nothing

  // Find prefix
  const char* p = szStr_;
  String sChars = szChars;
  while (sChars.findChar(UTF8_Char(p)) != 0 && *p != '\0') {
    p += UTF8_CharSize(p);
  }

  // Find suffix
  const char* q = szStr_ + nBytes_;
  int bDone = 0;
  do {
    q--;
    if (UTF8_CharSize(q)) {
      if (sChars.findChar(UTF8_Char(q)) == 0) {
        q += UTF8_CharSize(q);
        bDone = 1;
      }
    }
  } while (q >= szStr_ && !bDone);

  // Strip em
  if (p != szStr_ || q != szStr_ + nBytes_) {
    String sTmp;
    sTmp.append(p, q-p);
    append(sTmp.c_str(), sTmp.bytes(), 1);
  }
}

int String::reverseToken(const char* szSep, String& sPart)
{
  if (empty()) { return 0; } // do nothing

  int bFound = 0;
  String sSep = szSep;

  const char* p = szStr_ + nBytes_;
  int bDone = 0;
  if (p <= szStr_) {
    sPart.clear();
    // !found
  } else {
    do {
      p--;
      if (UTF8_CharSize(p)) {
        if (sSep.findChar(UTF8_Char(p)) != 0) {
          p += UTF8_CharSize(p);
          bDone = 1;
        }
      }
    } while (p > szStr_ && !bDone);
    sPart = p;
    bFound = !sPart.empty();
  }

  const char* q = p;
  bDone = 0;
  if (q <= szStr_) {
    clear();
  } else {
    do {
      q--;
      if (UTF8_CharSize(q)) {
        if (sSep.findChar(UTF8_Char(q)) == 0) {
          q += UTF8_CharSize(q);
          bDone = 1;
        }
      }
    } while (q > szStr_ && !bDone);
    String sTmp;
    sTmp.append(szStr_, q - szStr_);
    append(sTmp.c_str(), sTmp.bytes(), 1);
  }

  return bFound;
}

String String::filenameExtension(const char* szText)
{
  String sExt;
  String sText = szText;
  if (sText.findChar(".")) {
    sText.reverseToken(".", sExt);
    if (sText.empty()) {
      sExt.clear();
    }
  }
  return sExt;
}

String String::filenameBasePath(const char* szText)
{
  String sPath = szText;
  String sPart;

  if (sPath.endsWith("/")
    #if defined(WIN32)
    || sPath.endsWith("\\")
    #endif
    )
  {
      // Already is a base path
  }
  else
  {
    if (sPath.reverseToken("/" 
      #if defined(WIN32)
      "\\"
      #endif
      , sPart)) {
      sPath.append(szText + sPath.bytes(), ::strlen(szText) - (sPath.bytes() + sPart.bytes()));
    }
  }

  return sPath;
}

String String::filenameFile(const char* szText)
{
  String sPath = szText;
  String sPart;
  sPath.reverseToken("/" 
#if defined(WIN32)
    "\\"
#endif
    , sPart);
  return sPart;
}

bool String::filenameIsAbsolutePath(const char* szText)
{
  String sText = szText;
  return 
#if defined(WIN32)
      sText.subString(1, 1) == ":" || sText.startsWith("/") || sText.subString(0, 2) == String_FILEPATHSEPARATOR String_FILEPATHSEPARATOR
#else
      sText.startsWith(String_FILEPATHSEPARATOR);
#endif
    ;
}

String String::filenamePathSeparator()
{
  return String_FILEPATHSEPARATOR;
}

String& String::makeTrailingSlash(const char* szAppend)
{
#if defined(WIN32)
  if (!endsWith("\\") && !endsWith("/")) {
#else
  if (!endsWith("/")) {
#endif
    String sAppend;
    if (szAppend != 0) {
      sAppend = szAppend;
    } else {
      sAppend = filenamePathSeparator();
    }
    append(sAppend);
  }

  return *this;
}

String String::reverse(const char* szText)
{
  String sText = szText;
  String sReverse;

  int nLength = sText.bytes();
  Flexbuf<char> buf(nLength);
  char* p = (char*) sText.c_str();
  char* q = (char*) buf + nLength;
  while (*p != 0) {
    int n = UTF8_CharSize(p);
    q -= n;
    switch (n) {
      case 3: *q++ = *p++;
      case 2: *q++ = *p++;
      case 1: *q++ = *p++;
    }
    q -= n;
  }

  sReverse.set((char*) buf, nLength);
  return sReverse;
}

String String::truncate(const char* szText, int nLen, const char* szTail)
{
  String sHead;
  String sText = szText;
  String sTail = szTail;
  int nBytes = UTF8_ByteLen(sText, nLen - UTF8_CharLen(sTail));
  sHead.set(sText.c_str(), nBytes);
  sHead += sTail;
  return sHead;
}

String String::toLower(const char* szText)
{
  String sLower = szText;

  char* p = (char*) sLower.c_str();
  while (*p != 0) {
    int n = UTF8_CharSize(p);
    if (n == 1) {
      *p = ::tolower(*p);
    }
    p += n;
  }

  return sLower;
}

String String::toUpper(const char* szText)
{
  String sLower = szText;

  char* p = (char*) sLower.c_str();
  while (*p != 0) {
    int n = UTF8_CharSize(p);
    if (n == 1) {
      *p = ::toupper(*p);
    }
    p += n;
  }

  return sLower;
}

int String::isDigit(const char* szText)
{
  int is = 0;

  if (UTF8_CharSize(szText) == 1) {
    int c = *(const unsigned char*)szText;
    is = (::isdigit(c) != 0);
  }

  return is;
}

int String::isPunct(const char* szText)
{
  int is = 0;

  if (UTF8_CharSize(szText) == 1) {
    int c = *szText;
    is = (::ispunct(c) != 0);
  }

  return is;
}

String String::from(int n)
{
  String s;
  s.appendf("%d", n);
  return s;
}

// ------------------------------------
// global

String operator+(const String& a, const String& b)
{
  String ab = a;
  ab += b;
  return ab;
}

String operator+(const String& a, const char* b)
{
  String sb(b);
  return a + sb;
}

String operator+(const char* a, const String& b)
{
  String sa(a);
  return sa + b;
}

// ------------------------------------
// escape

static String::StringPair gString_Table_EscapeEmpty[] = { {0, 0} };

static String::StringPair gString_Table_EscapeAMP[] = {
                                                    {"&", "&amp;"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeXML[] = {
                                                    {"&", "&amp;"},
                                                    {"<", "&lt;"},
                                                    {">", "&gt;"},
                                                    {"'", "&apos;"},
                                                    {"\"", "&quot;"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeDoubleQuotes[] = {
                                                    {"'", "''"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeCRLF[] = {
                                                    {"\n", "\\n"},
                                                    {"\r", "\\r"},
                                                    {"\\", "\\\\"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeLogLine[] = {
                                                    {"\n", "\\n"},
                                                    {"\r", "\\r"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeSlash[] = {
                                                    {"/", "\\/"},
                                                    {"\\", "\\\\"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeSpace[] = {
                                                    {" ", "\\ "},
                                                    {"\\", "\\\\"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeQuotes[] = {
                                                    {"'", "\\'"},
                                                    {"\"", "\\\""},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeURL[] = {
                                                    {"\x00", "%00"},
                                                    {"\x01", "%01"},
                                                    {"\x02", "%02"},
                                                    {"\x03", "%03"},
                                                    {"\x04", "%04"},
                                                    {"\x05", "%05"},
                                                    {"\x06", "%06"},
                                                    {"\x07", "%07"},
                                                    {"\x08", "%08"},
                                                    {"\x09", "%09"},
                                                    {"\n",   "%0A"},
                                                    {"\x0b", "%0B"},
                                                    {"\x0c", "%0C"},
                                                    {"\r",   "%0D"},
                                                    {"\x0e", "%0E"},
                                                    {"\x0f", "%0F"},
                                                    {"\x10", "%10"},
                                                    {"\x11", "%11"},
                                                    {"\x12", "%12"},
                                                    {"\x13", "%13"},
                                                    {"\x14", "%14"},
                                                    {"\x15", "%15"},
                                                    {"\x16", "%16"},
                                                    {"\x17", "%17"},
                                                    {"\x18", "%18"},
                                                    {"\x19", "%19"},
                                                    {"\x1a", "%1A"},
                                                    {"\x1b", "%1B"},
                                                    {"\x1c", "%1C"},
                                                    {"\x1d", "%1D"},
                                                    {"\x1e", "%1E"},
                                                    {"\x1f", "%1F"},
                                                    {"\x20", "+"},
                                                  //{"\x21", "%21"},
                                                    {"\x22", "%22"},
                                                    {"\x23", "%23"},
                                                  //{"\x24", "%24"},
                                                    {"\x25", "%25"},
                                                    {"\x26", "%26"},
                                                  //{"\x27", "%27"},
                                                  //{"\x28", "%28"},
                                                  //{"\x29", "%29"},
                                                  //{"\x2a", "%2A"},
                                                    {"\x2b", "%2B"},
                                                  //{"\x2c", "%2C"},
                                                  //{"\x2d", "%2D"},
                                                  //{"\x2e", "%2E"},
                                                    {"\x2f", "%2F"},
                                                  //{"\x30", "%30"},
                                                  //{"\x31", "%31"},
                                                  //{"\x32", "%32"},
                                                  //{"\x33", "%33"},
                                                  //{"\x34", "%34"},
                                                  //{"\x35", "%35"},
                                                  //{"\x36", "%36"},
                                                  //{"\x37", "%37"},
                                                  //{"\x38", "%38"},
                                                  //{"\x39", "%39"},
                                                    {":", "%3A"},
                                                  //{"\x3b", "%3B"},
                                                    {"\x3c", "%3C"},
                                                    {"\x3d", "%3D"},
                                                    {"\x3e", "%3E"},
                                                    {"\x3f", "%3F"},
                                                    {"\x40", "%40"},
                                                  //{"\x41", "%41"},
                                                  //{"\x42", "%42"},
                                                  //{"\x43", "%43"},
                                                  //{"\x44", "%44"},
                                                  //{"\x45", "%45"},
                                                  //{"\x46", "%46"},
                                                  //{"\x47", "%47"},
                                                  //{"\x48", "%48"},
                                                  //{"\x49", "%49"},
                                                  //{"\x4a", "%4A"},
                                                  //{"\x4b", "%4B"},
                                                  //{"\x4c", "%4C"},
                                                  //{"\x4d", "%4D"},
                                                  //{"\x4e", "%4E"},
                                                  //{"\x4f", "%4F"},
                                                  //{"\x50", "%50"},
                                                  //{"\x51", "%51"},
                                                  //{"\x52", "%52"},
                                                  //{"\x53", "%53"},
                                                  //{"\x54", "%54"},
                                                  //{"\x55", "%55"},
                                                  //{"\x56", "%56"},
                                                  //{"\x57", "%57"},
                                                  //{"\x58", "%58"},
                                                  //{"\x59", "%59"},
                                                  //{"\x5a", "%5A"},
                                                  //{"[", "%5B"},
                                                    {"\x5c", "%5C"},
                                                  //{"]", "%5D"},
                                                    {"\x5e", "%5E"},
                                                  //{"\x5f", "%5F"},
                                                    {"\x60", "%60"},
                                                  //{"\x61", "%61"},
                                                  //{"\x62", "%62"},
                                                  //{"\x63", "%63"},
                                                  //{"\x64", "%64"},
                                                  //{"\x65", "%65"},
                                                  //{"\x66", "%66"},
                                                  //{"\x67", "%67"},
                                                  //{"\x68", "%68"},
                                                  //{"\x69", "%69"},
                                                  //{"\x6a", "%6A"},
                                                  //{"\x6b", "%6B"},
                                                  //{"\x6c", "%6C"},
                                                  //{"\x6d", "%6D"},
                                                  //{"\x6e", "%6E"},
                                                  //{"\x6f", "%6F"},
                                                  //{"\x70", "%70"},
                                                  //{"\x71", "%71"},
                                                  //{"\x72", "%72"},
                                                  //{"\x73", "%73"},
                                                  //{"\x74", "%74"},
                                                  //{"\x75", "%75"},
                                                  //{"\x76", "%76"},
                                                  //{"\x77", "%77"},
                                                  //{"\x78", "%78"},
                                                  //{"\x79", "%79"},
                                                  //{"\x7a", "%7A"},
                                                    {"\x7b", "%7B"},
                                                  //{"|", "%7C"},
                                                    {"\x7d", "%7D"},
                                                    {"\x7e", "%7E"},
                                                    {"\x7f", "%7F"},
                                                    {"\x80", "%80"},
                                                    {"\x81", "%81"},
                                                    {"\x82", "%82"},
                                                    {"\x83", "%83"},
                                                    {"\x84", "%84"},
                                                    {"\x85", "%85"},
                                                    {"\x86", "%86"},
                                                    {"\x87", "%87"},
                                                    {"\x88", "%88"},
                                                    {"\x89", "%89"},
                                                    {"\x8a", "%8A"},
                                                    {"\x8b", "%8B"},
                                                    {"\x8c", "%8C"},
                                                    {"\x8d", "%8D"},
                                                    {"\x8e", "%8E"},
                                                    {"\x8f", "%8F"},
                                                    {"\x90", "%90"},
                                                    {"\x91", "%91"},
                                                    {"\x92", "%92"},
                                                    {"\x93", "%93"},
                                                    {"\x94", "%94"},
                                                    {"\x95", "%95"},
                                                    {"\x96", "%96"},
                                                    {"\x97", "%97"},
                                                    {"\x98", "%98"},
                                                    {"\x99", "%99"},
                                                    {"\x9a", "%9A"},
                                                    {"\x9b", "%9B"},
                                                    {"\x9c", "%9C"},
                                                    {"\x9d", "%9D"},
                                                    {"\x9e", "%9E"},
                                                    {"\x9f", "%9F"},
                                                    {"\xa0", "%A0"},
                                                    {"\xa1", "%A1"},
                                                    {"\xa2", "%A2"},
                                                    {"\xa3", "%A3"},
                                                    {"\xa4", "%A4"},
                                                    {"\xa5", "%A5"},
                                                    {"\xa6", "%A6"},
                                                    {"\xa7", "%A7"},
                                                    {"\xa8", "%A8"},
                                                    {"\xa9", "%A9"},
                                                    {"\xaa", "%AA"},
                                                    {"\xab", "%AB"},
                                                    {"\xac", "%AC"},
                                                    {"\xad", "%AD"},
                                                    {"\xae", "%AE"},
                                                    {"\xaf", "%AF"},
                                                    {"\xb0", "%B0"},
                                                    {"\xb1", "%B1"},
                                                    {"\xb2", "%B2"},
                                                    {"\xb3", "%B3"},
                                                    {"\xb4", "%B4"},
                                                    {"\xb5", "%B5"},
                                                    {"\xb6", "%B6"},
                                                    {"\xb7", "%B7"},
                                                    {"\xb8", "%B8"},
                                                    {"\xb9", "%B9"},
                                                    {"\xba", "%BA"},
                                                    {"\xbb", "%BB"},
                                                    {"\xbc", "%BC"},
                                                    {"\xbd", "%BD"},
                                                    {"\xbe", "%BE"},
                                                    {"\xbf", "%BF"},
                                                    {"\xc0", "%C0"},
                                                    {"\xc1", "%C1"},
                                                    {"\xc2", "%C2"},
                                                    {"\xc3", "%C3"},
                                                    {"\xc4", "%C4"},
                                                    {"\xc5", "%C5"},
                                                    {"\xc6", "%C6"},
                                                    {"\xc7", "%C7"},
                                                    {"\xc8", "%C8"},
                                                    {"\xc9", "%C9"},
                                                    {"\xca", "%CA"},
                                                    {"\xcb", "%CB"},
                                                    {"\xcc", "%CC"},
                                                    {"\xcd", "%CD"},
                                                    {"\xce", "%CE"},
                                                    {"\xcf", "%CF"},
                                                    {"\xd0", "%D0"},
                                                    {"\xd1", "%D1"},
                                                    {"\xd2", "%D2"},
                                                    {"\xd3", "%D3"},
                                                    {"\xd4", "%D4"},
                                                    {"\xd5", "%D5"},
                                                    {"\xd6", "%D6"},
                                                    {"\xd7", "%D7"},
                                                    {"\xd8", "%D8"},
                                                    {"\xd9", "%D9"},
                                                    {"\xda", "%DA"},
                                                    {"\xdb", "%DB"},
                                                    {"\xdc", "%DC"},
                                                    {"\xdd", "%DD"},
                                                    {"\xde", "%DE"},
                                                    {"\xdf", "%DF"},
                                                    {"\xe0", "%E0"},
                                                    {"\xe1", "%E1"},
                                                    {"\xe2", "%E2"},
                                                    {"\xe3", "%E3"},
                                                    {"\xe4", "%E4"},
                                                    {"\xe5", "%E5"},
                                                    {"\xe6", "%E6"},
                                                    {"\xe7", "%E7"},
                                                    {"\xe8", "%E8"},
                                                    {"\xe9", "%E9"},
                                                    {"\xea", "%EA"},
                                                    {"\xeb", "%EB"},
                                                    {"\xec", "%EC"},
                                                    {"\xed", "%ED"},
                                                    {"\xee", "%EE"},
                                                    {"\xef", "%EF"},
                                                    {"\xf0", "%F0"},
                                                    {"\xf1", "%F1"},
                                                    {"\xf2", "%F2"},
                                                    {"\xf3", "%F3"},
                                                    {"\xf4", "%F4"},
                                                    {"\xf5", "%F5"},
                                                    {"\xf6", "%F6"},
                                                    {"\xf7", "%F7"},
                                                    {"\xf8", "%F8"},
                                                    {"\xf9", "%F9"},
                                                    {"\xfa", "%FA"},
                                                    {"\xfb", "%FB"},
                                                    {"\xfc", "%FC"},
                                                    {"\xfd", "%FD"},
                                                    {"\xfe", "%FE"},
                                                    {"\xff", "%FF"},
                                                    {0, 0}
                                                 };

static String::StringPair gString_Table_EscapeTest[] = {
                                                    {"a", "A"},
                                                    {"b", "B"},
                                                    {"ab", "AB"},
                                                    {"ba", "BA"},
                                                    {"aab", "AAB"},
                                                    {"c", "CCC"},
                                                    {"ddd", "D"},
                                                    {"cd", "CD1"},
                                                    {"dc", "DC1"},
                                                    {"dddc", "DC3"},
                                                    {"ddddc", "DC4"},

                                                    {"\xE9\xA6\x96", "A"},
                                                    {"\xE9\xA0\x81", "B"},
                                                    {"\xE9\xA6\x96" "\xE9\xA0\x81", "AB"},
                                                    {"\xE9\xA0\x81" "\xE9\xA6\x96", "BA"},
                                                    {"\xE9\xA6\x96" "\xE9\xA6\x96" "\xE9\xA0\x81", "AAB"},
                                                    {"\xC3\xA4", "CCC"},
                                                    {"\xC3\xB6" "\xC3\xB6" "\xC3\xB6", "D"},
                                                    {"\xC3\xA4" "\xC3\xB6", "CD1"},
                                                    {"\xC3\xB6" "\xC3\xA4", "DC1"},
                                                    {"\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "\xC3\xA4", "DC3"},
                                                    {"\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "\xC3\xB6" "\xC3\xA4", "DC4"},

                                                    {"x", "\xE9\xA6\x96" },
                                                    {"y" "\xC3\xA4", "\xE9\xA0\x81" },

                                                    {0, 0}
                                                 };

static const int String_Escape_Match_none = -2;
static const int String_Escape_Match_partial = -1;

static int String_replaceMatch(String::StringPair* pTable, String& s, bool bReverse)
{
  int nIndex = 0;
  const char* szKey = 0;
  if (bReverse) {
    szKey = pTable[nIndex].szRight;
  } else {
    szKey = pTable[nIndex].szLeft;
  }

  while (szKey != 0) {
    if (String::strncmp(s, szKey, s.bytes()) == 0) {
      if (s.bytes() == String::strlen(szKey)) {
        return nIndex;
      } else {
        return String_Escape_Match_partial;
      }
    }
    nIndex++;
    if (bReverse) {
      szKey = pTable[nIndex].szRight;
    } else {
      szKey = pTable[nIndex].szLeft;
    }
  }

  return String_Escape_Match_none;
}

int String::replaceCore(StringPair* pTable, bool bReverse)
{
  int nReplaced = 0;
  String sNew;
  String sToken;

  const char* szSrc = c_str();
  while (*szSrc != '\0') {
    unsigned int nCharLen = UTF8_CharSize(szSrc);
    if (nCharLen == 0) {
      // Not a start character: skip
      nCharLen = 1;
    }
    sToken.append(szSrc, nCharLen);
    szSrc += nCharLen;
    int nMatch = String_replaceMatch(pTable, sToken, bReverse);
    switch (nMatch) {
      case String_Escape_Match_none:
        sNew += sToken;
        sToken.clear();
        break;
      case String_Escape_Match_partial:
        break;
      default:
        nReplaced++;
        sToken.clear();
        if (bReverse) {
          sNew += pTable[nMatch].szLeft;
        } else {
          sNew += pTable[nMatch].szRight;
        }
    }
  }

  append(sNew.c_str(), sNew.bytes(), 1);
  return nReplaced;
}

int String::escape(Escape_Type nType, bool bReverse)
{
  int nEscaped = 0;

  StringPair* pTable = gString_Table_EscapeEmpty;
  switch (nType) {
    case EscapeAMP: pTable = gString_Table_EscapeAMP; break;
    case EscapeXML: pTable = gString_Table_EscapeXML; break;
    case EscapeDoubleQuotes: pTable = gString_Table_EscapeDoubleQuotes; break;
    case EscapeCRLF: pTable = gString_Table_EscapeCRLF; break;
    case EscapeLogLine: pTable = gString_Table_EscapeLogLine; break;
    case EscapeSlash: pTable = gString_Table_EscapeSlash; break;
    case EscapeSpace: pTable = gString_Table_EscapeSpace; break;
    case EscapeQuotes: pTable = gString_Table_EscapeQuotes; break;
    case EscapeURL: pTable = gString_Table_EscapeURL; break;
    case EscapeTest: pTable = gString_Table_EscapeTest; break;
    case EscapeNone: break;
  }

  nEscaped = replaceCore(pTable, bReverse);

  return nEscaped;
}

int String::unescape(Escape_Type nType)
{
  return escape(nType, true);
}

// ------------------------------------
// replace

int String::replace(const char* szSearch, const char* szReplace)
{
  int nReplaced = 0;

  if (!empty()) {
    StringPair pTable[2];
    pTable[0].szLeft = szSearch;
    pTable[0].szRight = szReplace;
    pTable[1].szLeft = 0;
    pTable[1].szRight = 0;

    nReplaced = replaceCore(pTable, false);
  }

  return nReplaced;
}

int String::replace(List& lSearchReplace)
{
  int nReplaced = 0;

  if (!empty()) {
    Flexbuf<StringPair> list(lSearchReplace.length() + 1);
    StringPair* pTable = (StringPair*) list;

    int nCnt = 0;
    for(Elem* e = 0; (e = lSearchReplace.Next(e));) {
      pTable[nCnt].szLeft = e->getName();
      pTable[nCnt].szRight = e->getString();
      nCnt++;
    }
    pTable[nCnt].szLeft = 0;
    pTable[nCnt].szRight = 0;

    nReplaced = replaceCore(pTable, false);
  }
  
  return nReplaced;
}
