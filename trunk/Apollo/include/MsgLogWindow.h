// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgLogWindow_h_INCLUDED)
#define MsgLogWindow_h_INCLUDED

#include "ApMessage.h"

class Msg_LogWindow_Open: public ApRequestMessage
{
public:
  Msg_LogWindow_Open() : ApRequestMessage("LogWindow_Open") {}
};

class Msg_LogWindow_Close: public ApRequestMessage
{
public:
  Msg_LogWindow_Close() : ApRequestMessage("LogWindow_Close") {}
};

class Msg_LogWindow_Test: public ApRequestMessage
{
public:
  Msg_LogWindow_Test() : ApRequestMessage("LogWindow_Test") {}
};

#endif // !defined(MsgLogWindow_h_INCLUDED)
