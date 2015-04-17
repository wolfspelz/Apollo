// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "XMLProcessor.h"
#include "MsgIdentity.h"
#include "Container.h"

int Container::set(const String& sData, const String& sDigest)
{
  int ok = 1;

  sData_ = sData;
  sDigest_ = sDigest;

  XMLProcessor* pXml = new XMLProcessor();
  if (pXml != 0) {

    ok = pXml->XmlText(sData_);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "xml.XmlText() failed err=%s, xml=%s", _sz(pXml->GetErrorString()), _sz(String::truncate(sData_, 100))));
    } else {
      bValid_ = 1;

      KeyValueList vlPreviousIds;
      (void) getItemIds(vlPreviousIds);

      XMLNode* pRoot = pXml->Root();
      if (pRoot != 0) {

        { // Async to that the data is already in the cache for requests by msg receivers
          ApAsyncMessage<Msg_Identity_ContainerBegin> msg;
          msg->sUrl = sUrl_;
          msg.Post();
        }

        for (XMLNode* pNode = 0; (pNode = pRoot->nextChild(pNode, "item")) != 0; ) {

          int bItemOk = 0;
          String sId = pNode->getAttribute("id").getValue();
          String sPreviousDigest;

          int bItemIsNew = 0;
          Item* pItem = 0;
          if (items_.Get(sId, pItem)) {
            sPreviousDigest = pItem->getAttribute("digest");
          } else {
            pItem = new Item(this, sId);
            bItemIsNew = 1;
          }

          if (pItem != 0) {
            bItemOk = pItem->parseXmlNode(pNode);
            if (!bItemOk) {
              apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "pItem->parseXmlNode() failed url=%s id=%s", _sz(sUrl_), _sz(sId)));
            }
          }

          if (bItemOk) {
            items_.Set(sId, pItem);

            if (pItem->getAttribute("type") == Msg_Identity_ItemType_Properties) {
              bPropertiesEvaluated_ = 0;
            }

            // Remove item id from list of previous ids
            KeyValueElem* e = vlPreviousIds.findString(sId);
            if (e != 0) {
              vlPreviousIds.removeElem(e);
              delete e;
            }

            if (bItemIsNew) {
              // Async to that the data is already in the cache for requests by msg receivers
              ApAsyncMessage<Msg_Identity_ItemAdded> msg;
              msg->sUrl = sUrl_;
              msg->sId = sId;
              msg.Post();
            } else {
              if (sPreviousDigest != pItem->getAttribute("digest")) {
                // Async to that the data is already in the cache for requests by msg receivers
                ApAsyncMessage<Msg_Identity_ItemChanged> msg;
                msg->sUrl = sUrl_;
                msg->sId = sId;
                msg.Post();
              }
            }

          } else {
            delete pItem;
            pItem = 0;
          }

        } // for XML nodes

        // Whats now in vlPreviousIds is not present in the new XML
        for (KeyValueElem* e = 0; (e = vlPreviousIds.nextElem(e)) != 0; ) {
          { // Async to that the data is already in the cache for requests by msg receivers
            ApAsyncMessage<Msg_Identity_ItemRemoved> msg;
            msg->sUrl = sUrl_;
            msg->sId = e->getString();
            msg.Post();
          }

          Item* pItem = 0;
          if (items_.Get(e->getString(), pItem)) {
            if (items_.Unset(e->getString())) {
              delete pItem;
              pItem = 0;
            }
          }
        }

        { // Async to that the data is already in the cache for requests by msg receivers
          ApAsyncMessage<Msg_Identity_ContainerEnd> msg;
          msg->sUrl = sUrl_;
          msg.Post();
        }

      } // if root node

    } // parse ok

    delete pXml;
    pXml = 0;
  } 

  bParsed_ = 1;

  return ok;
}

int Container::evaluateProperties()
{
  int ok = 0;

  if (bValid_) {
    Item* pItem = findItemByType(Msg_Identity_ItemType_Properties);
    if (pItem != 0) {
      if (pItem->hasData()) {
        ok = 1;

        String sProperties;
        pItem->getData(sProperties);

        int bPlain = 0;
        if (pItem->getAttribute("encoding") == "plain") {
          bPlain = 1;
        }

        if (bPlain) {
          lProperties_.Empty();
          KeyValueLfBlob2List(sProperties, lProperties_);
          ok = 1;
          bPropertiesEvaluated_ = 1;
        } else {
          // xml
          //ok = parse;
          bPropertiesEvaluated_ = 1;
        }
      }
    }
  }

  return ok;
}

Item* Container::findItemByType(const String& sType)
{
  Item* pResult = 0;

  List lPotetialIds;

  ItemIterator iter(items_); ItemNode* pNode = 0;
  while (pNode = iter.Next()) {
    Item* pItem = pNode->Value();
    if (pItem != 0) {
      if (pItem->getAttribute("type") == sType) {
        Elem* e = lPotetialIds.AddLast(pItem->getId());
        if (e != 0) {
          e->setInt(String::atoi(pItem->getAttribute("order")));
        }
      }
    }
  }

  if (lPotetialIds.length() > 0) {
    int nLowestOrder = -1;
    String sId;
    for (Elem* e = 0; (e = lPotetialIds.Next(e)) != 0; ) {
      if (nLowestOrder < 0 || e->getInt() < nLowestOrder) {
        nLowestOrder = e->getInt();
        sId = e->getName();
      }
    }

    if (!sId.empty()) {
      items_.Get(sId, pResult);
    }
  }

  return pResult;
}

void Container::getItemDataFromXmlNode(XMLNode* pItem, String& sMimeType, Buffer& sbData)
{
  String sCData = pItem->getCData();
  if (!sCData.empty()) {
    if (pItem->getAttribute("encoding").getValue() == "base64") {
      sbData.decodeBase64(sCData);
    } else {
      sMimeType = pItem->getAttribute("mimetype").getValue();
      sbData.SetData(sCData.c_str(), sCData.bytes());
    }
  }
}

//----------------------------

int Container::hasProperty(const String& sKey)
{
  int bIsProperty = 0;

  if (!bPropertiesEvaluated_) {
    evaluateProperties();
  }

  if (lProperties_.FindByName(sKey)) {
    bIsProperty = 1;
  }

  return bIsProperty;
}

String Container::getProperty(const String& sKey)
{
  String sValue;

  if (!bPropertiesEvaluated_) {
    evaluateProperties();
  }

  sValue = lProperties_[sKey].getString();

  return sValue;
}

int Container::getItemIds(const String& sType, Apollo::ValueList& vlMimeTypes, int nMax, ValueList& vlIds)
{
  int ok = 1;

  List lCandidates;

  if (bValid_) {
    ItemIterator iter(items_); ItemNode* pNode = 0;
    while (pNode = iter.Next()) {
      Item* pItem = pNode->Value();
      if (pItem != 0) {

        int bUseIt = 1;
        int nScore = 1000 * String::atoi(pItem->getAttribute("order"));

        if (!sType.empty()) {
          bUseIt = 0;
          if (pItem->getAttribute("type") == sType) {
            bUseIt = 1;
          }
        }

        if (bUseIt) {
          if (vlMimeTypes.length() > 0) {
            if (pItem->getAttribute("mimetype").empty()) {
              nScore += 1;
            } else {
              if (vlMimeTypes.findString(pItem->getAttribute("mimetype")) != 0) {
                nScore -= 1;
              } else {
                bUseIt = 0;
              }
            }
          }
        }

        if (bUseIt) {
          if (nMax == 0) {
            vlIds.add(pItem->getId());
          } else {
            Elem* e = new Elem(pItem->getId(), nScore);
            if (e != 0) {
              lCandidates.AddLast(e);
            }
          }
        }

      }

    }
  }

  if (nMax != 0) {
    int bDone = 0;

    while (!bDone) {
      int nScore = -1;
      Elem* eSelect = 0;
      for (Elem* e = 0; (e = lCandidates.Next(e)) != 0; ) {
        if (nScore < 0 || e->getInt() < nScore) {
          nScore = e->getInt();
          eSelect = e;
        }
      }

      if (eSelect != 0) {
        vlIds.add(eSelect->getName());

        lCandidates.Remove(eSelect);
        delete eSelect;
        eSelect = 0;
      }

      if (lCandidates.length() == 0) { bDone = 1; }
      if (vlIds.length() >= nMax) { bDone = 1; }
    }

  }

  return ok;
}

int Container::getItemIds(ValueList& vlIds)
{
  int ok = 1;

  if (bValid_) {
    ItemIterator iter(items_); ItemNode* pNode = 0;
    while (pNode = iter.Next()) {
      Item* pItem = pNode->Value();
      if (pItem != 0) {
        vlIds.add(pItem->getId());
      }
    }
  }

  return ok;
}

int Container::getItem(const String& sId, List& lAttributes)
{
  int ok = 0;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      (void) pItem->getAttributes(lAttributes);
      ok = 1;
    }
  }

  return ok;
}

int Container::hasItem(const String& sId)
{
  int bHas = 0;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      bHas = 1;
    }
  }

  return bHas;
}

String Container::getItemAttribute(const String& sId, const String& sKey)
{
  String sAttribute;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      sAttribute = pItem->getAttribute(sKey);
    }
  }

  return sAttribute;
}

int Container::hasItemData(const String& sId)
{
  int bHas = 0;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      bHas = pItem->hasData();
    }
  }

  return bHas;
}

int Container::getItemData(const String& sId, String& sMimeType, Buffer& sbData)
{
  int ok = 0;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      if (pItem->hasData()) {
        sMimeType = pItem->getAttribute("mimetype");
        ok = pItem->getData(sbData);
      }
    }
  }

  return ok;
}

int Container::setItemData(const String& sId, const String& sMimeType, Buffer& sbData)
{
  int ok = 0;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      ok = 1;
      pItem->setData(sMimeType, sbData);
    }
  }

  return ok;
}

int Container::deleteItemData(const String& sId)
{
  int ok = 0;

  if (bValid_) {
    Item* pItem = 0;
    if (items_.Get(sId, pItem)) {
      ok = pItem->deleteData();
    }
  }

  return ok;
}
