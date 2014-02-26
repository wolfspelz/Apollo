// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ItemInfo.h"

// For ~PointerTree() which iterates the list of ItemInfo(s) and delete(s) all entries
ItemInfo::~ItemInfo()
{
  Destroy();
}

void ItemInfo::Create(int nX, int nY, int nWidth, int nHeight)
{
  ApHandle hDialog = Apollo::newHandle();

  if (nWidth <= 0) { nWidth = Apollo::getModuleConfig(MODULE_NAME, "ItemInfo/DefaultWidth", 300); }
  if (nHeight <= 0) { nHeight = Apollo::getModuleConfig(MODULE_NAME, "ItemInfo/DefaultHeight", 200); }

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog; 
  msg.nLeft = nX;
  msg.nTop = nY;
  msg.nWidth = nWidth;
  msg.nHeight = nHeight;
  msg.bVisible = 1;
  msg.sCaption = Msg_Translation_Get::_(MODULE_NAME, "", "Item Info");
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "ItemIcon16.gif";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "ItemInfo.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  hDialog_ = hDialog;
}

void ItemInfo::Destroy()
{
  if (ApIsHandle(hDialog_)) {
    Msg_Dialog_Destroy msg;
    msg.hDialog = hDialog_;
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
    OnClosed();
  }

  hItem_ = ApNoHandle;
}

void ItemInfo::OnClosed()
{
  hDialog_ = ApNoHandle;
}

void ItemInfo::BringToFront()
{
  if (ApIsHandle(hDialog_)) {
  }
}
