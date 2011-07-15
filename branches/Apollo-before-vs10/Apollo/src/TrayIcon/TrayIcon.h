// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef TRAYICON_EXPORTS
    #define TRAYICON_API __declspec(dllexport)
  #else
    #define TRAYICON_API __declspec(dllimport)
  #endif
#else
  #define TRAYICON_API
#endif

#include "ApModule.h"

extern "C" {

  TRAYICON_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  TRAYICON_API int Load(AP_MODULE_CALL* pModuleData);
  TRAYICON_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
