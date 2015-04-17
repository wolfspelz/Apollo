// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgLogWindow_h_INCLUDED)
#define MsgLogWindow_h_INCLUDED

#include "ApMessage.h"

// -> LogWindow
class Msg_LogWindow_Open: public ApRequestMessage
{
public:
  Msg_LogWindow_Open() : ApRequestMessage("LogWindow_Open") {}
};

// -> LogWindow
class Msg_LogWindow_Close: public ApRequestMessage
{
public:
  Msg_LogWindow_Close() : ApRequestMessage("LogWindow_Close") {}
};

#endif // !defined(MsgLogWindow_h_INCLUDED)
