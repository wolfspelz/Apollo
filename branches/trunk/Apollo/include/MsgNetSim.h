// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgNetSim_h_INCLUDED)
#define MsgNetSim_h_INCLUDED

#include "ApMessage.h"

class Msg_NetSim_Engage: public ApRequestMessage
{
public:
  Msg_NetSim_Engage() : ApRequestMessage("NetSim_Engage") {}
};

class Msg_NetSim_Disengage: public ApRequestMessage
{
public:
  Msg_NetSim_Disengage() : ApRequestMessage("NetSim_Disengage") {}
};

// --------------------------

class Msg_NetSim_ContinueHttp: public ApRequestMessage
{
public:
  Msg_NetSim_ContinueHttp() : ApRequestMessage("NetSim_ContinueHttp") {}
  ApIN ApHandle hClient;
};

#endif // !defined(MsgNetSim_h_INCLUDED)
