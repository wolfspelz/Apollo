// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Container_H_INCLUDED)
#define Container_H_INCLUDED

#include "Apollo.h"
#include "Item.h"

using namespace Apollo;

typedef StringPointerTree<Item*> ItemList;
typedef StringPointerTreeNode<Item*> ItemNode;
typedef StringPointerTreeIterator<Item*> ItemIterator;

class Container
{
public:
  Container(const String& sUrl)
    :sUrl_(sUrl)
    ,tAccess_(Apollo::getNow().Sec())
    ,bParsed_(0)
    ,bValid_(0)
    ,bPropertiesEvaluated_(0)
  {}

  int set(const String& sData, const String& sDigest);

  String getUrl() { return sUrl_; }
  String getDigest() { return sDigest_; }
  String getData() { return sData_; }
  int isValid() { return bValid_; }

  int hasProperty(const String& sKey);
  String getProperty(const String& sKey);
  int getItemIds(const String& sType, Apollo::ValueList& vlMimeTypes, int nMax, ValueList& vlIds);
  int getItemIds(ValueList& vlIds);
  int hasItem(const String& sId);
  int getItem(const String& sId, List& lAttributes);
  String getItemAttribute(const String& sId, const String& sKey);
  int hasItemData(const String& sId);
  int getItemData(const String& sId, String& sMimeType, Buffer& sbData);
  int setItemData(const String& sId, const String& sMimeType, Buffer& sbData);
  int deleteItemData(const String& sId);

  void accessNow() { tAccess_ = Apollo::getNow().Sec(); }
  time_t getAccessTime() { return tAccess_; }

protected:
  int evaluateProperties();
  Item* findItemByType(const String& sType);
  
  void getItemDataFromXmlNode(XMLNode* pItem, String& sMimeType, Buffer& sbData);

protected:
  String sUrl_;
  String sDigest_;
  String sData_;
  time_t tAccess_;

  int bParsed_;
  int bValid_;

  int bPropertiesEvaluated_;
  List lProperties_;

  List lData_;
  ItemList items_;
};

#endif // Container_H_INCLUDED
