// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgUnitTest_h_INCLUDED)
#define MsgUnitTest_h_INCLUDED

#include "ApMessage.h"

// UnitTest module intercepts RunLevel "Normal" inserts "Test" and defers "Normal" until after the tests
#define Msg_System_RunLevel_Test "Test"

// UnitTest ->
class Msg_UnitTest_Begin: public ApNotificationMessage
{
public:
  Msg_UnitTest_Begin() : ApNotificationMessage("UnitTest_Begin") {}
};

// UnitTest ->
class Msg_UnitTest_Execute: public ApNotificationMessage
{
public:
  Msg_UnitTest_Execute() : ApNotificationMessage("UnitTest_Execute") {}
};

// UnitTest ->
class Msg_UnitTest_End: public ApNotificationMessage
{
public:
  Msg_UnitTest_End() : ApNotificationMessage("UnitTest_End") {}
};

// UnitTest <-
class Msg_UnitTest_Evaluate: public ApNotificationMessage
{
public:
  Msg_UnitTest_Evaluate() : ApNotificationMessage("UnitTest_Evaluate") {}
};

// UnitTest -> UnitTestGUI
class Msg_UnitTest_EvaluateResult: public ApNotificationMessage
{
public:
  Msg_UnitTest_EvaluateResult() : ApNotificationMessage("UnitTest_EvaluateResult") {}
  ApIN Apollo::ValueList vlMissingTests;
  ApIN long nTotal;
  ApIN long nFailed;
  ApIN long nUnknown;
};

// --------------------------------
// Individual tests

// UnitTest <-
class Msg_UnitTest_Register: public ApNotificationMessage
{
public:
  Msg_UnitTest_Register() : ApNotificationMessage("UnitTest_Register") {}
  ApIN String sName;
};

// UnitTest <-
class Msg_UnitTest_Complete: public ApNotificationMessage
{
public:
  Msg_UnitTest_Complete() : ApNotificationMessage("UnitTest_Complete"), bSuccess(0) {}
  ApIN String sName;
  ApIN int bSuccess;
  ApIN String sMessage;
};

// --------------------------------
// Progress

// UnitTest -> 
class Msg_UnitTest_Range: public ApNotificationMessage
{
public:
  Msg_UnitTest_Range() : ApNotificationMessage("UnitTest_Range"), nRange(0) {}
  ApIN long nRange;
};

// UnitTest -> 
class Msg_UnitTest_Progress: public ApNotificationMessage
{
public:
  Msg_UnitTest_Progress() : ApNotificationMessage("UnitTest_Progress"), nProgress(0), bSuccess(0), bKnown(0) {}
  ApIN long nProgress;
  ApIN String sName;
  ApIN int bSuccess;
  ApIN int bKnown;
  ApIN String sMessage;
};

// UnitTest -> 
class Msg_UnitTest_Done: public ApNotificationMessage
{
public:
  Msg_UnitTest_Done() : ApNotificationMessage("UnitTest_Done"), bSuccess(0) {}
  ApIN int bSuccess;
};

// UnitTest -> module -> module
class Msg_UnitTest_Token: public ApNotificationMessage
{
public:
  Msg_UnitTest_Token() : ApNotificationMessage("UnitTest_Token") { bForward_ = false; }
};

// -------------------------------------------------------------------

#define AP_UNITTEST_REGISTER(name_) { Msg_UnitTest_Register msg__; msg__.sName = # name_; msg__.Send(); }
#define AP_UNITTEST_SUCCESS(name_) { Msg_UnitTest_Complete msg__; msg__.sName = # name_; msg__.bSuccess = 1; msg__.sMessage = ""; msg__.Send(); }
#define AP_UNITTEST_FAILED(name_, text_) { Msg_UnitTest_Complete msg__; msg__.sName = # name_; msg__.bSuccess = 0; msg__.sMessage = text_; msg__.Send(); }
#define AP_UNITTEST_RESULT(name_, success_, text_) { Msg_UnitTest_Complete msg__; msg__.sName = # name_; msg__.bSuccess = success_; msg__.sMessage = text_; msg__.Send(); }
#define AP_UNITTEST_EXECUTE(name_) do { String s__ = name_(); Msg_UnitTest_Complete msg__; msg__.sName = # name_; msg__.bSuccess = s__.empty(); msg__.sMessage = s__; msg__.Send(); } while (0);
// Deprecated:
#define AP_UNITTEST_EXECUTE1(name_) do { int ok__ = name_(); Msg_UnitTest_Complete msg__; msg__.sName = # name_; msg__.bSuccess = ok__; msg__.Send(); } while (0);

// Optional support for module implementation
#if defined(AP_TEST)
  #define AP_UNITTEST_HOOK(class_, instance_) \
    { Msg_UnitTest_Begin msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(class_, UnitTest_Begin), instance_, ApCallbackPosNormal); }\
    { Msg_UnitTest_Execute msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(class_, UnitTest_Execute), instance_, ApCallbackPosNormal); }\
    { Msg_UnitTest_End msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(class_, UnitTest_End), instance_, ApCallbackPosNormal); }

  #define AP_UNITTEST_UNHOOK(class_, instance_) \
    { Msg_UnitTest_Begin msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(class_, UnitTest_Begin), instance_); }\
    { Msg_UnitTest_Execute msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(class_, UnitTest_Execute), instance_); }\
    { Msg_UnitTest_End msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(class_, UnitTest_End), instance_); }

  #define AP_UNITTEST_DECLAREHOOK() \
    void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);\
    void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);\
    void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#else
  #define AP_UNITTEST_HOOK(class_, instance_)
  #define AP_UNITTEST_UNHOOK(class_, instance_)
  #define AP_UNITTEST_DECLAREHOOK()
#endif // #if defined(AP_TEST)

#endif // !defined(MsgUnitTest_h_INCLUDED)
