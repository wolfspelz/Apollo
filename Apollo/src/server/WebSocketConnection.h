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
  {}
  virtual ~WebSocketConnection() {}

#if defined(AP_TEST)
  static String test_KeyString2Buffer1(const String& sKey, const String& sExpected);
  static String test_KeyString2Buffer();
#endif

protected:
  int OnConnected();
  int OnDataIn(unsigned char* pData, size_t nLen);
  int OnClosed();
  
  static void KeyString2Buffer(const String& sKey, Buffer& sbResult);

protected:
  HttpParser parser_;
};

#endif // !defined(WebSocketConnection_h_INCLUDED)
