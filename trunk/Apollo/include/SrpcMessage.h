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

AP_NAMESPACE_BEGIN

class APOLLO_API SrpcMessage: public List
{
public:
  SrpcMessage();
  virtual ~SrpcMessage();

  void set(const String& sKey, const String& sValue);
  void setString(const String& sKey, const String& sValue);
  void setInt(const String& sKey, int nValue);
  void setList(const String& sKey, ValueList& vlList);
  void setList(const String& sKey, KeyValueList& kvList);

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

protected:
  void moveTo(SrpcMessage& lTarget);
};

AP_NAMESPACE_END

#endif // !defined(SrpcMessage_h_INCLUDED)
