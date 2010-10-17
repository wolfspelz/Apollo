// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Display_h_INCLUDED)
#define Display_h_INCLUDED

#include "Apollo.h"

class Display
{
public:
  Display(const ApHandle& hDisplay, const String& sId, const String& sPathname, const String& sArgs)
    :hAp_(hDisplay)
    ,sId_(sId)
    ,sPathname_(sPathname)
    ,sArgs_(sArgs)
    ,nPid_(0)
    ,bCreated_(0)
    ,bLoaded_(0)
  {}
  virtual ~Display() {}

  int start();
  int stop();

  void onCreated(const ApHandle& hConnection);
  void onLoaded();
  void onBeforeUnload();
  void onUnloaded();
  void onDestroyed();

  inline ApHandle apHandle() { return hAp_; }
  inline String getPathname() { return sPathname_; }
  inline String getId() { return sId_; }

protected:
  ApHandle hAp_;
  String sPathname_;
  String sId_;
  String sArgs_;
  int nPid_;
  int bCreated_;
  int bLoaded_;
};

#endif // !defined(Display_h_INCLUDED)
