// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Local_H_INCLUDED)
#define Local_H_INCLUDED

#define MODULE_NAME "WebView"
#define LOG_CHANNEL MODULE_NAME

#include "apLog.h"
#include "MsgMainLoop.h"
#include "MsgSrpcGate.h"
#include "MsgWebView.h"
#include "MsgUnitTest.h"
#include "MsgSystem.h"
#include "MsgConfig.h"

//class LogContext
//{
//public:
//  static String CodePosition(const char* pszFile, int nLine)
//  {
//    String s(pszFile);
//    s += ":";
//    s += String::from(nLine);
//    return s;
//  }
//};
//
//#define LOG_CONTEXT LogContext::CodePosition(__FILE__, __LINE__)

//#define LOG_CONTEXT String(__func__) // g++
//#define LOG_CONTEXT String(__PRETTY_FUNCTION__) // gcc
#define LOG_CONTEXT String(__FUNCTION__) // MSVC8

#define _sz(x) StringType(x)
#define _szH(x) ApHandleType(x)

#endif // Local_H_INCLUDED
