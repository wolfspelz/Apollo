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
  ApIN String sPathName;
  ApOUT Buffer sbData;
};

class Msg_File_Save: public ApRequestMessage
{
public:
  Msg_File_Save() : ApRequestMessage("File_Save"), nFlags(Msg_File_Flag_CreatePath) {}
  ApIN String sPathName;
  ApIN Buffer sbData;
  ApIN int nFlags;
};

class Msg_File_Append: public ApRequestMessage
{
public:
  Msg_File_Append() : ApRequestMessage("File_Append"), nFlags(Msg_File_Flag_CreatePath) {}
  ApIN String sPathName;
  ApIN Buffer sbData;
  ApIN int nFlags;
};

class Msg_File_Delete: public ApRequestMessage
{
public:
  Msg_File_Delete() : ApRequestMessage("File_Delete") {}
  ApIN String sPathName;
};

class Msg_File_Exists: public ApRequestMessage
{
public:
  Msg_File_Exists() : ApRequestMessage("File_Exists") {}
  ApIN String sPathName;
  ApOUT int bExists;
};

class Msg_File_Rename: public ApRequestMessage
{
public:
  Msg_File_Rename() : ApRequestMessage("File_Rename") {}
  ApIN String sPathName;
  ApIN String sNewPathName;
  ApOUT int bExists;
};

#endif // MsgFile_H_INCLUDED



