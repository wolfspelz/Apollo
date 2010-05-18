// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApolloExports_h_INCLUDED)
#define ApolloExports_h_INCLUDED

#if defined(WIN32)
  #ifdef APOLLO_EXPORTS
    #define APOLLO_API __declspec(dllexport)
  #else
    #define APOLLO_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(APOLLO_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define APOLLO_API __attribute__ ((visibility("default")))
  #else
    #define APOLLO_API
  #endif
#else
  #define APOLLO_API
#endif

#define AP_NAMESPACE_BEGIN namespace Apollo {
#define AP_NAMESPACE_END }

#endif // !defined(ApolloExports_h_INCLUDED)



