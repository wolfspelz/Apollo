// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Request.h"

void GetGridsRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    Apollo::SrpcMessage kvIdValues;
    response.getKeyValueListAsSrpcMessage("kvValues", kvIdValues);
    pInventory_->GetGridsResponse(kvIdValues);
  }
}

void GetGridItemsRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    Apollo::SrpcMessage kvProperties;
    response.getKeyValueListAsSrpcMessage("kvProperties", kvProperties);
    pInventory_->GetGridItemsResponse(nGrid_, kvProperties);
  }
}

void GetItemsPropertiesRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    pInventory_->GetItemsPropertiesResponse(nGrid_, response);
  }
}

