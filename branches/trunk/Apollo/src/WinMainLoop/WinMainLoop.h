// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef WINMAINLOOP_EXPORTS
    #define WINMAINLOOP_API __declspec(dllexport)
  #else
    #define WINMAINLOOP_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(WINMAINLOOP_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define WINMAINLOOP_API __attribute__ ((visibility("default")))
  #else
    #define WINMAINLOOP_API
  #endif
#else
  #define WINMAINLOOP_API
#endif

#include "ApModule.h"

extern "C" {

  WINMAINLOOP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  WINMAINLOOP_API int Load(AP_MODULE_CALL* pModuleData);
  WINMAINLOOP_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
