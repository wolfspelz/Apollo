// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebSocketConnection_h_INCLUDED)
#define WebSocketConnection_h_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"
#include "HttpParser.h"

class WebSocketConnection: public Apollo::TCPConnection
{
public:
  WebSocketConnection(const char* szName, const ApHandle& hWebSocketConnection)
    :Apollo::TCPConnection(szName, hWebSocketConnection)
    ,nWebSocketVersion_(NoVersion)
  {}
  virtual ~WebSocketConnection() {}

  int SendWebSocketMessage(unsigned char* pData, size_t nLen);

#if defined(AP_TEST)
  static String test_KeyString2Buffer1(const String& sKey, const String& sExpected);
  static String test_KeyString2Buffer();
  static String test_Create_ChromePreStandard_Response1(const String& sKey1, const String& sKey2, const String& sKey3AsHex, const String& sExpectedAsHex);
  static String test_Create_ChromePreStandard_Response();
  static String test_Create_Standard_Response1(const String& sKey, const String& sExpected);
  static String test_Create_Standard_Response();
#endif

protected:
  int OnConnected();
  int OnDataIn(unsigned char* pData, size_t nLen);
  int OnClosed();
  
  void ReceiveWebSocketData(unsigned char* pData, size_t nLen);
  void ReceiveWebSocketData_ChromePreStandard(unsigned char* pData, size_t nLen);
  void ReceiveWebSocketData_Standard(unsigned char* pData, size_t nLen);
  void ReceiveWebSocketMessage(unsigned char* pData, size_t nLen);
  int SendWebSocketData_ChromePreStandard(unsigned char* pData, size_t nLen);
  int SendWebSocketData_Standard(unsigned char* pData, size_t nLen);

  static void KeyString2Buffer(const String& sKey, Buffer& sbResult);
  static void Create_ChromePreStandard_Response(const String& sKey1, const String& sKey2, Buffer& sbKey3, Buffer& sbResponse);
  static void Create_Standard_Response(const String& sKey, String& sResponse);

protected:
  HttpParser httpParser_;

  List lHeaders_;
  enum { NoVersion, ChromePreStandard, Standard };
  int nWebSocketVersion_;
  Buffer data_;
};

#endif // !defined(WebSocketConnection_h_INCLUDED)
