// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(logFile_H_INCLUDED)
#define logFile_H_INCLUDED

class FileLogger
{
public:
  FileLogger() {}
  virtual ~FileLogger() {}

public:
  void appendLog(const String& sLevel, const String& sChannel, const String& sContext, const String& sMessage);

protected:
  String getTime();

protected:
  String sFile_;
};

#endif // logFile_H_INCLUDED
