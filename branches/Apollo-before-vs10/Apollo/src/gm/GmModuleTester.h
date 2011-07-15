// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(GmModuleTester_H_INCLUDED)
#define GmModuleTester_H_INCLUDED

#if defined(AP_TEST)
class GmModuleTester
{
public:
  static void begin();
  static void execute();
  static void end();

  static String test_Encryption();
};
#endif

#endif // GmModuleTester_H_INCLUDED
