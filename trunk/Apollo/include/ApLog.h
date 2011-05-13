// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApLog_h_INCLUDED)
#define ApLog_h_INCLUDED

#include "ApMessage.h"

enum apLog_LogLevel { apLog_LevelNone = 0
  ,apLog_LevelFatal
  ,apLog_LevelError
  ,apLog_LevelWarning
  ,apLog_LevelUser
  ,apLog_LevelDebug
  ,apLog_LevelInfo
  ,apLog_LevelVerbose
  ,apLog_LevelTrace
  ,apLog_LevelVeryVerbose
  ,apLog_LevelAlert
  ,apLog_NLogLevels
};

enum apLog_LogMask { apLog_MaskNone = 0
  ,apLog_MaskFatal       = (1 << apLog_LevelFatal      ) // 2
  ,apLog_MaskError       = (1 << apLog_LevelError      ) // 4
  ,apLog_MaskWarning     = (1 << apLog_LevelWarning    ) // 8
  ,apLog_MaskUser        = (1 << apLog_LevelUser       ) // 16
  ,apLog_MaskDebug       = (1 << apLog_LevelDebug      ) // 32
  ,apLog_MaskInfo        = (1 << apLog_LevelInfo       ) // 64
  ,apLog_MaskVerbose     = (1 << apLog_LevelVerbose    ) // 128
  ,apLog_MaskTrace       = (1 << apLog_LevelTrace      ) // 256
  ,apLog_MaskVeryVerbose = (1 << apLog_LevelVeryVerbose) // 512
  ,apLog_MaskAlert       = (1 << apLog_LevelAlert      ) // 1024

  ,apLog_MaskAll         = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser|apLog_MaskDebug|apLog_MaskInfo|apLog_MaskVerbose|apLog_MaskTrace|apLog_MaskVeryVerbose|apLog_MaskAlert
  ,apLog_MaskMaxVerbose  = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser|apLog_MaskDebug|apLog_MaskInfo|apLog_MaskVerbose                                      |apLog_MaskAlert
  ,apLog_MaskMaxInfo     = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser|apLog_MaskDebug|apLog_MaskInfo
  ,apLog_MaskMaxDebug    = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser|apLog_MaskDebug
  ,apLog_MaskMaxUser     = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser
  ,apLog_MaskMaxError    = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning                                
  ,apLog_MaskSilent      = 0                                                                                
  ,apLog_MaskNoTrace     = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser|apLog_MaskDebug|apLog_MaskInfo|apLog_MaskVerbose                |apLog_MaskVeryVerbose|apLog_MaskAlert
  ,apLog_MaskDefault     = apLog_MaskFatal|apLog_MaskError|apLog_MaskWarning|apLog_MaskUser|apLog_MaskDebug|apLog_MaskInfo                                                        |apLog_MaskAlert
};

#define apLog_Fatal(_args_) apLog_FatalI _args_
#define apLog_Error(_args_) apLog_ErrorI _args_
#define apLog_Warning(_args_) apLog_WarningI _args_
#define apLog_User(_args_) apLog_UserI _args_
#define apLog_Debug(_args_) apLog_DebugI _args_
#define apLog_Info(_args_) apLog_InfoI _args_
#define apLog_Verbose(_args_) apLog_VerboseI _args_
#if defined(_DEBUG)
#define apLog_Trace(_args_) apLog_TraceObject apLog_TraceObject_Instance _args_
#define apLog_VeryVerbose(_args_) if (apLog_IsVeryVerbose) { apLog_VeryVerboseI _args_; }
#define apLog_Alert(_args_) apLog_AlertI _args_
#else
#define apLog_Trace(_args_) 
#define apLog_VeryVerbose(_args_) 
#define apLog_Alert(_args_) 
#endif

#define apLog_IsVerbose apLog_IsMask(apLog_MaskVerbose)
#define apLog_IsTrace apLog_IsMask(apLog_MaskTrace)
#define apLog_IsVeryVerbose apLog_IsMask(apLog_MaskVeryVerbose)

#define apLog_Const_MAXINDENT 512
#define apLog_Const_MAXOUT 2048
#define apLog_Const_MAXMESSAGE 2048
#define apLog_Const_MAXCONTEXT 1024

#endif // !defined(ApLog_h_INCLUDED)
