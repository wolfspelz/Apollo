// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSample_h_INCLUDED)
#define MsgSample_h_INCLUDED

#include "ApMessage.h"
#include "Image.h"

// -> Animator
class Msg_Animation_Create: public ApRequestMessage
{
public:
  Msg_Animation_Create() : ApRequestMessage("Animation_Create") {}
  ApIN ApHandle hItem;
};

// -> Animator
class Msg_Animation_Destroy: public ApRequestMessage
{
public:
  Msg_Animation_Destroy() : ApRequestMessage("Animation_Destroy") {}
  ApIN ApHandle hItem;
};

// -> Animator
class Msg_Animation_SetRate: public ApRequestMessage
{
public:
  Msg_Animation_SetRate() : ApRequestMessage("Animation_SetRate"), nMaxRate(10) {}
  ApIN ApHandle hItem;
  ApIN int nMaxRate;
};

// -> Animator
class Msg_Animation_SetData: public ApRequestMessage
{
public:
  Msg_Animation_SetData() : ApRequestMessage("Animation_SetData") {}
  ApIN ApHandle hItem;
  ApIN Buffer sbData;
  ApIN String sOriginalUrl;
  ApIN String sMimeType;
};

// -> Animator
class Msg_Animation_SetStatus: public ApRequestMessage
{
public:
  Msg_Animation_SetStatus() : ApRequestMessage("Animation_SetStatus") {}
  ApIN ApHandle hItem;
  ApIN String sStatus;
};

// -> Animator
class Msg_Animation_Event: public ApRequestMessage
{
public:
  Msg_Animation_Event() : ApRequestMessage("Animation_Event") {}
  ApIN ApHandle hItem;
  ApIN String sEvent;
};

// -------------------------------------------------------------------

// Animator ->
class Msg_Animation_Frame: public ApNotificationMessage
{
public:
  Msg_Animation_Frame() : ApNotificationMessage("Animation_Frame") {}
  ApIN ApHandle hItem;
  ApIN Apollo::Image iFrame;
};

#endif // !defined(MsgSample_h_INCLUDED)
