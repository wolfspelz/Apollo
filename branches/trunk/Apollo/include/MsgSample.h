// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSample_h_INCLUDED)
#define MsgSample_h_INCLUDED

#include "ApMessage.h"

class Msg_Sample_Create: public ApRequestMessage
{
public:
  Msg_Sample_Create() : ApRequestMessage("Sample_Create") {}
  ApIN ApHandle hSample;
};

class Msg_Sample_Destroy: public ApRequestMessage
{
public:
  Msg_Sample_Destroy() : ApRequestMessage("Sample_Destroy") {}
  ApIN ApHandle hSample;
};

class Msg_Sample_Get: public ApRequestMessage
{
public:
  Msg_Sample_Get() : ApRequestMessage("Sample_Get"), nValue(0) {}
  ApIN ApHandle hSample;
  ApIN String sKey;
  ApOUT int nValue;
};

#endif // !defined(MsgSample_h_INCLUDED)
