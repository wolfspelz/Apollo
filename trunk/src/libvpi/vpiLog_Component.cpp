// vpiLog_Component.cpp: Implementierung der Klasse vpiLog_Component.
//
//////////////////////////////////////////////////////////////////////

#include "vpiLog_Component.h"

#if defined(WIN32)
  #include <Windows.h>
#endif

#if !defined(WIN32)
  #include <stdio.h>
  //#define STDNS std
#else
//  #define STDNS
#endif

#if defined(__GNUC__)
  #define vpiLog_Unused_Arg(a) do {} while (&a == 0)
#else
  #define vpiLog_Unused_Arg(a) (a)
#endif

#if defined(__GNUC__)
  #define vpiLog_Unused_Variable __attribute__((unused))
#else
  #define vpiLog_Unused_Variable
#endif

#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////

class vpiLog_TraceLevel
{
public:
	vpiLog_TraceLevel(const char* _szChannel, const char* _szContext);
	virtual ~vpiLog_TraceLevel(void);

  const char* Channel(void);
  const char* Context(void);

  vpiLog_TraceLevel* Next(void);
  void Next(vpiLog_TraceLevel* next);

protected:
  vpiLog_TraceLevel* pNext_;

  char* szChannel_;
  char* szContext_;
};

class vpiLog_LogLine
{
public:
	vpiLog_LogLine(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage);
	virtual ~vpiLog_LogLine(void);

  int Mask(void);
  const char* Channel(void);
  const char* Context(void);
  const char* Message(void);

  vpiLog_LogLine* Next(void);
  void Next(vpiLog_LogLine* next);

protected:
  vpiLog_LogLine* pNext_;

  int nMask_;
  char* szChannel_;
  char* szContext_;
  char* szMessage_;
};

class vpiLog_LogImpl
{
public:
	vpiLog_LogImpl();
	virtual ~vpiLog_LogImpl();

  vpiLog_LogCallback SetLogCallback(vpiLog_LogCallback _fpCallback);
  int SetMask(int _nMask);

  static void ConsoleOut(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage);
  static void Format(char* _szBuffer, int nBufferLen, int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage);

  void Log(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage);

  void StoreLine(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage);
  int GetLineLength(int* _nChannel, int* _nContext, int* _nMessage);
  int GetLineData(int* _nMask, char* _szChannel, char* _szContext, char* _szMessage);

  void TraceEnter(const char* _szChannel, const char* _szContext);
  void TraceLeave(void);
  const char* TraceChannel(void);
  const char* TraceContext(void);

  int IsMask(int _nMask);

  static int Mask2Level(int _nMask);

protected:
  void DeleteTraceHead();
  void DeleteLineHead();

protected:
  vpiLog_TraceLevel* pTraceLevelHead_;
  int nTraceLevelDepth_;

  vpiLog_LogCallback fpOutCallback_;
  vpiLog_LogLine* pLogLineHead_;

  int nMask_;

  static const char* szLevelNames_[vpiLog_Interface::NLogLevels];
};

//----------------------------------------------------------

vpiLog_LogImpl vpiLog_LogInst;
const char* vpiLog_LogImpl::szLevelNames_[vpiLog_Interface::NLogLevels];

vpiLog_LogImpl::vpiLog_LogImpl()
:pTraceLevelHead_(0)
,nTraceLevelDepth_(0)
,fpOutCallback_(&vpiLog_LogImpl::ConsoleOut)
,pLogLineHead_(0)
,nMask_(vpiLog_Interface::MaskDefault)
{
  szLevelNames_[0] = "None.....";
  szLevelNames_[1] = "Fatal....";
  szLevelNames_[2] = "Error....";
  szLevelNames_[3] = "Warning..";
  szLevelNames_[4] = "Debug....";
  szLevelNames_[5] = "Info.....";
  szLevelNames_[6] = "Verbose..";
  szLevelNames_[7] = "Trace....";
  szLevelNames_[8] = "VVerbose.";
}

vpiLog_LogImpl::~vpiLog_LogImpl()
{
  while (pTraceLevelHead_ != 0) {
    DeleteTraceHead();
  }
  while (pLogLineHead_ != 0) {
    DeleteLineHead();
  }
}

void vpiLog_LogImpl::DeleteTraceHead()
{
  vpiLog_TraceLevel* pTraceLevel = pTraceLevelHead_;
  pTraceLevelHead_ = pTraceLevelHead_->Next();
  delete pTraceLevel;
}

void vpiLog_LogImpl::DeleteLineHead()
{
  vpiLog_LogLine* pLogLine = pLogLineHead_;
  pLogLineHead_ = pLogLineHead_->Next();
  delete pLogLine;
}

int vpiLog_LogImpl::SetMask(int _nMask)
{
  int nMask = nMask_;
  nMask_ = _nMask;
  return nMask;
}

int vpiLog_LogImpl::IsMask(int _nMask)
{
  return (_nMask & nMask_) != 0;
}

int vpiLog_LogImpl::Mask2Level(int _nMask)
{
  switch (_nMask) {
    case vpiLog_Interface::MaskFatal: return 1;
    case vpiLog_Interface::MaskError: return 2;
    case vpiLog_Interface::MaskWarning: return 3;
    case vpiLog_Interface::MaskDebug: return 4;
    case vpiLog_Interface::MaskInfo: return 5;
    case vpiLog_Interface::MaskVerbose: return 6;
    case vpiLog_Interface::MaskTrace: return 7;
    case vpiLog_Interface::MaskVeryVerbose: return 8;
    default:
    return 0;
  }
}

vpiLog_LogCallback vpiLog_LogImpl::SetLogCallback(vpiLog_LogCallback _fpCallback)
{
  vpiLog_LogCallback fpOldCallback = fpOutCallback_;
  fpOutCallback_ = _fpCallback;
  return fpOldCallback;
}

#define vpiLog_Const_MAXINDENT 512
#define vpiLog_Const_MAXOUT 2048
#define vpiLog_Const_MAXMESSAGE 2048
#define vpiLog_Const_MAXCONTEXT 1024

static int vpiLog_Unused_Variable vpiLog_Const_MaxIndent = vpiLog_Const_MAXINDENT;
static int vpiLog_Unused_Variable vpiLog_Const_MaxOut = vpiLog_Const_MAXOUT;
static int vpiLog_Unused_Variable vpiLog_Const_MaxMessage = vpiLog_Const_MAXMESSAGE;
static int vpiLog_Unused_Variable vpiLog_Const_MaxContext = vpiLog_Const_MAXCONTEXT;


#include <stdarg.h>
#if defined(WIN32)
  #include <strsafe.h>
#endif

void vpiLog_LogImpl::Log(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage)
{
  if (_nMask > vpiLog_Interface::MaskNone && _nMask <= vpiLog_Interface::MaskVeryVerbose) {

    const char* szChannel = _szChannel; if (szChannel == 0) szChannel = "-";
    const char* szMessage = _szMessage; if (szMessage == 0) szMessage = "-";

    char szContext[vpiLog_Const_MAXCONTEXT];
    char* p = szContext;
    for (int i = 1; i < nTraceLevelDepth_ && (p-szContext) + 1 < vpiLog_Const_MaxIndent; i++) {
      *p++ = ' ';
      *p++ = ' ';
    }
    *p = '\0';

    if (_szContext != 0) {
      int nContextLen = ::strlen(_szContext);
      int nMemCpyLen = nContextLen + (p-szContext) + 3 < vpiLog_Const_MaxContext ? nContextLen : vpiLog_Const_MaxContext - (p-szContext) - 3;
      ::memcpy(p, _szContext, nMemCpyLen);
      p += nMemCpyLen;
    }
    *p = '\0';

    if (fpOutCallback_ == vpiLog_LogCallbackStore) {
      StoreLine(_nMask, szChannel, szContext, szMessage);
    } else if (fpOutCallback_ != 0) {
      fpOutCallback_(_nMask, szChannel, szContext, szMessage);
    }

  }
}

void vpiLog_LogImpl::Format(char* _szBuffer, int nBufferLen, int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage)
{
  const char* szChannel = _szChannel; if (szChannel == 0) szChannel = "-";
  const char* szContext = _szContext; if (szContext == 0) szContext = "-";
  const char* szMessage = _szMessage; if (szMessage == 0) szMessage = "-";

  int nLen = 0;
  const char* szData = 0;
  char* p = _szBuffer;

  szData = szLevelNames_[vpiLog_LogImpl::Mask2Level(_nMask)];
  nLen = ::strlen(szData); if (nLen > 0) { ::memcpy(p, szData, nLen + (p-_szBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_szBuffer) - 3 ); p += nLen; } *p++ = ' ';

  szData = szChannel;
  nLen = ::strlen(szData); if (nLen > 0) { ::memcpy(p, szData, nLen + (p-_szBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_szBuffer) - 3 ); p += nLen; } *p++ = ' ';

  szData = szContext;
  nLen = ::strlen(szData); if (nLen > 0) { ::memcpy(p, szData, nLen + (p-_szBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_szBuffer) - 3 ); p += nLen; } *p++ = ' ';

  szData = szMessage;
  nLen = ::strlen(szData); if (nLen > 0) { ::memcpy(p, szData, nLen + (p-_szBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_szBuffer) - 3 ); p += nLen; } *p++ = ' ';

  *p++ = '\n';
  *p = '\0';
}

void vpiLog_LogImpl::StoreLine(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage)
{
  vpiLog_LogLine* pLogLine = new vpiLog_LogLine(_nMask, _szChannel, _szContext, _szMessage);
  if (pLogLine != 0) {
    if (pLogLineHead_ == 0) {
      pLogLineHead_ = pLogLine;
    } else {
      vpiLog_LogLine* pLastLogLine = pLogLineHead_;
      while (pLastLogLine != 0) {
        vpiLog_LogLine* pNextLogLine = pLastLogLine->Next();
        if (pNextLogLine == 0) {
          pLastLogLine->Next(pLogLine);
        }
        pLastLogLine = pNextLogLine;
      }
    }
  }
}

int vpiLog_LogImpl::GetLineLength(int* _nChannel, int* _nContext, int* _nMessage)
{
  if (pLogLineHead_ == 0) {
    return 0;
  } else {
    *_nChannel = ::strlen(pLogLineHead_->Channel()) + 1;
    *_nContext = ::strlen(pLogLineHead_->Context()) + 1;
    *_nMessage = ::strlen(pLogLineHead_->Message()) + 1;
    return 1;
  }
}

int vpiLog_LogImpl::GetLineData(int* _nMask, char* _szChannel, char* _szContext, char* _szMessage)
{
  *_szChannel = '\0';
  *_szContext = '\0';
  *_szMessage = '\0';

  if (pLogLineHead_ == 0) {
    return 0;
  } else {

    int nChannelLen = ::strlen(pLogLineHead_->Channel());
    ::memcpy(_szChannel, pLogLineHead_->Channel(), nChannelLen);
    _szChannel[nChannelLen] = '\0';

    int nContextLen = ::strlen(pLogLineHead_->Context());
    ::memcpy(_szContext, pLogLineHead_->Context(), nContextLen);
    _szContext[nContextLen] = '\0';

    int nMessageLen = ::strlen(pLogLineHead_->Message());
    ::memcpy(_szMessage, pLogLineHead_->Message(), nMessageLen);
    _szMessage[nMessageLen] = '\0';

    *_nMask = pLogLineHead_->Mask();
    
    DeleteLineHead();

    return 1;
  }
}

void vpiLog_LogImpl::TraceEnter(const char* _szChannel, const char* _szContext)
{
  vpiLog_TraceLevel* pTraceLevel = new vpiLog_TraceLevel(_szChannel, _szContext);
  if (pTraceLevel != 0) {
    if (pTraceLevelHead_ != 0) {
      pTraceLevel->Next(pTraceLevelHead_);
    }
    pTraceLevelHead_ = pTraceLevel;
  }
  nTraceLevelDepth_++;
}

void vpiLog_LogImpl::TraceLeave(void)
{
  nTraceLevelDepth_--;
  if (pTraceLevelHead_ != 0) {
    DeleteTraceHead();
  }
}

const char* vpiLog_LogImpl::TraceChannel(void)
{
  const char* result = "";
  if (pTraceLevelHead_ != 0) {
    result = pTraceLevelHead_->Channel();
  }
  return result;
}

const char* vpiLog_LogImpl::TraceContext(void)
{
  const char* result = "";
  if (pTraceLevelHead_ != 0) {
    result = pTraceLevelHead_->Context();
  }
  return result;
}

//----------------------------------------------------------

vpiLog_LogLine::vpiLog_LogLine(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage)
:pNext_(0)
,nMask_(0)
,szChannel_(0)
,szContext_(0)
,szMessage_(0)
{
  nMask_ = _nMask;

  if (_szChannel != 0) {
    int nChannelLen = ::strlen(_szChannel);
    szChannel_ = new char[nChannelLen+1];
    if (szChannel_ != 0) {
      ::memcpy(szChannel_, _szChannel, nChannelLen+1);
    }
  }
  if (_szContext != 0) {
    int nContextLen = ::strlen(_szContext);
    szContext_ = new char[nContextLen+1];
    if (szContext_ != 0) {
      ::memcpy(szContext_, _szContext, nContextLen+1);
    }
  }
  if (_szMessage != 0) {
    int nMessageLen = ::strlen(_szMessage);
    szMessage_ = new char[nMessageLen+1];
    if (szMessage_ != 0) {
      ::memcpy(szMessage_, _szMessage, nMessageLen+1);
    }
  }
}

vpiLog_LogLine::~vpiLog_LogLine(void)
{
  if (szChannel_ != 0) {
    delete [] szChannel_;
  }
  if (szContext_ != 0) {
    delete [] szContext_;
  }
  if (szMessage_ != 0) {
    delete [] szMessage_;
  }
}

int vpiLog_LogLine::Mask(void)
{
  return nMask_;
}

const char* vpiLog_LogLine::Channel()
{
  return szChannel_ ? szChannel_ : "";
}

const char* vpiLog_LogLine::Context()
{
  return szContext_ ? szContext_ : "";
}

const char* vpiLog_LogLine::Message()
{
  return szMessage_ ? szMessage_ : "";
}

vpiLog_LogLine* vpiLog_LogLine::Next()
{
  return pNext_;
}

void vpiLog_LogLine::Next(vpiLog_LogLine* next)
{
  pNext_ = next;
}

//----------------------------------------------------------

vpiLog_TraceLevel::vpiLog_TraceLevel(const char* _szChannel, const char* _szContext)
:pNext_(0)
,szChannel_(0)
,szContext_(0)
{
  if (_szChannel != 0) {
    unsigned long nLen = ::strlen(_szChannel);
    szChannel_ = new char[nLen+1];
    if (szChannel_ != 0) {
      ::memcpy(szChannel_, _szChannel, nLen+1);
    }
  }
  if (_szContext != 0) {
    unsigned long nLen = ::strlen(_szContext);
    szContext_ = new char[nLen+1];
    if (szContext_ != 0) {
      ::memcpy(szContext_, _szContext, nLen+1);
    }
  }
}

vpiLog_TraceLevel::~vpiLog_TraceLevel(void)
{
  if (szChannel_ != 0) {
    delete [] szChannel_;
  }
  if (szContext_ != 0) {
    delete [] szContext_;
  }
}

const char* vpiLog_TraceLevel::Channel()
{
  const char* result = "";
  if (szChannel_ != 0) {
    result = szChannel_;
  }
  return result;
}

const char* vpiLog_TraceLevel::Context()
{
  const char* result = "";
  if (szContext_ != 0) {
    result = szContext_;
  }
  return result;
}

vpiLog_TraceLevel* vpiLog_TraceLevel::Next()
{
  return pNext_;
}

void vpiLog_TraceLevel::Next(vpiLog_TraceLevel* next)
{
  pNext_ = next;
}

vpiLog_TraceObject::vpiLog_TraceObject(const char* _szChannel, const char* _szContext, void* _pInstance)
{
  vpiLog_Unused_Arg(_pInstance);
  if (vpiLog_LogInst.IsMask(vpiLog_Interface::MaskTrace)) {
    vpiLog_LogInst.TraceEnter(_szChannel, _szContext);
    vpiLog_LogInst.Log(vpiLog_Interface::MaskTrace, vpiLog_LogInst.TraceChannel(), vpiLog_LogInst.TraceContext(), "Entering");
  }
}

vpiLog_TraceObject::~vpiLog_TraceObject()
{
  if (vpiLog_LogInst.IsMask(vpiLog_Interface::MaskTrace)) {
    vpiLog_LogInst.Log(vpiLog_Interface::MaskTrace, vpiLog_LogInst.TraceChannel(), vpiLog_LogInst.TraceContext(), "Leaving");
    vpiLog_LogInst.TraceLeave();
  }
}

//////////////////////////////////////////////////////////////////////

class vpiLog_ConfigImpl
{
public:
	vpiLog_ConfigImpl();
	virtual ~vpiLog_ConfigImpl();

  static const char* DefaultReadConfig(const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength);
  static void DefaultWriteConfig(const char* _szPath, const char* _szValue);

  vpiLog_ReadCallback SetReadCallback(vpiLog_ReadCallback _fpCallback);
  vpiLog_WriteCallback SetWriteCallback(vpiLog_WriteCallback _fpCallback);

  const char* Read(const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength);
  int Read(const char* _szPath, int _nDefault);
  void Write(const char* _szPath, const char* _szValue);
  void Write(const char* _szPath, int _nValue);

  const char* Read(const char* _szLevel1, const char* _szLevel2, const char* _szItem, const char* _szDefault);
  int Read(const char* _szLevel1, const char* _szLevel2, const char* _szItem, int _nDefault);
  void Write(const char* _szLevel1, const char* _szLevel2, const char* _szItem, const char* _szValue);
  void Write(const char* _szLevel1, const char* _szLevel2, const char* _szItem, int _nValue);

  static int Level2Path(char* _szPath, int _nPathLen, const char* _szLevel1, const char* _szLevel2, const char* _szItem);

protected:
  vpiLog_ReadCallback fpReadCallback_;
  vpiLog_WriteCallback fpWriteCallback_;

  static const char* szSep_;
};

//----------------------------------------------------------

vpiLog_ConfigImpl vpiLog_ConfigInst;
const char* vpiLog_ConfigImpl::szSep_ = vpiLog_PathSep;

vpiLog_ConfigImpl::vpiLog_ConfigImpl()
:fpReadCallback_(&vpiLog_ConfigImpl::DefaultReadConfig)
,fpWriteCallback_(&vpiLog_ConfigImpl::DefaultWriteConfig)
{
}

vpiLog_ConfigImpl::~vpiLog_ConfigImpl()
{
}

vpiLog_ReadCallback vpiLog_ConfigImpl::SetReadCallback(vpiLog_ReadCallback _fpCallback)
{
  vpiLog_ReadCallback fpOldCallback = fpReadCallback_;
  fpReadCallback_ = _fpCallback;
  return fpOldCallback;
}

vpiLog_WriteCallback vpiLog_ConfigImpl::SetWriteCallback(vpiLog_WriteCallback _fpCallback)
{
  vpiLog_WriteCallback fpOldCallback = fpWriteCallback_;
  fpWriteCallback_ = _fpCallback;
  return fpOldCallback;
}

#define vpiLog_Const_MAXPATH 2048
static int vpiLog_Unused_Variable vpiLog_Const_MaxPath = vpiLog_Const_MAXPATH;

int vpiLog_ConfigImpl::Level2Path(char* _szPath, int _nPathLen, const char* _szLevel1, const char* _szLevel2, const char* _szItem)
{
  int ok = 1;

  char* p = _szPath;

  if (_szLevel1 != 0) {
    const char* szData = _szLevel1;
    int nLen = ::strlen(szData);
    if (nLen > 0) {
      ::memcpy(p, szData, nLen + (p-_szPath) + 3 < _nPathLen ? nLen : _nPathLen - (p-_szPath) - 3 ); p += nLen;
      *p++ = vpiLog_ConfigImpl::szSep_[0];
    }
  }

  if (_szLevel2 != 0) {
    const char* szData = _szLevel2;
    int nLen = ::strlen(szData);
    if (nLen > 0) {
      ::memcpy(p, szData, nLen + (p-_szPath) + 3 < _nPathLen ? nLen : _nPathLen - (p-_szPath) - 3 ); p += nLen;
      *p++ = vpiLog_ConfigImpl::szSep_[0];
    }
  }

  if (_szItem != 0) {
    const char* szData = _szItem;
    int nLen = ::strlen(szData);
    if (nLen > 0) {
      ::memcpy(p, szData, nLen + (p-_szPath) + 3 < _nPathLen ? nLen : _nPathLen - (p-_szPath) - 3 ); p += nLen;
    }
  }

  *p = '\0';

  return ok;
}

const char* vpiLog_ConfigImpl::Read(const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength)
{
  const char* szResult = _szDefault;
  if (fpWriteCallback_ != 0) {
   const char* szValue = fpReadCallback_(_szPath, _szDefault, _szBuffer, _nBufferLength);
   if (szValue != 0) {
     szResult = szValue;
   }
  }
  return szResult;
}

int vpiLog_ConfigImpl::Read(const char* _szPath, int _nDefault)
{
  int nValue = _nDefault;
  char szBuffer[100];
  szBuffer[0] = '\0';
  char szDefault[100];
  sprintf(szDefault, "%d", _nDefault);
  const char* szValue = Read(_szPath, szDefault, szBuffer, sizeof(szBuffer));
  if (szValue != 0 || szValue[0] == '\0') {
    nValue = ::atoi(szValue);
  }
  return nValue;
}

void vpiLog_ConfigImpl::Write(const char* _szPath, const char* _szValue)
{
  if (fpWriteCallback_ != 0) {
    fpWriteCallback_(_szPath, _szValue);
  }
}

void vpiLog_ConfigImpl::Write(const char* _szPath, int _nValue)
{
  char szValue[100];
#if defined(WIN32)
  ::itoa(_nValue, szValue, 10);
#else 
  sprintf(szValue, "%d", _nValue);
#endif
  Write(_szPath, szValue);
}

//////////////////////////////////////////////////////////////////////


vpiLog_LogCallback vpiLog_Interface::SetLogCallback(vpiLog_LogCallback _fpCallback)
{
  return vpiLog_LogInst.SetLogCallback(_fpCallback);
}

vpiLog_ReadCallback vpiLog_Interface::SetReadCallback(vpiLog_ReadCallback _fpCallback)
{
  return vpiLog_ConfigInst.SetReadCallback(_fpCallback);
}

vpiLog_WriteCallback vpiLog_Interface::SetWriteCallback(vpiLog_WriteCallback _fpCallback)
{
  return vpiLog_ConfigInst.SetWriteCallback(_fpCallback);
}

int vpiLog_Interface::SetMask(int _nMask)
{
  return vpiLog_LogInst.SetMask(_nMask);
}

int vpiLog_Interface::IsMask(int _nMask)
{
  return vpiLog_LogInst.IsMask(_nMask);
}

int vpiLog_Interface::GetLineLength(int* _nChannel, int* _nContext, int* _nMessage)
{
  return vpiLog_LogInst.GetLineLength(_nChannel, _nContext, _nMessage);
}

int vpiLog_Interface::GetLineData(int* _nMask, char* _szChannel, char* _szContext, char* _szMessage)
{
  return vpiLog_LogInst.GetLineData(_nMask, _szChannel, _szContext, _szMessage);
}

void vpiLog_Interface::Fatal(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskFatal)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskFatal, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::Error(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskError)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskError, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::Warning(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskWarning)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskWarning, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::Debug(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskDebug)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskDebug, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::Info(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskInfo)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskInfo, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::Verbose(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskVerbose)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif  
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskVerbose, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::Trace(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskTrace)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskTrace, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

void vpiLog_Interface::VeryVerbose(const char* _szChannel, const char* _szContext, const char *fmt, ...)
{
  if (vpiLog_LogInst.IsMask(MaskVeryVerbose)) {
    if (fmt != 0) {
      char szMessage[vpiLog_Const_MAXMESSAGE]; szMessage[0] = '\0';
      va_list argp;
      va_start(argp, fmt);
    #if defined(WIN32)
      ::StringCbVPrintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #else
      ::vsnprintf(szMessage, vpiLog_Const_MaxMessage, fmt, argp);
    #endif
      va_end(argp);
      vpiLog_LogInst.Log(vpiLog_Interface::MaskVeryVerbose, _szChannel, _szContext, szMessage);
    } else {
      vpiLog_Warning(("vpiLog_Interface", "Info", "fmt == 0"));
    }
  }
}

//----------------------------------------------------------

const char* vpiLog_Interface::Read(const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength)
{
  return vpiLog_ConfigInst.Read(_szPath, _szDefault, _szBuffer, _nBufferLength);
}

int vpiLog_Interface::Read(const char* _szPath, int _nDefault)
{
  return vpiLog_ConfigInst.Read(_szPath, _nDefault);
}

void vpiLog_Interface::Write(const char* _szPath, const char* _szValue)
{
  vpiLog_ConfigInst.Write(_szPath, _szValue);
}

void vpiLog_Interface::Write(const char* _szPath, int _nValue)
{
  vpiLog_ConfigInst.Write(_szPath, _nValue);
}

//////////////////////////////////////////////////////////////////////

void vpiLog_LogImpl::ConsoleOut(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage)
{
  char szOut[vpiLog_Const_MAXOUT];
  vpiLog_LogImpl::Format(szOut, vpiLog_Const_MAXOUT, _nMask, _szChannel, _szContext, _szMessage);

#if defined(WIN32)
  ::OutputDebugString((LPCSTR) szOut);
#else
  ::fprintf(stderr, szOut);
#endif
}

const char* vpiLog_ConfigImpl::DefaultReadConfig(const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength)
{
  vpiLog_Unused_Arg(_szPath);
  if (_szDefault != 0 && _szBuffer != 0) {
    size_t nLength = ::strlen(_szDefault);
    if (_nBufferLength > nLength) {
      ::memcpy(_szBuffer, _szDefault, nLength + 1);
      return _szDefault;
    }
  }
  return "";
}

void vpiLog_ConfigImpl::DefaultWriteConfig(const char* _szPath, const char* _szValue)
{
  vpiLog_Unused_Arg(_szPath);
  vpiLog_Unused_Arg(_szValue);
  // do nothing
  // could store the pairs in a list here
}
