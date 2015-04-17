// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Stanza.h"
#include "Protocol.h"

Stanza::Stanza(const char* szName)
:Apollo::XMLNode(szName)
{
}

int Stanza::getError(int& nError, String& sError)
{
  // <iq id='0' type='error' from='googlemail.com'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>wolf.heiner</username>
  //   </query>
  //   <error code='405' type='cancel'>
  //     <not-allowed xmlns='urn:ietf:params:xml:ns:xmpp-stanzas' />
  //     <text xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'>Connection must be encrypted.</text>
  //   </error>
  // </iq>

  int ok = 0;

  Apollo::XMLNode& error = getChildRef("error");
  if (error) {
    ok = 1;

    sError = error.getCData();
    sError.trimWSP();

    if (sError.empty()) {
      Apollo::XMLNode& text = error.getChildRef("text");
      sError = text.getCData();
    }

    nError = String::atoi(error.getAttribute("code").getValue());
  }

  return ok;
}

// ----------------------------------

IQStanza::IQStanza()
:Stanza("iq")
{
}

Apollo::XMLNode& IQStanza::addQuery(const char* szNameSpace)
{
  Apollo::XMLNode& query = addChildRef("query");
  if (query) {
    query.addAttribute("xmlns", szNameSpace);
  }
  return query;
}

GetStanza::GetStanza(const char* szId, const char* szTo)
{
  addAttribute("id", szId);
  addAttribute("to", szTo);
  addAttribute("type", "get");
}

SetStanza::SetStanza(const char* szId, const char* szTo)
{
  addAttribute("id", szId);
  addAttribute("to", szTo);
  addAttribute("type", "set");
}

ResultStanza::ResultStanza(const char* szId, const char* szTo)
{
  addAttribute("id", szId);
  addAttribute("to", szTo);
  addAttribute("type", "result");
}

// ----------------------------------

PresenceStanza::PresenceStanza(const char* szType, const char* szTo)
:Stanza("presence")
{
  String sType = szType;
  if (sType != XMPP_PRESENCE_AVAILABLE) {
    addAttribute("type", szType);
  }

  addAttribute("to", szTo);
}

// ----------------------------------

MessageStanza::MessageStanza(const char* szType, const char* szTo)
:Stanza("message")
{
  addAttribute("type", szType);
  addAttribute("to", szTo);
}

Apollo::XMLNode& MessageStanza::addText(const char* szText)
{
  Apollo::XMLNode& body = addChildRef("body");
  if (body) {
    body.setCData(szText);
  }
  return body;
}

GroupchatMessageStanza::GroupchatMessageStanza(const char* szTo)
:MessageStanza("groupchat", szTo)
{
}
