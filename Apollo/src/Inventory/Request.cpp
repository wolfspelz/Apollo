// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Request.h"

void GetPanelsRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    Apollo::SrpcMessage kvIdValues;
    response.getKeyValueListAsSrpcMessage("kvValues", kvIdValues);
    pInventory_->GetPanelsResponse(kvIdValues);
  }
}

void GetPanelItemsRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    Apollo::SrpcMessage kvProperties;
    response.getKeyValueListAsSrpcMessage("kvProperties", kvProperties);
    pInventory_->GetPanelItemsResponse(hPanel_, kvProperties);
  }
}

void GetItemsPropertiesRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    pInventory_->GetItemsPropertiesResponse(hPanel_, response);
  }
}

