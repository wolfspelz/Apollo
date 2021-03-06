// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgAnimation_h_INCLUDED)
#define MsgAnimation_h_INCLUDED

#include "ApMessage.h"
#include "Image.h"

// -> Animation
class Msg_Animation_Create: public ApRequestMessage
{
public:
  Msg_Animation_Create() : ApRequestMessage("Animation_Create") {}
  ApIN ApHandle hItem;
  ApIN String sMimeType;
};

// -> Animation
class Msg_Animation_Destroy: public ApRequestMessage
{
public:
  Msg_Animation_Destroy() : ApRequestMessage("Animation_Destroy") {}
  ApIN ApHandle hItem;
};

// -> Animation
class Msg_Animation_Start: public ApRequestMessage
{
public:
  Msg_Animation_Start() : ApRequestMessage("Animation_Start") {}
  ApIN ApHandle hItem;
};

// -> Animation
class Msg_Animation_Stop: public ApRequestMessage
{
public:
  Msg_Animation_Stop() : ApRequestMessage("Animation_Stop") {}
  ApIN ApHandle hItem;
};

// -> Animation
class Msg_Animation_SetRate: public ApRequestMessage
{
public:
  Msg_Animation_SetRate() : ApRequestMessage("Animation_SetRate"), nMaxRate(10) {}
  ApIN ApHandle hItem;
  ApIN int nMaxRate;
};

// -> Animation
class Msg_Animation_SetData: public ApRequestMessage
{
public:
  Msg_Animation_SetData() : ApRequestMessage("Animation_SetData") {}
  ApIN ApHandle hItem;
  ApIN Buffer sbData;
  ApIN String sSourceUrl; // to resolve relative paths
};

// -> Animation
class Msg_Animation_SetStatus: public ApRequestMessage
{
public:
  Msg_Animation_SetStatus() : ApRequestMessage("Animation_SetStatus") {}
  ApIN ApHandle hItem;
  ApIN String sStatus;
};

// -> Animation
class Msg_Animation_SetCondition: public ApRequestMessage
{
public:
  Msg_Animation_SetCondition() : ApRequestMessage("Animation_SetCondition") {}
  ApIN ApHandle hItem;
  ApIN String sCondition;
};

// -> Animation
class Msg_Animation_Event: public ApRequestMessage
{
public:
  Msg_Animation_Event() : ApRequestMessage("Animation_Event") {}
  ApIN ApHandle hItem;
  ApIN String sEvent;
};

// -> Animation
class Msg_Animation_Activity: public ApRequestMessage
{
public:
  #define Msg_Animation_Activity_Idle "idle"
  #define Msg_Animation_Activity_MoveRight "moveright"
  #define Msg_Animation_Activity_MoveLeft "moveleft"
  Msg_Animation_Activity() : ApRequestMessage("Animation_Activity") {}
  ApIN ApHandle hItem;
  ApIN String sActivity;
};

//// -> Animation
//class Msg_Animation_Static: public ApRequestMessage
//{
//public:
//  Msg_Animation_Static() : ApRequestMessage("Animation_Static"), bState(1) {}
//  ApIN ApHandle hItem;
//  ApIN int bState;
//};

// -------------------------------------------------------------------
// Information

// -> Animation
class Msg_Animation_GetGroups: public ApRequestMessage
{
public:
  Msg_Animation_GetGroups() : ApRequestMessage("Animation_GetGroups") {}
  ApIN ApHandle hItem;
  ApOUT Apollo::ValueList vlGroups;
};

// -> Animation
class Msg_Animation_GetGroupSequences: public ApRequestMessage
{
public:
  Msg_Animation_GetGroupSequences() : ApRequestMessage("Animation_GetGroupSequences") {}
  ApIN ApHandle hItem;
  ApIN String sGroup;
  ApOUT Apollo::ValueList vlSequences;
};

// -> Animation
class Msg_Animation_GetSequenceInfo: public ApRequestMessage
{
public:
  Msg_Animation_GetSequenceInfo() : ApRequestMessage("Animation_GetSequenceInfo"), nProbability(0), nDx(0), nDy(0) {}
  ApIN ApHandle hItem;
  ApIN String sSequence;
  ApOUT String sGroup;
  ApOUT String sType;
  ApOUT String sCondition;
  ApOUT String sSrc;
  ApOUT int nProbability;
  ApOUT String sIn;
  ApOUT String sOut;
  ApOUT int nDx;
  ApOUT int nDy;
  ApOUT int nDuration;
};

//// -> Animation
//class Msg_Animation_SetPosition: public ApRequestMessage
//{
//public:
//  Msg_Animation_SetPosition() : ApRequestMessage("Animation_SetPosition"), nX(0), nY(0), nZ(0) {}
//  ApIN ApHandle hItem;
//  ApIN int nX;
//  ApIN int nY;
//  ApIN int nZ;
//};
//
//// -> Animation
//class Msg_Animation_MoveTo: public ApRequestMessage
//{
//public:
//  Msg_Animation_MoveTo() : ApRequestMessage("Animation_MoveTo"), nX(0), nY(0), nZ(0) {}
//  ApIN ApHandle hItem;
//  ApIN int nX;
//  ApIN int nY;
//  ApIN int nZ;
//};
//
//// -> Animation
//class Msg_Animation_GetPosition: public ApRequestMessage
//{
//public:
//  Msg_Animation_GetPosition() : ApRequestMessage("Animation_GetPosition"), nX(0), nY(0), nZ(0) {}
//  ApIN ApHandle hItem;
//  ApOUT int nX;
//  ApOUT int nY;
//  ApOUT int nZ;
//};

// -------------------------------------------------------------------

// Animation ->
class Msg_Animation_SequenceBegin: public ApNotificationMessage
{
public:
  Msg_Animation_SequenceBegin() : ApNotificationMessage("Animation_SequenceBegin") {}
  ApIN ApHandle hItem;
  ApIN String sName;
  ApIN String sGroup;
  ApIN String sSrc; // Original source URL
  ApIN String sUrl; // Use this URL. May point to internal pre-processed data
};

// Animation ->
class Msg_Animation_Frame: public ApNotificationMessage
{
public:
  Msg_Animation_Frame() : ApNotificationMessage("Animation_Frame") {}
  ApIN ApHandle hItem;
  ApIN Apollo::Image iFrame;
};

// Animation ->
class Msg_Animation_SequenceEnd: public ApNotificationMessage
{
public:
  Msg_Animation_SequenceEnd() : ApNotificationMessage("Animation_SequenceEnd") {}
  ApIN ApHandle hItem;
  ApIN String sName;
  ApIN String sGroup;
};

#endif // !defined(MsgAnimation_h_INCLUDED)
