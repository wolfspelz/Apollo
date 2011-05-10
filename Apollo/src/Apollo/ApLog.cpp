// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgSystem.h"
#include "MsgLog.h"

class apLog_TraceLevel
{
public:
  apLog_TraceLevel(const char* _szChannel, const char* _szContext);
  virtual ~apLog_TraceLevel(void);

  const char* Channel(void);
  const char* Context(void);

  apLog_TraceLevel* Next(void);
  void Next(apLog_TraceLevel* next);

protected:
  apLog_TraceLevel* pNext_;

  char* szChannel_;
  char* szContext_;
};

static int apLog_nMask = apLog_MaskDefault;
static apLog_TraceLevel* pTraceLevelHead = 0;
static int apLog_nTraceLevelDepth = 0;
static int apLog_bInLog = 0;
static Apollo::Mutex apLog_oMutex;

//----------------------------------------------------------

class apLog_OS
{
public:
  static int vsnprintf(char* str, size_t size, const char* format, va_list ap);
};

int apLog_OS::vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
  int result = 0;
#if defined(WIN32)
  HRESULT hr = ::StringCbVPrintfA(str, size, format, ap);
  if (FAILED(hr)) {
    result = -1;
  }
#else
  result = ::vsnprintf(str, size, format, ap);
#endif
  return result;
}

//----------------------------------------------------------

class apLog_MutexObject
{
public:
  apLog_MutexObject();
  virtual ~apLog_MutexObject();
};

apLog_MutexObject::apLog_MutexObject(void)
{
  apLog_oMutex.Acquire();
}

apLog_MutexObject::~apLog_MutexObject(void)
{
  apLog_oMutex.Release();
}

//----------------------------------------------------------
// Trace

apLog_TraceLevel::apLog_TraceLevel(const char* _szChannel, const char* _szContext)
:pNext_(0)
,szChannel_(0)
,szContext_(0)
{
  if (_szChannel != 0) {
    unsigned long nLen = String::strlen(_szChannel);
    szChannel_ = new char[nLen+1];
    if (szChannel_ != 0) {
      ::memcpy(szChannel_, _szChannel, nLen+1);
    }
  }
  if (_szContext != 0) {
    unsigned long nLen = String::strlen(_szContext);
    szContext_ = new char[nLen+1];
    if (szContext_ != 0) {
      ::memcpy(szContext_, _szContext, nLen+1);
    }
  }
}

apLog_TraceLevel::~apLog_TraceLevel(void)
{
  if (szChannel_ != 0) {
    delete [] szChannel_;
  }
  if (szContext_ != 0) {
    delete [] szContext_;
  }
}

const char* apLog_TraceLevel::Channel()
{
  const char* result = "";
  if (szChannel_ != 0) {
    result = szChannel_;
  }
  return result;
}

const char* apLog_TraceLevel::Context()
{
  const char* result = "";
  if (szContext_ != 0) {
    result = szContext_;
  }
  return result;
}

apLog_TraceLevel* apLog_TraceLevel::Next()
{
  return pNext_;
}

void apLog_TraceLevel::Next(apLog_TraceLevel* next)
{
  pNext_ = next;
}

void apLog_TraceEnter(const char* _szChannel, const char* _szContext)
{
  apLog_MutexObject mutex;

  apLog_TraceLevel* pTraceLevel = new apLog_TraceLevel(_szChannel, _szContext);
  if (pTraceLevel != 0) {
    if (pTraceLevelHead != 0) {
      pTraceLevel->Next(pTraceLevelHead);
    }
    pTraceLevelHead = pTraceLevel;
  }
}

void apLog_DeleteTraceHead()
{
  apLog_TraceLevel* pTraceLevel = pTraceLevelHead;
  pTraceLevelHead = pTraceLevelHead->Next();
  delete pTraceLevel;
}

void apLog_TraceLeave(void)
{
  apLog_MutexObject mutex;

  if (pTraceLevelHead != 0) {
    apLog_DeleteTraceHead();
  }
}

const char* apLog_TraceChannel(void)
{
  const char* result = "";
  if (pTraceLevelHead != 0) {
    result = pTraceLevelHead->Channel();
  }
  return result;
}

const char* apLog_TraceContext(void)
{
  const char* result = "";
  if (pTraceLevelHead != 0) {
    result = pTraceLevelHead->Context();
  }
  return result;
}

apLog_TraceObject::apLog_TraceObject(const char* _szChannel, const char* _szContext)
{
  apLog_TraceEnter(_szChannel, _szContext);
  if (apLog_IsMask(apLog_MaskTrace)) {
    apLog_LogMessage(apLog_MaskTrace, apLog_TraceChannel(), apLog_TraceContext(), "Entering");
  }
  apLog_nTraceLevelDepth++;
}

apLog_TraceObject::~apLog_TraceObject()
{
  apLog_nTraceLevelDepth--;
  if (apLog_IsMask(apLog_MaskTrace)) {
    apLog_LogMessage(apLog_MaskTrace, apLog_TraceChannel(), apLog_TraceContext(), "Leaving");
  }
  apLog_TraceLeave();
}

//----------------------------------------------------------

int apLog_Mask2Level(int _nMask)
{
  switch (_nMask) {
    case apLog_MaskFatal: return apLog_LevelFatal;
    case apLog_MaskError: return apLog_LevelError;
    case apLog_MaskWarning: return apLog_LevelWarning;
    case apLog_MaskUser: return apLog_LevelUser;
    case apLog_MaskDebug: return apLog_LevelDebug;
    case apLog_MaskInfo: return apLog_LevelInfo;
    case apLog_MaskVerbose: return apLog_LevelVerbose;
    case apLog_MaskTrace: return apLog_LevelTrace;
    case apLog_MaskVeryVerbose: return apLog_LevelVeryVerbose;
    case apLog_MaskAlert: return apLog_LevelAlert;
    default:
    return 0;
  }
}

int apLog_Level2Mask(int _nLevel)
{
  return 1 << _nLevel;
}

int apLog_SetMask(int _nMask)
{
  int nMask = apLog_nMask;
  apLog_nMask = _nMask;
  return nMask;
}

int apLog_IsMask(int _nMask)
{
  return (_nMask & apLog_nMask) != 0;
}

void apLog_LogMessage(int _nMask, const char* szChannel, const char* szContext, const char* szMessage)
{
  apLog_MutexObject mutex;

  if (!apLog_bInLog) {
    apLog_bInLog = 1;
    Apollo::sendLog(_nMask, szChannel, szContext, szMessage);
    apLog_bInLog = 0;
  }
}

//----------------------------------------------------------

void apLog_FatalI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskFatal)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskFatal, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_ErrorI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskError)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskError, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_WarningI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskWarning)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskWarning, _szChannel, _szContext, (const char*)szMessage);
    }
  }
}

void apLog_UserI(const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskUser)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskUser, "", "", (const char*)szMessage);
    }
  }
}

void apLog_DebugI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskDebug)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskDebug, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_InfoI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskInfo)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskInfo, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_VerboseI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskVerbose)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskVerbose, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_TraceI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskTrace)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskTrace, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_VeryVerboseI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskVeryVerbose)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskVeryVerbose, _szChannel, _szContext, szMessage);
    }
  }
}

void apLog_AlertI(const char* _szChannel, const char* _szContext, const char* szFmt, ...)
{
  if (apLog_IsMask(apLog_MaskAlert)) {
    if (szFmt != 0) {
      char szMessage[apLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, szFmt);
      apLog_OS::vsnprintf(szMessage, apLog_Const_MAXMESSAGE, szFmt, argp);
      va_end(argp);
      apLog_LogMessage(apLog_MaskAlert, _szChannel, _szContext, szMessage);
    }
  }
}

// -------------------------------------------------------------------
// Msg wrappers

void Apollo::sendLog(int nMask, const char* szChannel, const char* szContext, const char* szMessage)
{
  Msg_Log_Line* pMsg = new Msg_Log_Line();
  if (pMsg != 0) {
    pMsg->nMask = nMask;
    pMsg->sChannel = szChannel;
    pMsg->sContext = szContext;
    pMsg->sMessage = szMessage;
  }
  if (Apollo::isMainThread()) {
    Apollo::callMsg(pMsg);
    delete pMsg;
  } else {
    pMsg->PostAsync();
  }
}
