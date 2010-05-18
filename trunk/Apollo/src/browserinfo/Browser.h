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
  Context(ApHandle hContext)
    :hAp_(hContext)
    ,bVisible_(0)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
  {}

  ApHandle hAp_;

  void SetVisibility(int bVisible) { bVisible_ = bVisible; }
  void SetPosition(int nX, int nY) { nX_ = nX; nY_ = nY; }
  void SetSize(int nW, int nH) { nW_ = nW; nH_ = nH; }

  int IsVisible() { return bVisible_; }
  void GetPosition(int& nX, int& nY) { nX = nX_; nY = nY_; }
  void GetSize(int& nW, int& nH) { nW = nW_; nH = nH_; }

protected:
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;
};

typedef ApHandlePointerTree<Context*> ContextList;
typedef ApHandlePointerTreeNode<Context*> ContextNode;

class Browser
{
public:
  Browser(Apollo::WindowHandle& win)
    :win_(win)
    ,bVisible_(0)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
  {}
  virtual ~Browser() {}

  void AddContext(ApHandle hContext);
  void RemoveContext(ApHandle hContext);
  Context* GetContext(ApHandle hContext);
  bool HasContexts() { return contexts_.Count(); }
  bool HasContext(ApHandle hContext) { return contexts_.Find(hContext) != 0; }
  Apollo::WindowHandle GetWindow() { return win_; }

  virtual void SecTimer() {}
  virtual void Browser::Evaluate(int bVisible, int nX, int nY, int nW, int nH);

  int IsVisible() { return bVisible_; }

protected:
  virtual void StartTracking() {}
  virtual void StopTracking() {}

protected:
  Apollo::WindowHandle win_;
  ContextList contexts_;

  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;
};

#endif // !defined(Browser_h_INCLUDED)
