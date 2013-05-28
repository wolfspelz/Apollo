// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgFile.h"
#include "logModule.h"
#include "logFile.h"

String FileLogger::getTime()
{
  Apollo::TimeValue tvNow = Apollo::TimeValue::getTime();
  return tvNow.toString();
}

void FileLogger::appendLog(const String& sLevel, const String& sChannel, const String& sContext, const String& sMessage)
{
  String sLine;
  sLine += getTime();

  if (sLevel) {
    sLine += " ";
    sLine += sLevel;
  }

  if (sChannel) {
    sLine += " ";
    sLine += sChannel;
  }

  if (sContext) {
    sLine += " ";
    sLine += sContext;
  }

  if (sMessage) {
    sLine += " ";
    sLine += sMessage;
  }

  sLine.escape(String::EscapeLogLine);
  sLine += "\n";

  if (sFile_.empty()) {
    String sFile = Apollo::getModuleConfig(MODULE_NAME, "File/Path", "");

    //if (!sFile.empty() && !String::filenameIsAbsolutePath(sFile)) {
    //  sFile = Apollo::getAppBasePath() + sFile;
    //}

    if (!sFile.empty()) {
      if (Msg_File_Exists::_(sFile)) {
        String sFileOld = sFile + ".old";
        Msg_File_Rename::_(sFile, sFileOld);
      }
    }

    sFile_ = sFile;
    Msg_File_Append::_(sFile_, "-----------------------------------------------------------------------------\n");
  }

  if (!sFile_.empty()) {
    Msg_File_Append::_(sFile_, sLine);
  }
}
