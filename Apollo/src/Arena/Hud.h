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
  int OnMouseEvent(Msg_Scene_MouseEvent* pMsg);

protected:
};

#endif // Hud_H_INCLUDED
