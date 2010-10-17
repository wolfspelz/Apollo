// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "MsgFluid.h"
#include "MsgOS.h"
#include "Display.h"

int Display::start()
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "Display::start", "Client " ApHandleFormat " starting display process, exe=%s", ApHandleType(hAp_), StringType(sPathname_)));

  {
    // Kill existing display processes of same name
    String sFilename = String::filenameFile(sPathname_);
    int bKillNext = 1;
    while (bKillNext) {
      bKillNext = 0;
      Msg_OS_GetProcessId msgGet;
      msgGet.sName = sFilename;
      if (msgGet.Request()) {
        Msg_OS_KillProcess msgKill;
        msgKill.nPid = msgGet.nPid;
        if (msgKill.Request()) {
          bKillNext = 1;
        } else {
          apLog_Error((LOG_CHANNEL, "Display::start", "Client " ApHandleFormat " Msg_OS_KillProcess() failed, exe=%s", ApHandleType(hAp_), StringType(sPathname_)));
        }
      } else {
        // not an error, just no such process
        //apLog_Error((LOG_CHANNEL, "Display::start", "Client " ApHandleFormat " Msg_OS_GetProcessId() failed, exe=%s", ApHandleType(hAp_), StringType(msgGet.sName)));
      }
    }
  }

  {
    Msg_OS_StartProcess msg;
    msg.sExePath = sPathname_;
    msg.sArgs = sArgs_;
    msg.sCwdPath = String::filenameBasePath(sPathname_);
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Display::start", "Client " ApHandleFormat " Msg_OS_CreateProcess() failed, exe=%s", ApHandleType(hAp_), StringType(sPathname_)));
    } else {
      nPid_ = msg.nPid;
    }
  }

  return ok;
}

int Display::stop()
{
  int ok = 1;

  apLog_Verbose((LOG_CHANNEL, "Display::stop", "Client " ApHandleFormat " stopping display process, exe=%s", ApHandleType(hAp_), StringType(sPathname_)));

  Msg_OS_KillProcess msg;
  msg.nPid = nPid_;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "Display::stop", "Client " ApHandleFormat " Msg_OS_KillProcess() failed, exe=%s", ApHandleType(hAp_), StringType(sPathname_)));
  }

  return ok;
}

void Display::onCreated(const ApHandle& hConnection)
{
  if (!bCreated_) {
    bCreated_ = 1;

    Msg_Fluid_Created msg;
    msg.hDisplay = hAp_;
    msg.hConnection = hConnection;
    msg.Send();
  }
}

void Display::onLoaded()
{
  if (!bLoaded_) {
    bLoaded_ = 1;

    Msg_Fluid_Loaded msg;
    msg.hDisplay = hAp_;
    msg.Send();
  }
}

void Display::onBeforeUnload()
{
  if (bLoaded_) {
    Msg_Fluid_BeforeUnload msg;
    msg.hDisplay = hAp_;
    msg.Send();
  }
}

void Display::onUnloaded()
{
  if (bLoaded_) {
    bLoaded_ = 0;

    Msg_Fluid_Unloaded msg;
    msg.hDisplay = hAp_;
    msg.Send();
  }
}

void Display::onDestroyed()
{
  if (bCreated_) {
    if (bLoaded_) {
      onUnloaded();
    }

    bCreated_ = 0;

    Msg_Fluid_Destroyed msg;
    msg.hDisplay = hAp_;
    msg.Send();
  }
}
