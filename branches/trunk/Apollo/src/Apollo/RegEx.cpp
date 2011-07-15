// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApCompiler.h"
#include "RegEx.h"

/* We need access to the data tables that PCRE uses. So as not to have to keep
two copies, we include the source file here, changing the names of the external
symbols to prevent clashes. */

#define _pcre_utf8_table1      utf8_table1
#define _pcre_utf8_table1_size utf8_table1_size
#define _pcre_utf8_table2      utf8_table2
#define _pcre_utf8_table3      utf8_table3
#define _pcre_utf8_table4      utf8_table4
#define _pcre_utt              utt
#define _pcre_utt_size         utt_size
#define _pcre_OP_lengths       OP_lengths

#include "pcre/pcre_tables.c"

static int ord2utf8(int cvalue, uschar *buffer)
{
  register int i, j;
  for (i = 0; i < utf8_table1_size; i++)
  {
    if (cvalue <= utf8_table1[i]) break;
  }
  buffer += i;
  for (j = i; j > 0; j--)
  {
    *buffer-- = 0x80 | (cvalue & 0x3f);
    cvalue >>= 6;
  }
  *buffer = utf8_table2[i] | cvalue;
  return i + 1;
}

static int AP_UNUSED_FUNCTION utf82ord(unsigned char *buffer, int *vptr)
{
  int c = *buffer++;
  int d = c;
  int i, j, s;
  
  for (i = -1; i < 6; i++)               /* i is number of additional bytes */
  {
    if ((d & 0x80) == 0) break;
    d <<= 1;
  }
  
  if (i == -1) { *vptr = c; return 1; }  /* ascii character */
  if (i == 0 || i == 6) return 0;        /* invalid UTF-8 */
  
  /* i now has a value in the range 1-5 */
  
  s = 6*i;
  d = (c & utf8_table3[i]) << s;
  
  for (j = 0; j < i; j++)
  {
    c = *buffer++;
    if ((c & 0xc0) != 0x80) return -(j+1);
    s -= 6;
    d |= (c & 0x3f) << s;
  }
  
  /* Check that encoding was the correct unique one */
  
  for (j = 0; j < utf8_table1_size; j++)
  {
    if (d <= utf8_table1[j]) break;
  }
  if (j != i) return -(i+1);
  
  /* Valid value */
  
  *vptr = d;
  return i+1;
}

int pregcstr_2_utf8(unsigned char* src, unsigned char* dbuffer, int* opt)
{
  int c;
  int AP_UNUSED_VARIABLE start_offset = 0;
  int options = 0;

  unsigned char *p = src;
  char* q = (char*) dbuffer;
  int len = 0;

  while ((c = *p++) != 0)
    {
    int i = 0;
    int AP_UNUSED_VARIABLE n = 0;

    if (c == '\\') switch ((c = *p++))
      {
/*
      case 'a': c =    7; break;
      case 'b': c = '\b'; break;
      case 'e': c =   27; break;
      case 'f': c = '\f'; break;
      case 'n': c = '\n'; break;
      case 'r': c = '\r'; break;
      case 't': c = '\t'; break;
      case 'v': c = '\v'; break;

      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
      c -= '0';
      while (i++ < 2 && isdigit(*p) && *p != '8' && *p != '9')
        c = c * 8 + *p++ - '0';
      break;
*/
      case 'x':

      /* Handle \x{..} specially - new Perl thing for utf8 */

#if !defined NOUTF8
      if (*p == '{')
        {
        unsigned char *pt = p;
        c = 0;
        while (isxdigit(*(++pt)))
          c = c * 16 + tolower(*pt) - ((isdigit(*pt))? '0' : 'W');
        if (*pt == '}')
          {
          unsigned char buff8[8];
          int ii, utn;
          utn = ord2utf8(c, buff8);
          for (ii = 0; ii < utn - 1; ii++) *q++ = buff8[ii];
          c = buff8[ii];   /* Last byte */
          p = pt + 1;
          break;
          }
        /* Not correct form; fall through */
        }
#endif

      /* Ordinary \x */

      c = 0;
      while (i++ < 2 && isxdigit(*p))
        {
        c = c * 16 + tolower(*p) - ((isdigit(*p))? '0' : 'W');
        p++;
        }
      break;
/*
      case 0:   // \ followed by EOF allows for an empty line
      p--;
      continue;

      case '>':
      while(isdigit(*p)) start_offset = start_offset * 10 + *p++ - '0';
      continue;

      case 'A':
      options |= PCRE_ANCHORED;
      continue;

      case 'B':
      options |= PCRE_NOTBOL;
      continue;

      case 'N':
      options |= PCRE_NOTEMPTY;
      continue;

      case 'P':
      options |= PCRE_PARTIAL;
      continue;

      case 'Z':
      options |= PCRE_NOTEOL;
      continue;

      case '?':
      options |= PCRE_NO_UTF8_CHECK;
      continue;
*/
    }
    *q++ = c;
    }
  *q = 0;
  len = (char*) q - (char*) dbuffer;

  *opt = options;

  return len;
}

static void *RegEx_malloc(size_t size)
{
  return ::malloc(size);
}

static void RegEx_free(void *block)
{
  ::free(block);
}


static void *RegEx_stack_malloc(size_t size)
{
  return ::malloc(size);
}

static void RegEx_stack_free(void *block)
{
  ::free(block);
}

// -------------------------------------------------------------------

class RegExInit
{
public:
  RegExInit();
  virtual ~RegExInit();
};

RegExInit::RegExInit()
{
  pcre_malloc = RegEx_malloc;
  pcre_free = RegEx_free;
  pcre_stack_malloc = RegEx_stack_malloc;
  pcre_stack_free = RegEx_stack_free;
}

RegExInit::~RegExInit()
{
}

static RegExInit g_oRegExInit;

// -------------------------------------------------------------------

AP_NAMESPACE_BEGIN

RegEx::RegEx()
:pPCRE_(0)
,pInputData_(0)
,nInputLen_(0)
,nMatches_(0)
{
}

RegEx::~RegEx()
{
  if (pPCRE_ != 0) {
    RegEx_free(pPCRE_);
  }
  if (pInputData_ != 0) {
    delete [] pInputData_;
  }
}

int RegEx::Compile(const char* szExpression)
{
  int ok = 0;

  String sExpression = szExpression;

  const char* p = (const char*) sExpression;
  const char *error;
  int erroroffset;
  const unsigned char *tables = NULL;
  int options = PCRE_UTF8;

  if (pPCRE_ != 0) {
    RegEx_free(pPCRE_);
  }

  pPCRE_ = (void*) pcre_compile(
    p, 
    options, 
    &error, 
    &erroroffset, 
    tables
    );

  if (pPCRE_ != 0) {
    ok = 1;
  } else {
    // error
  }

  return ok;
}

int RegEx::Match(const char* szInput)
{
  nMatches_ = 0;

  if (pInputData_ != 0) {
    delete [] pInputData_;
  }

  nInputLen_ = String::strlen(szInput);
  pInputData_ = new char[nInputLen_ + 1];
  if (pInputData_ != 0) {
    ::memcpy(pInputData_, szInput, nInputLen_ + 1);

    pcre_extra *extra = 0;
    int start_offset = 0;
    int options = 0;
    nMatches_ = pcre_exec(
      (pcre*) pPCRE_, 
      extra, 
      (const char*) pInputData_, 
      nInputLen_, 
      start_offset, 
      options, 
      pOffsets, 
      RegEx_MAX_MATCHES * 4
    );
  }

  return nMatches_;
}

String RegEx::Replace(const char* szTemplate)
{
  String sReplaced;

  int AP_UNUSED_VARIABLE nOffsets = nMatches_ * 2;

  String sPart;
  const char* p = szTemplate;
  while (*p != '\0') {
    int nCharLen = String::UTF8_CharSize(p);
    int bIsMarker = 0;

    if (*p == '\\' || *p == '$') {
      char cN = *(p+1);
      if (cN >= '0' && cN <= '9') {
        int nMarker = cN - '0';
        if (nMarker <= nMatches_ && nMarker <= RegEx_MAX_MATCHES) {
          bIsMarker = 1;
          p += nCharLen;
          nCharLen = String::UTF8_CharSize(p);
          if (pOffsets[nMarker * 2] >= 0 && pOffsets[nMarker * 2 + 1] >= 0) {
            sPart.set(&(pInputData_[pOffsets[nMarker * 2]]), pOffsets[nMarker * 2 + 1] - pOffsets[nMarker * 2]);
            sReplaced += sPart;
          }
        }
      }
    }

    if (!bIsMarker) {
      sReplaced.append(p, nCharLen);
    }

    p += nCharLen;
  }

  return sReplaced;
}

AP_NAMESPACE_END
