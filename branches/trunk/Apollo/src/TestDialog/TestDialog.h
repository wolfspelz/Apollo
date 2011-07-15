// ============================================================================
//
// Apollo
//
// ============================================================================

#if defined(WIN32)
  #ifdef TESTDIALOG_EXPORTS
    #define TESTDIALOG_API __declspec(dllexport)
  #else
    #define TESTDIALOG_API __declspec(dllimport)
  #endif
#else
  #define TESTDIALOG_API
#endif

#include "ApModule.h"

extern "C" {

  TESTDIALOG_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData);
  TESTDIALOG_API int Load(AP_MODULE_CALL* pModuleData);
  TESTDIALOG_API int UnLoad(AP_MODULE_CALL* pModuleData);

}
