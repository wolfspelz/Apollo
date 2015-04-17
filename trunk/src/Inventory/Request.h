// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Request_H_INCLUDED)
#define Request_H_INCLUDED

// Forward decl
class Inventory;
class Item;

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
  GetPanelItemsRequest(Inventory* pInventory, const ApHandle& hPanel) : Request(pInventory), hPanel_(hPanel) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  ApHandle hPanel_;
};

class GetItemsPropertiesRequest : public Request
{
public:
  GetItemsPropertiesRequest(Inventory* pInventory, const ApHandle& hPanel) : Request(pInventory), hPanel_(hPanel) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  ApHandle hPanel_;
};

class RezToLocationRequest : public Request
{
public:
  RezToLocationRequest(Inventory* pInventory, const ApHandle& hItem, const String& sLocation, const String& sDestination) : Request(pInventory), hItem_(hItem), sLocation_(sLocation), sDestination_(sDestination) {}

  void HandleResponse(Apollo::SrpcMessage& response);

  ApHandle hItem_;
  String sLocation_;
  String sDestination_;
};

#endif // Request_H_INCLUDED
