// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ServerModule.h"
#include "HttpParser.h"

void HttpParser::parse(unsigned char* pData, size_t nLength)
{
  sbData_.Append(pData, nLength);
  findHeader();
  if (hasHeader()) {
    findBody();
  }
}

void HttpParser::findHeader()
{
  if (sbData_.Length() > 5 && !hasHeader()) {
    unsigned char* p = sbData_.Data();
    unsigned int nOffset = 0;
    int bCR = 0;
    int bStartOfLine = 0;
    int bHeaderComplete = 0;
    int bDone = 0;
    while (nOffset < sbData_.Length() && !bDone) {
      unsigned char c = p[nOffset];

      if (c == '\r') {
        nOffset++;
        if (nOffset < sbData_.Length()) {
          c = p[nOffset];
        } else {
          bDone = 1;
        }
      }

      if (!bDone) {
        if (c == '\n') {
          if (bStartOfLine) {
            bHeaderComplete = 1;
            bDone = 1;
          } else {
            bStartOfLine = 1;
          }
        } else {
          bStartOfLine = 0;
        }

        nOffset++;
      }
    }

    if (bHeaderComplete) {
      sHeader_.set((const char *) sbData_.Data(), nOffset);
      bHasHeader_ = 1;
      parseHeader();
    }
  }
}

void HttpParser::parseHeader()
{
  if (hasHeader() && !bHeaderParsed_) {
    lHeaders_.Empty();
    KeyValueLfBlob2List(sHeader_, lHeaders_);

    { // Request line
      Elem* e = lHeaders_.First();
      if (e) {
        sMethod_ = e->getName();
        sProtocol_ = e->getString();
        sProtocol_.nextToken(" ", sUri_);
        lHeaders_.Remove(e);
        delete e;
        e = 0;
      }
    }

    { // Body size
      if (0) {
      } else if (sMethod_ == "GET") {
        nContentLength_ = 0;
      } else if (sMethod_ == "HEAD") {
        nContentLength_ = 0;
      } else {
        Elem* e = lHeaders_.FindByNameCase("content-length");
        if (e) {
          nContentLength_ = String::atoi(e->getString());
        }
      }
    }

    bHeaderParsed_ = 1;
  }
}

void HttpParser::findBody()
{
  if (hasHeader() && !hasBody()) {
    if (nContentLength_ > 0) {
      if (sHeader_.bytes() + nContentLength_ <= sbData_.Length()) {
        sbBody_.SetData(sbData_.Data() + sHeader_.bytes(), nContentLength_);
        bHasBody_ = 1;
      }
    } else if (nContentLength_ == 0) {
      bHasBody_ = 1;
    } else {
      if (bFinal_) {
        sbBody_.SetData(sbData_.Data() + sHeader_.bytes(), sbData_.Length() - sHeader_.bytes());
        bHasBody_ = 1;
      }
    }
  }
}

void HttpParser::final()
{
  bFinal_ = 1;

  if (hasHeader()) {
    findBody();
  }
}

void HttpParser::getBody(Buffer& sb)
{
  if (hasBody()) {
    sb = sbBody_;
  }
}

void HttpParser::skipRequest()
{
  if (hasRequest()) {
    sbData_.Discard(sHeader_.bytes() + sbBody_.Length());

    bHeaderParsed_ = 0;
    bHasHeader_ = 0;
    bHasBody_ = 0;
    nContentLength_ = -1;
    sHeader_.clear();
    sbBody_.Empty();
    sMethod_.clear();
    sUri_.clear();
    sProtocol_.clear();
    lHeaders_.Empty();

    findHeader();
    if (hasHeader()) {
      findBody();
    }
  }
}

//--------------------------

#if defined(AP_TEST)

String HttpParser::test()
{
  String s;

  // Basic
  if (!s) {
    HttpParser p;
    #define HttpParser_test_1 "GET /uri HTTP/1.1\r\n" "\r\n"
    p.parse((unsigned char*) HttpParser_test_1, strlen(HttpParser_test_1));
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
  }

  // \n instead of \r\n
  if (!s) {
    HttpParser p;
    #define HttpParser_test_2 "GET /uri HTTP/1.1\n" "\n"
    p.parse((unsigned char*) HttpParser_test_2, strlen(HttpParser_test_2));
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
  }

  // last \r\n in 2 parts
  if (!s) {
    HttpParser p;
    #define HttpParser_test_3_1 "GET /uri HTTP/1.1\r\n" "\r"
    #define HttpParser_test_3_2 "\n"
    p.parse((unsigned char*) HttpParser_test_3_1, strlen(HttpParser_test_3_1));
    if (!s) { if (p.hasHeader()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) HttpParser_test_3_2, strlen(HttpParser_test_3_2));
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
  }

  // More header fields
  if (!s) {
    HttpParser p;
    #define HttpParser_test_4 "GET /uri HTTP/1.1\r\n" "Content-length: 0\r\n" "Accept: text/html,text/xml,image/gif\r\n" "\r\n"
    p.parse((unsigned char*) HttpParser_test_4, strlen(HttpParser_test_4));
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
    if (!s) {
      List lHeaders;
      p.getHeaders(lHeaders);
      List lExpected; lExpected.AddLast("Content-length", "0"); lExpected.AddLast("Accept", "text/html,text/xml,image/gif");
      s = Apollo::Test_CompareLists(String::from(__LINE__), lHeaders, lExpected);
    }
  }

  // Many parts, final()
  if (!s) {
    HttpParser p;
    #define HttpParser_test_5_1 "GET /ur"
    #define HttpParser_test_5_2 "i HTTP/1.1\r"
    #define HttpParser_test_5_3 "\n" "Content-length: 0\r\n"
    #define HttpParser_test_5_4 "Accept: text/html,text/xml"
    #define HttpParser_test_5_5 ",image/gif\r\n" "\r\n"
    p.parse((unsigned char*) HttpParser_test_5_1, strlen(HttpParser_test_5_1));
    if (!s) { if (p.hasHeader()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) HttpParser_test_5_2, strlen(HttpParser_test_5_2));
    if (!s) { if (p.hasHeader()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) HttpParser_test_5_3, strlen(HttpParser_test_5_3));
    if (!s) { if (p.hasHeader()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) HttpParser_test_5_4, strlen(HttpParser_test_5_4));
    if (!s) { if (p.hasHeader()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) HttpParser_test_5_5, strlen(HttpParser_test_5_5));
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
    if (!s) {
      List lHeaders;
      p.getHeaders(lHeaders);
      List lExpected; lExpected.AddLast("Content-length", "0"); lExpected.AddLast("Accept", "text/html,text/xml,image/gif");
      s = Apollo::Test_CompareLists(String::from(__LINE__), lHeaders, lExpected);
    }
    p.final();
    p.skipRequest();
    if (!s) { if (p.hasHeader()) { s = String::from(__LINE__); } }
  }

  // body, final()
  if (!s) {
    HttpParser p;
    #define HttpParser_test_6 "POST /uri HTTP/1.1\r\n" "Content-type: text/plain\r\n" "Accept: text/html,text/xml,image/gif\r\n" "\r\n" "body"
    p.parse((unsigned char*) HttpParser_test_6, strlen(HttpParser_test_6));
    p.final();
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "POST") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
    if (!s) {
      List lHeaders;
      p.getHeaders(lHeaders);
      List lExpected; lExpected.AddLast("Content-type", "text/plain"); lExpected.AddLast("Accept", "text/html,text/xml,image/gif");
      s = Apollo::Test_CompareLists(String::from(__LINE__), lHeaders, lExpected);
    }
    if (!s) {
      Buffer sbBody;
      p.getBody(sbBody);
      String sBody;
      sbBody.GetString(sBody);
      if (sBody != "body") { s = String::from(__LINE__); }
    }
  }

  // body, skipRequest()
  if (!s) {
    HttpParser p;
    #define HttpParser_test_7 "POST /uri HTTP/1.1\r\n" "Content-length: 4\r\n" "Accept: text/html,text/xml,image/gif\r\n" "\r\n" "body" "GET /uri HTTP/1.1\r\n" "\r\n"
    p.parse((unsigned char*) HttpParser_test_7, strlen(HttpParser_test_7));
    p.final();
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "POST") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) {
      Buffer sbBody;
      p.getBody(sbBody);
      if (sbBody.Length() != strlen("body")) { s = String::from(__LINE__); }
    }
    p.skipRequest();
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri") { s = String::from(__LINE__); } }
    if (!s) { if (p.protocol() != "HTTP/1.1") { s = String::from(__LINE__); } }
  }

  // partial big body...
  if (!s) {
    HttpParser p;
    #define HttpParser_test_8 "POST /uri HTTP/1.1\r\n" "Content-Length: 10000\r\n" "Content-type: text/plain\r\n" "\r\n" 
    p.parse((unsigned char*) HttpParser_test_8, strlen(HttpParser_test_8));
    #define n10k 10000
    unsigned char p10k[n10k];
    for (int i = 0; i < n10k; ++i) { p10k[i] = '0' + i % 10; }
    p.parse((unsigned char*) p10k, n10k/2);
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.hasBody()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) p10k + n10k/2, n10k/2 - 1);
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.hasBody()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) p10k + n10k - 1, 1);
    if (!s) { if (!p.hasBody()) { s = String::from(__LINE__); } }
    if (!s) { Buffer sbBody; p.getBody(sbBody); if (sbBody.Length() != n10k) { s = String::from(__LINE__); } }
  }

  // multiple: skipRequest() some with body...
  if (!s) {
    HttpParser p;
    #define HttpParser_test_9_1 "POST /uri1 HTTP/1.1\r\n" "Content-length: 4\r\n" "Accept: text/html,text/xml,image/gif\r\n" "\r\n" "body"
    #define HttpParser_test_9_2 "GET /uri2 HTTP/1.1\r\n" "\r\n"
    #define HttpParser_test_9_3 "POST /uri3 HTTP/1.1\r\n" "Content-Length: 10\r\n" "Content-type: text/plain\r\n" "\r\n" "1234567890"
    #define HttpParser_test_9_4 "POST /uri4 HTTP/1.1\r\n" "Content-Length: 10000\r\n" "Content-type: text/plain\r\n" "\r\n" 
    #define HttpParser_test_9_5 "GET /uri5 HTTP/1.1\r\n" "\r\n"
    #define HttpParser_test_9_6 "GET /uri6 HTTP/1.1\r\n" "\r\n"
    p.parse((unsigned char*) HttpParser_test_9_1, strlen(HttpParser_test_9_1));
    p.parse((unsigned char*) HttpParser_test_9_2, strlen(HttpParser_test_9_2));
    p.parse((unsigned char*) HttpParser_test_9_3, strlen(HttpParser_test_9_3));
    p.parse((unsigned char*) HttpParser_test_9_4, strlen(HttpParser_test_9_4));
    #define n10k 10000
    unsigned char p10k[n10k];
    for (int i = 0; i < n10k; ++i) { p10k[i] = '0' + i % 10; }
    p.parse((unsigned char*) p10k, n10k);
    p.parse((unsigned char*) HttpParser_test_9_5, strlen(HttpParser_test_9_5));
    p.parse((unsigned char*) HttpParser_test_9_6, strlen(HttpParser_test_9_6));
    if (!s) { if (!p.hasRequest()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "POST") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri1") { s = String::from(__LINE__); } }
    if (!s) { Buffer sbBody; p.getBody(sbBody); if (sbBody.Length() != 4) { s = String::from(__LINE__); } }
    p.skipRequest();
    if (!s) { if (!p.hasRequest()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri2") { s = String::from(__LINE__); } }
    p.skipRequest();
    if (!s) { if (!p.hasRequest()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "POST") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri3") { s = String::from(__LINE__); } }
    if (!s) { Buffer sbBody; p.getBody(sbBody); if (sbBody.Length() != 10) { s = String::from(__LINE__); } }
    p.skipRequest();
    if (!s) { if (!p.hasRequest()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "POST") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri4") { s = String::from(__LINE__); } }
    if (!s) { Buffer sbBody; p.getBody(sbBody); if (sbBody.Length() != n10k) { s = String::from(__LINE__); } }
    p.skipRequest();
    if (!s) { if (!p.hasRequest()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri5") { s = String::from(__LINE__); } }
    p.skipRequest();
    if (!s) { if (!p.hasRequest()) { s = String::from(__LINE__); } }
    if (!s) { if (p.method() != "GET") { s = String::from(__LINE__); } }
    if (!s) { if (p.uri() != "/uri6") { s = String::from(__LINE__); } }
    p.skipRequest();
    if (!s) { if (p.hasRequest()) { s = String::from(__LINE__); } }
  }

  // wrong content-length for big body...
  if (!s) {
    HttpParser p;
    #define HttpParser_test_10 "POST /uri HTTP/1.1\r\n" "Content-Length: 10001\r\n" "Content-type: text/plain\r\n" "\r\n" 
    p.parse((unsigned char*) HttpParser_test_10, strlen(HttpParser_test_10));
    #define n10k 10000
    unsigned char p10k[n10k];
    for (int i = 0; i < n10k; ++i) { p10k[i] = '0' + i % 10; }
    p.parse((unsigned char*) p10k, n10k/2);
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.hasBody()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) p10k + n10k/2, n10k/2 - 1);
    if (!s) { if (!p.hasHeader()) { s = String::from(__LINE__); } }
    if (!s) { if (p.hasBody()) { s = String::from(__LINE__); } }
    p.parse((unsigned char*) p10k + n10k - 1, 1);
    if (!s) { if (p.hasBody()) { s = String::from(__LINE__); } }
    p.final();
    if (!s) { if (p.hasBody()) { s = String::from(__LINE__); } }
  }

  return s;
}

#endif // #if defined(AP_TEST)
