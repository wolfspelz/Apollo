// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ServerModule.h"
#include "WebSocketConnection.h"
#include "MessageDigest.h"

int WebSocketConnection::OnConnected()
{
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " opened", ApHandlePrintf(hAp_)));

  return 1;
}

void WebSocketConnection::KeyString2Buffer(const String& sKey, Buffer& sbResult)
{
  int nChars = sKey.chars();
  int nBlanks = 0;
  String sNumber;
  for (int i = 0; i < nChars; ++i) {
    String sChar = sKey.subString(i, 1);
    String::UTF8Char c = String::UTF8_Char(sChar);
    if (c == ' ') {
      nBlanks++;
    } else if (c >= '0' && c <= '9') {
      sNumber += sChar;
    }
  }

  char* pNext = 0;
  unsigned long nNumber = strtoul(sNumber, &pNext, 10);
  unsigned long nKey = nNumber / nBlanks;
  for (int i = 3; i >= 0; --i) {
    unsigned char nPart = 0xff & (nKey >> 8*i);
    sbResult.Append(&nPart, 1);
  }
}

// Real data:
//
// -> GET / HTTP/1.1
// -> Upgrade: WebSocket
// -> Connection: Upgrade
// -> Host: echo.websocket.org
// -> Origin: http://websocket.org
// -> Sec-WebSocket-Key1: 527z5   [| 2 @1     ? ,566
// -> Sec-WebSocket-Key2: 2  0 l 542 6  9    6 8
// -> Cookie: __utma=9925811.2132841395.1312270503.1312270503.1312270503.1; __utmb=9925811.5.10.1312270503; __utmc=9925811; __utmz=9925811.1312270503.1.1.utmcsr=google|utmccn=(organic)|utmcmd=organic|utmctr=websockets
// -> 
// -> g..P..)%
// <- HTTP/1.1 101 Web Socket Protocol Handshake
// <- Upgrade: WebSocket
// <- Connection: Upgrade
// <- Sec-WebSocket-Origin: http://websocket.org
// <- Sec-WebSocket-Location: ws://echo.websocket.org/
// <- Server: Kaazing Gateway
// <- Date: Tue, 02 Aug 2011 08:03:08 GMT
// <- Access-Control-Allow-Origin: http://websocket.org
// <- Access-Control-Allow-Credentials: true
// <- Access-Control-Allow-Headers: content-type
// <- 
// <- F.gA2.v.N...e...
// -> .Rock it with HTML5 WebSocket.
// <- .Rock it with HTML5 WebSocket.
//
// 00000000  47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a GET / HT TP/1.1..
// 00000010  55 70 67 72 61 64 65 3a  20 57 65 62 53 6f 63 6b Upgrade:  WebSock
// 00000020  65 74 0d 0a 43 6f 6e 6e  65 63 74 69 6f 6e 3a 20 et..Conn ection: 
// 00000030  55 70 67 72 61 64 65 0d  0a 48 6f 73 74 3a 20 65 Upgrade. .Host: e
// 00000040  63 68 6f 2e 77 65 62 73  6f 63 6b 65 74 2e 6f 72 cho.webs ocket.or
// 00000050  67 0d 0a 4f 72 69 67 69  6e 3a 20 68 74 74 70 3a g..Origi n: http:
// 00000060  2f 2f 77 65 62 73 6f 63  6b 65 74 2e 6f 72 67 0d //websoc ket.org.
// 00000070  0a 53 65 63 2d 57 65 62  53 6f 63 6b 65 74 2d 4b .Sec-Web Socket-K
// 00000080  65 79 31 3a 20 35 32 37  7a 35 20 20 20 5b 7c 20 ey1: 527 z5   [| 
// 00000090  32 20 40 31 20 20 20 20  20 3f 20 2c 35 36 36 0d 2 @1      ? ,566.
// 000000A0  0a 53 65 63 2d 57 65 62  53 6f 63 6b 65 74 2d 4b .Sec-Web Socket-K
// 000000B0  65 79 32 3a 20 32 20 20  30 20 6c 20 35 34 32 20 ey2: 2   0 l 542 
// 000000C0  36 20 20 39 20 20 20 20  36 20 38 0d 0a 43 6f 6f 6  9     6 8..Coo
// 000000D0  6b 69 65 3a 20 5f 5f 75  74 6d 61 3d 39 39 32 35 kie: __u tma=9925
// 000000E0  38 31 31 2e 32 31 33 32  38 34 31 33 39 35 2e 31 811.2132 841395.1
// 000000F0  33 31 32 32 37 30 35 30  33 2e 31 33 31 32 32 37 31227050 3.131227
// 00000100  30 35 30 33 2e 31 33 31  32 32 37 30 35 30 33 2e 0503.131 2270503.
// 00000110  31 3b 20 5f 5f 75 74 6d  62 3d 39 39 32 35 38 31 1; __utm b=992581
// 00000120  31 2e 35 2e 31 30 2e 31  33 31 32 32 37 30 35 30 1.5.10.1 31227050
// 00000130  33 3b 20 5f 5f 75 74 6d  63 3d 39 39 32 35 38 31 3; __utm c=992581
// 00000140  31 3b 20 5f 5f 75 74 6d  7a 3d 39 39 32 35 38 31 1; __utm z=992581
// 00000150  31 2e 31 33 31 32 32 37  30 35 30 33 2e 31 2e 31 1.131227 0503.1.1
// 00000160  2e 75 74 6d 63 73 72 3d  67 6f 6f 67 6c 65 7c 75 .utmcsr= google|u
// 00000170  74 6d 63 63 6e 3d 28 6f  72 67 61 6e 69 63 29 7c tmccn=(o rganic)|
// 00000180  75 74 6d 63 6d 64 3d 6f  72 67 61 6e 69 63 7c 75 utmcmd=o rganic|u
// 00000190  74 6d 63 74 72 3d 77 65  62 73 6f 63 6b 65 74 73 tmctr=we bsockets
// 000001A0  0d 0a 0d 0a 67 ec c1 50  84 ff 29 25             ....g..P ..)%
//     00000000  48 54 54 50 2f 31 2e 31  20 31 30 31 20 57 65 62 HTTP/1.1  101 Web
//     00000010  20 53 6f 63 6b 65 74 20  50 72 6f 74 6f 63 6f 6c  Socket  Protocol
//     00000020  20 48 61 6e 64 73 68 61  6b 65 0d 0a 55 70 67 72  Handsha ke..Upgr
//     00000030  61 64 65 3a 20 57 65 62  53 6f 63 6b 65 74 0d 0a ade: Web Socket..
//     00000040  43 6f 6e 6e 65 63 74 69  6f 6e 3a 20 55 70 67 72 Connecti on: Upgr
//     00000050  61 64 65 0d 0a 53 65 63  2d 57 65 62 53 6f 63 6b ade..Sec -WebSock
//     00000060  65 74 2d 4f 72 69 67 69  6e 3a 20 68 74 74 70 3a et-Origi n: http:
//     00000070  2f 2f 77 65 62 73 6f 63  6b 65 74 2e 6f 72 67 0d //websoc ket.org.
//     00000080  0a 53 65 63 2d 57 65 62  53 6f 63 6b 65 74 2d 4c .Sec-Web Socket-L
//     00000090  6f 63 61 74 69 6f 6e 3a  20 77 73 3a 2f 2f 65 63 ocation:  ws://ec
//     000000A0  68 6f 2e 77 65 62 73 6f  63 6b 65 74 2e 6f 72 67 ho.webso cket.org
//     000000B0  2f 0d 0a 53 65 72 76 65  72 3a 20 4b 61 61 7a 69 /..Serve r: Kaazi
//     000000C0  6e 67 20 47 61 74 65 77  61 79 0d 0a 44 61 74 65 ng Gatew ay..Date
//     000000D0  3a 20 54 75 65 2c 20 30  32 20 41 75 67 20 32 30 : Tue, 0 2 Aug 20
//     000000E0  31 31 20 30 38 3a 30 33  3a 30 38 20 47 4d 54 0d 11 08:03 :08 GMT.
//     000000F0  0a 41 63 63 65 73 73 2d  43 6f 6e 74 72 6f 6c 2d .Access- Control-
//     00000100  41 6c 6c 6f 77 2d 4f 72  69 67 69 6e 3a 20 68 74 Allow-Or igin: ht
//     00000110  74 70 3a 2f 2f 77 65 62  73 6f 63 6b 65 74 2e 6f tp://web socket.o
//     00000120  72 67 0d 0a 41 63 63 65  73 73 2d 43 6f 6e 74 72 rg..Acce ss-Contr
//     00000130  6f 6c 2d 41 6c 6c 6f 77  2d 43 72 65 64 65 6e 74 ol-Allow -Credent
//     00000140  69 61 6c 73 3a 20 74 72  75 65 0d 0a 41 63 63 65 ials: tr ue..Acce
//     00000150  73 73 2d 43 6f 6e 74 72  6f 6c 2d 41 6c 6c 6f 77 ss-Contr ol-Allow
//     00000160  2d 48 65 61 64 65 72 73  3a 20 63 6f 6e 74 65 6e -Headers : conten
//     00000170  74 2d 74 79 70 65 0d 0a  0d 0a                   t-type.. ..
//     0000017A  46 f2 67 41 32 ca 76 c9  4e ba e9 a9 65 88 ad ec F.gA2.v. N...e...
//  000001AC  00 52 6f 63 6b 20 69 74  20 77 69 74 68 20 48 54 .Rock it  with HT
//  000001BC  4d 4c 35 20 57 65 62 53  6f 63 6b 65 74 ff       ML5 WebS ocket.
//     0000018A  00 52 6f 63 6b 20 69 74  20 77 69 74 68 20 48 54 .Rock it  with HT
//     0000019A  4d 4c 35 20 57 65 62 53  6f 63 6b 65 74 ff       ML5 WebS ocket.

void WebSocketConnection::Create_ChromePreStandard_Response(const String& sKey1, const String& sKey2, Buffer& sbKey3, Buffer& sbResponse)
{
  Buffer sbKey1;
  Buffer sbKey2;
  KeyString2Buffer(sKey1, sbKey1);
  KeyString2Buffer(sKey2, sbKey2);
  Buffer sbData;
  sbData.Append(sbKey1.Data(), sbKey1.Length());
  sbData.Append(sbKey2.Data(), sbKey2.Length());
  sbData.Append(sbKey3.Data(), sbKey3.Length());
  Apollo::MessageDigest md(sbData.Data(), sbData.Length());
  sbResponse = md.getMD5();
}

// http://datatracker.ietf.org/doc/draft-ietf-hybi-thewebsocketprotocol/?include_text=1
//
// -> GET /chat HTTP/1.1
// -> Host: server.example.com
// -> Upgrade: websocket
// -> Connection: Upgrade
// -> Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
// -> Sec-WebSocket-Origin: http://example.com
// -> Sec-WebSocket-Protocol: chat, superchat
// -> Sec-WebSocket-Version: 8
// 
// <- HTTP/1.1 101 Switching Protocols
// <- Upgrade: websocket
// <- Connection: Upgrade
// <- Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=

void WebSocketConnection::Create_Standard_Response(const String& sKey, String& sResponse)
{
  String sData = sKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  Apollo::MessageDigest sha1((unsigned char*) sData.c_str(), sData.bytes());
  Buffer b = sha1.getSHA1();
  b.encodeBase64(sResponse);
}

void WebSocketConnection::ReceiveWebSocketData(unsigned char* pData, size_t nLen)
{
  switch (nWebSocketVersion_) {
    case ChromePreStandard: ReceiveWebSocketData_ChromePreStandard(pData, nLen); break;
    case Standard: ReceiveWebSocketData_Standard(pData, nLen); break;
    default:
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " no WebSocket version", ApHandlePrintf(hAp_)));
  }
}

void WebSocketConnection::ReceiveWebSocketData_ChromePreStandard(unsigned char* pData, size_t nLen)
{
  data_.Append(pData, nLen);

  int bDone = 0;
  while (!bDone) {
    bDone = 1;

    // Search frame limiter: 0x00 and 0xff
    unsigned char* p = data_.Data();
    int n = data_.Length();
    int nStart = -1;
    int nEnd = -1;
    int nPos = 0;
    while (nPos < n) {
      if (p[nPos] == 0x00) {
        nStart = nPos + 1;
      }
      if (p[nPos] == 0xff) {
        nEnd = nPos;
      }
      if (nStart >= 0 && nEnd >= 0) {
        ReceiveWebSocketMessage(&(p[nStart]), nEnd - nStart);
        bDone = 0;
        data_.Discard(nEnd - nStart + 2);
        break;
      }
      nPos++;
    }

  } // while !done
}

void WebSocketConnection::ReceiveWebSocketData_Standard(unsigned char* pData, size_t nLen)
{
  throw ApException(LOG_CHANNEL, LOG_CONTEXT, "Not implemented");
}

void WebSocketConnection::ReceiveWebSocketMessage(unsigned char* pData, size_t nLen)
{
  Msg_WebSocketServer_ReceiveText msg;
  msg.hConnection = apHandle();
  msg.sData.set((char*) pData, nLen);
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_WebSocketServer_ReceiveText failed conn=" ApHandleFormat "", ApHandlePrintf(msg.hConnection)));
  }
}

int WebSocketConnection::SendWebSocketMessage(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  switch (nWebSocketVersion_) {
    case ChromePreStandard: ok = SendWebSocketData_ChromePreStandard(pData, nLen); break;
    case Standard: ok = SendWebSocketData_Standard(pData, nLen); break;
    default:
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " no WebSocket version", ApHandlePrintf(hAp_)));
  }

  return ok;
}

int WebSocketConnection::SendWebSocketData_ChromePreStandard(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  if (ok) {
    unsigned char b = 0x00;
    ok = DataOut(&b, 1);
  }

  if (ok) {
    ok = DataOut(pData, nLen);
  }

  if (ok) {
    unsigned char b = 0xff;
    ok = DataOut(&b, 1);
  }

  return ok;
}

int WebSocketConnection::SendWebSocketData_Standard(unsigned char* pData, size_t nLen)
{
  throw ApException(LOG_CHANNEL, LOG_CONTEXT, "Not implemented");
}

//static int gbFirst = 1;
//int WebSocketConnection::OnDataIn(unsigned char* pData, size_t nLen)
//{
//  if (gbFirst) {
//    char* p = (char*) pData;
//    const char* pEnd = String::strstr((char*) pData, "\r\n\r\n");
//    if (pEnd != 0) {
//      pEnd += 4;
//      gbFirst = 0;
//      OnDataInX(pData, pEnd - p);
//      OnDataInX((unsigned char*) pEnd, nLen - (pEnd - p));
//    }
//  } else {
//    OnDataInX(pData, nLen);
//  }
//  return 1;
//}

int WebSocketConnection::OnDataIn(unsigned char* pData, size_t nLen)
{
  if (nWebSocketVersion_ == NoVersion) {

    httpParser_.parse(pData, nLen);
    if (httpParser_.hasRequest()) {
      httpParser_.getHeaders(lHeaders_);
      httpParser_.skipRequest();
    }

    String sUpgrade = lHeaders_["Upgrade"].getString();
    String sVersion = lHeaders_["Sec-WebSocket-Version"].getString();

    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "conn=" ApHandleFormat " version=%s", ApHandlePrintf(apHandle()), _sz(sVersion)));

    int nVersion = String::atoi(sVersion);
    if (nVersion >= 6) {
      nWebSocketVersion_ = Standard;
    } else {
      if (httpParser_.getBuffer().Length() >= 8) {
        nWebSocketVersion_ = ChromePreStandard;
      }
    }

    switch (nWebSocketVersion_) {
      case ChromePreStandard:
        {
          String sHost = lHeaders_["Host"].getString();
          String sOrigin = lHeaders_["Origin"].getString();
          String sProtocol = lHeaders_["Sec-WebSocket-Protocol"].getString();

          {
            Msg_WebSocketServer_Connected msg;
            msg.hConnection = apHandle();
            msg.sOrigin = sOrigin;
            msg.sProtocol = sProtocol;
            msg.Send();
          }

          Buffer sbKey3;
          sbKey3.SetData(httpParser_.getBuffer().Data(), 8);
          httpParser_.getBuffer().Discard(8);
          String sKey1 = lHeaders_["Sec-WebSocket-Key1"].getString();
          String sKey2 = lHeaders_["Sec-WebSocket-Key2"].getString();

          Buffer sbSecResponse;
          Create_ChromePreStandard_Response(sKey1, sKey2, sbKey3, sbSecResponse);

          String sResponse = 
            "HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
            "Upgrade: WebSocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Origin: " + sOrigin + "\r\n"
            "Sec-WebSocket-Location: ws://" + sHost + "/\r\n"
            "Server: Apollo WebSocket Interface\r\n"
            "Access-Control-Allow-Origin: " + sOrigin + "\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "Access-Control-Allow-Headers: content-type\r\n"
            "\r\n"
            ;
          DataOut((unsigned char*) sResponse.c_str(), sResponse.bytes());
          DataOut(sbSecResponse.Data(), sbSecResponse.Length());
            
          if (httpParser_.getBuffer().Length() > 0) {
            ReceiveWebSocketData_ChromePreStandard(httpParser_.getBuffer().Data(), httpParser_.getBuffer().Length());
          }
        }
        break;

      case Standard:
        {
          String sHost = lHeaders_["Host"].getString();
          String sOrigin = lHeaders_["Sec-WebSocket-Origin"].getString();
          String sProtocol = lHeaders_["Sec-WebSocket-Protocol"].getString();

          {
            Msg_WebSocketServer_Connected msg;
            msg.hConnection = apHandle();
            msg.sOrigin = sOrigin;
            msg.sProtocol = sProtocol;
            msg.Send();
          }

          String sKey = lHeaders_["Sec-WebSocket-Key"].getString();

          String sSecResponse;
          Create_Standard_Response(sKey, sSecResponse);

          String sResponse = 
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: " + sSecResponse + "\r\n"
            "\r\n"
            ;
          DataOut((unsigned char*) sResponse.c_str(), sResponse.bytes());

          if (httpParser_.getBuffer().Length() > 0) {
            ReceiveWebSocketData_Standard(httpParser_.getBuffer().Data(), httpParser_.getBuffer().Length());
          }
        }
        break;
    }

  } else {

    ReceiveWebSocketData(pData, nLen);

  } // !bWebSocketProtocolActive_
  
  return 1;
}

int WebSocketConnection::OnClosed()
{
  AutoDelete(1);
  apLog_VeryVerbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " closed", ApHandlePrintf(hAp_)));

  Msg_WebSocketServer_Disconnected msg;
  msg.hConnection = apHandle();
  msg.Send();

  ServerModuleInstance::Get()->removeWebSocketConnection(apHandle());

  return 1;
}

#if defined(AP_TEST)

String WebSocketConnection::test_KeyString2Buffer1(const String& sKey, const String& sExpected)
{
  String s;
  
  Buffer b;
  KeyString2Buffer(sKey, b);
  String sBinhex;
  b.encodeBinhex(sBinhex);
  if (String::toLower(sBinhex) != String::toLower(sExpected)) {
    s.appendf("For \"%s\": got=%s, expected=%s", _sz(sKey), _sz(sBinhex), _sz(sExpected));
  }

  return s;
}

String WebSocketConnection::test_KeyString2Buffer()
{
  String s;

  // See real data above
  if (!s) { s = test_KeyString2Buffer1("527z5   [| 2 @1     ? ,566", "02DBC21A"); }
  if (!s) { s = test_KeyString2Buffer1("2  0 l 542 6  9    6 8", "010536C2"); }

  // http://en.wikipedia.org/wiki/WebSockets#draft-ietf-hybi-thewebsocketprotocol-00
  if (!s) { s = test_KeyString2Buffer1("4 @1  46546xW%0l 1 5", "316E4113"); }
  if (!s) { s = test_KeyString2Buffer1("12998 5 Y3 1  .P00", "0F7ED63C"); }

  // http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-00
  if (!s) { s = test_KeyString2Buffer1("P388 O503D&ul7 {K%gX( %7  15", "2E5031B7"); }
  if (!s) { s = test_KeyString2Buffer1("1 N ?|k UT0or 3o  4 I97N 5-S3O 31", "06DAB80B"); }

  return s;
}

String WebSocketConnection::test_Create_ChromePreStandard_Response1(const String& sKey1, const String& sKey2, const String& sKey3, const String& sExpectedAsHex)
{
  String s;

  Buffer sbKey3;
  sbKey3.Append((unsigned char*) (const char*) sKey3, sKey3.bytes());
  Buffer sbResponse;
  Create_ChromePreStandard_Response(sKey1, sKey2, sbKey3, sbResponse);
  String sResultAsHex;
  sbResponse.encodeBinhex(sResultAsHex);
  if (String::toLower(sResultAsHex) != String::toLower(sExpectedAsHex)) {
    s.appendf("key1=\"%s\" key2=\"%s\" got=%s, expected=%s", _sz(sKey1), _sz(sKey2), _sz(sResultAsHex), _sz(sExpectedAsHex));
  }

  return s;
}

String WebSocketConnection::test_Create_ChromePreStandard_Response()
{
  String s;

  // http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-00
  if (!s) { s = test_Create_ChromePreStandard_Response1("P388 O503D&ul7 {K%gX( %7  15", "1 N ?|k UT0or 3o  4 I97N 5-S3O 31", "\x47\x30\x22\x2D\x5A\x3F\x47\x58", "30737433526c26712d325a555e776575"); }

  // http://en.wikipedia.org/wiki/WebSockets#draft-ietf-hybi-thewebsocketprotocol-00
  if (!s) { s = test_Create_ChromePreStandard_Response1("4 @1  46546xW%0l 1 5", "12998 5 Y3 1  .P00", "^n:ds[4U", "386a4b5327793a472a436f2c5778612d"); }

  // See real data above
  if (!s) { s = test_Create_ChromePreStandard_Response1("527z5   [| 2 @1     ? ,566", "2  0 l 542 6  9    6 8", "\x67\xec\xc1\x50\x84\xff\x29\x25", "46f2674132ca76c94ebae9a96588adec"); }

  return s;
}

String WebSocketConnection::test_Create_Standard_Response1(const String& sKey, const String& sExpected)
{
  String s;

  String sResponse;
  Create_Standard_Response(sKey, sResponse);
  if (String::toLower(sResponse) != String::toLower(sExpected)) {
    s.appendf("key1=\"%s\" key2=\"%s\" got=%s, expected=%s", _sz(sKey), _sz(sResponse), _sz(sExpected));
  }

  return s;
}

String WebSocketConnection::test_Create_Standard_Response()
{
  String s;

  // http://datatracker.ietf.org/doc/draft-ietf-hybi-thewebsocketprotocol/?include_text=1
  if (!s) { s = test_Create_Standard_Response1("dGhlIHNhbXBsZSBub25jZQ==", "s3pPLMBiTxaQ9kYGzzhZRbK+xOo="); }

  // http://en.wikipedia.org/wiki/WebSockets#draft-ietf-hybi-thewebsocketprotocol-06
  if (!s) { s = test_Create_Standard_Response1("x3JJHMbDL1EzLkh9GBhXDw==", "HSmrc0sMlYUkAGmm5OPpG2HaGWk="); }

  return s;
}

#endif
