// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "SrpcStreamParser.h"

AP_NAMESPACE_BEGIN

SrpcStreamParser::~SrpcStreamParser()
{
  if (pCurrentMessage_ != 0) {
    delete pCurrentMessage_;
    pCurrentMessage_ = 0;
  }
}

// Returned SrpcMessage has been new-ed by SrpcStreamParser and must be delete-d by caller.
Apollo::SrpcMessage* SrpcStreamParser::getNextMessage()
{
  Apollo::SrpcMessage* pMessage = lMessages_.First();
  
  if (pMessage != 0) {
    lMessages_.Remove(pMessage);
  }

  return pMessage;
}

int SrpcStreamParser::parse(unsigned char* pData, size_t nLen)
{
  int ok = 1;

  sbData_.Append(pData, nLen);

  int nUsed = 0;
  const char* pWork = (const char* ) sbData_.Data();
  size_t nWork = sbData_.Length();
  const char* pLineStart = pWork;
  for (const char* p = pWork; p < pWork + nWork; p++) {

    if (*p == '\r' || *p == '\n') {
      String sLine;
      if (p > pLineStart) {
        sLine.set(pLineStart, p - pLineStart);
      }
      pLineStart = p + 1;
      nUsed = pLineStart - pWork;
      if (sLine.empty()) {
        if (pCurrentMessage_ != 0) {
          Apollo::SrpcMessage* pMessage = pCurrentMessage_;
          pCurrentMessage_ = 0;
          lMessages_.AddLast(pMessage);
        }
      } else {
        String sKey;
        if (sLine.nextToken("=", sKey)) {
          if (!sKey.empty()) {
            if (pCurrentMessage_ == 0) {
              pCurrentMessage_ = new Apollo::SrpcMessage();
            }
            if (pCurrentMessage_ != 0) {
              pCurrentMessage_->set(sKey, sLine);
            }
          }
        }
      }

      if (*p == '\r' && *(p+1) == '\n') { p++; nUsed++; pLineStart++; }
    } // \r \n
  } // for

  if (nUsed > 0) {
    sbData_.Discard(nUsed);
  }

  return ok;
}

int SrpcStreamParser::parse(const String& sData)
{
  return parse((unsigned char*) sData.c_str(), sData.bytes());
}

AP_NAMESPACE_END
