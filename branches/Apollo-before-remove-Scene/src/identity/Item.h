// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Item_H_INCLUDED)
#define Item_H_INCLUDED

#include "Apollo.h"
#include "XMLProcessor.h"

using namespace Apollo;

class Container;

class Item
{
public:
  Item(Container* pContainer, const String& sId)
    :pContainer_(pContainer)
    ,sId_(sId)
    ,tAccess_(Apollo::getNow().Sec())
    ,bDataAvailable_(0)
  {}

  String getId() { return sId_; }

  void accessNow() { tAccess_ = Apollo::getNow().Sec(); }
  time_t getAccessTime() { return tAccess_; }

  int parseXmlNode(XMLNode* pNode);

  int getAttributes(List& lAttributes);
  String getAttribute(const String& sKey);
  void setAttribute(const String& sKey, const String& sValue);

  int hasData() { return bDataAvailable_; }
  int setData(const String& sMimeType, Buffer& sbData);
  int getData(String& sData);
  int getData(Buffer& sbData);
  int deleteData();

protected:
  Container* pContainer_;
  String sId_;
  time_t tAccess_;
  List lAttributes_;
  //String sDigest_;
  //String sType_;
  //String sMimeType_;
  //int nOrder_;
  //int nSize_;
  //String sSrc_;
  Buffer sbData_;
  int bDataAvailable_;
  String sTrust_;
};

#endif // Item_H_INCLUDED
