// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgVp_h_INCLUDED)
#define MsgVp_h_INCLUDED

#include "ApMessage.h"

// input -> VP module
class Msg_Vp_OpenContext: public ApRequestMessage
{
public:
  Msg_Vp_OpenContext() : ApRequestMessage("Vp_OpenContext") {}
  ApIN ApHandle hContext;
};

// -> VP module
class Msg_Vp_NavigateContext: public ApRequestMessage
{
public:
  Msg_Vp_NavigateContext() : ApRequestMessage("Vp_NavigateContext") {}
  ApIN ApHandle hContext;
  ApIN String sUrl;
};

// input -> VP module
class Msg_Vp_CloseContext: public ApRequestMessage
{
public:
  Msg_Vp_CloseContext() : ApRequestMessage("Vp_CloseContext") {}
  ApIN ApHandle hContext;
};

// -------------------------------

// vp -> vp
class Msg_Vp_CreateContext: public ApRequestMessage
{
public:
  Msg_Vp_CreateContext() : ApRequestMessage("Vp_CreateContext") {}
  ApIN ApHandle hContext;
};

// vp -> vp
class Msg_Vp_DestroyContext: public ApRequestMessage
{
public:
  Msg_Vp_DestroyContext() : ApRequestMessage("Vp_DestroyContext") {}
  ApIN ApHandle hContext;
};

// vp -> vp
class Msg_Vp_CreateLocation: public ApRequestMessage
{
public:
  Msg_Vp_CreateLocation() : ApRequestMessage("Vp_CreateLocation") {}
  ApIN ApHandle hLocation;
  ApIN String sLocationUrl;
};

// vp -> vp
class Msg_Vp_DestroyLocation: public ApRequestMessage
{
public:
  Msg_Vp_DestroyLocation() : ApRequestMessage("Vp_DestroyLocation") {}
  ApIN ApHandle hLocation;
};

// vp -> vp
// Add a context to a location 
class Msg_Vp_AddLocationContext: public ApRequestMessage
{
public:
  Msg_Vp_AddLocationContext() : ApRequestMessage("Vp_AddLocationContext") {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hContext;
};

// vp -> vp
// Remove a context from a location 
class Msg_Vp_RemoveLocationContext: public ApRequestMessage
{
public:
  Msg_Vp_RemoveLocationContext() : ApRequestMessage("Vp_RemoveLocationContext") {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hContext;
};

//-----------------------------------------

// vp -> vp
// After URL resolved and if not yet entered: physically enter a location 
class Msg_Vp_EnterLocation: public ApRequestMessage
{
public:
  Msg_Vp_EnterLocation() : ApRequestMessage("Vp_EnterLocation") {}
  ApIN ApHandle hLocation;
};

// vp -> vp
// Physically leave a location if last context disappears
class Msg_Vp_LeaveLocation: public ApRequestMessage
{
public:
  Msg_Vp_LeaveLocation() : ApRequestMessage("Vp_LeaveLocation") {}
  ApIN ApHandle hLocation;
};

// vp -> vp
// Process chat command
class Msg_Vp_FilterPublicChat: public ApFilterMessage
{
public:
  Msg_Vp_FilterPublicChat() : ApFilterMessage("Vp_FilterPublicChat") {}
  ApIN ApHandle hLocation;
  ApIN ApHandle hParticipant;
  ApINOUT String sText;
};

//-----------------------------------------

class Msg_Vp_SendPublicChat: public ApRequestMessage
{
public:
  Msg_Vp_SendPublicChat() : ApRequestMessage("Vp_SendPublicChat") {}
  ApIN ApHandle hLocation;
  ApIN String sText;
};

//-----------------------------------------

class Msg_Vp_SendPosition: public ApRequestMessage
{
public:
  Msg_Vp_SendPosition() : ApRequestMessage("Vp_SendPosition") {}
  ApIN ApHandle hLocation;
  ApIN Apollo::KeyValueList kvParams;
};

class Msg_Vp_SendCondition: public ApRequestMessage
{
public:
  Msg_Vp_SendCondition() : ApRequestMessage("Vp_SendCondition") {}
  ApIN ApHandle hLocation;
  ApIN Apollo::KeyValueList kvParams;
};

#endif // !defined(MsgVp_h_INCLUDED)
