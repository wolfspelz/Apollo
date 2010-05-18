// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"

#include "Local.h"
#include "XmppProcessor.h"

XmppProcessor::XmppProcessor(XmppProtocol& oProtocol)
:oProtocol_(oProtocol)
{
}

XmppProcessor::~XmppProcessor()
{
}

int XmppProcessor::OnXMLDocBegin(Apollo::XMLNode* _node)
{
  oProtocol_.OnStanzaIn(_node);

  return 1;
}

int XmppProcessor::OnXMLNodeTree(Apollo::XMLNode* _node)
{
  if (_node->Depth() == 1) {
    oProtocol_.OnStanzaIn(_node);
    _node->Delete(1);
  }

  return 1;
}

int XmppProcessor::OnXMLDocEnd(const TCHAR* _name)
{
  return 1;
}
