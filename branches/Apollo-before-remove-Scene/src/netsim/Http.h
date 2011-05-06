// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Http_H_INCLUDED)
#define Http_H_INCLUDED

class Http
{
public:
  Http(const ApHandle& hClient, const String& sMethod, const String& sUrl, Apollo::KeyValueList& kvHeader, Buffer& sbBody)
    :sMethod_(sMethod)
    ,hClient_(hClient)
    ,sUrl_(sUrl)
    ,kvHeader_(kvHeader)
  {
    sbBody_ = sbBody;
  }

  String sMethod_;
  ApHandle hClient_;
  String sUrl_;
  Apollo::KeyValueList kvHeader_;
  Buffer sbBody_;
};

#endif // Http_H_INCLUDED
