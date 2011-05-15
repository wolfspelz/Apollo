// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSample_h_INCLUDED)
#define MsgSample_h_INCLUDED

#include "ApMessage.h"

class Msg_Dialog_XX: public ApRequestMessage
{
public:
  Msg_Dialog_XX() : ApRequestMessage("Dialog_XX") {}
  ApOUT int nValue;
};

#endif // !defined(MsgSample_h_INCLUDED)
