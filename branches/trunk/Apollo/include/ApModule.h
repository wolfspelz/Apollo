// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApModule_h_INCLUDED)
#define ApModule_h_INCLUDED

#include "ApExports.h"
#include "ApTypes.h"

#define AP_MODULE_INTERFACE_FLAGS_UTF8 (1<<0)
#define AP_MODULE_INTERFACE_FLAGS_CPP (1<<1)

typedef struct {
  int nSize;
  int nFlags;
  const char* szName;
  const char* szServices;
  const char* szLongName;
  const char* szVersion;
  const char* szDescription;
  const char* szAuthor;
  const char* szEmail;
  const char* szCopyright;
  const char* szHomepageUrl;
} AP_MODULE_INFO;
// Provided by the module. Result of Info()

typedef void* (*ApGetAddressByNameF) (const char* szName);

typedef struct {
  int nSize;
  int nFlags;
  //ApGetAddressByNameF fpGetAddressByName;
} AP_MODULE_CALL;
// Provided by the framework to module functions Info, Load, UnLoad

typedef AP_MODULE_INFO* (*ApModuleInfoF) (AP_MODULE_CALL* pModuleData);
typedef int (*ApModuleLoadF) (AP_MODULE_CALL* pModuleData);
typedef int (*ApModuleUnLoadF) (AP_MODULE_CALL* pModuleData);

//----------------------------------------------------------

template <class T> class ApModuleSingleton
{
public:
  static T* Get()
  {
    if (pInstance_ == 0) {
      pInstance_ = new T();
    }

    return pInstance_;
  }

  static void Delete()
  {
    if (pInstance_ != 0) {
      delete pInstance_;
      pInstance_ = 0;
    }
  }

protected:
  static T* pInstance_;
};

template <class T> T* ApModuleSingleton<T>::pInstance_ = 0;

//----------------------------------------------------------

typedef int (*ApHookMsgF) (const char* szCallName, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
typedef int (*ApUnHookMsgF) (const char* szCallName, const char* szModuleName, ApCallback fnHandler);

class APOLLO_API ApModuleEnvironment
{
public:
  ApModuleEnvironment()
    :bInitialized_(0)
    ,fpHook_(0)
    ,fpUnHook_(0)
  {}
  virtual ~ApModuleEnvironment() {}

  int Init(const char* szModuleName, AP_MODULE_CALL* pModuleData);

  int hookMsg(ApMessage &msg, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
  int unhookMsg(ApMessage& msg, ApCallback fnHandler);

public:
  int bInitialized_;
  String sModuleName_;
  ApHookMsgF fpHook_;
  ApUnHookMsgF fpUnHook_;
};

#endif // !defined(ApModule_h_INCLUDED)
