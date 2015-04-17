// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SSystem_h_INCLUDED)
#define SSystem_h_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#if defined(WIN32)
  // Exclude rarely-used stuff from Windows headers, especially <winsock.h>
  #define WIN32_LEAN_AND_MEAN
  #define _WIN32_WINNT 0x0501
  #include <windows.h>
  #undef WIN32_LEAN_AND_MEAN

  #include <direct.h>

#if defined(_MSC_VER)
  #include <crtdbg.h>
#endif

  #include <tchar.h>
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <errno.h>
  #include <strings.h>
  #include "limits.h" // INT_MAX
#endif

#if defined(WIN32)
  #ifdef APOLLO_EXPORTS
    #define SExport __declspec(dllexport)
  #else
    #define SExport __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(APOLLO_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define SExport __attribute__ ((visibility("default")))
  #else
    #define SExport
  #endif
#else
  #define SExport
#endif

#define SInline inline

class SExport SSystem
{
public:
  static void* malloc(size_t len)
  {
    return ::malloc(len);
  }
  static void free(void* ptr)
  {
    ::free(ptr);
  }
  static void* realloc(void* mem, size_t newLen)
  {
    return ::realloc(mem, newLen);
  }

#if defined(WIN32)
  static int mkdir(const char* dirname) { return ::_mkdir(dirname); };
  #if defined(_UNICODE)
  static int mkdir(const WCHAR* dirname) { return ::_wmkdir(dirname); };
  #endif
#else
  static int mkdir(const char* dirname) { return ::mkdir(dirname, 0770); };// 0770 not 770 due to octal
#endif
};

// ============================================================================
// Delete Sentinel

// A watchdog tells the referer, when the referee is deleted so that the referer
// can be reset to 0. The user of the pointer can then check the pointer against 0.

// Instead of:
//   FB_DisplayGroup* display_;
// write:
//   SReferer<FB_DisplayGroup> display_;
//
// At the target class add a sentinel as instance variable:
//   SRefererSentinel<FB_DisplayGroup> delete_watchdog_;
// This sentinel will demand initialization with a reference of the pointer,
// which is to be notified upon deletion of the sentinel. So Initialize the
// sentinel like:
//   FB_DisplayGroup::FB_DisplayGroup(SReferer<FB_DisplayGroup>& _referer, ....
//   :delete_watchdog_(_referer)

template <class T> class SRefererSentinel;

template <class T> class  SReferer
{
public:
  // it wants to be initialized
  // supply 0 if you don't know better
  SReferer<T>(T* _p)
  {
    forward_ptr_= _p;
    sentinel_= 0;
  }

  ~SReferer<T>()
  {
    if (sentinel_ != 0) {
      sentinel_->dtor();
    }
  }

  operator T* (void)
  {
    return forward_ptr_;
  }

  T* operator -> (void)
  {
    return forward_ptr_;
  }

  SReferer<T>* operator = (T* _old)
  {
    forward_ptr_= _old;
    return this;
  }

  SReferer<T>& operator = (SReferer<T>& _old)
  {
    forward_ptr_= _old.forward_ptr_;
    sentinel_= _old.sentinel_;
    return *this;
  }

  void dtor(void)
  {
    forward_ptr_= 0;
    sentinel_= 0;
  }

  void ctor(SRefererSentinel<T>* _sentinel)
  {
    sentinel_= _sentinel;
  }

private:
  T* forward_ptr_;
  SRefererSentinel<T>* sentinel_;
};

template <class T> class SRefererSentinel
{
public:
  SRefererSentinel<T>(SReferer<T>& _referer)
    :referer_(&_referer)
  {
    if (referer_ != 0) {
      referer_->ctor(this);
    }
  }

  ~SRefererSentinel<T>(void)
  {
    if (referer_ != 0) {
      referer_->dtor();
    }
  }

  void dtor(void)
  {
    referer_= 0;
  }

private:
  SReferer<T>* referer_;
};

#include "SString.h"
#include "SElem.h"
#include "SListT.h"
#include "STree.h"

#endif // !defined(SSystem_h_INCLUDED)
