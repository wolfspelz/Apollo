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

  } else if (sMethod == "GetRezableLocations") {
    Apollo::KeyValueList kvLocations;

    Msg_VpView_GetLocations msgVVGL;
    if (!msgVVGL.Request()) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_VpView_GetLocations failed"));
    } else {
      for (Apollo::ValueElem* eLoc = 0; (eLoc = msgVVGL.vlLocations.nextElem(eLoc)) != 0; ) {

        Msg_VpView_GetLocationContexts msgGLC;
        msgGLC.hLocation = eLoc->getHandle();
        if (!msgGLC.Request()) {
          apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_VpView_GetLocationContexts failed for for loc=" ApHandleFormat "", ApHandlePrintf(msgGLC.hLocation)));
        } else {
          for (Apollo::ValueElem* eCtxt = 0; (eCtxt = msgGLC.vlContexts.nextElem(eCtxt)) != 0; ) {
            Msg_VpView_GetContextDetail msgVVGCD;
            msgVVGCD.hContext = eCtxt->getHandle();
            msgVVGCD.sKey = Msg_VpView_ContextDetail_DocumentUrl;
            if (!msgVVGCD.Request()) {
              apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_VpView_GetContextDetail(Msg_VpView_ContextDetail_DocumentUrl) failed for for loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandlePrintf(msgGLC.hLocation), ApHandlePrintf(msgVVGCD.hContext)));
            } else {
              if (msgVVGCD.sValue) {
                kvLocations.add(msgGLC.hLocation.toString(), msgVVGCD.sValue);
                break;
              }
            }
          }

        }
      }
    }

    //kvLocations.add(ApHandle(1, 23).toString(), "http://www.destination1.com/");
    //kvLocations.add(ApHandle(4, 56).toString(), "http://www.destination2.com/");
    response.set("kvLocations", kvLocations);

  } else if (sMethod == "RezItemToLocation") {
    ApHandle hLocation = request.getHandle("hLocation");
    inventory_.RezToLocation(hItem_, hLocation);

  } else {
    throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
  }
}

void ItemInfo::BringToFront()
{
  if (ApIsHandle(hDialog_)) {
  }
}

