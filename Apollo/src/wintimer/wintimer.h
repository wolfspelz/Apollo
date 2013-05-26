// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef WINTIMER_EXPORTS
    #define WINTIMER_API __declspec(dllexport)
  #else
    #define WINTIMER_API __declspec(dllimport)
  #endif
#else
  #define WINTIMER_API
#endif

#include "ApModule.h"

extern "C" {

  WINTIMER_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  WINTIMER_API int Load(AP_MODULE_CALL* pModuleData);
  WINTIMER_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
