// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgFile_H_INCLUDED)
#define MsgFile_H_INCLUDED

#include "ApMessage.h"

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

#endif // MsgFile_H_INCLUDED



