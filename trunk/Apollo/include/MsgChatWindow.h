// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgChatWindow_h_INCLUDED)
#define MsgChatWindow_h_INCLUDED

#include "ApMessage.h"

class Msg_ChatWindow_OpenForLocation: public ApRequestMessage
{
public:
  Msg_ChatWindow_OpenForLocation() : ApRequestMessage("ChatWindow_OpenForLocation") {}
  ApIN ApHandle hChat;
  ApIN ApHandle hLocation;
};

class Msg_ChatWindow_Close: public ApRequestMessage
{
public:
  Msg_ChatWindow_Close() : ApRequestMessage("ChatWindow_Close") {}
  ApIN ApHandle hChat;
};

#endif // !defined(MsgChatWindow_h_INCLUDED)
