// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSetup_h_INCLUDED)
#define MsgSetup_h_INCLUDED

#include "ApMessage.h"

class Msg_Setup_Get: public ApRequestMessage
{
public:
  Msg_Setup_Get() : ApRequestMessage("Setup_Get"), nValue(0) {}
  ApIN ApHandle hSetup;
  ApIN String sKey;
  ApOUT int nValue;
};

#endif // !defined(MsgSetup_h_INCLUDED)
