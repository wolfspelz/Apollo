// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ItemAcquisitionTask_H_INCLUDED)
#define ItemAcquisitionTask_H_INCLUDED

#include "Apollo.h"

class ItemAcquisitionTask
{
public:
  ItemAcquisitionTask(const ApHandle& hAp, const String& sUrl, const String& sType, const String& sMimeType)
    :hAp_(hAp)
    ,sUrl_(sUrl)
    ,sType_(sType)
  {}
  ~ItemAcquisitionTask() {}

  inline ApHandle apHandle() { return hAp_; }
  inline String& getUrl() { return sUrl_; }
  inline String& getType() { return sType_; }
  inline String& getMIMEType() { return sMimeType_; }

  inline void setRequest(const ApHandle& hRequest) { hRequest_ = hRequest; }
  inline ApHandle getRequest() { return hRequest_; }

protected:
  ApHandle hAp_;
  String sUrl_;
  String sType_;
  String sMimeType_;
  ApHandle hRequest_;
};

#endif // ItemAcquisitionTask_H_INCLUDED
