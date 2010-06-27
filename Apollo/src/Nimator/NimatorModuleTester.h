// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(NimatorModuleTester_H_INCLUDED)
#define NimatorModuleTester_H_INCLUDED

#if defined(AP_TEST)
class NimatorModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Test_Parse();
  static String Test_LoadGIF();
  static String Test_SelectByGroup1(Group& g, int nRnd, const String& sExpectedSequence);
  static String Test_SelectByGroup();
  static String Test_Play();
};
#endif

#endif // NimatorModuleTester_H_INCLUDED
