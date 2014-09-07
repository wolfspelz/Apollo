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
  ApIN ApHandle hLogWindow;
};

// -> LogWindow
class Msg_LogWindow_Close: public ApRequestMessage
{
public:
  Msg_LogWindow_Close() : ApRequestMessage("LogWindow_Close") {}
  ApIN ApHandle hLogWindow;
};

// -> LogWindow
class Msg_LogWindow_SetFilterMask: public ApRequestMessage
{
public:
  Msg_LogWindow_SetFilterMask() : ApRequestMessage("LogWindow_SetFilterMask"), nMask(0) {}
  ApIN ApHandle hLogWindow;
  ApIN int nMask;
};

#endif // !defined(MsgLogWindow_h_INCLUDED)
