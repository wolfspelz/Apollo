// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Layer_H_INCLUDED)
#define Layer_H_INCLUDED

#include "MsgScene.h"
#include "MsgTimer.h"

class Display;
class ArenaModule;

class Layer
{
public:
  Layer(Display* pDisplay);
  virtual ~Layer();

  virtual void OnSetSize(int nW, int nH);
  virtual int OnTimerEvent(Msg_Timer_Event* pMsg);
  virtual int OnMouseEvent(Msg_Scene_MouseEvent* pMsg);

protected:
  inline Display* GetDisplay() { return pDisplay_; }
  ArenaModule* GetModule();

protected:
  Display* pDisplay_;
  ApHandle hScene_;
};

#endif // Layer_H_INCLUDED
