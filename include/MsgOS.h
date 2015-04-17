// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgOs_H_INCLUDED)
#define MsgOs_H_INCLUDED

#include "ApMessage.h"

// module -> os
class APOLLO_API Msg_OSTimer_Start: public ApRequestMessage
{
public:
  Msg_OSTimer_Start() : ApRequestMessage("OSTimer_Start"), nSec(0), nMicroSec(0) {}
public:
  ApIN ApHandle hTimer;
  ApIN int nSec;
  ApIN int nMicroSec;
};

// module -> os
class APOLLO_API Msg_OSTimer_Cancel: public ApRequestMessage
{
public:
  Msg_OSTimer_Cancel() : ApRequestMessage("OSTimer_Cancel") {}
  ApIN ApHandle hTimer;
};

// os -> module
class APOLLO_API Msg_OSTimer_Event: public ApNotificationMessage
{
public:
  Msg_OSTimer_Event() : ApNotificationMessage("OSTimer_Event") {}
  ApIN ApHandle hTimer;
};

// ----------------------------------------------------------

class Msg_Process_Start: public ApRequestMessage
{
public:
  Msg_Process_Start() : ApRequestMessage("Process_Start"), nPid(0) {}
  ApIN String sExePath;
  ApIN String sCwdPath;
  ApIN Apollo::ValueList vlArgs;
  ApOUT int nPid;
};

class Msg_Process_Kill: public ApRequestMessage
{
public:
  Msg_Process_Kill() : ApRequestMessage("Process_Kill"), nPid(0) {}
  ApIN int nPid;
};

class Msg_Process_GetId: public ApRequestMessage
{
public:
  Msg_Process_GetId() : ApRequestMessage("Process_GetId"), nPid(0) {}
  ApIN String sName;
  ApOUT int nPid;
};

class Msg_Process_GetInfo: public ApRequestMessage
{
public:
  Msg_Process_GetInfo() : ApRequestMessage("Process_GetInfo"), nPid(0) {}
  ApIN int nPid;
  #define Msg_Process_GetInfo_ProgramPath "Name"
  ApOUT Apollo::KeyValueList kvInfo;
};

// ----------------------------------------------------------

#define Msg_File_Flag_CreatePath 1

class Msg_File_Load: public ApRequestMessage
{
public:
  Msg_File_Load() : ApRequestMessage("File_Load") {}
  static int _(const String& sFile, Buffer& sbData)
  {
    Msg_File_Load msg;
    msg.sPathName = sFile;
    int ok = msg.Request();
    if (ok) {
      sbData.SetData(msg.sbData.Data(), msg.sbData.Length());
    }
    return ok;
  }
  static int _(const String& sFile, String& sData)
  {
    Msg_File_Load msg;
    msg.sPathName = sFile;
    int ok = msg.Request();
    if (ok) {
      msg.sbData.GetString(sData);
    }
    return ok;
  }
public:
  ApIN String sPathName;
  ApOUT Buffer sbData;
};

class Msg_File_Save: public ApRequestMessage
{
public:
  Msg_File_Save() : ApRequestMessage("File_Save"), nFlags(Msg_File_Flag_CreatePath) {}
  static int _(const String& sFile, const Buffer& sbData)
  {
    Msg_File_Save msg;
    msg.sPathName = sFile;
    msg.nFlags = Msg_File_Flag_CreatePath;
    msg.sbData.SetData(sbData.Data(), sbData.Length());  
    return msg.Request();
  }
  static int _(const String& sFile, const String& sData)
  {
    Msg_File_Save msg;
    msg.sPathName = sFile;
    msg.nFlags = Msg_File_Flag_CreatePath;
    msg.sbData.SetData(sData);
    return msg.Request();
  }
public:
  ApIN String sPathName;
  ApIN Buffer sbData;
  ApIN int nFlags;
};

class Msg_File_Append: public ApRequestMessage
{
public:
  Msg_File_Append() : ApRequestMessage("File_Append"), nFlags(Msg_File_Flag_CreatePath) {}
  static int _(const String& sFile, const Buffer& sbData)
  {
    Msg_File_Append msg;
    msg.sPathName = sFile;
    msg.nFlags = Msg_File_Flag_CreatePath;
    msg.sbData.SetData(sbData.Data(), sbData.Length());
    return msg.Request();
  }
  static int _(const String& sFile, const String& sData)
  {
    Msg_File_Append msg;
    msg.sPathName = sFile;
    msg.nFlags = Msg_File_Flag_CreatePath;
    msg.sbData.SetData(sData);
    return msg.Request();
  }
public:
  ApIN String sPathName;
  ApIN Buffer sbData;
  ApIN int nFlags;
};

class Msg_File_Delete: public ApRequestMessage
{
public:
  Msg_File_Delete() : ApRequestMessage("File_Delete") {}
  static int _(const String& sFile)
  {
    Msg_File_Delete msg;
    msg.sPathName = sFile;
    return msg.Request();
  }
public:
  ApIN String sPathName;
};

class Msg_File_Exists: public ApRequestMessage
{
public:
  Msg_File_Exists() : ApRequestMessage("File_Exists") {}
  static int _(const String& sFile)
  {
    Msg_File_Exists msg;
    msg.sPathName = sFile;
    if (msg.Request()) {
      return msg.bExists;
    }
    return 0;
  }
public:
  ApIN String sPathName;
  ApOUT int bExists;
};

class Msg_File_Rename: public ApRequestMessage
{
public:
  Msg_File_Rename() : ApRequestMessage("File_Rename") {}
  static int _(const String& sFile, const String& sNewName)
  {
    Msg_File_Rename msg;
    msg.sPathName = sFile;
    msg.sNewPathName = sNewName;
    return msg.Request();
  }
public:
  ApIN String sPathName;
  ApIN String sNewPathName;
  ApOUT int bExists;
};

// ----------------------------------------------------------

#endif // MsgOs_H_INCLUDED
