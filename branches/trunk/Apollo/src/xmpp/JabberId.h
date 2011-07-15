// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(JabberId_h_INCLUDED)
#define JabberId_h_INCLUDED

class JabberId : public String
{
public:
  JabberId() 
    :bParsed_(0)
  {}
  JabberId(const char* szBegin) 
    :String(szBegin)
    ,bParsed_(0)
  {}
  JabberId(String& sString)
    :String(sString)
    ,bParsed_(0)
  {}

  void operator=(const char* szBegin) { String::operator=(szBegin); }
  void operator=(String& sString) { String::operator=(sString); }

  String& user();
  String& host();
  String& port();
  String& resource();
  String& base();

  static String TestOne(const char* szJid, const char* szUser, const char* szHost, const char* szPort, const char* szResource, const char* szBase);
  static String Test();

protected:
  void parse();

protected:
  String sUser_;
  String sHost_;
  String sPort_;
  String sResource_;
  String sBase_;

  typedef enum _Case { Case_Unknown
    ,Case_Host
    ,Case_UserHost
    ,Case_ProtocolHost
    ,Case_ProtocolUserHost
    ,Case_UserHostPort
  } Case;

  int bParsed_;
};

#endif // !defined(JabberId_h_INCLUDED)
