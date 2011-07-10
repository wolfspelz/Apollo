// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSetup_h_INCLUDED)
#define MsgSetup_h_INCLUDED

#include "ApMessage.h"

class Msg_Setup_Open: public ApRequestMessage
{
public:
  Msg_Setup_Open() : ApRequestMessage("Setup_Open") {}
};

class Msg_Setup_Close: public ApRequestMessage
{
public:
  Msg_Setup_Close() : ApRequestMessage("Setup_Close") {}
};

#endif // !defined(MsgSetup_h_INCLUDED)
