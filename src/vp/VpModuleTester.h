// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(VpModuleTester_H_INCLUDED)
#define VpModuleTester_H_INCLUDED

#if defined(AP_TEST)
class VpModuleTester
{
public:
  static String test_Identity2ParticipantMapping();
  static String test_DisplayProfile();
  static String test_ItemDataExternUrl();
};
#endif

#endif // VpModuleTester_H_INCLUDED
