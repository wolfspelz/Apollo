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

#if defined(AP_TEST_Nimator)

static void Test_Nimator_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Nimator_UnitTest_TokenEnd", "Finished Test/Nimator"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

// ----------------------------------

class Test_Nimator_Display_Controller
{
public:
  Test_Nimator_Display_Controller()
    :hInstance_(NULL)
    ,hWnd_(NULL)
    ,nCntWindows_(0)
  {}

  void CreateDisplay();
  void DestroyDisplay();

  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HINSTANCE hInstance_;
  HWND hWnd_;
  int nCntWindows_;
  ApHandle hItem_;
};

#define Test_Nimator_Display_WindowClass _T("Test_Nimator_DisplayClass")
#define Test_Nimator_Display_WindowCaption _T("Test_Nimator_Display")

LRESULT CALLBACK Test_Nimator_Display_Controller::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    Test_Nimator_Display_Controller* pTest_Nimator_Display_Controller = (Test_Nimator_Display_Controller*) ::GetWindowLong(hWnd, GWL_USERDATA);
#pragma warning(pop)
    if (pTest_Nimator_Display_Controller != 0) {
      nResult = pTest_Nimator_Display_Controller->WndProc(hWnd, message, wParam, lParam);
    } else {
      nResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }
  }

  return nResult;
}

LRESULT CALLBACK Test_Nimator_Display_Controller::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void Test_Nimator_Display_Controller::CreateDisplay()
{
  {
    Msg_Win32_GetInstance msg;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Test_Nimator_Display_Controller::CreateDisplay", "Msg_Win32_GetInstance failed"));
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
      wcex.lpszClassName = Test_Nimator_Display_WindowClass;
      wcex.lpfnWndProc = Test_Nimator_Display_Controller::StaticWndProc;
      BOOL bOK = ::RegisterClassEx(&wcex);
      if (!bOK) {
        apLog_Error((LOG_CHANNEL, "Test_Nimator_Display_Controller::CreateDisplay", "RegisterClassEx failed"));
      }
    }

    hWnd_ = ::CreateWindowEx(
      WS_EX_TOOLWINDOW, 
      Test_Nimator_Display_WindowClass, 
      Test_Nimator_Display_WindowCaption, 
      0, 
      0, 200, 100, 100, 
      NULL, 
      NULL, 
      hInstance_, 
      this
      );
    if (hWnd_ == NULL) {
      apLog_Error((LOG_CHANNEL, "Test_Nimator_Display_Controller::CreateDisplay", "CreateWindowEx failed"));
    }
  }
}

void Test_Nimator_Display_Controller::DestroyDisplay()
{
  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;

    nCntWindows_--;
    if (nCntWindows_ == 0) {
      ::UnregisterClass(Test_Nimator_Display_WindowClass, hInstance_);
    }
  }
}

static void Test_Nimator_Display_Animation_SequenceBegin(Msg_Animation_SequenceBegin* pMsg)
{
  Test_Nimator_Display_Controller* pTest_Nimator_Display_Controller = (Test_Nimator_Display_Controller*) pMsg->Ref();
  if (pTest_Nimator_Display_Controller->hItem_ != pMsg->hItem) { return; }

  pTest_Nimator_Display_Controller->CreateDisplay();
}

static void Test_Nimator_Display_Animation_Frame(Msg_Animation_Frame* pMsg)
{
  Test_Nimator_Display_Controller* pTest_Nimator_Display_Controller = (Test_Nimator_Display_Controller*) pMsg->Ref();
  if (pTest_Nimator_Display_Controller->hItem_ != pMsg->hItem) { return; }

}

static void Test_Nimator_Display_Animation_SequenceEnd(Msg_Animation_SequenceEnd* pMsg)
{
  Test_Nimator_Display_Controller* pTest_Nimator_Display_Controller = (Test_Nimator_Display_Controller*) pMsg->Ref();
  if (pTest_Nimator_Display_Controller->hItem_ != pMsg->hItem) { return; }

  {
    Msg_Animation_Stop msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.Request();
  }

  {
    Msg_Animation_Destroy msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.Request();
  }

  pTest_Nimator_Display_Controller->DestroyDisplay();

  { Msg_Animation_SequenceBegin msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Nimator_Display_Animation_SequenceBegin, pTest_Nimator_Display_Controller); }
  { Msg_Animation_Frame msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Nimator_Display_Animation_Frame, pTest_Nimator_Display_Controller); }
  { Msg_Animation_SequenceEnd msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Nimator_Display_Animation_SequenceEnd, pTest_Nimator_Display_Controller); }

  delete pTest_Nimator_Display_Controller;
  pTest_Nimator_Display_Controller = 0;

  AP_UNITTEST_SUCCESS(Test_Nimator_Display_Complete);

  Test_Nimator_UnitTest_TokenEnd();
}

#define Test_Nimator_Display_Url "http://ydentiti.org/test/Nimator/avatar.xml"
#define Test_Nimator_Display_Data \
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

String Test_Nimator_Display()
{
  String s;

  Test_Nimator_Display_Controller* pTest_Nimator_Display_Controller = new Test_Nimator_Display_Controller();
  pTest_Nimator_Display_Controller->hItem_ = Apollo::newHandle();

  { Msg_Animation_SequenceBegin msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Nimator_Display_Animation_SequenceBegin, pTest_Nimator_Display_Controller, ApCallbackPosNormal); }
  { Msg_Animation_Frame msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Nimator_Display_Animation_Frame, pTest_Nimator_Display_Controller, ApCallbackPosNormal); }
  { Msg_Animation_SequenceEnd msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Nimator_Display_Animation_SequenceEnd, pTest_Nimator_Display_Controller, ApCallbackPosNormal); }

  if (!s) {
    Msg_Animation_Create msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.sMimeType = "avatar/gif";
    if (!msg.Request()) {
      s = "Msg_Animation_Create failed";
    }
  }

  if (!s) {
    Msg_Animation_SetRate msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.nMaxRate = 10;
    if (!msg.Request()) {
      s = "Msg_Animation_SetRate failed";
    }
  }

  if (!s) {
    Msg_Animation_SetData msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.sbData.SetData(Test_Nimator_Display_Data);
    msg.sSourceUrl = Test_Nimator_Display_Url;
    if (!msg.Request()) {
      s = "Msg_Animation_SetData failed";
    }
  }

  if (!s) {
    Msg_Animation_SetStatus msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.sStatus = "idle";
    if (!msg.Request()) {
      s = "Msg_Animation_SetStatus failed";
    }
  }

  if (!s) {
    Msg_Animation_Event msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    msg.sEvent = "moveleft";
    if (!msg.Request()) {
      s = "Msg_Animation_Event failed";
    }
  }

  if (!s) {
    Msg_Animation_Start msg;
    msg.hItem = pTest_Nimator_Display_Controller->hItem_;
    if (!msg.Request()) {
      s = "Msg_Animation_Start failed";
    }
  }

  return s;
}

//---------------

static void Test_Nimator_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Nimator_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Nimator_UnitTest_Token", "Starting Test/Nimator"));
  int bTokenEndNow = 0;

  AP_UNITTEST_EXECUTE(Test_Nimator_Display);

  if (bTokenEndNow) { Test_Nimator_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#endif // AP_TEST_Nimator

void Test_Nimator_Register()
{
#if defined(AP_TEST_Nimator)
  if (Apollo::isLoadedModule("Nimator")) {
    AP_UNITTEST_REGISTER(Test_Nimator_Display);
    AP_UNITTEST_REGISTER(Test_Nimator_Display_Complete);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Nimator_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Nimator
}

void Test_Nimator_Begin()
{
#if defined(AP_TEST_Nimator)
  if (Apollo::isLoadedModule("Nimator")) {
  }
#endif // AP_TEST_Nimator
}

void Test_Nimator_End()
{
#if defined(AP_TEST_Nimator)
  if (Apollo::isLoadedModule("Nimator")) {
  }
#endif // AP_TEST_Nimator
}
