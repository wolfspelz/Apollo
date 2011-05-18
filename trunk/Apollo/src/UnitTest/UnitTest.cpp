// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "UnitTest.h"
#include "MsgUnitTest.h"
#include "MsgMainLoop.h"
#include "MsgSystem.h"
#include "ApLog.h"

#if defined(WIN32)
  // For INITCOMMONCONTROLSEX
#ifndef _WIN32_IE
  #define _WIN32_IE 0x0500
#endif
  #include "RichEdit.h"
  #include "resource.h"
  
  static HINSTANCE g_hDllInstance = NULL;
  HINSTANCE g_hRichEditDll = NULL;

  BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
  {
    AP_MEMORY_CHECK(dwReason);
    g_hDllInstance = (HINSTANCE) hModule;
    AP_UNUSED_ARG(dwReason);
    AP_UNUSED_ARG(lpReserved);
    return TRUE;
  }
#endif // defined(WIN32)

#define LOG_CHANNEL "WinUnitTest"
#define MODULE_NAME "WinUnitTest"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "WinUnitTest",
  "WinUnitTest Module",
  "1",
  "WinUnitTest Module.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

UNITTEST_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class UnitTest;

#if defined(WIN32)

class UnitTestDialog
{
public:
  UnitTestDialog(SReferer<UnitTestDialog>& ref, UnitTest* pController)
    :m_hDlg(NULL)
    ,m_bGreen(1)
    ,watchDog(ref)
    ,pController_(pController)
  {}
  int Create();
  int Destroy();
  int DeferredDestroy();

  enum { NoTimer
    ,TimerShow
    ,TimerDestroy
  };

  static BOOL CALLBACK StaticDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

  BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

  void SetGreen(int bGreen);
  void SetRange(int nTotal);
  void SetPosition(int nCurrent);
  void WriteLine(const char* szText);

  HWND m_hDlg;

  int m_bGreen;

  RECT m_rWindow;
  RECT m_rDialog;
  
  SRefererSentinel<UnitTestDialog> watchDog;
  UnitTest* pController_;
};

#endif // defined(WIN32)
//----------------------------------------------------------

class UnitTestItem: public Elem
{
public:
  UnitTestItem(const char* szName)
    :Elem(szName)
    ,bComplete_(0)
    ,bSuccess_(0)
  {}

  int bComplete_;
  int bSuccess_;
  String sMessage_;
};

// --------------------------------

class UnitTest
{
public:
  UnitTest();
  virtual ~UnitTest();

  static UnitTest* Instance();
  static void DeleteInstance();

  int Init();
  void Exit();

  int Start();
  int End();

  void On_UnitTest_Token(Msg_UnitTest_Token* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);
  void On_MainLoop_EventLoopEnd(Msg_MainLoop_EventLoopEnd* pMsg);
  void On_UnitTest_Register(Msg_UnitTest_Register* pMsg);
  void On_UnitTest_Complete(Msg_UnitTest_Complete* pMsg);
  void On_UnitTest_Evaluate(Msg_UnitTest_Evaluate* pMsg);
  void On_System_RunMode(Msg_System_RunMode* pMsg);

#if defined(WIN32)
  UnitTestDialog* Dialog();
  int HasDialog();
#endif
  void Evaluate();
  String Summary();

protected:
  void Register(const char* szName);
  void Complete(const char* szName, int bSuccess, const char* szMessage);

  static UnitTest* pInstance_;

  int bConsole_;
#if defined(WIN32)
  SReferer<UnitTestDialog> pDlg_;
  int bGotDialog_;
#endif // defined(WIN32)
  List lTests_;
  int nComplete_;
  int nSuccess_;
  int nFail_;
  int bSuccess_;
  int nUnknown_;

  int bStarted_;
  int bInSendRunLevelNormal_;

  AP_MSG_REGISTRY_DECLARE;
};

//----------------------------------------------------------

#if defined(WIN32)

#include "commctrl.h"

#define COLOR_OK 0x00CC00
#define COLOR_NOTOK 0x0000ff

BOOL CALLBACK UnitTestDialog::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  AP_UNUSED_ARG(lParam);
  switch (message) {
    case WM_INITDIALOG:
      {
        RECT rDialog;
        ::GetClientRect(hDlg, &rDialog);

        int cx = 0;
        int cy = 0;
        if ((cx = ::GetSystemMetrics(SM_CXVIRTUALSCREEN)) == 0) {
          cx = ::GetSystemMetrics(SM_CXSCREEN);
        }
        if ((cy = ::GetSystemMetrics(SM_CYVIRTUALSCREEN)) == 0) {
          cy = ::GetSystemMetrics(SM_CYSCREEN);
        }
        if (cx !=0 && cy != 0) {
          // Center
          //::SetWindowPos(hDlg, NULL, (cx - (rDialog.right - rDialog.left)) / 3, (cy - (rDialog.bottom - rDialog.top)) / 3, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

          // TopRight
          ::SetWindowPos(hDlg, NULL, cx - (rDialog.right - rDialog.left), 0, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
        }

        ::SetWindowText(::GetDlgItem(hDlg, IDC_SUMMARY), pController_->Summary());
        //::ShowWindow(hDlg, FALSE);
      }
      return TRUE;
      break;
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_EVALUATE: {
          if (pController_ != 0) {
            pController_->Evaluate();
          }
        } break;
      }
      break;
    case WM_TIMER:
      switch (wParam) {
        case TimerShow:
          ::KillTimer(hDlg, wParam);
          ::ShowWindow(hDlg, TRUE);
          break;
        case TimerDestroy:
          ::KillTimer(hDlg, wParam);
          Destroy();
          break;
        default:;
      }
      break;
    case WM_CLOSE:
      // Crashed on DestroyWindow when no tests (!?)
      //::DestroyWindow(hDlg);
      break;
    case WM_DESTROY:
      delete this;
      break;
  }

  return FALSE;
}

BOOL CALLBACK UnitTestDialog::StaticDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  LRESULT nResult = 0;
  UnitTestDialog* pUnitTestDialog = 0;

  if (msg == WM_INITDIALOG) {
    pUnitTestDialog = (UnitTestDialog*) lParam;
    SetWindowLong(hDlg, GWL_USERDATA, (LONG) pUnitTestDialog);
  } else {
    pUnitTestDialog = (UnitTestDialog*) GetWindowLong(hDlg, GWL_USERDATA);
  }

  if (pUnitTestDialog != 0) {
    nResult = pUnitTestDialog->DlgProc(hDlg, msg, wParam, lParam);
  } else {
    nResult = DefWindowProc(hDlg, msg, wParam, lParam);
  }

  return nResult;
}

int UnitTestDialog::Create()
{
  m_hDlg = ::CreateDialogParam(g_hDllInstance, MAKEINTRESOURCE(IDD_PROGRESS), NULL, UnitTestDialog::StaticDlgProc, (LPARAM) this);
  if (m_hDlg != NULL) {
    int nTimeout = Apollo::getModuleConfig(MODULE_NAME, "ShowTimeout", 3);
    if (nTimeout > 0) {
      ::SetTimer(m_hDlg, TimerShow, nTimeout * 1000, 0);
    } else {
      ::ShowWindow(m_hDlg, TRUE);
    }
  }
  return 0;
}

int UnitTestDialog::DeferredDestroy()
{
  if (m_hDlg != NULL) {
    int nTimeout = Apollo::getModuleConfig(MODULE_NAME, "SuccessCloseTimeout", 3);
    if (nTimeout > 0) {
      ::SetTimer(m_hDlg, TimerDestroy, nTimeout * 1000, 0);
    } else if (nTimeout == 0) {
      Destroy();
    } else {
      // Keep it open
    }
  }
  return 0;
}

int UnitTestDialog::Destroy()
{
  if (m_hDlg != NULL) {
    ::SendMessage(m_hDlg, WM_CLOSE, 0, 0);
  }
  return 0;
}

void UnitTestDialog::SetGreen(int bGreen)
{
  m_bGreen = bGreen;
  if (!bGreen) {
    ::ShowWindow(m_hDlg, TRUE);
  }
  ::SendMessage(::GetDlgItem(m_hDlg, IDC_PROGRESS), PBM_SETBARCOLOR, 0, (m_bGreen ? COLOR_OK : COLOR_NOTOK));
}

void UnitTestDialog::SetRange(int nTotal)
{
  ::SendMessage(::GetDlgItem(m_hDlg, IDC_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM(0, nTotal));
}

void UnitTestDialog::SetPosition(int nCurrent)
{
  ::SendMessage(::GetDlgItem(m_hDlg, IDC_PROGRESS), PBM_SETPOS, nCurrent, 0);
  ::SendMessage(::GetDlgItem(m_hDlg, IDC_PROGRESS), PBM_SETBARCOLOR, 0, (m_bGreen ? COLOR_OK : COLOR_NOTOK));
  ::SetWindowText(::GetDlgItem(m_hDlg, IDC_SUMMARY), pController_->Summary());
}

static String UnitTest_GetWindowText(HWND hWnd)
{
  String text;

  int len= ::GetWindowTextLength(hWnd);
  if (len > 0) {
    Flexbuf<TCHAR> buf(len + 1);
    if (buf != 0) {
      len= ::GetWindowText(hWnd, buf, len+1);
      if (len > 0) {
        text.set(buf, len);
      }
    }
  }

  return text;
}

void UnitTestDialog::WriteLine(const char* szText)
{
  HWND hWnd = GetDlgItem(m_hDlg, IDC_LOG);

  String s = UnitTest_GetWindowText(hWnd);
  s += szText;
  s += "\r\n";
  ::SetWindowText(hWnd, s);

  int nCharCount = (int) s.chars();
  int nLineCount = (int) SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
  (void) SendMessage(hWnd, EM_LINESCROLL, 0, (LPARAM) nLineCount);
  (void) SendMessage(hWnd, EM_SCROLL, SB_PAGEDOWN, 0);
  (void) SendMessage(hWnd, EM_SETSEL, (WPARAM) nCharCount, (LPARAM) nCharCount);
}

#endif // defined(WIN32)

//----------------------------------------------------------

UnitTest::UnitTest()
:bConsole_(1)
#if defined(WIN32)
,pDlg_(0)
,bGotDialog_(0)
#endif // defined(WIN32)
,nComplete_(0)
,nSuccess_(0)
,nFail_(0)
,bSuccess_(1)
,nUnknown_(0)
,bStarted_(0)
,bInSendRunLevelNormal_(0)
{
}

UnitTest::~UnitTest()
{
}

UnitTest* UnitTest::pInstance_ = 0;
UnitTest* UnitTest::Instance()
{
  if (pInstance_ == 0) {
    pInstance_ = new UnitTest();
  }

  return pInstance_;
}

void UnitTest::DeleteInstance()
{
  if (pInstance_ != 0) {
#if defined(WIN32)
    if (pInstance_->HasDialog()) {
      pInstance_->Dialog()->Destroy();
      // Auto delete in WM_DESTROY
      //delete pDlg_;
      //pDlg_ = 0;

      if (g_hRichEditDll != NULL) {
        ::FreeLibrary(g_hRichEditDll);
        g_hRichEditDll = NULL;
      }
    }
#endif // defined(WIN32)
  }

  if (pInstance_ != 0) {
    delete pInstance_;
    pInstance_ = 0;
  }
}

#if defined(WIN32)
UnitTestDialog* UnitTest::Dialog()
{
  if (Apollo::isLoadedModule("WinMainLoop") && !bConsole_ && pDlg_ == 0 && !bGotDialog_) {
    pDlg_ = new UnitTestDialog(pDlg_, this);
    if (pDlg_ != 0) {
      bGotDialog_ = 1;

      INITCOMMONCONTROLSEX InitCtrlEx;
      InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
      InitCtrlEx.dwICC  = ICC_PROGRESS_CLASS;
      ::InitCommonControlsEx(&InitCtrlEx);

      if (g_hRichEditDll == NULL) {
        g_hRichEditDll = ::LoadLibraryA("RICHED32.DLL");
      }

      pDlg_->Create();
    }
  }

  return pDlg_;
}

int UnitTest::HasDialog()
{
  return (pDlg_ != 0);
}
#endif // defined(WIN32)

void UnitTest::Register(const char* szName)
{
  UnitTestItem* e = new UnitTestItem(szName);
  if (e != 0) {
    lTests_.AddLast(e);
  }
#if defined(WIN32)
  if (Dialog() != 0) { Dialog()->SetRange(lTests_.length()); }
#endif
}

void UnitTest::Evaluate()
{
  Msg_UnitTest_EvaluateResult msg;
  
  String sText;
  for (UnitTestItem* pItem = 0; (pItem = (UnitTestItem*) lTests_.Next(pItem)); ) {
    if (!pItem->bComplete_) {
      sText = "";
      sText.appendf("%s missing", StringType(pItem->getName()));
#if defined(WIN32)
      if (Dialog() != 0) {
        Dialog()->WriteLine(sText);
      } else {
#else
      {
#endif
        apLog_Info((LOG_CHANNEL, "UnitTest", StringType(sText)));
      }

      { msg.vlMissingTests.add(pItem->getName()); }
    }
  }

  sText = "";
  sText.appendf("Total:%d Failed:%d Missing:%d Unknown:%d", lTests_.length(), nFail_, lTests_.length() - nComplete_, nUnknown_);
#if defined(WIN32)
  if (Dialog() != 0) {
    Dialog()->WriteLine(sText);
  } else {
#else
  {
#endif
    apLog_Debug((LOG_CHANNEL, "UnitTest", "%s", StringType(sText)));
  }
  
  { msg.nTotal = lTests_.length(); msg.nFailed = nFail_; msg.nUnknown = nUnknown_; msg.Send(); }
}

String UnitTest::Summary()
{
  String sSummary;
  sSummary.appendf("Total=%d Complete=%d Success=%d ", lTests_.length(), nComplete_, nSuccess_);
  return sSummary;
}

void UnitTest::Complete(const char* szName, int bSuccess, const char* szMessage)
{
  String sText;
  Msg_UnitTest_Progress msg; 
  { msg.sName = szName; msg.bSuccess = bSuccess; msg.sMessage = szMessage; }
  UnitTestItem* pItem = (UnitTestItem*) lTests_.FindByName(szName);
  if (pItem == 0) {
    msg.bSuccess = 0;
    msg.bKnown = 0;
    nUnknown_++;
    bSuccess_ = 0;
    sText.appendf("%s unknown: %s", StringType(szName), StringType(szMessage));
  } else {
    msg.bKnown = 1;
    if (!pItem->bComplete_) {
      pItem->bComplete_ = 1;
      pItem->bSuccess_ = bSuccess;
      pItem->sMessage_ = szMessage;
      nComplete_++;
      nSuccess_ += bSuccess?1:0;
      nFail_ += bSuccess?0:1;
      bSuccess_ &= bSuccess;

      if (!bSuccess) {
        sText.appendf("##### %s failed: %s", StringType(szName), StringType(szMessage));
      } else {
        sText = szName;
      }
    }
  }

  msg.nProgress = nComplete_;
  msg.Send(); // Msg_UnitTest_Progress

  if (sText.empty()) {
#if defined(WIN32)
    if (Dialog() != 0) {
      Dialog()->SetPosition(nComplete_);
    } else {
#else
    {
#endif
      apLog_Debug((LOG_CHANNEL, "UnitTest::Complete", "%03d/%03d: %s", nComplete_, lTests_.length(), StringType(szName)));
    }
  } else {
#if defined(WIN32)
    if (Dialog() != 0) {
      Dialog()->SetGreen(bSuccess_);
      Dialog()->SetPosition(nComplete_);
      Dialog()->WriteLine(sText);
    } else {
#else
    {
#endif
      apLog_Debug((LOG_CHANNEL, "UnitTest", "%s", StringType(sText)));
    }
  }

  if (lTests_.length() == (unsigned int) nComplete_) {
    Msg_UnitTest_Done msg;
    msg.bSuccess = (lTests_.length() == (unsigned int) nSuccess_);
    msg.Send();
  }

  if (lTests_.length() == (unsigned int) nSuccess_ && nUnknown_ == 0) {
#if defined(WIN32)
    if (Dialog() != 0) {
      Dialog()->DeferredDestroy();
    }
#endif
  }
}

//----------------------------------------------------------

int UnitTest::Start()
{
  bStarted_ = 1;

  apLog_Info((LOG_CHANNEL, "UnitTest::Start", "Register tests"));

  { // Let modules register
    Msg_UnitTest_Begin msg;
    msg.Send();
  }

  { // Set range based on registered tests
    Msg_UnitTest_Range msg;
    msg.nRange = lTests_.length();
    msg.Send();
  }

  apLog_Info((LOG_CHANNEL, "UnitTest::Start", "Execute synchronous tests"));
  apLog_User(("Testing"));

  { // Execute synchronous tests
    Msg_UnitTest_Execute msg;
    msg.Send();
  }

  apLog_Info((LOG_CHANNEL, "UnitTest::Start", "Start asynchronous test sequence"));

  { // Start asynchronous test sequence
    ApAsyncMessage<Msg_UnitTest_Token> msg;
    msg.Post();
  }

  return 1;
}

int UnitTest::End()
{
  if (bStarted_) {
    { Msg_UnitTest_End msg; msg.Send(); }
    apLog_Info((LOG_CHANNEL, "UnitTest::On_MainLoop_EventLoopBeforeEnd", "End tests"));
    { Msg_UnitTest_Evaluate msg; msg.Send(); }
    bStarted_ = 0;

#if defined(WIN32)
    if (Dialog()) {
      if (nComplete_ == (int) lTests_.length() && nComplete_ == nSuccess_) {
      } else {
        Dialog()->SetGreen(0);
      }
    }
#endif
  }

  return 1;
}
//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(UnitTest, UnitTest_Token)
{
  // Got the token, although registered "Late"
  // So, we assume that everyone who registered for token got ist already

  apLog_Info((LOG_CHANNEL, "UnitTest::UnitTest_Token", "Finished asynchronous test sequence"));

  if (bSuccess_) {
    apLog_User(("Test successful"));
  } else {
    String sText;
    sText.appendf("Did %d tests", lTests_.length());
    
    int nFailed = nComplete_ - nSuccess_;
    if (nFailed > 0) {
      sText.appendf(" / %d test(s) failed", nFailed);
    }
    
    int nMissing = lTests_.length() - nComplete_;
    if (nMissing > 0) {
      sText.appendf(" / %d result(s) missing", nMissing);
    }
    
    apLog_User(("%s", StringType(sText)));
  }

  bInSendRunLevelNormal_ = 1;
  Msg_System_RunLevel msg; msg.sLevel = Msg_System_RunLevel_Normal; msg.Send();
  bInSendRunLevelNormal_ = 0;
}

AP_MSG_HANDLER_METHOD(UnitTest, System_RunLevel)
{
  if (pMsg->sLevel == Msg_System_RunLevel_Normal && !bInSendRunLevelNormal_ && bStarted_) {
    // Stop the "Normal" run level until unit tests are done
    pMsg->Stop();
  }
}

AP_MSG_HANDLER_METHOD(UnitTest, MainLoop_EventLoopBegin)
{
  AP_UNUSED_ARG(pMsg);

  int bStart = 0;

  if (!bStart) {
    Msg_System_GetCmdLineArgs msg;
    if (msg.Request()) {
      List lArgv;
      for (int n = 0; n < msg.nArgc; n++) {
        lArgv.AddLast(msg.pszArgv[n]);
      }
      if (lArgv.FindByName("-test") != 0) {
        bStart = 1;
      }
    }
  }

  if (!bStart) {
    if (Apollo::getModuleConfig(MODULE_NAME, "Test", 0)) {
      bStart = 1;
    }
  }

  if (bStart) {
    (void) Start();
  }
}

AP_MSG_HANDLER_METHOD(UnitTest, MainLoop_EventLoopBeforeEnd)
{
  AP_UNUSED_ARG(pMsg);
  if (bStarted_) {
    (void) End();
  }
}

AP_MSG_HANDLER_METHOD(UnitTest, MainLoop_EventLoopEnd)
{
  AP_UNUSED_ARG(pMsg);
  if (bStarted_) {
    (void) End();
  }
}

AP_MSG_HANDLER_METHOD(UnitTest, UnitTest_Register)
{
  Register(pMsg->sName);
}

AP_MSG_HANDLER_METHOD(UnitTest, UnitTest_Complete)
{
  Complete(pMsg->sName, pMsg->bSuccess, pMsg->sMessage);
}

AP_MSG_HANDLER_METHOD(UnitTest, UnitTest_Evaluate)
{
  AP_UNUSED_ARG(pMsg);
  Evaluate();
}

AP_MSG_HANDLER_METHOD(UnitTest, System_RunMode)
{
  if (pMsg->nMode == Msg_System_RunMode::RunModeGUI) {
    bConsole_ = 0;
  } else {
    bConsole_ = 1;
  }
}

//-----------------------------

int UnitTest::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, UnitTest_Token, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, System_RunLevel, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, MainLoop_EventLoopBegin, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, MainLoop_EventLoopBeforeEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, MainLoop_EventLoopEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, UnitTest_Register, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, UnitTest_Complete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, UnitTest_Evaluate, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, UnitTest, System_RunMode, this, ApCallbackPosNormal);

  return ok;
}

void UnitTest::Exit()
{
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

UNITTEST_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 0;

  UnitTest::DeleteInstance();
  if (UnitTest::Instance() != 0) { ok = UnitTest::Instance()->Init(); }

  return ok;
}

UNITTEST_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (UnitTest::Instance() != 0) {
    UnitTest::Instance()->Exit();
    UnitTest::DeleteInstance();
  }

  return ok;
}
