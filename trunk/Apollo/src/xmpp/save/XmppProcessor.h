// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XmppProcessor_H_INCLUDED)
#define XmppProcessor_H_INCLUDED

#include "XMLProcessor.h"

#include "XmppProtocol.h"

class XmppProcessor : public Apollo::XMLProcessor
{
public:
  XmppProcessor(XmppProtocol& oProtocol);
  virtual ~XmppProcessor();

  int OnXMLDocBegin(Apollo::XMLNode* _node);
  int OnXMLNodeTree(Apollo::XMLNode* _node);
  int OnXMLDocEnd(const TCHAR* _name);

protected:
  XmppProtocol& oProtocol_;
};

#endif // !defined(XmppProcessor_H_INCLUDED)
