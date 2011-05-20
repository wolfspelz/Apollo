// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Participant_H_INCLUDED)
#define Participant_H_INCLUDED

#include "ApContainer.h"
#include "Thingy.h"

//------------------------------------------------------------

class Participant;
class ParticipantThingyProvider;

class ParticipantThingy: public Thingy
{
public:
  ParticipantThingyProvider* getProvider() { return (ParticipantThingyProvider*) pProvider_; }
  void setProvider(ParticipantThingyProvider* pProvider) { pProvider_ = (ThingyProvider*) pProvider; pProvider_->setOwner(this); }
};

//------------------------------

class ParticipantThingyProvider: public ThingyProvider
{
public:
  ParticipantThingyProvider(): nVariant_(Variant_Unknown) {}
  void setParticipant(Participant* pParticipant) { pParticipant_ = pParticipant; }

  typedef enum _Variant { 
     Variant_Unknown
    ,Variant_String
    ,Variant_Nickname
    ,Variant_KeyValue
    ,Variant_IdentityProperty
    ,Variant_IdentityItem
  } Variant;

  Variant getVariant() { return nVariant_; }

  virtual int hasData() = 0;

  virtual int isIdentityBased() { return 0; }
  virtual int isPropertyBased() { return 0; }
  virtual int isBasedOnItemType(const String& sType) { return 0; }

protected:
  Participant* pParticipant_;
  Variant nVariant_;
};

//------------------------------

class ParticipantThingyListBase: public ThingyList<ParticipantThingy>
{
public:
  void setParticipant(Participant* pParticipant) { pParticipant_ = pParticipant; }

protected:
  Participant* pParticipant_;
};

class ParticipantThingyList: public ParticipantThingyListBase
{
public:
  void sendChanges(Apollo::ValueList& vlChanges);

  ParticipantThingyProvider::Variant getVariantByKey(const String& sKey);
  ThingyProvider* newProvider(const String& sKey);
};

//------------------------------

class UnknownParticipantThingyProvider: public ParticipantThingyProvider
{
public:
  virtual int hasData() { return 0; }
  virtual void getString(String& sValue, String& sMimeType) {} // Do nothing, just to be able to instantiate
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // Do nothing, just to be able to instantiate
};

//------------------------------

class IdentityUrlParticipantThingyProvider: public ParticipantThingyProvider
{
public:
  virtual void getString(String& sValue, String& sMimeType);
};

//------------------------------

class StringParticipantThingyProvider: public ParticipantThingyProvider
{
public:
  StringParticipantThingyProvider(): bHasData_(0) { nVariant_ = Variant_String; }
  virtual int hasData();
  virtual void setData(const String& sValue);
  virtual void getString(String& sValue, String& sMimeType);
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // shut up, string only

protected:
  int bHasData_;
  String sValue_;
};

//------------------------------

class KeyValueParticipantThingyProvider: public ParticipantThingyProvider
{
public:
  KeyValueParticipantThingyProvider(): bHasData_(0) { nVariant_ = Variant_KeyValue; }
  virtual int hasData();
  virtual void setData(Apollo::KeyValueList& kvList);
  virtual void getString(String& sValue, String& sMimeType);
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // shut up, string only

protected:
  int bHasData_;
  Apollo::KeyValueList kvList_;
};

//------------------------------

class IdentityItemParticipantThingyProvider: public ParticipantThingyProvider
{
public:
  IdentityItemParticipantThingyProvider() { nVariant_ = Variant_IdentityItem; }
  virtual int isIdentityBased() { return 1; }
  virtual int isBasedOnItemType(const String& sType) { return sType == getItemType(); }
  virtual String getItemType();
  virtual int hasData();
  virtual void getString(String& sValue, String& sMimeType);
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource);
  virtual void getDataRef(String& sUrl, String& sMimeType);
  virtual void onSubscribe();

protected:
  virtual void getDataCore(Buffer& sbData, String& sMimeType, String& sLocalUrl, String& sOriginalUrl);
};

//------------------------------

class IdentityPropertyParticipantThingyProvider: public IdentityItemParticipantThingyProvider
{
public:
  IdentityPropertyParticipantThingyProvider() { nVariant_ = Variant_IdentityProperty; }
  virtual int isPropertyBased() { return 1; }
  virtual String getItemType();
  virtual void getString(String& sValue, String& sMimeType);
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // shut up, string only

  void setReferenceData(const String& sValue) { sReferenceValue_ = sValue; }
  String getReferenceData() { return sReferenceValue_; }

protected:
  void getIdentityProperty(const String& sKey, String& sValue);

protected:
  String sReferenceValue_;
};

//------------------------------

class NicknameParticipantThingyProvider: public IdentityPropertyParticipantThingyProvider
{
  typedef IdentityPropertyParticipantThingyProvider base;
public:
  NicknameParticipantThingyProvider() { nVariant_ = Variant_Nickname; }
  virtual int hasData();
  virtual void getString(String& sValue, String& sMimeType);
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // shut up, string only
  virtual void setProtocolNickname(const String& sNickname);

protected:
  String sProtocolNickname_;
};

//------------------------------------------------------------

class Location;

class Participant
{
public:
  Participant(const ApHandle& hParticipant, Location* pLocation);

  inline ApHandle apHandle() { return hAp_; }
  inline Location* getLocation() { return pLocation_; }

  int isDetailAvailable(const String& sKey, Apollo::ValueList& vlMimeTypes);
  int getDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes, String& sValue, String& sMimeType);
  int getDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes, Buffer& sbData, String& sMimeType, String& sSource);
  int getDetailRef(const String& sKey, Apollo::ValueList& vlMimeTypes, String& sUrl, String& sMimeType);

  int subscribeDetail(const String& sKey, Apollo::ValueList& vlMimeTypes);
  int unsubscribeDetail(const String& sKey, Apollo::ValueList& vlMimeTypes);
  int isSubscribedDetail(const String& sKey);
  int addSubscription(const String& sKey, Apollo::ValueList& vlMimeTypes);
  int removeSubscription(const String& sKey, Apollo::ValueList& vlMimeTypes);

  void onProtocolDataBegin();
  void onProtocolNickname(const String& sNickname);
  void onProtocolStatus(const String& sStatus);
  void onProtocolMessage(const String& sMessage);
  void onProtocolJabberId(const String& sJabberId);
  void onProtocolMucUser(const String& sJabberId, const String& sAffiliation, const String& sRole);
  void onProtocolIdentity(const String& sId, const String& sSrc, const String& sDigest);
  void onProtocolPosition(Apollo::KeyValueList& kvParams);
  void onProtocolCondition(Apollo::KeyValueList& kvParams);
  void onProtocolFirebatFeatures(const String& sFeatures);
  void onProtocolDataEnd();

  void onIdentityContainerBegin(const String& sUrl);
  void onIdentityItemAdded(const String& sUrl, const String& sId);
  void onIdentityItemChanged(const String& sUrl, const String& sId);
  void onIdentityItemRemoved(const String& sUrl, const String& sId);
  void onIdentityContainerEnd(const String& sUrl);

  void onIdentityItemDataAvailable(const String& sUrl, const String& sId, const String& sType, const String& sMimeType, const String& sSrc, int nSize);

  int requestIdentityContainer();
  int requestIdentityItem(const String sId);

  int hasItemDataById(const String& sId);
  int hasItemDataByType(const String& sType);

  int needItemDataByType(const String& sType);
  int acquireItemDataByType(const String& sType);

  String& getIdentitySrc() { return sIdentitySrc_; }
  String& getIdentityDigest() { return sIdentityDigest_; }

protected:
  void adjustThingyVariants();

protected:
  String sName_; // for debugging
  ApHandle hAp_;
  Location* pLocation_;

  List lNeededTypes_;

  String sIdentityId_;
  String sIdentitySrc_;
  String sIdentityDigest_;

  ParticipantThingyList lThingys_;
};

#endif // Participant_H_INCLUDED
