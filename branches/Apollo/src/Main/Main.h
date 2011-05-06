// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Main_h_INCLUDED)
#define Main_h_INCLUDED

#include "Apollo.h"
#include "MsgMainLoop.h"
#include "MsgSystem.h"
#include "MsgCore.h"
#include "MsgConfig.h"
#include "MsgLog.h"
#include "ApLog.h"

void On_Log_Line(Msg_Log_Line* pMsg);
void On_Log_SetMask(Msg_Log_SetMask* pMsg);
void On_MainLoop_ConsoleInput(Msg_MainLoop_ConsoleInput* pMsg);

#endif
