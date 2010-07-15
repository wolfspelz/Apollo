// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(GalileoModuleTester_H_INCLUDED)
#define GalileoModuleTester_H_INCLUDED

#if defined(AP_TEST)
class GalileoModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Test_Parse();

  static String Test_LoadGIF();

  static String Test_SelectByGroup1(Group& g, int nRnd, const String& sExpectedSequence);
  static String Test_SelectByGroup();

  static String Test_PlayStep(Item& i, Apollo::TimeValue& t, const String& sExpectedSequence, int nExpectedTime);
  static String Test_PlayStill();
  static String Test_PlayWave();
};
#endif

#endif // GalileoModuleTester_H_INCLUDED
