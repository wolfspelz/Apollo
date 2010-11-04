// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
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

int Layer::OnTimer(const ApHandle& hTimer)
{
  int bProcessed = 0;
  return bProcessed;
}

int Layer::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  int bProcessed = 0;
  return bProcessed;
}

