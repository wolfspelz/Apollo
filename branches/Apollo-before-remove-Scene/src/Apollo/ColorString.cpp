// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ColorString.h"

Apollo::ColorString::ColorString(const String& sColor)
{
  a = 1;
  String s = String::toLower(sColor);
  if (s.startsWith("#")) {
    if (s.chars() == 4) {
      String c;
      c = s.subString(1, 1); r = Hex2(c + c);
      c = s.subString(2, 1); g = Hex2(c + c);
      c = s.subString(3, 1); b = Hex2(c + c);
    } else if (s.chars() == 7) {
      r = Hex2(s.subString(1, 2));
      g = Hex2(s.subString(3, 2));
      b = Hex2(s.subString(5, 2));
    } else if (s.chars() == 9) {
      r = Hex2(s.subString(1, 2));
      g = Hex2(s.subString(3, 2));
      b = Hex2(s.subString(5, 2));
      a = Hex2(s.subString(7, 2));
    } else {
      r = g = b = 0;
    }
  } else {
    r = g = b = 0;
  }
}

double Apollo::ColorString::Hex2(const String& s)
{
  return (16 * Hex1(s) + Hex1(s.subString(1))) / 255.0;
}

double Apollo::ColorString::Hex1(const String& s)
{
  String::UTF8Char c = String::UTF8_Char(s);
  if (::isdigit(c)) {
    return c - '0';
  }
  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  return 0.0;
}
