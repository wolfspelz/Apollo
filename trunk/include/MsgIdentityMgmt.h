// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgIdentityMgmt_h_INCLUDED)
#define MsgIdentityMgmt_h_INCLUDED

#include "ApMessage.h"

class Msg_IdentityMgmt_SetProperty: public ApRequestMessage
{
public:
  Msg_IdentityMgmt_SetProperty() : ApRequestMessage("IdentityMgmt_SetProperty") {}
  ApIN String sKey;
  ApIN String sValue;
};

#endif // !defined(MsgIdentityMgmt_h_INCLUDED)
