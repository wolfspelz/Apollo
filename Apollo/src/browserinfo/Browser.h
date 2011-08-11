// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Browser_h_INCLUDED)
#define Browser_h_INCLUDED

#include "Apollo.h"
#include "ApContainer.h"
#include "WindowHandle.h"

class Context
{
public:
  Context(const ApHandle& hContext)
    :hAp_(hContext)
    ,bCombinedVisible_(0)
    ,bVisible_(0)
    ,nLeft_(0)
    ,nBottom_(0)
    ,nWidth_(0)
    ,nHeight_(0)
  {}

  inline ApHandle apHandle() { return hAp_; }

  void AdjustPosition(int bVisible, int nLeft, int nBottom, int nWidth, int nHeight);
  void SetVisibility(int bVisible) { bVisible_ = bVisible; }
  void SetPosition(int nLeft, int nBottom) { nLeft_ = nLeft; nBottom_ = nBottom; }
  void SetSize(int nWidth, int nHeight) { nWidth_ = nWidth; nHeight_ = nHeight; }
  void SetWindow(const Apollo::WindowHandle& win) { win_ = win; }

  int IsVisible() { return bVisible_; }
  void GetPosition(int& nLeft, int& nBottom) { nLeft = nLeft_; nBottom = nBottom_; }
  void GetSize(int& nWidth, int& nHeight) { nWidth = nWidth_; nHeight = nHeight_; }
  Apollo::WindowHandle GetWindow() { return win_; }

protected:
  ApHandle hAp_;
  int bCombinedVisible_;
  int bVisible_;
  int nLeft_;
  int nBottom_;
  int nWidth_;
  int nHeight_;
  Apollo::WindowHandle win_;
};

typedef ApHandlePointerTree<Context*> ContextList;
typedef ApHandlePointerTreeNode<Context*> ContextNode;

class Browser
{
public:
  Browser(Apollo::WindowHandle& win)
    :win_(win)
    ,bVisible_(0)
    ,nLeft_(0)
    ,nBottom_(0)
    ,nWidth_(0)
    ,nHeight_(0)
  {}
  virtual ~Browser() {}

  void AddContext(const ApHandle& hContext);
  void RemoveContext(const ApHandle& hContext);
  Context* GetContext(const ApHandle& hContext);
  bool HasContexts() { return contexts_.Count(); }
  bool HasContext(const ApHandle& hContext) { return contexts_.Find(hContext) != 0; }
  Apollo::WindowHandle GetWindow() { return win_; }

  virtual void OnTimer() {}
  virtual void AdjustPosition(int bVisible, int nLeft, int nBottom, int nWidth, int nHeight);
  virtual void AdjustStackingOrder() {}

  int IsVisible() { return bVisible_; }

protected:
  virtual void StartTracking() {}
  virtual void StopTracking() {}

protected:
  Apollo::WindowHandle win_;
  ContextList contexts_;

  int bVisible_;
  int nLeft_;
  int nBottom_;
  int nWidth_;
  int nHeight_;
};

#endif // !defined(Browser_h_INCLUDED)
