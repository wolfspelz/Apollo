// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(HttpParser_h_INCLUDED)
#define HttpParser_h_INCLUDED

#include "Apollo.h"
#include "netapi/NetInterface.h"

class HttpParser
{
public:
  HttpParser()
    :bFinal_(0)
    ,bHeaderParsed_(0)
    ,bHasHeader_(0)
    ,bHasBody_(0)
    ,nContentLength_(-1)
  {}

  void parse(unsigned char* pData, size_t nLength);
  void final();
  int hasHeader() { return bHasHeader_; }
  int hasBody() { return bHasBody_; }
  int hasRequest() { return hasHeader() && hasBody(); }
  String& method() { return sMethod_; }
  String& uri() { return sUri_; }
  String& protocol() { return sProtocol_; }
  void getHeaders(List& l) { l = lHeaders_; }
  void getBody(Buffer& sb);
  void skipRequest();

  Buffer& getBuffer() { return sbData_; }

#if defined(AP_TEST)
  static String test();
#endif // #if defined(AP_TEST)

protected:
  void parseData();
  void findHeader();
  void findBody();
  void parseHeader();

protected:
  Buffer sbData_;
  int bFinal_;
  int bHeaderParsed_;
  int bHasHeader_;
  int bHasBody_;
  int nContentLength_;

  String sHeader_;
  Buffer sbBody_;
  String sMethod_;
  String sUri_;
  String sProtocol_;
  List lHeaders_;
};

#endif // !defined(HttpParser_h_INCLUDED)
