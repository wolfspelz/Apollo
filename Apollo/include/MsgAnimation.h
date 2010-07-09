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
  ApIN String sMimeType;
};

// -> Animator
class Msg_Animation_Destroy: public ApRequestMessage
{
public:
  Msg_Animation_Destroy() : ApRequestMessage("Animation_Destroy") {}
  ApIN ApHandle hItem;
};

// -> Animator
class Msg_Animation_Start: public ApRequestMessage
{
public:
  Msg_Animation_Start() : ApRequestMessage("Animation_Start") {}
  ApIN ApHandle hItem;
};

// -> Animator
class Msg_Animation_Stop: public ApRequestMessage
{
public:
  Msg_Animation_Stop() : ApRequestMessage("Animation_Stop") {}
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
  ApIN String sSourceUrl; // to resolve relative paths
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
class Msg_Animation_SetCondition: public ApRequestMessage
{
public:
  Msg_Animation_SetCondition() : ApRequestMessage("Animation_SetCondition") {}
  ApIN ApHandle hItem;
  ApIN String sCondition;
};

// -> Animator
class Msg_Animation_Event: public ApRequestMessage
{
public:
  Msg_Animation_Event() : ApRequestMessage("Animation_Event") {}
  ApIN ApHandle hItem;
  ApIN String sEvent;
};

// -> Animator
class Msg_Animation_SetPosition: public ApRequestMessage
{
public:
  Msg_Animation_SetPosition() : ApRequestMessage("Animation_SetPosition") {}
  ApIN ApHandle hItem;
  ApIN int nX;
  ApIN int nY;
  ApIN int nZ;
};

// -> Animator
class Msg_Animation_MoveTo: public ApRequestMessage
{
public:
  Msg_Animation_MoveTo() : ApRequestMessage("Animation_MoveTo") {}
  ApIN ApHandle hItem;
  ApIN int nX;
  ApIN int nY;
  ApIN int nZ;
};

// -------------------------------------------------------------------

// -> Animator
class Msg_Animation_GetPosition: public ApRequestMessage
{
public:
  Msg_Animation_GetPosition() : ApRequestMessage("Animation_GetPosition") {}
  ApIN ApHandle hItem;
  ApOUT int nX;
  ApOUT int nY;
  ApOUT int nZ;
};

// -------------------------------------------------------------------

// Animator ->
class Msg_Animation_SequenceBegin: public ApNotificationMessage
{
public:
  Msg_Animation_SequenceBegin() : ApNotificationMessage("SequenceBegin") {}
  ApIN ApHandle hItem;
  ApIN String sName;
};

// Animator ->
class Msg_Animation_Frame: public ApNotificationMessage
{
public:
  Msg_Animation_Frame() : ApNotificationMessage("Animation_Frame") {}
  ApIN ApHandle hItem;
  ApIN Apollo::Image iFrame;
};

// Animator ->
class Msg_Animation_SequenceEnd: public ApNotificationMessage
{
public:
  Msg_Animation_SequenceEnd() : ApNotificationMessage("SequenceEnd") {}
  ApIN ApHandle hItem;
  ApIN String sName;
};

// -------------------------------------------------------------------

// Animator -> Animator
class Msg_Animator_RequestAnimation: public ApRequestMessage
{
public:
  Msg_Animator_RequestAnimation() : ApRequestMessage("Animator_RequestAnimation") {}
  ApIN ApHandle hRequest;
  ApIN String sUrl;
};

// Animator -> Animator
// In response to Msg_Animator_RequestAnimation
class Msg_Animator_RequestAnimationComplete: public ApNotificationMessage
{
public:
  Msg_Animator_RequestAnimationComplete() : ApNotificationMessage("Animator_RequestAnimationComplete"), bSuccess(0) {}
  ApIN ApHandle hRequest;
  ApIN int bSuccess;
  ApIN String sUrl;
  ApIN Buffer sbData;
  ApIN String sMimeType;
};






#endif // !defined(MsgSample_h_INCLUDED)
