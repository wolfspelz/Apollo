// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Item.h"

void Item::AddFieldsForDisplay(Apollo::SrpcMessage& srpc)
{
  srpc.set("sName", sName_);
  srpc.set("sIcon", sIcon_);
  srpc.set("nSlot", nSlot_);
  srpc.set("nStacksize", nStacksize_);
  srpc.set("bIsRezable", bIsRezable_);
  srpc.set("bRezzed", bRezzed_);
}
