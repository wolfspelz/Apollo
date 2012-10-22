// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Request.h"

//----------------------------------------------------------

void RebuildRequest::HandleResponse(Apollo::SrpcMessage& response)
{
  if (pInventory_ != 0) {
    pInventory_->Purge();

    Apollo::KeyValueList kvValues;
    response.getKeyValueList("kvValues", kvValues);

    pInventory_->BuildPanes(kvValues);
  }
}

//----------------------------------------------------------

