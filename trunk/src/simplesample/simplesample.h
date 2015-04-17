// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef SIMPLESAMPLE_EXPORTS
    #define SIMPLESAMPLE_API __declspec(dllexport)
  #else
    #define SIMPLESAMPLE_API __declspec(dllimport)
  #endif
#else
  #define SIMPLESAMPLE_API
#endif

#include "ApModule.h"

extern "C" {

  SIMPLESAMPLE_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  SIMPLESAMPLE_API int Load(AP_MODULE_CALL* pModuleData);
  SIMPLESAMPLE_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
