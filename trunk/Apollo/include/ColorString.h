// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ColorString_h_INCLUDED)
#define ColorString_h_INCLUDED

#include "Apollo.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

class APOLLO_API ColorString
{
public:
  ColorString(const String& sColor);

protected:
  double Hex2(const String& s);
  double Hex1(const String& s);

public:
  double r;
  double g;
  double b;
  double a;
};

AP_NAMESPACE_END

#endif // !defined(ColorString_h_INCLUDED)
