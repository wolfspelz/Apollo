// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(IdentityModuleTester_H_INCLUDED)
#define IdentityModuleTester_H_INCLUDED

#if defined(AP_TEST)
class IdentityModuleTester
{
public:
  static void begin();
  static void execute();
  static void end();

  static String test_CacheBasic();
  static String test_CacheStorage();
  static String test_GetProperty();
  static String test_GetItem();
  static String test_GetItemData();
  static String test_Change();
  static String test_selectItemId();
  static String test_RequestContainer_Start();
  static String test_RequestItem_Start();
};
#endif

#endif // IdentityModuleTester_H_INCLUDED
