// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Layer.h"
#include "Display.h"

Layer::Layer(Display* pDisplay)
:pDisplay_(pDisplay)
{
  hScene_ = pDisplay_->GetScene();
}

Layer::~Layer()
{
}

void Layer::OnSetSize(int nW, int nH)
{
}

int Layer::OnTimerEvent(Msg_Timer_Event* pMsg)
{
  int bProcessed = 0;
  return bProcessed;
}

int Layer::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  int bProcessed = 0;
  return bProcessed;
}


ArenaModule* Layer::GetModule()
{
  return GetDisplay()->GetModule();
}

