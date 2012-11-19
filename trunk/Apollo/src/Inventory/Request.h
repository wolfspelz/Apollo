// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Request_H_INCLUDED)
#define Request_H_INCLUDED

// Forward decl
class Inventory;

class Request
{
public:
  Request(Inventory* pInventory)
    :pInventory_(pInventory)
  {}

  virtual void HandleResponse(Apollo::SrpcMessage& response) {}

protected:
  Inventory* pInventory_;
  Apollo::TimeValue tvCreated_;
};

class GetGridsRequest : public Request
{
public:
  GetGridsRequest(Inventory* pInventory) : Request(pInventory) {}

  void HandleResponse(Apollo::SrpcMessage& response);
};

class GetGridItemsRequest : public Request
{
public:
  GetGridItemsRequest(Inventory* pInventory, const String& sGrid) : Request(pInventory), sGrid_(sGrid) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  String sGrid_;
};

class GetItemsPropertiesRequest : public Request
{
public:
  GetItemsPropertiesRequest(Inventory* pInventory, const String& sGrid) : Request(pInventory), sGrid_(sGrid) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  String sGrid_;
};

#endif // Request_H_INCLUDED
