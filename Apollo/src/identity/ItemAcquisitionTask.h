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
  ItemAcquisitionTask(ApHandle hAp, const String& sUrl, const String& sType, const String& sMimeType)
    :hAp_(hAp)
    ,sUrl_(sUrl)
    ,sType_(sType)
  {}
  ~ItemAcquisitionTask() {}

  ApHandle apHandle() { return hAp_; }
  String& getUrl() { return sUrl_; }
  String& getType() { return sType_; }
  String& getMIMEType() { return sMimeType_; }

  void setRequest(ApHandle hRequest) { hRequest_ = hRequest; }
  ApHandle getRequest() { return hRequest_; }

protected:
  ApHandle hAp_;
  String sUrl_;
  String sType_;
  String sMimeType_;
  ApHandle hRequest_;
};

#endif // ItemAcquisitionTask_H_INCLUDED
