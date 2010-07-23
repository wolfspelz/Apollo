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

  static String Test_PlayStep(Item& i, Apollo::TimeValue& t, const String& sExpectedSequence, int nExpectedTime, int nExpectedX);
  static String Test_PlayStill();
  static String Test_PlayWave();
  static String Test_PlayMoveRight();
  static String Test_SelectNextSequence_KeepStatus();
  static String Test_SelectNextSequence_Event();
  static String Test_SelectNextSequence_StatussedEvent();
  static String Test_SelectNextSequence_StatusTransition();
  static String Test_SelectNextSequence_StatusTransitionNextSequence();
  static String Test_SelectNextSequence_ConditionedEvent();
  static String Test_SelectNextSequence_StatussedConditionedEvent();
  static String Test_SelectNextSequence_ConditionedStatusTransition();
};
#endif

#endif // GalileoModuleTester_H_INCLUDED
