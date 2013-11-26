// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef TIMER_EXPORTS
    #define TIMER_API __declspec(dllexport)
  #else
    #define TIMER_API __declspec(dllimport)
  #endif
#else
  #define TIMER_API
#endif

#include "ApModule.h"

extern "C" {

  TIMER_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  TIMER_API int Load(AP_MODULE_CALL* pModuleData);
  TIMER_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
