// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SString_h_INCLUDED)
#define SString_h_INCLUDED

#include "SSystem.h"
class List;

class SExport String
{
public:
// Core interface
  String(void);
  String(const char* szBegin);
#if defined(WIN32) && defined(UNICODE)
  String(const PWSTR wzBegin);
#endif
  String(String const &s);
  virtual ~String(void);
  void init(void);
  void clear(void);

  typedef enum _UTF8Char { UTF8Char_Zero = 0, UTF8Char_Max = INT_MAX } UTF8Char;
  //typedef unsigned int UTF8Char;

  // set, append
  String& operator=(const char* szBegin);
  String& operator=(const String& s);
  void set(const char* szBegin, size_t nLen);
  void set(const String& s);
  void append(const char* szBegin, size_t nBytes, int bClear = 0);
  String& append(const char* szBegin, const char* szEnd);
  String& append(const char* szBegin);
  void operator+=(const char* szBegin);
  void operator+=(const String& s);
#if defined(WIN32) && defined(UNICODE)
  void append(const PWSTR wzBegin, size_t nLen, int bClear = 0);
  void set(const PWSTR wzBegin, size_t nLen);
  String& operator=(PWSTR wzBegin);
#endif
  // Append formatted
  int appendf(const char* szFmt, ...);

  // get
  const char* c_str() const;
#if defined(WIN32)
  #if defined(UNICODE)
    WCHAR* w_str() const;
    inline operator WCHAR*(void) const { return w_str(); }
    inline operator const WCHAR*(void) const { return w_str(); }
  #endif
  const TCHAR* t_str() const;
#endif
  inline operator bool(void) const { return !empty(); }
  inline operator const char*(void) const { return c_str(); }
  unsigned int bytes(void) const;
  unsigned int chars(void) const;
  bool empty() const;
  bool isset() const { return !empty(); }
  int operator<(const String& s) const;
  int operator>(const String& s) const;
  bool operator==(const char* s) const;
  inline bool operator==(char* s) const { return operator==((const char*)s); }
  bool operator==(String& s) const;
  bool operator!=(const char* s) const;
  inline bool operator!=(char* s) const { return operator!=((const char*)s); }
  bool operator!=(String& s) const;

  String subString(int nFirstChar, int nCountChars = -1) const;
  int startsWith(const char* szBegin) const;
  int endsWith(const char* szEnd) const;
  int contains(const char* szBegin) const;
  const char* findChar(String::UTF8Char nC);
  const char* findChar(const char* szChars);
  const char* findNotChar(const char* szChars);
  int nextToken(const char* szSep, String& sPart);
  int reverseToken(const char* szSep, String& sPart);
  void trim(const char* szChars);
  inline void trimWSP() { trim(" \r\n\t"); }

  // Replaces szSearch with szReplace
  int replace(const char* szSearch, const char* szReplace);
  // Replaces e->getName() with e->getString()
  int replace(List& lSearchReplace);

  static bool filenameIsAbsolutePath(const char* szText);
  static String filenameExtension(const char* szText);
  static String filenameBasePath(const char* szText);
  static String filenameFile(const char* szText);
  static String filenamePathSeparator();
  void makeTrailingSlash();

  static String reverse(const char* szText);
  static String truncate(const char* szText, int nLen, const char* szTail = "...");
  static String toLower(const char* szText);
  static int isDigit(const char* szText);
  static int isPunct(const char* szText);
  static String from(int n);
  static int atoi(const String& s) { return ::atoi(s); }
  static double atof(const String& s) { return ::atof(s); }

  // escape
  typedef struct _StringPair {
    const char* szLeft;
    const char* szRight;
  } StringPair;
  typedef enum _Escape_Type { EscapeNone
    ,EscapeAMP // & only
    ,EscapeXML
    ,EscapeDoubleQuotes // ' -> ''
    ,EscapeCRLF // reversible single line (with \ -> \\) 
    ,EscapeLogLine // irreversible single line (no \ -> \\)
    ,EscapeSlash // reversible / -> \/ 
    ,EscapeSpace // reversible " " -> "\ "
    ,EscapeURL
    ,EscapeTest
  } Escape_Type;

  int replaceCore(StringPair* pTable, bool bReverse);
  int escape(Escape_Type nType, bool bReverse = false);
  int unescape(Escape_Type nType);

  // static
  static int strcmp(const char* s1, const char* s2);
  static int strncmp(const char* s1, const char* s2, size_t n);
  static int stricmp(const char* s1, const char* s2);
  static int strncasecmp(const char* s1, const char* s2, size_t n);
  static const char* strstr(const char* s1, const char* s2);
  // safer version with buffer size
  static char* strncpy(char* dest, const char* src, size_t bufSize);
  static size_t strlen(const char* s);
  static int UTF8_CharLen(const char *p, int nByte = -1);
  static int UTF8_ByteLen(const char *p, int nChar = -1);
  static UTF8Char UTF8_Char(const char* c);
  static unsigned int UTF8_CharSize(const char* c);

public:
  // For testing
  inline unsigned int allocated() const { return nAllocated_; }

protected:
  static unsigned int countBytes(const char* szStr);
  static unsigned int countCharacters(const char* szStr, unsigned int nBytes = 0);

protected:
  char* szStr_;
  unsigned int nAllocated_;
  unsigned int nBytes_;
  unsigned int nChars_;
#if defined(WIN32) && defined(UNICODE)
  mutable WCHAR* wzStr_;
#else
  mutable void* pDummy_; // this asserts unique structure size, even if a module DLL uses this class, but does not define _UNICODE,UNICODE
#endif
  enum { kStaticSize = 64 };
  //char pStatic_[kStaticSize];
};

SExport String operator+(const String& a, const String& b);
SExport String operator+(const String& a, const char* b);
SExport String operator+(const char* a, const String& b);

#if defined(WIN32)
  #define String_FILEPATHSEPARATOR "\\"
#else
  #define String_FILEPATHSEPARATOR "/" 
#endif

#define StringFormat "%s"
#define StringType(__s__) (const char*) __s__ // extractor for printf

// ============================================================================

#define Flexbuf_DefaultLength 8192
template <class T, int N = Flexbuf_DefaultLength> class Flexbuf
{
  // allocates a static buffer of STATIC_LENGTH size
  // if this is not sufficient for size then allocate a dynamic one
public:
  enum {
    DefaultLength = N
  };

  Flexbuf<T,N>(size_t size)
    :pDynamic_(0)
    ,nLength_(N)
  {
    if (size > N) {
      size_t alloc_size= size;
      pDynamic_= new T [alloc_size];
      nLength_= alloc_size;
    }
  }

  ~Flexbuf<T,N>()
  {
    if (pDynamic_ != 0) {
      delete [] pDynamic_;
    }
  }

  // Main getter
  T* get(void)
  {
    if (pDynamic_ == 0) {
      return pStatic_;
    } else {
      return pDynamic_;
    }
  }

  size_t length(void)
  {
    return nLength_;
  }

  // Additional getter
  operator T*(void)
  {
    return get();
  }

  operator bool(void) { return (get() != 0); }

private:
  T* pDynamic_;
  size_t nLength_;
  T pStatic_[N];
};

#endif // !defined(SString_h_INCLUDED)
