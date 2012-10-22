// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SrpcMessage_h_INCLUDED)
#define SrpcMessage_h_INCLUDED

#include "SSystem.h"
#include "ApTypes.h"
#include "ApExports.h"

class APOLLO_API Srpc
{
public:
  class APOLLO_API Key
  {
  public:
    static String Method;
    static String Status;
    static String Message;
    static String SrpcId;
  };
};

AP_NAMESPACE_BEGIN

class APOLLO_API SrpcMessage: public List
{
public:
  SrpcMessage();
  virtual ~SrpcMessage();

  void setUnescaped(const String& sKey, const String& sValue);
  //void setString(const String& sKey, const String& sValue);
  //void setInt(const String& sKey, int nValue);
  //void setList(const String& sKey, ValueList& vlList);
  //void setList(const String& sKey, KeyValueList& kvList);
  void set(const String& sKey, const String& sValue);
  void set(const String& sKey, int nValue);
  void set(const String& sKey, const ApHandle& hValue);
  void set(const String& sKey, ValueList& vlList);
  void set(const String& sKey, KeyValueList& kvList);

  String getString(const String& sKey);
  int getInt(const String& sKey);
  ApHandle getHandle(const String& sKey);
  void getValueList(const String& sKey, ValueList& vlList);
  void getKeyValueList(const String& sKey, KeyValueList& kvList);

  // Move items from this instance to the target list
  void operator>>(SrpcMessage& target) { moveTo(target); }

  void createResponse(SrpcMessage& request);
  void createError(SrpcMessage& request, const String& sMessage);
  String toString();
  void fromString(const String& s);
  void cloneTo(SrpcMessage& target);

protected:
  void moveTo(SrpcMessage& target);
};

AP_NAMESPACE_END

#endif // !defined(SrpcMessage_h_INCLUDED)
