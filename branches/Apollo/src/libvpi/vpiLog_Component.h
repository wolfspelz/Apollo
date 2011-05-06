#if !defined(vpiLog_Component_h_INCLUDED)
#define vpiLog_Component_h_INCLUDED

#if !defined(_MSC_VER)
  #include <sys/types.h>
#elif _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000

#define vpiLog_ConfigReadBufferLength 10000
typedef char vpiLog_ConfigReadBuffer[vpiLog_ConfigReadBufferLength];

//////////////////////////////////////////////////////////////////////
// This is the interface to be used:

#define vpiLog_Fatal(_args_) vpiLog_Interface::Fatal _args_
#define vpiLog_Error(_args_) vpiLog_Interface::Error _args_
#define vpiLog_Warning(_args_) vpiLog_Interface::Warning _args_
#define vpiLog_Debug(_args_) vpiLog_Interface::Debug _args_
#define vpiLog_Info(_args_) vpiLog_Interface::Info _args_

#ifdef _DEBUG
  #define vpiLog_Verbose(_args_) vpiLog_Interface::Verbose _args_
  #define vpiLog_Trace(_args_) vpiLog_TraceObject vpiLog_TraceInstance _args_
  #define vpiLog_VeryVerbose(_args_) vpiLog_Interface::VeryVerbose _args_
  #define vpiLog_IsVerbose vpiLog_Interface::IsMask(vpiLog_Interface::MaskVerbose)
  #define vpiLog_IsVeryVerbose vpiLog_Interface::IsMask(vpiLog_Interface::MaskVeryVerbose)
#else
  #define vpiLog_Verbose(_args_) vpiLog_Interface::Verbose _args_
  #define vpiLog_Trace(_args_) vpiLog_TraceObject vpiLog_TraceInstance _args_
  #define vpiLog_VeryVerbose(_args_) vpiLog_Interface::VeryVerbose _args_
  #define vpiLog_IsVerbose vpiLog_Interface::IsMask(vpiLog_Interface::MaskVerbose)
  #define vpiLog_IsVeryVerbose vpiLog_Interface::IsMask(vpiLog_Interface::MaskVeryVerbose)
  // Compile out for release:
  //#define vpiLog_Trace(_args_)
  //#define vpiLog_VeryVerbose(_args_)
  //#define vpiLog_IsVerbose 0
  //#define vpiLog_IsVeryVerbose 0
#endif

// Use these...
#define vpiLog_Read(_path_, _default_, _buffer_) vpiLog_Interface::Read(_path_, _default_, (char*) _buffer_, vpiLog_ConfigReadBufferLength)
#define vpiLog_ReadInt(_path_, _default_) vpiLog_Interface::Read(_path_, _default_)
#define vpiLog_Write(_path_, _value_) vpiLog_Interface::Write(_path_, _value_)


//////////////////////////////////////////////////////////////////////
// Usage:

//  vpiLog_Trace(("CTestModule", "Method"));
//  vpiLog_Info(("CTestModule", "Method", "A message with args: %s, %d", "String", 42));
//  if (vpiLog_IsVeryVerbose) {}
//  const char* param = vpiLog_ConfigRead("Path", "DefaultString");
//  int param = vpiLog_ConfigRead(CONFIG_LEVEL1 "/" CONFIG_LEVEL2 "/" "Item", 42);
//  vpiLog_ConfigWrite("Path", "Value");
//  vpiLog_ConfigWrite(CONFIG_LEVEL1 "/" CONFIG_LEVEL2 "/" "Item", 42);

//  void logfunction(int level, char* channel, char* context, char* message) {
//    printf("%d %s %s %s\n", level, channel, context, message);
//  }
//  vpiLog_Interface::SetLogCallback((void*) logfunction);



//////////////////////////////////////////////////////////////////////
// This also needs to be here:

typedef void (*vpiLog_LogCallback) (int _nLevel, const char* _szChannel, const char* _szContext, const char* _szMessage);
typedef const char* (*vpiLog_ReadCallback) (const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength);
typedef void (*vpiLog_WriteCallback) (const char* _szPath, const char* _szValue);
#define vpiLog_PathSep "/"
#define vpiLog_LogCallbackStore ((vpiLog_LogCallback) 1)

class vpiLog_Interface
{
public:
  static void Fatal(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void Error(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void Warning(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void Debug(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void Info(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void Verbose(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void Trace(const char* _szChannel, const char* _szContext, const char *fmt, ...);
  static void VeryVerbose(const char* _szChannel, const char* _szContext, const char *fmt, ...);

  static void Info(const char* _szChannel, const char* _szContext, int _pInstance, const char *fmt, ...);

  static vpiLog_LogCallback SetLogCallback(vpiLog_LogCallback _fpCallback);
  static vpiLog_ReadCallback SetReadCallback(vpiLog_ReadCallback _fpCallback);
  static vpiLog_WriteCallback SetWriteCallback(vpiLog_WriteCallback _fpCallback);

  static int GetLineLength(int* _nChannel, int* _nContext, int* _nMessage);
  static int GetLineData(int* _nMask, char* _szChannel, char* _szContext, char* _szMessage);

  static int SetMask(int _nMask);
  static int IsMask(int _nMask);

  enum LogMask {
     MaskNone =        (1 << 0)

    ,MaskFatal       = (1 << 1) // 2
    ,MaskError       = (1 << 2) // 4
    ,MaskWarning     = (1 << 3) // 8
    ,MaskDebug       = (1 << 4) // 16
    ,MaskInfo        = (1 << 5) // 32
    ,MaskVerbose     = (1 << 6) // 64
    ,MaskTrace       = (1 << 7) // 128
    ,MaskVeryVerbose = (1 << 8) // 256

    ,MaskAll         = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo|MaskVerbose|MaskTrace|MaskVeryVerbose
    ,MaskDefault     = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo|MaskVerbose          |MaskVeryVerbose
    ,NLogLevels      = 9
  };

  static const char* Read(const char* _szPath, const char* _szDefault, char* _szBuffer, size_t _nBufferLength);
  static int Read(const char* _szPath, int _nDefault);
  static void Write(const char* _szPath, const char* _szValue);
  static void Write(const char* _szPath, int _nValue);
};

class vpiLog_TraceObject
{
public:
	vpiLog_TraceObject(const char* _szChannel, const char* _szContext, void* _pInstance = 0);
	virtual ~vpiLog_TraceObject();
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(vpiLog_Component_h_INCLUDED)
