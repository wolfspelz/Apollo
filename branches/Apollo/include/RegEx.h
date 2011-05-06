// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(RegEx_h_INCLUDED)
#define RegEx_h_INCLUDED

#include "SSystem.h"
#include "ApExports.h"

#define RegEx_MAX_MATCHES 10 // \0 - \9

// -------------------------------------------------------------------

AP_NAMESPACE_BEGIN

class APOLLO_API RegEx
{
public:
  RegEx();
  virtual ~RegEx();

  int Compile(const char* szExpression);
  int Match(const char* szInput);
  String Replace(const char* szTemplate);

protected:
  void* pPCRE_; // void* so, that RegEx users do not have to #include "pcre.h"
  int pOffsets[RegEx_MAX_MATCHES * 4];
  char* pInputData_;
  size_t nInputLen_;
  int nMatches_;
};

AP_NAMESPACE_END

#endif // !defined(RegEx_h_INCLUDED)

