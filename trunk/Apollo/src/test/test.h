// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef TEST_EXPORTS
    #define TEST_API __declspec(dllexport)
  #else
    #define TEST_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(TEST_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define TEST_API __attribute__ ((visibility("default")))
  #else
    #define TEST_API
  #endif
#else
  #define TEST_API
#endif // defined(WIN32)

#include "Apollo.h"

extern "C" {

  TEST_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  TEST_API int Load(AP_MODULE_CALL* pModuleData);
  TEST_API int UnLoad(AP_MODULE_CALL* pModuleData);

}

//----------------------------------------------------------

#define LOG_CHANNEL "test"
#define MODULE_NAME "test"

#if defined (AP_TEST)
  #define AP_TEST_Builtin
  #define AP_TEST_Core
  #define AP_TEST_Config
  #define AP_TEST_Db
  #define AP_TEST_Translation
  #define AP_TEST_Net
  #define AP_TEST_Vpi
  #define AP_TEST_Vp
  #define AP_TEST_Galileo
  #define AP_TEST_Arena
  #if defined (AP_TEST_Net)
    #define AP_TEST_Net_TCP
    #define AP_TEST_Net_TCP_Advanced
    #define AP_TEST_Net_HTTP
  #endif
  #define AP_TEST_Threads
#endif

//----------------------------------------------------------

void Test_Builtin_Register();
void Test_Builtin_Execute();

void Test_Core_Register();
void Test_Core_Execute();

void Test_Translation_Register();
void Test_Translation_Execute();

void Test_Net_Register();
void Test_Net_Begin();
void Test_Net_End();

void Test_Vpi_Register();
void Test_Vpi_Begin();
void Test_Vpi_End();

void Test_Threads_Register();
void Test_Threads_Begin();
void Test_Threads_End();

void Test_Vp_Register();
void Test_Vp_Begin();
void Test_Vp_End();

void Test_Galileo_Register();
void Test_Galileo_Begin();
void Test_Galileo_End();

void Test_Arena_Register();
void Test_Arena_Begin();
void Test_Arena_End();

