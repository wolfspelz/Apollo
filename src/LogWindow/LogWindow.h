// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef LOGWINDOW_EXPORTS
    #define LOGWINDOW_API __declspec(dllexport)
  #else
    #define LOGWINDOW_API __declspec(dllimport)
  #endif
#else
  #define LOGWINDOW_API
#endif

#include "ApModule.h"

extern "C" {

  LOGWINDOW_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  LOGWINDOW_API int Load(AP_MODULE_CALL* pModuleData);
  LOGWINDOW_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
