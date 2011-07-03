// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgConfig_h_INCLUDED)
#define MsgConfig_h_INCLUDED

#include "ApMessage.h"

class Msg_TrayIcon_Hello: public ApNotificationMessage
{
public:
  Msg_TrayIcon_Hello() : ApNotificationMessage("TrayIcon_Hello") {}
};

class Msg_TrayIcon_Status: public ApNotificationMessage
{
public:
  Msg_TrayIcon_Status() : ApNotificationMessage("TrayIcon_Status"), nConnections(0), nLogins(0) {}
  ApIN int nConnections;
  ApIN int nLogins;
};

#endif // !defined(MsgConfig_h_INCLUDED)
