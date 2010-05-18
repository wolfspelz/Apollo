// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApModule.h"

ApException::ApException(const char* szFmt, ...)
{
  if (szFmt != 0) {
    char szMessage[nMaxMessage]; szMessage[0] = '\0';
    va_list argp;
    va_start(argp, szFmt);
    Apollo::formatVarArg(szMessage, nMaxMessage, szFmt, argp);
    va_end(argp);
    sText_ = szMessage;
    //apLog_LogMessage(nMask, szChannel, szContext, szMessage);
  }
}
