// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Hud_H_INCLUDED)
#define Hud_H_INCLUDED

#include "Layer.h"

class Hud: public Layer
{
public:
  Hud(Display* pDisplay);
  virtual ~Hud();

  void OnSetSize(int nW, int nH);
  int OnTimerEvent(Msg_Timer_Event* pMsg);
  int OnMouseEvent(Msg_Scene_MouseEvent* pMsg);

protected:
  void CreateBottomSensor();
  void DestroyBottomSensor();
  void SizeBottomSensor();

  void CreateSizeSensor();
  void DestroySizeSensor();
  void SizeSizeSensor();
  void CaptureMouseToSizeSensor();
  void ReleaseeMouseFromSizeSensor();

  void StartMouseSize(int nX, int nY);
  void StopMouseSize();
  void DoMouseSize(int nX, int nY);
  inline int InMouseSize() { return bMouseDown_; }

  inline int BottomBarIsVisible() { return bBottomBarEngaged_; }
  void CreateBottomBar();
  void DestroyBottomBar();
  void SizeBottomBar();
  void ShowBottomBar();
  void HideBottomBar();
  int GetBottomBarWidth();
  int GetBottomBarHeight();
  int InsideBottomBar(int nX, int nY);

  inline int BottomBarTimerActive() { return ApIsHandle(hBottomBarTimer_); }
  void RestartBottomBarTimer();
  void StartBottomBarTimer();
  void CancelBottomBarTimer();

protected:
  int nW_;
  int nH_;

  double fMouseDownX_;
  double fMouseDownY_;
  int nMouseDownW_;
  int nMouseDownH_;
  int bMouseDown_;

  int bBottomBarEngaged_;
  ApHandle hBottomBarTimer_;
};

#endif // Hud_H_INCLUDED
