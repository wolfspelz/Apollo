// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "ApLog.h"
#include "MsgUnitTest.h"
#include "MsgAnimation.h"
#include "MsgMainLoop.h"
#include "MsgGalileo.h"
#include "MsgDB.h"

#if defined(AP_TEST_Galileo)

static void Test_Galileo_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Galileo_UnitTest_TokenEnd", "Finished Test/Galileo"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

// ----------------------------------

class Test_Galileo_Display_Controller
{
public:
  Test_Galileo_Display_Controller()
    :hInstance_(NULL)
    ,hWnd_(NULL)
    ,nCntWindows_(0)
  {}

  void CreateDisplay();
  void DestroyDisplay();

  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void Begin();
  void End();

  HINSTANCE hInstance_;
  HWND hWnd_;
  int nCntWindows_;
  ApHandle hItem_;
  String sOrigDb_;
};

#define Test_Galileo_Display_DB "GalileoDisplayTest"

void Test_Galileo_Display_Controller::Begin()
{
  { Msg_Galileo_SetStorageName msg; msg.sName = Test_Galileo_Display_DB; if (msg.Request()) { sOrigDb_ = msg.sPreviousName; } }
  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
}

void Test_Galileo_Display_Controller::End()
{
  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Galileo_SetStorageName msg; msg.sName = sOrigDb_; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Galileo_Display_DB; (void) msg.Request(); }
}

#define Test_Galileo_Display_WindowClass _T("Test_Galileo_DisplayClass")
#define Test_Galileo_Display_WindowCaption _T("Test_Galileo_DisplayCaption")

LRESULT CALLBACK Test_Galileo_Display_Controller::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT nResult = 0;

  if (message == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam; 
#pragma warning(push)
#pragma warning(disable : 4311)
    ::SetWindowLong(hWnd, GWL_USERDATA, (LONG) lpcs->lpCreateParams);
#pragma warning(pop)
    nResult = 0;
  } else {
#pragma warning(push)
#pragma warning(disable : 4312)
    Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) ::GetWindowLong(hWnd, GWL_USERDATA);
#pragma warning(pop)
    if (pTest_Galileo_Display_Controller != 0) {
      nResult = pTest_Galileo_Display_Controller->WndProc(hWnd, message, wParam, lParam);
    } else {
      nResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }
  }

  return nResult;
}

LRESULT CALLBACK Test_Galileo_Display_Controller::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void Test_Galileo_Display_Controller::CreateDisplay()
{
  {
    Msg_Win32_GetInstance msg;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "Msg_Win32_GetInstance failed"));
    } else {
      hInstance_ = msg.hInstance;
    }
  }

  if (hInstance_ != NULL) {
    if (nCntWindows_ == 0) {
      nCntWindows_++;

      WNDCLASSEX wcex;
      memset(&wcex, 0, sizeof(wcex));
      wcex.cbSize = sizeof(wcex);
      wcex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
      wcex.hCursor = LoadCursor(NULL, IDC_HAND);
      wcex.hInstance = hInstance_;
      wcex.lpszClassName = Test_Galileo_Display_WindowClass;
      wcex.lpfnWndProc = Test_Galileo_Display_Controller::StaticWndProc;
      BOOL bOK = ::RegisterClassEx(&wcex);
      if (!bOK) {
        apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "RegisterClassEx failed"));
      }
    }

    hWnd_ = ::CreateWindowEx(
      WS_EX_TOOLWINDOW, 
      Test_Galileo_Display_WindowClass, 
      Test_Galileo_Display_WindowCaption, 
      0, 
      0, 200, 100, 100, 
      NULL, 
      NULL, 
      hInstance_, 
      this
      );
    if (hWnd_ == NULL) {
      apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "CreateWindowEx failed"));
    }
  }
}

void Test_Galileo_Display_Controller::DestroyDisplay()
{
  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;

    nCntWindows_--;
    if (nCntWindows_ == 0) {
      ::UnregisterClass(Test_Galileo_Display_WindowClass, hInstance_);
    }
  }
}

static void Test_Galileo_Display_Animation_SequenceBegin(Msg_Animation_SequenceBegin* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  if (pTest_Galileo_Display_Controller->hItem_ != pMsg->hItem) { return; }
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Animation_SequenceBegin", "" ApHandleFormat "", ApHandleType(pMsg->hItem)));
  pTest_Galileo_Display_Controller->CreateDisplay();
}

static void Test_Galileo_Display_Animation_Frame(Msg_Animation_Frame* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  if (pTest_Galileo_Display_Controller->hItem_ != pMsg->hItem) { return; }
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Animation_Frame", "" ApHandleFormat "", ApHandleType(pMsg->hItem)));
}

static void Test_Galileo_Display_Galileo_RequestAnimation(Msg_Galileo_RequestAnimation* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Galileo_RequestAnimation", "%s", StringType(pMsg->sUrl)));

  //String sFile = String::filenameFile(pMsg->sUrl);

  //ApAsyncMessage<Msg_Galileo_RequestAnimationComplete> msg;
  //msg->hRequest = pMsg->hRequest;
  //msg->bSuccess = 1;
  //msg->sUrl = pMsg->sUrl;
  //Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + sFile, msg->sbData);
  //msg->sMimeType = "image/gif";
  //msg.Post();

  pMsg->apStatus = ApMessage::Ok;
}

//static void Test_Galileo_Display_Galileo_RequestAnimationComplete(Msg_Galileo_RequestAnimationComplete* pMsg)
//{
//  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
//  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Galileo_RequestAnimationComplete", "%s", StringType(pMsg->sUrl)));
//  pMsg->apStatus = ApMessage::Ok;
//}

static void Test_Galileo_Display_Galileo_SaveAnimationDataToStorage(Msg_Galileo_SaveAnimationDataToStorage* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Galileo_SaveAnimationDataToStorage", "%s", StringType(pMsg->sUrl)));
  pMsg->apStatus = ApMessage::Ok;
}

static void Test_Galileo_Display_Galileo_IsAnimationDataInStorage(Msg_Galileo_IsAnimationDataInStorage* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Galileo_IsAnimationDataInStorage", "%s", StringType(pMsg->sUrl)));
  pMsg->bAvailable = 1;
  pMsg->apStatus = ApMessage::Ok;
}

static void Test_Galileo_Display_Galileo_LoadAnimationDataFromStorage(Msg_Galileo_LoadAnimationDataFromStorage* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Galileo_LoadAnimationDataFromStorage", "%s", StringType(pMsg->sUrl)));

  String sFile = String::filenameFile(pMsg->sUrl);
  Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + sFile, pMsg->sbData);
  pMsg->sMimeType = "image/gif";

  pMsg->apStatus = ApMessage::Ok;
}

static void Test_Galileo_Display_Animation_SequenceEnd(Msg_Animation_SequenceEnd* pMsg)
{
  String s;

  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  if (pTest_Galileo_Display_Controller->hItem_ != pMsg->hItem) { return; }
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Animation_SequenceEnd", "" ApHandleFormat "", ApHandleType(pMsg->hItem)));

  { Msg_Animation_SequenceBegin msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceBegin, pTest_Galileo_Display_Controller); }
  { Msg_Animation_Frame msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_Frame, pTest_Galileo_Display_Controller); }
  { Msg_Animation_SequenceEnd msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceEnd, pTest_Galileo_Display_Controller); }
  { Msg_Galileo_RequestAnimation msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimation, pTest_Galileo_Display_Controller); }
  //{ Msg_Galileo_RequestAnimationComplete msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimationComplete, pTest_Galileo_Display_Controller); }
  { Msg_Galileo_SaveAnimationDataToStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_SaveAnimationDataToStorage, pTest_Galileo_Display_Controller); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_IsAnimationDataInStorage, pTest_Galileo_Display_Controller); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_LoadAnimationDataFromStorage, pTest_Galileo_Display_Controller); }

  {
    Msg_Animation_Stop msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.Request();
  }

  {
    Msg_Animation_Destroy msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.Request();
  }

  AP_UNITTEST_RESULT(Test_Galileo_Display_End, s.empty(), s);

  pTest_Galileo_Display_Controller->DestroyDisplay();
  pTest_Galileo_Display_Controller->End();

  delete pTest_Galileo_Display_Controller;
  pTest_Galileo_Display_Controller = 0;

  Test_Galileo_UnitTest_TokenEnd();
}

#define Test_Galileo_Display_Url "http://ydentiti.org/test/Galileo/avatar.xml"
#define Test_Galileo_Display_Data \
"<config xmlns='http://schema.bluehands.de/character-config' version='1.0'>\n" \
"  <param name='defaultsequence' value='idle'/>\n" \
"  <sequence group='idle' name='still' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"  <sequence group='idle' name='idle1' type='status' probability='100' in='standard' out='standard'><animation src='idle-1.gif'/></sequence>\n" \
"  <sequence group='moveleft' name='moveleft' type='basic' probability='1' in='moveleft' out='moveleft'><animation dx='-55' dy='0' src='walk-l.gif'/></sequence>\n" \
"  <sequence group='moveright' name='moveright' type='basic' probability='1' in='moveright' out='moveright'><animation dx='55' dy='0' src='walk-r.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat1' type='basic' probability='1000' in='standard' out='standard'><animation src='chat.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat2' type='basic' probability='100' in='standard' out='standard'><animation src='chat-2.gif'/></sequence>\n" \
"  <sequence group='wave' name='wave' type='emote' probability='1000' in='standard' out='standard'><animation src='wave.gif'/></sequence>\n" \
"  <sequence group='sleep' name='sleep' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"</config>"

String Test_Galileo_Display_Begin()
{
  String s;

  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = new Test_Galileo_Display_Controller();
  pTest_Galileo_Display_Controller->hItem_ = Apollo::newHandle();

  { Msg_Animation_SequenceBegin msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceBegin, pTest_Galileo_Display_Controller, ApCallbackPosNormal); }
  { Msg_Animation_Frame msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_Frame, pTest_Galileo_Display_Controller, ApCallbackPosNormal); }
  { Msg_Animation_SequenceEnd msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceEnd, pTest_Galileo_Display_Controller, ApCallbackPosNormal); }
  { Msg_Galileo_RequestAnimation msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimation, pTest_Galileo_Display_Controller, ApCallbackPosEarly); }
  //{ Msg_Galileo_RequestAnimationComplete msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimationComplete, pTest_Galileo_Display_Controller, ApCallbackPosEarly); }
  { Msg_Galileo_SaveAnimationDataToStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_SaveAnimationDataToStorage, pTest_Galileo_Display_Controller, ApCallbackPosEarly); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_IsAnimationDataInStorage, pTest_Galileo_Display_Controller, ApCallbackPosEarly); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_LoadAnimationDataFromStorage, pTest_Galileo_Display_Controller, ApCallbackPosEarly); }

  pTest_Galileo_Display_Controller->Begin();

  if (!s) {
    Msg_Animation_Create msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.sMimeType = "avatar/gif";
    if (!msg.Request()) {
      s = "Msg_Animation_Create failed";
    }
  }

  if (!s) {
    Msg_Animation_SetRate msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.nMaxRate = 10;
    if (!msg.Request()) {
      s = "Msg_Animation_SetRate failed";
    }
  }

  if (!s) {
    Msg_Animation_SetData msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.sbData.SetData(Test_Galileo_Display_Data);
    msg.sSourceUrl = Test_Galileo_Display_Url;
    if (!msg.Request()) {
      s = "Msg_Animation_SetData failed";
    }
  }

  if (!s) {
    Msg_Animation_SetStatus msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.sStatus = "idle";
    if (!msg.Request()) {
      s = "Msg_Animation_SetStatus failed";
    }
  }

  if (!s) {
    Msg_Animation_Event msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    msg.sEvent = "wave";
    if (!msg.Request()) {
      s = "Msg_Animation_Event failed";
    }
  }

  if (!s) {
    Msg_Animation_Start msg;
    msg.hItem = pTest_Galileo_Display_Controller->hItem_;
    if (!msg.Request()) {
      s = "Msg_Animation_Start failed";
    }
  }

  return s;
}

//---------------

static void Test_Galileo_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Galileo_UnitTest_Token", "Starting Test/Galileo"));
  int bTokenEndNow = 1;
  
  AP_UNITTEST_EXECUTE(Test_Galileo_Display_Begin); bTokenEndNow = 0;

  if (bTokenEndNow) { Test_Galileo_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#endif // AP_TEST_Galileo

void Test_Galileo_Register()
{
#if defined(AP_TEST_Galileo)
  if (Apollo::isLoadedModule("Galileo")) {
    AP_UNITTEST_REGISTER(Test_Galileo_Display_Begin);
    AP_UNITTEST_REGISTER(Test_Galileo_Display_End);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Galileo
}

void Test_Galileo_Begin()
{
#if defined(AP_TEST_Galileo)
  if (Apollo::isLoadedModule("Galileo")) {
  }
#endif // AP_TEST_Galileo
}

void Test_Galileo_End()
{
#if defined(AP_TEST_Galileo)
  if (Apollo::isLoadedModule("Galileo")) {
  }
#endif // AP_TEST_Galileo
}
