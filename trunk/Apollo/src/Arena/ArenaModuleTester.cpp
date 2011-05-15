// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "MsgUnitTest.h"
#include "MsgVpView.h"
#include "ArenaModuleTester.h"

#if defined(AP_TEST)

String ArenaModuleTester::Dev()
{
  String s;

  ApHandle hContext = Apollo::newHandle();

  {
    Msg_VpView_ContextCreated msg;
    msg.hContext = hContext;
    msg.Send();
  }

  {
    Msg_VpView_ContextPosition msg;
    msg.hContext = hContext;
    msg.nLeft = 100;
    msg.nBottom = 600;
    msg.Send();
  }

  {
    Msg_VpView_ContextSize msg;
    msg.hContext = hContext;
    msg.nWidth = 1200;
    msg.nHeight = 500;
    msg.Send();
  }

  {
    Msg_VpView_ContextVisibility msg;
    msg.hContext = hContext;
    msg.bVisible = 1;
    msg.Send();
  }


  return s;
}

//----------------------------------------------------------

void ArenaModuleTester::Begin()
{
  //AP_UNITTEST_REGISTER(ArenaModuleTester::LoadHtml);
}

void ArenaModuleTester::Execute()
{
  //AP_UNITTEST_EXECUTE(ArenaModuleTester::LoadHtml);
  (void) ArenaModuleTester::Dev();
}

void ArenaModuleTester::End()
{
}

#endif // #if defined(AP_TEST)
