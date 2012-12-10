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

class GetPanelsRequest : public Request
{
public:
  GetPanelsRequest(Inventory* pInventory) : Request(pInventory) {}

  void HandleResponse(Apollo::SrpcMessage& response);
};

class GetPanelItemsRequest : public Request
{
public:
  GetPanelItemsRequest(Inventory* pInventory, const String& sPanel) : Request(pInventory), sPanel_(sPanel) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  String sPanel_;
};

class GetItemsPropertiesRequest : public Request
{
public:
  GetItemsPropertiesRequest(Inventory* pInventory, const String& sPanel) : Request(pInventory), sPanel_(sPanel) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  String sPanel_;
};

#endif // Request_H_INCLUDED
