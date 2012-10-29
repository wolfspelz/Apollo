// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Inventory.h"

Inventory::~Inventory()
{
}

//---------------------------------------------------
// Interface

void Inventory::Create()
{
  ApHandle h = Apollo::newHandle();

  Msg_Dialog_Create msg;
  msg.hDialog = h; 
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 0;
  msg.sCaption = Apollo::translate(MODULE_NAME, "", "Inventory");
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Inventory.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  hCandidate_ = h;
}

void Inventory::Destroy()
{
  if (ApIsHandle(hDialog_)) {
    Msg_Dialog_Destroy msg;
    msg.hDialog = hDialog_;
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
    hDialog_ = ApNoHandle;
  }
}

void Inventory::SetVisibility(int bShow)
{
  if (ApIsHandle(hDialog_)) {
    ApHandle hView = Msg_Dialog_GetView::_(hDialog_);
    if (ApIsHandle(hView)) {
      Msg_WebView_Visibility::_(hView, bShow);
    }
  }
}

void Inventory::Show(int bShow)
{
  bVisible_ = bShow;

  SetVisibility(bShow);
}

void Inventory::OnOpened(const ApHandle& hDialog)
{
  if (hCandidate_ == hDialog) {
    hDialog_ = hDialog;

    SetVisibility(bVisible_);

    {
      Msg_Dialog_ContentCall msg;
      msg.hDialog = hDialog_;
      msg.sFunction = "Start";
      msg.Request();
    }

    {
      ApHandle h = Apollo::newHandle();

      Msg_Gm_SendRequest msg;
      msg.hRequest = h;
      msg.srpc.set(Srpc::Key::Method, "Item.GetItemIdsAndValuesByProperty");
      msg.srpc.set("sInventory", Apollo::getModuleConfig(MODULE_NAME, "Name", ""));
      msg.srpc.set("sKey", "IsGrid");
  
      if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Gm_SendRequest failed"); }

      RebuildRequest* pRequest = new RebuildRequest(this);
      if (pRequest != 0) {
        requests_.Set(h, pRequest);
      }
    }

  }
}

void Inventory::OnClosed(const ApHandle& hDialog)
{
  if (hDialog_ == hDialog) {
    hDialog_ = ApNoHandle;
  }
}

//---------------------------------------------------
// Internal

int Inventory::ConsumeResponse(const ApHandle& hRequest, Apollo::SrpcMessage& response)
{
  int bConsumed = 0;

  ApHandleTreeNode<Request*>* pNode = requests_.Find(hRequest);
  if (pNode != 0) {
    Request* pRequest = pNode->Value();
    requests_.Unset(hRequest);

    if (pRequest != 0) {
      pRequest->HandleResponse(response);
    }

    delete pRequest;
    pRequest = 0;

    bConsumed = 1;
  }

  return bConsumed;
}

void Inventory::Purge()
{
  nGrid_ = 0;
}

void Inventory::BuildPanes(Apollo::KeyValueList& kvValues)
{
  for (Apollo::KeyValueElem* e = 0; (e = kvValues.nextElem(e)); ) {
    long nId = String::atol(e->getKey());
    int bIsGrid = e->getInt();
    if (bIsGrid) {
      nGrid_ = nId;
    }
  }
}
