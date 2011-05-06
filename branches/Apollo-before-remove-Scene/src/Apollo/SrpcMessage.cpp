// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApCompiler.h"
#include "SrpcMessage.h"

AP_NAMESPACE_BEGIN

SrpcMessage::SrpcMessage()
{
}

SrpcMessage::~SrpcMessage()
{
}

void SrpcMessage::set(const String& sKey, const String& sValue)
{
  AddLast(sKey, sValue);
}

void SrpcMessage::setString(const String& sKey, const String& sValue)
{
  String sEscapedValue = sValue;
  sEscapedValue.escape(String::EscapeCRLF);
  set(sKey, sEscapedValue);
}

void SrpcMessage::setInt(const String& sKey, int nValue)
{
  set(sKey, String::from(nValue));
}

void SrpcMessage::setList(const String& sKey, ValueList& vlList)
{
  setString(sKey, vlList.toString());
}

void SrpcMessage::setList(const String& sKey, KeyValueList& kvList)
{
  setString(sKey, kvList.toString());
/*
  SrpcMessage srpc;

  for (KeyValueElem* e = 0; (e = kvList.nextElem(e)) != 0; ) {
    switch (e->getType()) {
      case KeyValueElem::TypeInt:
        srpc.setInt(e->getKey(), e->getInt());
        break;
      case KeyValueElem::TypeString:
        srpc.setString(e->getKey(), e->getString());
        break;
      case KeyValueElem::TypeHandle:
        srpc.setString(e->getKey(), e->getHandle().toString());
        break;
    }
  }

  String sValue = srpc.toString();

  setString(sKey, sValue);
*/
}

String SrpcMessage::toString()
{
  String sMsg;

  for (Elem* e = 0; (e = Next(e)) != 0; ) {
    sMsg.appendf("%s=%s\n", StringType(e->getName()), StringType(e->getString()));
  }

  return sMsg;
}

void SrpcMessage::fromString(const String& s)
{
  KeyValueBlob2List(s, *this, "\r\n", "=", ""); 
}

String SrpcMessage::getString(const String& sKey)
{
  String sValue;
  Elem* e = FindByName(sKey);
  if (e != 0) {
    sValue = e->getString();
    sValue.unescape(String::EscapeCRLF);
  }
  return sValue;
}

int SrpcMessage::getInt(const String& sKey)
{
  int nValue = 0;
  Elem* e = FindByName(sKey);
  if (e != 0) {
    String sValue = e->getString();
    nValue = String::atoi(sValue);
  }
  return nValue;
}

ApHandle SrpcMessage::getHandle(const String& sKey)
{
  ApHandle hValue;
  Elem* e = FindByName(sKey);
  if (e != 0) {
    hValue.fromString(e->getString());
  }
  return hValue;
}

void SrpcMessage::getValueList(const String& sKey, ValueList& vlList)
{
  String sTokenizer = getString(sKey);

  String sToken;
  while (sTokenizer.nextToken(" ", sToken)) {
    vlList.add(sToken);
  }
}

void SrpcMessage::getKeyValueList(const String& sKey, KeyValueList& kvList)
{
  String sList = getString(sKey);
  List l;
  KeyValueLfBlob2List(sList, l);
  kvList.fromList(l);
}

void SrpcMessage::moveTo(SrpcMessage& target)
{
  Elem* e = 0;
  while ((e = First()) != 0) {
    Remove(e);
    target.AddLast(e);
  }
}

void SrpcMessage::createResponse(SrpcMessage& request)
{
  setInt("Status", 1);
  String sSrpcId = request.getString("SrpcId");
  if (!sSrpcId.empty()) {
    setString("SrpcId", sSrpcId);
  }
}

void SrpcMessage::createError(SrpcMessage& request, const String& sMessage)
{
  setInt("Status", 0);
  String sSrpcId = request.getString("SrpcId");
  if (!sSrpcId.empty()) {
    setString("SrpcId", sSrpcId);
  }
  setString("Message", sMessage);
}

AP_NAMESPACE_END
