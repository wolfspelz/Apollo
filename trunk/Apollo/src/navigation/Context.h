// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Context_h_INCLUDED)
#define Context_h_INCLUDED

#include "Apollo.h"

class Context
{
public:
  Context(const ApHandle& hContext)
    :hAp_(hContext)
  {}
  virtual ~Context() {}

  inline ApHandle apHandle() { return hAp_; }

  void create(); // throws ApException
  void destroy(); // throws ApException

  void navigate(const String& sUrl); // throws ApException
  void nativeWindow(const String& sType, Apollo::KeyValueList& kvSignature); // throws ApException
  void show(); // throws ApException
  void hide(); // throws ApException
  void position(int nX, int nY); // throws ApException
  void size(int nWidth, int nHeight); // throws ApException

protected:
  ApHandle hAp_;
};

#endif // !defined(Context_h_INCLUDED)
