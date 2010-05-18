// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApContainers_h_INCLUDED)
#define ApContainers_h_INCLUDED

#include "STree.h"
#include "ApTypes.h"

//----------------------------------------------------------
// The key is always an ApHandle 

template <class INT_ID>
class ApHandleTreeNode : public TreeNode<ApHandle, INT_ID>
{
public:
  ApHandle& Key(void) { return TreeNode<ApHandle, INT_ID>::key(); }
  INT_ID& Value(void) { return TreeNode<ApHandle, INT_ID>::item(); }
};

template <class INT_ID>
class ApHandleTree : public Tree<ApHandle, INT_ID, LessThan<ApHandle> >
{
public:
  ApHandleTreeNode<INT_ID>* Find(const ApHandle &ext_id) { return (ApHandleTreeNode<INT_ID>*) Tree<ApHandle, INT_ID, LessThan<ApHandle> >::Find(ext_id); }
  ApHandleTreeNode<INT_ID>* Next(ApHandleTreeNode<INT_ID>* entry) { return (ApHandleTreeNode<INT_ID>*) Tree<ApHandle, INT_ID, LessThan<ApHandle> >::Next(entry); }
  int Get(const ApHandle &ext_id, INT_ID &int_id) { return Tree<ApHandle, INT_ID, LessThan<ApHandle> >::Get(ext_id, int_id); }
};

template <class INT_ID>
class ApHandleTreeIterator : public TreeIterator<ApHandle, INT_ID, LessThan<ApHandle> >
{
public:
  ApHandleTreeIterator(const ApHandleTree<INT_ID> &tree, int set_first = 1):TreeIterator<ApHandle, INT_ID, LessThan<ApHandle> >(tree, set_first) {}
  ApHandleTreeNode<INT_ID>* Next(void) { return (ApHandleTreeNode<INT_ID>* ) TreeIterator<ApHandle, INT_ID, LessThan<ApHandle> >::Next(); }
};

//----------------------------------------------------------
// The key is always an ApHandle plus the value is a pointer to a new(ed) instance

template <class INT_ID>
class ApHandlePointerTreeNode : public PointerTreeNode<ApHandle, INT_ID>
{
public:
};

template <class INT_ID>
class ApHandlePointerTree : public PointerTree<ApHandle, INT_ID, LessThan<ApHandle> >
{
public:
  ApHandlePointerTreeNode<INT_ID>* Find(const ApHandle &ext_id) { return (ApHandlePointerTreeNode<INT_ID>*) PointerTree<ApHandle, INT_ID, LessThan<ApHandle> >::Find(ext_id); }
  ApHandlePointerTreeNode<INT_ID>* Next(ApHandlePointerTreeNode<INT_ID>* entry) { return (ApHandlePointerTreeNode<INT_ID>*) PointerTree<ApHandle, INT_ID, LessThan<ApHandle> >::Next(entry); }
};

template <class INT_ID>
class ApHandlePointerTreeIterator : public PointerTreeIterator<ApHandle, INT_ID, LessThan<ApHandle> >
{
public:
  ApHandlePointerTreeIterator(const ApHandlePointerTree<INT_ID> &tree, int set_first = 1):PointerTreeIterator<ApHandle, INT_ID, LessThan<ApHandle> >(tree, set_first) {}
  ApHandlePointerTreeNode<INT_ID>* Next(void) { return (ApHandlePointerTreeNode<INT_ID>* ) PointerTreeIterator<ApHandle, INT_ID, LessThan<ApHandle> >::Next(); }
};

#endif // !defined(ApContainers_h_INCLUDED)
