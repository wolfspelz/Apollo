// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef CHAT_EXPORTS
    #define CHAT_API __declspec(dllexport)
  #else
    #define CHAT_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(CHAT_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define CHAT_API __attribute__ ((visibility("default")))
  #else
    #define CHAT_API
  #endif
#else
  #define CHAT_API
#endif

#include "ApModule.h"

extern "C" {

  CHAT_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  CHAT_API int Load(AP_MODULE_CALL* pModuleData);
  CHAT_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
