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

void ItemInfo::Create(const String& sTitle, int nX, int nY, int nWidth, int nHeight)
{
  ApHandle hDialog = Apollo::newHandle();

  int nOffsetX = Apollo::getModuleConfig(MODULE_NAME, "ItemInfo/OffsetX", 0);
  int nOffsetY = Apollo::getModuleConfig(MODULE_NAME, "ItemInfo/OffsetY", 0);

  if (nWidth <= 0) { nWidth = Apollo::getModuleConfig(MODULE_NAME, "ItemInfo/DefaultWidth", 300); }
  if (nHeight <= 0) { nHeight = Apollo::getModuleConfig(MODULE_NAME, "ItemInfo/DefaultHeight", 200); }

  bOpen_ = 0;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog; 
  msg.nLeft = nX + nOffsetX;
  msg.nTop = nY + nOffsetY;
  msg.nWidth = nWidth;
  msg.nHeight = nHeight;
  msg.bVisible = 1;
  msg.sCaption = sTitle;
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

void ItemInfo::OnOpened()
{
  bOpen_ = 1;

  Msg_Dialog_ContentCall msg;
  msg.hDialog = hDialog_;
  msg.sFunction = "Start";
  msg.Request();
}

void ItemInfo::OnClosed()
{
  hDialog_ = ApNoHandle;
}

void ItemInfo::OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString(Srpc::Key::Method);

  if (0) {
  } else if (sMethod == "GetItemProperties") {
    Item* pItem = inventory_.FindItem(hItem_);
    if (pItem != 0) {
      pItem->AddFieldsForDisplay(response);
    }

  } else if (sMethod == "GetRezableDestinations") {
    Apollo::KeyValueList kvDestinations;
    kvDestinations.add(ApHandle(0, 1).toString(), "http://www.destination1.com/");
    kvDestinations.add(ApHandle(0, 2).toString(), "http://www.destination2.com/");
    response.set("kvDestinations", kvDestinations);

  } else if (sMethod == "RezItemToDestination") {
    String sDestination = request.getString("sDestination");
    RezToDestination(sDestination);

  } else {
    throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
  }
}

void ItemInfo::BringToFront()
{
  if (ApIsHandle(hDialog_)) {
  }
}

//-------------------------

void ItemInfo::RezToDestination(const String& sDestination)
{
  inventory_.SendRezToLocationRequest(hItem_, "xmpp:room@conference.osiris", sDestination, 200);
}

