// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApCompiler_H_INCLUDED)
#define ApCompiler_H_INCLUDED

// ------------------------------------

#define AP_MEMORY_CHECK_USE_VLD 0

// ------------------------------------

#if defined(NAMESPACE_BEGIN)
#undef NAMESPACE_BEGIN
#endif

#define NAMESPACE_BEGIN( _ns_ )             namespace _ns_ {
#define NAMESPACE_END( _ns_ )               };

#if defined(_MSC_VER) && defined(_DEBUG)
  #define AP_DEBUG_BREAK() __asm { int 3 }
#elif defined(__GNUC__) && defined(_DEBUG)
  #define AP_DEBUG_BREAK() asm("int $0x3")
#else
  #define AP_DEBUG_BREAK() do {;} while(0)
#endif

#if defined(_DEBUG)
  #define AP_ASSERT( _cond_ ) if ( !( _cond_ ) ) { AP_DEBUG_BREAK(); }
#else
  #define AP_ASSERT( _cond_ )
#endif


#if defined(__GNUC__)
  #define AP_UNUSED_FUNCTION __attribute__((unused))
#else
  #define AP_UNUSED_FUNCTION
#endif

#if defined(__GNUC__)
  #define AP_UNUSED_VARIABLE __attribute__((unused))
#else
  #define AP_UNUSED_VARIABLE
#endif

#if defined(__GNUC__)
//#if defined (ghs) || defined (__GNUC__) || defined (__hpux) || defined (__sgi) || defined (__DECCXX) || defined (__KCC) || defined (__rational__) || defined (__USLC__) || defined (ACE_RM544)
// Some compilers complain about "statement with no effect" with (a).
// This eliminates the warnings, and no code is generated for the null
// conditional statement.  NOTE: that may only be true if -O is enabled,
// such as with GreenHills (ghs) 1.8.8.
  #define AP_UNUSED_ARG(a) do {} while (&a == 0)
#else // __GNUC__
  #define AP_UNUSED_ARG(a) (a)
#endif // __GNUC__

#if defined(_DEBUG) && defined(_MSC_VER)
  #if defined(AP_MEMORY_CHECK_USE_VLD) && (AP_MEMORY_CHECK_USE_VLD == 1)
    #include "VisualLeakDetector.h"
    #define AP_MEMORY_CHECK(dwReason)                                           \
      {                                                                         \
        switch (dwReason) {                                                     \
          case DLL_PROCESS_ATTACH:                                              \
          case DLL_THREAD_ATTACH:                                               \
            { (void)VLDEnable(); } break;                                       \
          default:                                                              \
            {} break;                                                           \
        }                                                                       \
      }
  #else
    #define AP_MEMORY_CHECK(dwReason)                                           \
      {                                                                         \
        switch (dwReason) {                                                     \
          case DLL_PROCESS_ATTACH:                                              \
          case DLL_THREAD_ATTACH:                                               \
            {                                                                   \
              _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );   \
              _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);                 \
              _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);                \
              _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);               \
            } break;                                                            \
          default:                                                              \
            {} break;                                                           \
        }                                                                       \
      }
  #endif
#else
  #define AP_MEMORY_CHECK(dwReason)
#endif

#endif // ApCompiler_H_INCLUDED

