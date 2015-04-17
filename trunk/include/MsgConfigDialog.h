// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgConfigDialog_H_INCLUDED)
#define MsgConfigDialog_H_INCLUDED

#include "ApMessage.h"

/*
// CD <-
class Msg_ConfigDialog_Open: public ApRequestMessage
{
public:
  Msg_ConfigDialog_Open() : ApRequestMessage("ConfigDialog_Open") {}
};

// CD ->
class Msg_ConfigDialog_Opened: public ApNotificationMessage
{
public:
  Msg_ConfigDialog_Opened() : ApNotificationMessage("ConfigDialog_Opened"),hConfigDialog(ApNoHandle) {}
  ApHandle hConfigDialog;
};

// CD ->
class Msg_ConfigDialog_Closed: public ApNotificationMessage
{
public:
  Msg_ConfigDialog_Closed() : ApNotificationMessage("ConfigDialog_Closed") {}
};

// CD ->
class Msg_ConfigDialog_RequestRegisterPanel: public ApNotificationMessage
{
public:
  Msg_ConfigDialog_RequestRegisterPanel() : ApNotificationMessage("ConfigDialog_RequestRegisterPanel") {}
};

// CD <-
class Msg_ConfigDialog_RegisterPanel: public ApRequestMessage
{
public:
  Msg_ConfigDialog_RegisterPanel() : ApRequestMessage("ConfigDialog_RegisterPanel"), {}
  ApIN ApHandle hContext;
  ApIN String sLabel;
  ApIN String sIconPath;
};

// CD <-
class Msg_ConfigDialog_DeregisterPanel: public ApRequestMessage
{
public:
  Msg_ConfigDialog_DeregisterPanel() : ApRequestMessage("ConfigDialog_DeregisterPanel") {}
  ApIN ApHandle hContext;
};

// CD ->  -> CD
class Msg_ConfigDialog_SortPanels: public ApFilterMessage
{
public:
  Msg_ConfigDialog_SortPanels() : ApFilterMessage("ConfigDialog_SortPanels") {}
  ApINOUT Apollo::KVList kvPanelList;
};


// CD ->
class Msg_ConfigDialog_Load: public ApRequestMessage
{
public:
  Msg_ConfigDialog_Load() : ApRequestMessage("ConfigDialog_Load") {}
  ApIN ApHandle hContext;
  ApIN ApHandle hWebView;
};

// CD ->
class Msg_ConfigDialog_BeforeUnload: public ApRequestMessage
{
public:
  Msg_ConfigDialog_BeforeUnload() : ApRequestMessage("ConfigDialog_BeforeUnload") {}
  ApIN ApHandle hContext;
  ApIN ApHandle hWebView;
};

// CD ->
class Msg_ConfigDialog_Unloaded: public ApNotificationMessage
{
public:
  Msg_ConfigDialog_Unloaded() : ApNotificationMessage("ConfigDialog_Unloaded") {}
  ApIN ApHandle hContext;
};

typedef enum {
    CDMODE_None          = 0x0000
  , CDMODE_Small         = 0x0001
  , CDMODE_Normal        = 0x0002
  , CDMODE_Extended      = 0x0004
} ConfigDialogMode;
// CD ->
class Msg_ConfigDialog_SetMode: public ApNotificationMessage
{
public:
  Msg_ConfigDialog_SetMode() : ApNotificationMessage("ConfigDialog_SetMode") {}
  ApIN ConfigDialogMode nMode;
};

//   -> CD ->  
class Msg_ConfigDialog_GetMode: public ApRequestMessage
{
public:
  Msg_ConfigDialog_GetMode() : ApRequestMessage("ConfigDialog_GetMode") {}
  ApOUT ConfigDialogMode nMode;
};

// CD ->
class Msg_ConfigDialog_Apply: public ApRequestMessage
{
public:
  Msg_ConfigDialog_Apply() : ApRequestMessage("ConfigDialog_Apply") {}
};

// CD ->
class Msg_ConfigDialog_Cancel: public ApRequestMessage
{
public:
  Msg_ConfigDialog_Cancel() : ApRequestMessage("ConfigDialog_Cancel") {}
};
*/

#endif // MsgLuaScript_H_INCLUDED



