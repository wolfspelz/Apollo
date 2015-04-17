// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(IdentityModule_H_INCLUDED)
#define IdentityModule_H_INCLUDED

#include "identity.h"
#include "ApContainer.h"
#include "MsgSystem.h"
#include "MsgIdentity.h"
#include "MsgUnitTest.h"
#include "MsgHttpServer.h"
#include "Local.h"
#include "Container.h"
#include "ContainerClient.h"
#include "ItemClient.h"
#include "ItemAcquisitionTask.h"
#include "IdentityModuleTester.h"

typedef StringPointerTree<Container*> IdentityList;
typedef StringPointerTreeNode<Container*> IdentityNode;
typedef StringPointerTreeIterator<Container*> IdentityIterator;

typedef ApHandlePointerTree<ItemAcquisitionTask*> ItemAcquisitionTaskList;
typedef ApHandlePointerTreeNode<ItemAcquisitionTask*> ItemAcquisitionTaskListNode;
typedef ApHandlePointerTreeIterator<ItemAcquisitionTask*> ItemAcquisitionTaskListIterator;

class ContainerRequest: public Elem
{
public:
  void addHandle(ApHandle h) { vlHandles_.add(h); }
  ValueList& getHandles() { return vlHandles_; }

protected:
  ValueList vlHandles_;
};

class ItemRequest: public Elem
{
public:
  void addHandle(ApHandle h) { vlHandles_.add(h); }
  ValueList& getHandles() { return vlHandles_; }

protected:
  ValueList vlHandles_;
};

typedef ListT<ContainerRequest, Elem> ContainerRequestList;
typedef ListT<ItemRequest, Elem> ItemRequestList;

class IdentityModule
{
public:
  IdentityModule()
    :bInShutdown_(false)
    ,sDb_(DB_NAME)
  {}

  int init();
  void exit();

  void On_System_AfterLoadModules(Msg_System_AfterLoadModules* pMsg);
  void On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg);

  void On_Identity_RequestContainer(Msg_Identity_RequestContainer* pMsg);
  void On_Identity_ReceiveContainer(Msg_Identity_ReceiveContainer* pMsg);
  void On_Identity_SetContainer(Msg_Identity_SetContainer* pMsg);
  void On_Identity_SaveContainerToStorage(Msg_Identity_SaveContainerToStorage* pMsg);
  void On_Identity_LoadContainerFromStorage(Msg_Identity_LoadContainerFromStorage* pMsg);
  void On_Identity_IsContainerAvailable(Msg_Identity_IsContainerAvailable* pMsg);
  void On_Identity_DeleteContainer(Msg_Identity_DeleteContainer* pMsg);
  void On_Identity_DeleteContainerFromMemory(Msg_Identity_DeleteContainerFromMemory* pMsg);
  void On_Identity_DeleteContainerFromStorage(Msg_Identity_DeleteContainerFromStorage* pMsg);

  void On_Identity_ContainerBegin(Msg_Identity_ContainerBegin* pMsg);
  void On_Identity_ItemAdded(Msg_Identity_ItemAdded* pMsg);
  void On_Identity_ItemChanged(Msg_Identity_ItemChanged* pMsg);
  void On_Identity_ItemRemoved(Msg_Identity_ItemRemoved* pMsg);
  void On_Identity_ContainerEnd(Msg_Identity_ContainerEnd* pMsg);

  void On_Identity_HasProperty(Msg_Identity_HasProperty* pMsg);
  void On_Identity_GetProperty(Msg_Identity_GetProperty* pMsg);
  void On_Identity_GetItemIds(Msg_Identity_GetItemIds* pMsg);

  void On_Identity_GetItem(Msg_Identity_GetItem* pMsg);
  void On_Identity_IsItemDataAvailable(Msg_Identity_IsItemDataAvailable* pMsg);
  void On_Identity_GetItemData(Msg_Identity_GetItemData* pMsg);
  void On_Identity_RequestItem(Msg_Identity_RequestItem* pMsg);
  void On_Identity_ReceiveItem(Msg_Identity_ReceiveItem* pMsg);
  void On_Identity_SetItemData(Msg_Identity_SetItemData* pMsg);
  void On_Identity_SaveItemDataToStorage(Msg_Identity_SaveItemDataToStorage* pMsg);
  void On_Identity_LoadItemDataFromStorage(Msg_Identity_LoadItemDataFromStorage* pMsg);
  void On_Identity_DeleteItemData(Msg_Identity_DeleteItemData* pMsg);
  void On_Identity_DeleteItemDataFromMemory(Msg_Identity_DeleteItemDataFromMemory* pMsg);
  void On_Identity_DeleteItemDataFromStorage(Msg_Identity_DeleteItemDataFromStorage* pMsg);
  void On_Identity_AcquireItemData(Msg_Identity_AcquireItemData* pMsg);

  void On_Identity_ClearAllCache(Msg_Identity_ClearAllCache* pMsg);
  void On_Identity_ExpireAllCache(Msg_Identity_ExpireAllCache* pMsg);

  void On_Identity_ClearAllStorage(Msg_Identity_ClearAllStorage* pMsg);
  void On_Identity_ExpireAllStorage(Msg_Identity_ExpireAllStorage* pMsg);
  void On_Identity_SetStorageName(Msg_Identity_SetStorageName* pMsg);

  void On_HttpServer_ReceiveRequest(Msg_HttpServer_ReceiveRequest* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);

  friend class IdentityModuleTester;
#endif

protected:
  //String prepareDbKey(const String& sText) { return String::reverse(sText); }
  //String prepareDbKey(const String& sText) { return sText; }
  String prepareDbKey(const String& sText) { String sResult = Apollo::getShortHash(sText, 4) + " " + sText; return sResult; }

  ItemAcquisitionTask* findItemAcquisitionTaskByRequest(const ApHandle& hRequest);
  String selectItemId(const String& sUrl, const String& sType, const String& sMimeType);

protected:
  bool bInShutdown_;
  String sDb_;

  IdentityList data_;
  ItemAcquisitionTaskList itemTasks_;

  ContainerRequestList lContainerRequests_;
  ItemRequestList lItemRequests_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<IdentityModule> IdentityModuleInstance;

// ---------------------------------

#endif // IdentityModule_H_INCLUDED
