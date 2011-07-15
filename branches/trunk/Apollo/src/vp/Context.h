// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Context_H_INCLUDED)
#define Context_H_INCLUDED

#include "Thingy.h"

//------------------------------------------------------------

class Context;
class ContextThingyProvider;

class ContextThingy: public Thingy
{
public:
  ContextThingyProvider* getProvider() { return (ContextThingyProvider*) pProvider_; }
};

class ContextThingyListBase: public ThingyList<ContextThingy>
{
public:
  void setContext(Context* pContext) { pContext_ = pContext; }

protected:
  Context* pContext_;
};

class ContextThingyList: public ContextThingyListBase
{
public:
  ThingyProvider* newProvider(const String& sKey);
  void sendChanges(Apollo::ValueList& vlChanges);
};

//------------------------------

class ContextThingyProvider: public ThingyProvider
{
public:
  void setContext(Context* pContext) { pContext_ = pContext; }
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // shut up, string only

protected:
  Context* pContext_;
};

//------------------------------

class DocumentUrlContextThingyProvider: public ContextThingyProvider
{
public:
  void getString(String& sValue, String& sMimeType);
};

//------------------------------

class LocationUrlContextThingyProvider: public ContextThingyProvider
{
public:
  void getString(String& sValue, String& sMimeType);
};

//------------------------------

class BoolContextThingyProvider: public ContextThingyProvider
{
public:
  BoolContextThingyProvider(): bHasData_(0), bValue_(0) {}
  virtual int hasData() { return bHasData_; }
  virtual void setData(int bValue);
  virtual void getString(String& sValue, String& sMimeType);

protected:
  int bHasData_;
  int bValue_;
};

//------------------------------

class VisibilityContextThingyProvider: public BoolContextThingyProvider
{
public:
  void getString(String& sValue, String& sMimeType);
};

//------------------------------

class KeyValueContextThingyProvider: public ContextThingyProvider
{
public:
  KeyValueContextThingyProvider(): bHasData_(0) {}
  virtual int hasData() { return bHasData_; }
  virtual void setData(Apollo::KeyValueList& kvList);
  virtual void getString(String& sValue, String& sMimeType);

protected:
  int bHasData_;
  Apollo::KeyValueList kvList_;
};

//------------------------------------------------------------

class Context
{
public:
  Context(const ApHandle& hContext);
  virtual ~Context();

  int navigate(const String& sUrl);
  int setLocationXml(const String& sLocationXml);

  inline ApHandle apHandle() { return hAp_; }
  inline String& getDocumentUrl() { return sDocumentUrl_; }
  inline String& getLocationUrl() { return sLocationUrl_; }
  inline ApHandle getMapping() { return hMapping_; }

  void setVisibility(int bVisible);
  void setPosition(int nLeft, int nBottom);
  void setSize(int nWidth, int nHeight);

  void getDetailString(const String& sKey, String& sValue, String& sMimeType);
  int subscribeDetail(const String& sKey, String& sValue, String& sMimeType);
  int unsubscribeDetail(const String& sKey);
  int addSubscription(const String& sKey);
  int removeSubscription(const String& sKey);

protected:
  int resolve();
  int resolved();
  int sameLocation(const String& sLocationUrl);
  int changeLocation(const String& sLocationUrl);

protected:
  ApHandle hAp_;
  String sDocumentUrl_;
  String sLocationXml_;
  String sLocationUrl_;
  ApHandle hMapping_;
  time_t tCreated_;

  ContextThingyList lThingys_;
};

#endif // Context_H_INCLUDED
