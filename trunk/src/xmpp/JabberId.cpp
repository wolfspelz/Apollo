// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "JabberId.h"

String& JabberId::user()
{
  if (!bParsed_) parse();
  return sUser_;
}

String& JabberId::host()
{
  if (!bParsed_) parse();
  return sHost_;
}

String& JabberId::port()
{
  if (!bParsed_) parse();
  return sPort_;
}

String& JabberId::resource()
{
  if (!bParsed_) parse();
  return sResource_;
}

String& JabberId::base()
{
  if (sBase_.empty()) {
    sBase_ = user();
    sBase_ += "@";
    sBase_ += host();
    if (!port().empty()) {
      sBase_ += ":";
      sBase_ += port();
    }
  }
  return sBase_;
}

void JabberId::parse()
{
  bParsed_ = 1;

  String sTokenizer(szStr_);
  const char* pColon = sTokenizer.findChar(":");
  const char* pAt = sTokenizer.findChar("@");

  Case nCase = Case_Unknown;
  if (pColon == 0) {
    if (pAt == 0) {
      nCase = Case_Host;
    } else {
      nCase = Case_UserHost;
    }
  } else {
    if (pAt == 0) {
      nCase = Case_ProtocolHost;
    } else {
      if (pColon < pAt) {
        nCase = Case_ProtocolUserHost;
      } else {
        nCase = Case_UserHostPort;
      }
    }
  }

  switch (nCase) {
  case Case_Host:
    break;
  case Case_UserHost:
    {
      sTokenizer.nextToken("@", sUser_);
    }
    break;
  case Case_ProtocolHost:
    {
      String sProtocol;
      sTokenizer.nextToken(":", sProtocol);
    }
    break;
  case Case_ProtocolUserHost:
    {
      String sProtocol;
      sTokenizer.nextToken(":", sProtocol);
      sTokenizer.nextToken("@", sUser_);
    }
    break;
  case Case_UserHostPort:
    {
      sTokenizer.nextToken("@", sUser_);
    }
    break;
  default:;
  }

  sUser_.trim("/");

  if (sTokenizer.findChar(":")) {
    sTokenizer.nextToken(":", sHost_);
    sTokenizer.nextToken("/", sPort_);
  } else {
    sTokenizer.nextToken("/", sHost_);
  }

  sResource_ = sTokenizer;
}

#if defined(AP_TEST)

String JabberId::TestOne(const char* szJid, const char* szUser, const char* szHost, const char* szPort, const char* szResource, const char* szBase)
{
  String s;

  JabberId jid(szJid);
  if (0
    || jid.user() != szUser
    || jid.host() != szHost 
    || jid.port() != szPort
    || jid.resource() != szResource
    || jid.base() != szBase
    ) {
    s.appendf("%s: user[%s %s] host[%s %s] port[%s %s] resource[%s %s] base[%s %s] ", _sz(szJid),
      _sz(jid.user()), _sz(szUser), 
      _sz(jid.host()), _sz(szHost), 
      _sz(jid.port()), _sz(szPort), 
      _sz(jid.resource()), _sz(szResource), 
      _sz(jid.base()), _sz(szBase)
      );
  }

  return s;
}

String JabberId::Test()
{
  String s;

  if (!s) { s = JabberId::TestOne("user@domain.org", "user", "domain.org", "", "", "user@domain.org"); }
  if (!s) { s = JabberId::TestOne("user@domain.org/resource", "user", "domain.org", "", "resource", "user@domain.org"); }
  if (!s) { s = JabberId::TestOne("user@domain.org:5222/resource", "user", "domain.org", "5222", "resource", "user@domain.org:5222"); }
  if (!s) { s = JabberId::TestOne("xmpp:user@domain.org:5222/resource", "user", "domain.org", "5222", "resource", "user@domain.org:5222"); }
  if (!s) { s = JabberId::TestOne("user@domain.org:443/resource", "user", "domain.org", "443", "resource", "user@domain.org:443"); }
  if (!s) { s = JabberId::TestOne("xmpp:user@domain.org/resource", "user", "domain.org", "", "resource", "user@domain.org"); }
  if (!s) { s = JabberId::TestOne("xmpp:user@domain.org", "user", "domain.org", "", "", "user@domain.org"); }
  if (!s) { s = JabberId::TestOne("xmpp://user@domain.org", "user", "domain.org", "", "", "user@domain.org"); }

  return s;
}

#endif // #if defined(AP_TEST)
