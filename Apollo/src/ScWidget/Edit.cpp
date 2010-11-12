// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgScWidget.h"
#include "Edit.h"

Edit::Edit(const ApHandle& hScene, const String& sPath)
:Widget(hScene, sPath)
{
}

void Edit::Create()
{
  Msg_Scene_CreateMouseSensor::_(hScene_, sPath_, fX_, fY_, fW_, fH_);
}

