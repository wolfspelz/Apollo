// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScene_h_INCLUDED)
#define MsgScene_h_INCLUDED

#include "ApMessage.h"

class Msg_Scene_Get: public ApRequestMessage
{
public:
  Msg_Scene_Get() : ApRequestMessage("Scene_Get") {}
  ApOUT int nValue;
};

#endif // !defined(MsgScene_h_INCLUDED)
