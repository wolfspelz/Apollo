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
    ,hBitmap_(NULL)
    ,pBits_(0)
    ,nX_(20)
    ,nY_(200)
    ,nW_(100)
    ,nH_(100)
    ,nCntWindows_(0)
    ,nCntSeqenceEnd_(0)
  {}

  void CreateDisplay();
  void DestroyDisplay();
  void DisplayFrame(Apollo::Image& image);

  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  String Begin();
  String End();

  HINSTANCE hInstance_;
  HWND hWnd_;
  HBITMAP hBitmap_;
  unsigned char* pBits_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;
  int nCntWindows_;
  ApHandle hItem_;
  String sOrigDb_;
  int nCntSeqenceEnd_;
};

#define Test_Galileo_Display_DB "GalileoDisplayTest"

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
  PAINTSTRUCT ps;
  HDC hdc = NULL;

  switch (message) {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    RECT rt;
    GetClientRect(hWnd, &rt);
    //DrawText(hdc, "Apollo", _tcslen("Apollo"), &rt, DT_CENTER);
    EndPaint(hWnd, &ps);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

void Test_Galileo_Display_Controller::CreateDisplay()
{
  int ok = 1;

  {
    Msg_Win32_GetInstance msg;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "Msg_Win32_GetInstance failed"));
    } else {
      hInstance_ = msg.hInstance;
    }
  }

  if (hInstance_ == NULL) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "Missing hInstance"));
  }

  if (ok) {
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
      ATOM a = ::RegisterClassEx(&wcex);
      if (a == 0) {
        ok = 0;
        apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "RegisterClassEx failed"));
      }
    }
  }

  if (ok) {
    hWnd_ = ::CreateWindowEx(
      WS_EX_LAYERED | WS_EX_TOOLWINDOW, 
      Test_Galileo_Display_WindowClass, 
      Test_Galileo_Display_WindowCaption, 
      WS_VISIBLE,
      nX_, nY_, nW_, nH_,
      NULL, 
      NULL, 
      hInstance_, 
      this
      );
    if (hWnd_ == NULL) {
      ok = 0;
      apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "CreateWindowEx failed"));
    }
  }

  if (ok) {
    HDC dcScreen = ::GetDC(NULL);

    BITMAPINFO bi;
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = nW_;
    bi.bmiHeader.biHeight = nH_;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biSizeImage = (bi.bmiHeader.biWidth * bi.bmiHeader.biHeight) * 4;
    bi.bmiHeader.biXPelsPerMeter = 0;
    bi.bmiHeader.biYPelsPerMeter = 0;
    bi.bmiHeader.biClrUsed = 0;
    bi.bmiHeader.biClrImportant = 0;

    hBitmap_ = CreateDIBSection(
      dcScreen,
      &bi,
      DIB_RGB_COLORS,
      (void**) &pBits_,
      NULL,
      0    
    );

    if (hBitmap_ == NULL) {
      ok = 0;
      DWORD dw = ::GetLastError(); // returns ERROR_NOT_ENOUGH_MEMORY ?
      apLog_Error((LOG_CHANNEL, "Test_Galileo_Display_Controller::CreateDisplay", "CreateDIBSection failed: GetLastError()=%d", dw));
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

  if (hBitmap_ != NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
    pBits_ = 0;
  }
}

void Test_Galileo_Display_Controller::DisplayFrame(Apollo::Image& image)
{
  if (hBitmap_ == NULL) { return; }

  int nW = nW_;
  int nH = nH_;

  HDC dcScreen = ::GetDC(NULL);
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);

  HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(dcMemory, hBitmap_);

  //{ // almost transp background
  //  for (int y = 0; y < nH; ++y) {
  //    BYTE* pPixel = pBits_ + nW * 4 * y;
  //    for (int x = 0; x < nW ; ++x) {
  //      pPixel[3] = 0x80;
  //      pPixel[0] = 0x80;
  //      pPixel[1] = 0;
  //      pPixel[2] = 0;
  //      pPixel+= 4;
  //    }
  //  }
  //}

  { // almost transp background
    BYTE* pPixel = pBits_;
    int w = image.Width();
    Apollo::Pixel* p = image.Pixels();
    for (int y = 0; y < nH; ++y) {
      for (int x = 0; x < nW ; ++x) {
        int i = y * w + x;
        *pPixel++ = p[i].blue;
        *pPixel++ = p[i].green;
        *pPixel++ = p[i].red;
        //*pPixel++ = p[i].alpha;
        *pPixel++ = 250;
      }
    }
  }

  // setup the blend function
	BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
  POINT ptSrc = {0, 0}; // start point of the copy from dcMemory to dcScreen

  // calculate the new window position/size based on the bitmap size
	POINT ptWindowPosition; 
  ptWindowPosition.x = nX_;
  ptWindowPosition.y = nY_;

	SIZE szWindowSize;
  szWindowSize.cx = nW;
  szWindowSize.cy = nH;

  // perform the alpha blend
	BOOL bRet= ::UpdateLayeredWindow(
      hWnd_, 
      dcScreen, 
      0, //&ptWindowPosition, 
      &szWindowSize, 
      dcMemory,
		  &ptSrc, 
      0, 
      &blendPixelFunction, 
      ULW_ALPHA
      );

	// clean up
  ::SelectObject(dcMemory, hOldBitmap);

  ::DeleteDC(dcMemory);
  ::ReleaseDC(NULL, dcScreen);
}

static void Test_Galileo_Display_Animation_SequenceBegin(Msg_Animation_SequenceBegin* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  if (pTest_Galileo_Display_Controller->hItem_ != pMsg->hItem) { return; }
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Animation_SequenceBegin", "" ApHandleFormat "", ApHandleType(pMsg->hItem)));
}

static void Test_Galileo_Display_Animation_Frame(Msg_Animation_Frame* pMsg)
{
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  if (pTest_Galileo_Display_Controller->hItem_ != pMsg->hItem) { return; }
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Animation_Frame", "" ApHandleFormat "", ApHandleType(pMsg->hItem)));

  pTest_Galileo_Display_Controller->DisplayFrame(pMsg->iFrame);
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
  Test_Galileo_Display_Controller* pTest_Galileo_Display_Controller = (Test_Galileo_Display_Controller*) pMsg->Ref();
  if (pTest_Galileo_Display_Controller->hItem_ != pMsg->hItem) { return; }
  apLog_Verbose((LOG_CHANNEL, "Test_Galileo_Display_Animation_SequenceEnd", "" ApHandleFormat "", ApHandleType(pMsg->hItem)));

  pTest_Galileo_Display_Controller->nCntSeqenceEnd_++;
  if (pTest_Galileo_Display_Controller->nCntSeqenceEnd_ >= 2) {
    String s = pTest_Galileo_Display_Controller->End();
    delete pTest_Galileo_Display_Controller;
    pTest_Galileo_Display_Controller = 0;
    AP_UNITTEST_RESULT(Test_Galileo_Display_End, s.empty(), s);
  }
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
  s = pTest_Galileo_Display_Controller->Begin();

  return s;
}

String Test_Galileo_Display_Controller::Begin()
{
  String s;

  { Msg_Galileo_SetStorageName msg; msg.sName = Test_Galileo_Display_DB; if (msg.Request()) { sOrigDb_ = msg.sPreviousName; } }
  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }

  CreateDisplay();

  { Msg_Animation_SequenceBegin msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceBegin, this, ApCallbackPosNormal); }
  { Msg_Animation_Frame msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_Frame, this, ApCallbackPosNormal); }
  { Msg_Animation_SequenceEnd msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceEnd, this, ApCallbackPosNormal); }
  { Msg_Galileo_RequestAnimation msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimation, this, ApCallbackPosEarly); }
  //{ Msg_Galileo_RequestAnimationComplete msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimationComplete, this, ApCallbackPosEarly); }
  { Msg_Galileo_SaveAnimationDataToStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_SaveAnimationDataToStorage, this, ApCallbackPosEarly); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_IsAnimationDataInStorage, this, ApCallbackPosEarly); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_LoadAnimationDataFromStorage, this, ApCallbackPosEarly); }

  hItem_ = Apollo::newHandle();

  if (!s) {
    Msg_Animation_Create msg;
    msg.hItem = hItem_;
    msg.sMimeType = "avatar/gif";
    if (!msg.Request()) {
      s = "Msg_Animation_Create failed";
    }
  }

  if (!s) {
    Msg_Animation_SetRate msg;
    msg.hItem = hItem_;
    msg.nMaxRate = 10;
    if (!msg.Request()) {
      s = "Msg_Animation_SetRate failed";
    }
  }

  if (!s) {
    Msg_Animation_SetData msg;
    msg.hItem = hItem_;
    msg.sbData.SetData(Test_Galileo_Display_Data);
    msg.sSourceUrl = Test_Galileo_Display_Url;
    if (!msg.Request()) {
      s = "Msg_Animation_SetData failed";
    }
  }

  if (!s) {
    Msg_Animation_SetStatus msg;
    msg.hItem = hItem_;
    msg.sStatus = "wave";
    if (!msg.Request()) {
      s = "Msg_Animation_SetStatus failed";
    }
  }

  //if (!s) {
  //  Msg_Animation_Event msg;
  //  msg.hItem = hItem_;
  //  msg.sEvent = "wave";
  //  if (!msg.Request()) {
  //    s = "Msg_Animation_Event failed";
  //  }
  //}

  if (!s) {
    Msg_Animation_Start msg;
    msg.hItem = hItem_;
    if (!msg.Request()) {
      s = "Msg_Animation_Start failed";
    }
  }

  return s;
}

String Test_Galileo_Display_Controller::End()
{
  String s;

  { Msg_Animation_SequenceBegin msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceBegin, this); }
  { Msg_Animation_Frame msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_Frame, this); }
  { Msg_Animation_SequenceEnd msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Animation_SequenceEnd, this); }
  { Msg_Galileo_RequestAnimation msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimation, this); }
  //{ Msg_Galileo_RequestAnimationComplete msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_RequestAnimationComplete, this); }
  { Msg_Galileo_SaveAnimationDataToStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_SaveAnimationDataToStorage, this); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_IsAnimationDataInStorage, this); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_Display_Galileo_LoadAnimationDataFromStorage, this); }

  {
    Msg_Animation_Stop msg;
    msg.hItem = hItem_;
    msg.Request();
  }

  {
    Msg_Animation_Destroy msg;
    msg.hItem = hItem_;
    msg.Request();
  }

  DestroyDisplay();

  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Galileo_SetStorageName msg; msg.sName = sOrigDb_; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Galileo_Display_DB; (void) msg.Request(); }

  Test_Galileo_UnitTest_TokenEnd();

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
