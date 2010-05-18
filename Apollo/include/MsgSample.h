// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSample_h_INCLUDED)
#define MsgSample_h_INCLUDED

#include "ApMessage.h"

class Msg_Sample_Get: public ApRequestMessage
{
public:
  Msg_Sample_Get() : ApRequestMessage("Sample_Get") {}
  ApOUT int nValue;
};

#endif // !defined(MsgSample_h_INCLUDED)
