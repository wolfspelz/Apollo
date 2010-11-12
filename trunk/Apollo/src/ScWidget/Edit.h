// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Edit_H_INCLUDED)
#define Edit_H_INCLUDED

#include "Widget.h"

class Edit: public Widget
{
public:
  Edit(const ApHandle& hScene, const String& sPath);
  virtual ~Edit() {}

  inline int IsEdit() { return 1; }

  void Create();

protected:

protected:
};

#endif // Edit_H_INCLUDED
