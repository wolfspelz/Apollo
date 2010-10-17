// ============================================================================
// ============================================================================

#if !defined(Tree_h_INCLUDED)
#define Tree_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// -------------------------------------------------------

template <class EXT_ID, class INT_ID, class COMPARE_KEYS> class TreeIterator;

// -------------------------------------------------------

class TreeNodeBase
{
public:
  enum RB_TreeNodeColor {RED, BLACK};
};

template <class EXT_ID, class INT_ID>
class TreeNode : public TreeNodeBase
{
public:
  TreeNode (const EXT_ID &k, const INT_ID &t);
  ~TreeNode (void);

  EXT_ID &key (void);
  const EXT_ID &key (void) const;

  INT_ID &item (void);
  const INT_ID &item (void) const;

  void item (const INT_ID& t);
  INT_ID& operator=(const INT_ID& t);

  void color (RB_TreeNodeColor c);
  RB_TreeNodeColor color (void);

  TreeNode<EXT_ID, INT_ID>* parent (void);
  void parent (TreeNode<EXT_ID, INT_ID>* p);

  TreeNode<EXT_ID, INT_ID>* left (void);
  const TreeNode<EXT_ID, INT_ID>* left (void) const;
  void left (TreeNode<EXT_ID, INT_ID>* l);

  TreeNode<EXT_ID, INT_ID>* right (void);
  const TreeNode<EXT_ID, INT_ID>* right (void) const;
  void right (TreeNode<EXT_ID, INT_ID>* r);

  EXT_ID& Key(void) { return TreeNode<EXT_ID, INT_ID>::key(); }
  INT_ID& Value(void) { return TreeNode<EXT_ID, INT_ID>::item(); }

private:
  EXT_ID k_;
  INT_ID t_;

  RB_TreeNodeColor color_;
  TreeNode<EXT_ID, INT_ID>* parent_;
  TreeNode<EXT_ID, INT_ID>* left_;
  TreeNode<EXT_ID, INT_ID>* right_;
};

// -------------------------------------------------------

class TreeBase
{
public:
  enum RB_SearchResult {LEFT, EXACT, RIGHT};
};

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
class Tree : public TreeBase
{

public:
  friend class TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>;

  Tree (void);
  Tree (const Tree<EXT_ID, INT_ID, COMPARE_KEYS> &rbt);
  virtual ~Tree (void);

  int open (void);
  int close (void);

  inline int Set(const EXT_ID &item, const INT_ID &int_id);
  inline TreeNode<EXT_ID, INT_ID>* Find(const EXT_ID &ext_id);
  inline int Get(const EXT_ID &ext_id, INT_ID &int_id);
  const TreeNode<EXT_ID, INT_ID>* Find(const EXT_ID &ext_id) const;
  int IsSet(const EXT_ID &ext_id) const;
  inline int Unset(const EXT_ID &ext_id);
  inline TreeNode<EXT_ID, INT_ID>* Next(TreeNode<EXT_ID, INT_ID>* entry);
  inline const TreeNode<EXT_ID, INT_ID>* Next(const TreeNode<EXT_ID, INT_ID>* entry) const;

  // Sets EXT_ID, if not already there
  //TreeNode<EXT_ID, INT_ID> &operator[](const EXT_ID &ext_id);

  size_t Count(void) { return current_size_; }
  const Tree<EXT_ID, INT_ID, COMPARE_KEYS>& operator=(const Tree<EXT_ID, INT_ID, COMPARE_KEYS> &rbt);
  virtual int lessthan (const EXT_ID &k1, const EXT_ID &k2) const;

protected:
  void RB_rotate_right (TreeNode<EXT_ID, INT_ID>* x);
  void RB_rotate_left (TreeNode<EXT_ID, INT_ID>* x);
  void RB_delete_fixup (TreeNode<EXT_ID, INT_ID>* x, TreeNode<EXT_ID, INT_ID>* parent);
  TreeNode<EXT_ID, INT_ID>* RB_tree_successor (TreeNode<EXT_ID, INT_ID>* x) const;
  //TreeNode<EXT_ID, INT_ID>* RB_tree_predecessor (TreeNode<EXT_ID, INT_ID>* x) const;
  TreeNode<EXT_ID, INT_ID>* RB_tree_minimum (TreeNode<EXT_ID, INT_ID>* x) const;
  TreeNode<EXT_ID, INT_ID>* RB_tree_maximum (TreeNode<EXT_ID, INT_ID>* x) const;

  TreeNode<EXT_ID, INT_ID>* find_node (const EXT_ID &k, TreeBase::RB_SearchResult &result);
  const TreeNode<EXT_ID, INT_ID>* find_node (const EXT_ID &k, TreeBase::RB_SearchResult &result) const;
  void RB_rebalance (TreeNode<EXT_ID, INT_ID>* x);

  int close_i (void);
  int find_i (const EXT_ID &ext_id, TreeNode<EXT_ID, INT_ID>* &entry, int find_exact = 1);
  int find_i (const EXT_ID &ext_id, const TreeNode<EXT_ID, INT_ID>* &entry, int find_exact = 1) const;
  int insert_i (const EXT_ID &k, const INT_ID &t, TreeNode<EXT_ID, INT_ID>* &entry);
  int remove_i (const EXT_ID &k);
  int remove_i (TreeNode<EXT_ID, INT_ID>* z);

private:
  TreeNode<EXT_ID, INT_ID>* root_;
  COMPARE_KEYS compare_keys_;
  size_t current_size_;
};

// -------------------------------------------------------

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
class TreeIterator
{
public:
  int done (void) const;

protected:
  TreeIterator (const Tree<EXT_ID, INT_ID, COMPARE_KEYS> &tree, int set_first = 1);
  ~TreeIterator (void);

  TreeNode<EXT_ID, INT_ID>* Next (void);

  int forward_i (void);
  //int reverse_i (void);

protected:
  const Tree<EXT_ID, INT_ID, COMPARE_KEYS>* tree_;
  TreeNode <EXT_ID, INT_ID>* node_;
};

template <class CLASS> class LessThan
{
public:
  int operator () (const CLASS &lhs, const CLASS &rhs) const;
};

// -------------------------------------------------------

template <class INT_ID>
class StringTreeNode : public TreeNode<String, INT_ID>
{
public:
};

template <class INT_ID>
class StringTree : public Tree<String, INT_ID, LessThan<String> >
{
public:
  inline StringTreeNode<INT_ID>* Find(const String &ext_id) { return (StringTreeNode<INT_ID>*) Tree<String, INT_ID, LessThan<String> >::Find(ext_id); }
  inline StringTreeNode<INT_ID>* Next(StringTreeNode<INT_ID>* entry) { return (StringTreeNode<INT_ID>*) Tree<String, INT_ID, LessThan<String> >::Next(entry); }
};

template <class INT_ID>
class StringTreeIterator : public TreeIterator<String, INT_ID, LessThan<String> >
{
public:
  StringTreeIterator(const StringTree<INT_ID> &tree, int set_first = 1):TreeIterator<String, INT_ID, LessThan<String> >(tree, set_first) {}
  inline StringTreeNode<INT_ID>* Next(void) { return (StringTreeNode<INT_ID>* ) TreeIterator<String, INT_ID, LessThan<String> >::Next(); }
};

//----------------------------------------------------------
// The value is a pointer to a new(ed) instance
// This template basically adds a disposing destructor to the base

template <class EXT_ID, class INT_ID>
class PointerTreeNode : public TreeNode<EXT_ID, INT_ID>
{
public:
};

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
class PointerTree : public Tree<EXT_ID, INT_ID, COMPARE_KEYS>
{
public:
  virtual ~PointerTree();
  inline PointerTreeNode<EXT_ID, INT_ID>* Find(const EXT_ID &ext_id) { return (PointerTreeNode<EXT_ID, INT_ID>*) Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Find(ext_id); }
  inline PointerTreeNode<EXT_ID, INT_ID>* Next(PointerTreeNode<EXT_ID, INT_ID>* entry) { return (PointerTreeNode<EXT_ID, INT_ID>*) Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Next(entry); } 
  inline int Get(const EXT_ID &ext_id, INT_ID &int_id) { return Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Get(ext_id, int_id); }
};

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
class PointerTreeIterator : public TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>
{
public:
  PointerTreeIterator(const PointerTree<EXT_ID, INT_ID, COMPARE_KEYS> &tree, int set_first = 1):TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>(tree, set_first) {}
  inline PointerTreeNode<EXT_ID, INT_ID>* Next(void) { return (PointerTreeNode<EXT_ID, INT_ID>* ) TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>::Next(); }
};

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
PointerTree<EXT_ID, INT_ID, COMPARE_KEYS>::~PointerTree()
{
  while (Count() > 0) {
    PointerTreeIterator<EXT_ID, INT_ID, COMPARE_KEYS> iter(*this);
    PointerTreeNode<EXT_ID, INT_ID>* pNode = iter.Next();
    if (pNode != 0) {
      INT_ID pItem = pNode->Value();
      if (pItem != 0) {
        Unset(pNode->Key());
        delete pItem;
        pItem = 0;
      }
    }
  }
}

//----------------------------------------------------------
// The key is always an String plus the value is a pointer to a new(ed) instance

template <class INT_ID>
class StringPointerTreeNode : public PointerTreeNode<String, INT_ID>
{
public:
};

template <class INT_ID>
class StringPointerTree : public PointerTree<String, INT_ID, LessThan<String> >
{
public:
  inline StringPointerTreeNode<INT_ID>* Find(const String &ext_id) { return (StringPointerTreeNode<INT_ID>*) PointerTree<String, INT_ID, LessThan<String> >::Find(ext_id); }
  inline StringPointerTreeNode<INT_ID>* Next(StringTreeNode<INT_ID>* entry) { return (StringPointerTreeNode<INT_ID>*) PointerTree<String, INT_ID, LessThan<String> >::Next(entry); }
};

template <class INT_ID>
class StringPointerTreeIterator : public PointerTreeIterator<String, INT_ID, LessThan<String> >
{
public:
  StringPointerTreeIterator(const StringPointerTree<INT_ID> &tree, int set_first = 1):PointerTreeIterator<String, INT_ID, LessThan<String> >(tree, set_first) {}
  inline StringPointerTreeNode<INT_ID>* Next(void) { return (StringPointerTreeNode<INT_ID>* ) PointerTreeIterator<String, INT_ID, LessThan<String> >::Next(); }
};

// -------------------------------------------------------

template <class EXT_ID, class INT_ID>
TreeNode<EXT_ID, INT_ID>::TreeNode (const EXT_ID &k, const INT_ID &t)
:k_(k)
,t_(t)
,color_(RED)
,parent_(0)
,left_(0)
,right_(0)
{
}

template <class EXT_ID, class INT_ID>
TreeNode<EXT_ID, INT_ID>::~TreeNode (void)
{
  delete left_;
  delete right_;
}

// -------------------------------------------------------

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Tree ()
:root_(0)
,current_size_(0)
{
  if (this->open () == -1) {
    // error
  }
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Tree(const Tree<EXT_ID, INT_ID, COMPARE_KEYS> &rbt)
:root_(0)
,current_size_(0)
{
  *this = rbt;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
Tree<EXT_ID, INT_ID, COMPARE_KEYS>::~Tree ()
{
  this->close ();
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
const Tree<EXT_ID, INT_ID, COMPARE_KEYS>& Tree<EXT_ID, INT_ID, COMPARE_KEYS>::operator=(const Tree<EXT_ID, INT_ID, 
                                                           COMPARE_KEYS> &rbt)
{
  const TreeNode<EXT_ID, INT_ID>* pNodeOther = NULL;
  while((pNodeOther = rbt.Next(pNodeOther))) {
    this->Set(pNodeOther->key(), pNodeOther->item());
  }
  return *this;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::lessthan (const EXT_ID &k1, const EXT_ID &k2) const
{
  return this->compare_keys_ (k1, k2);
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::open ()
{
  this->close_i ();
  return 0;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::close (void)
{
  return this->close_i ();
}

// -------------------------------------------------------

template <class T>
int LessThan<T>::operator () (const T &lhs, const T &rhs) const
{
  return (lhs < rhs);
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::close_i ()
{
  delete root_;
  current_size_ = 0;
  root_ = 0;

  return 0;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::insert_i (const EXT_ID &k, const INT_ID &t, TreeNode<EXT_ID, INT_ID> *&entry)
{
  // Find the closest matching node, if there is one.
  RB_SearchResult result = LEFT;
  TreeNode<EXT_ID, INT_ID> *current = find_node (k, result);
  if (current)
  {
    // If the keys match, just return a pointer to the node's item.
    if (result == EXACT)
    {
      current->item(t);
      entry = current;
      return 1;
    }
    // Otherwise if we're to the left of the insertion
    // point, insert into the right subtree.
    else if (result == LEFT)
    {
      if (current->right ())
      {
        // If there is already a right subtree, complain.
        //ACE_ERROR_RETURN ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nright subtree already present in ") ACE_LIB_TEXT ("Tree<EXT_ID, INT_ID>::insert_i\n")), -1);
        return -1;
      }
      else
      {
        // The right subtree is empty: insert new node there.
        TreeNode<EXT_ID, INT_ID> *tmp = 0;
        tmp = new TreeNode<EXT_ID, INT_ID>(k, t);
        if (tmp == 0) {
          return -1;
        }
        current->right (tmp);

        // If the node was successfully inserted, set its parent, rebalance
        // the tree, color the root black, and return a pointer to the
        // inserted item.
        entry = current->right ();
        current->right ()->parent (current);
        RB_rebalance (current->right ());
        root_->color (TreeNodeBase::BLACK);
        ++current_size_;
        return 0;
      }
    }
    // Otherwise, we're to the right of the insertion point, so
    // insert into the left subtree.
    else // (result == RIGHT)
    {
      if (current->left ()) {
        // If there is already a left subtree, complain.
        //ACE_ERROR_RETURN ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nleft subtree already present in ") ACE_LIB_TEXT ("Tree<EXT_ID, INT_ID>::insert_i\n")), -1);
        return -1;
      }
      else
      {
        // The left subtree is empty: insert new node there.
        TreeNode<EXT_ID, INT_ID> *tmp = 0;
        tmp =  new TreeNode<EXT_ID, INT_ID>(k, t);
        if (tmp == 0) {
          return -1;
        }
        current->left (tmp);
        // If the node was successfully inserted, set its
        // parent, rebalance the tree, color the root black, and
        // return a pointer to the inserted item.
        entry = current->left ();
        current->left ()->parent (current);
        RB_rebalance (current->left ());
        root_->color (TreeNodeBase::BLACK);
        ++current_size_;
        return 0;
      }
    }
  }
  else
  {
    // The tree is empty: insert at the root and color the root black.
    root_ = new TreeNode<EXT_ID, INT_ID>(k, t);
    if (root_ == 0) {
      return -1;
    }
    root_->color (TreeNodeBase::BLACK);
    ++current_size_;
    entry = root_;
    return 0;
  }
}

template <class EXT_ID, class INT_ID>
TreeNode<EXT_ID, INT_ID> * TreeNode<EXT_ID, INT_ID>::left ()
{
  return left_;
}

template <class EXT_ID, class INT_ID>
const TreeNode<EXT_ID, INT_ID> * TreeNode<EXT_ID, INT_ID>::left () const
{
  return left_;
}

template <class EXT_ID, class INT_ID>
void TreeNode<EXT_ID, INT_ID>::left (TreeNode<EXT_ID, INT_ID> * l)
{
  left_ = l;
}

template <class EXT_ID, class INT_ID>
TreeNode<EXT_ID, INT_ID> * TreeNode<EXT_ID, INT_ID>::right ()
{
  return right_;
}

template <class EXT_ID, class INT_ID>
const TreeNode<EXT_ID, INT_ID> * TreeNode<EXT_ID, INT_ID>::right () const
{
  return right_;
}

template <class EXT_ID, class INT_ID>
void TreeNode<EXT_ID, INT_ID>::right (TreeNode<EXT_ID, INT_ID> * r)
{
  right_ = r;
}

template <class EXT_ID, class INT_ID>
EXT_ID & TreeNode<EXT_ID, INT_ID>::key ()
{
  return k_;
}

template <class EXT_ID, class INT_ID>
const EXT_ID &TreeNode<EXT_ID, INT_ID>::key (void) const
{
  return (const EXT_ID &)k_;
}

template <class EXT_ID, class INT_ID>
INT_ID & TreeNode<EXT_ID, INT_ID>::item ()
{
  return t_;
}

template <class EXT_ID, class INT_ID>
const INT_ID &TreeNode<EXT_ID, INT_ID>::item (void) const
{
  return (const INT_ID &)t_;
}


template <class EXT_ID, class INT_ID>
void TreeNode<EXT_ID, INT_ID>::item (const INT_ID &t)
{
  t_ = t;
}

template <class EXT_ID, class INT_ID>
INT_ID& TreeNode<EXT_ID, INT_ID>::operator= (const INT_ID& t)
{
  t_ = t;
  return t_;
}

template <class EXT_ID, class INT_ID>
void TreeNode<EXT_ID, INT_ID>::color (TreeNodeBase::RB_TreeNodeColor c)
{
  color_ = c;
}

template <class EXT_ID, class INT_ID>
void TreeNode<EXT_ID, INT_ID>::parent (TreeNode<EXT_ID, INT_ID> * p)
{
  parent_ = p;
}

template <class EXT_ID, class INT_ID>
TreeNodeBase::RB_TreeNodeColor TreeNode<EXT_ID, INT_ID>::color ()
{
  return color_;
}


template <class EXT_ID, class INT_ID>
TreeNode<EXT_ID, INT_ID> * TreeNode<EXT_ID, INT_ID>::parent ()
{
  return parent_;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
void Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_rebalance (TreeNode<EXT_ID, INT_ID> * x)
{
  TreeNode<EXT_ID, INT_ID> *y = 0;

  while (x && x->parent () && x->parent ()->color () == TreeNodeBase::RED)
  {
    if (! x->parent ()->parent ())
    {
      // If we got here, something is drastically wrong!
      //ACE_ERROR ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nerror: parent's parent is null in ") ACE_LIB_TEXT ("Tree<EXT_ID, INT_ID>::RB_rebalance\n")));
      return;
    }

    if (x->parent () == x->parent ()->parent ()->left ())
    {
      y = x->parent ()->parent ()->right ();
      if (y && y->color () == TreeNodeBase::RED)
      {
        // Handle case 1 (see CLR book, pp. 269).
        x->parent ()->color (TreeNodeBase::BLACK);
        y->color (TreeNodeBase::BLACK);
        x->parent ()->parent ()->color (TreeNodeBase::RED);
        x = x->parent ()->parent ();
      }
      else
      {
        if (x == x->parent ()->right ())
        {
          // Transform case 2 into case 3 (see CLR book, pp. 269).
          x = x->parent ();
          RB_rotate_left (x);
        }

        // Handle case 3 (see CLR book, pp. 269).
        x->parent ()->color (TreeNodeBase::BLACK);
        x->parent ()->parent ()->color (TreeNodeBase::RED);
        RB_rotate_right (x->parent ()->parent ());
      }
    }
    else
    {
      y = x->parent ()->parent ()->left ();
      if (y && y->color () == TreeNodeBase::RED)
      {
        // Handle case 1 (see CLR book, pp. 269).
        x->parent ()->color (TreeNodeBase::BLACK);
        y->color (TreeNodeBase::BLACK);
        x->parent ()->parent ()->color (TreeNodeBase::RED);
        x = x->parent ()->parent ();
      }
      else
      {
        if (x == x->parent ()->left ())
        {
          // Transform case 2 into case 3 (see CLR book, pp. 269).
          x = x->parent ();
          RB_rotate_right (x);
        }

        // Handle case 3 (see CLR book, pp. 269).
        x->parent ()->color (TreeNodeBase::BLACK);
        x->parent ()->parent ()->color (TreeNodeBase::RED);
        RB_rotate_left (x->parent ()->parent ());
      }
    }
  }
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::find_node (const EXT_ID &k, TreeBase::RB_SearchResult &result)
{
  // Start at the root.
  TreeNode<EXT_ID, INT_ID> *current = root_;

  while (current)
  {
    // While there are more nodes to examine.
    if (this->lessthan (current->key (), k))
    {
      // If the search key is greater than the current node's key.
      if (current->right ())
        // If the right subtree is not empty, search to the right.
        current = current->right ();
      else
      {
        // If the right subtree is empty, we're done searching,
        // and are positioned to the left of the insertion point.
        result = LEFT;
        break;
      }
    }
    else if (this->lessthan (k, current->key ()))
    {
      // Else if the search key is less than the current node's key.
      if (current->left ())
        // If the left subtree is not empty, search to the left.
        current = current->left ();
      else
      {
        // If the left subtree is empty, we're done searching,
        // and are positioned to the right of the insertion point.
        result = RIGHT;
        break;
      }
    }
    else
    {
      // If the keys match exactly, we're done as well.
      result = EXACT;
      break;
    }
  }

  return current;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
const TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::find_node (const EXT_ID &k, TreeBase::RB_SearchResult &result) const
{
  // Start at the root.
  const TreeNode<EXT_ID, INT_ID> *current = root_;

  while (current)
  {
    // While there are more nodes to examine.
    if (this->lessthan (current->key (), k))
    {
      // If the search key is greater than the current node's key.
      if (current->right ())
        // If the right subtree is not empty, search to the right.
        current = current->right ();
      else
      {
        // If the right subtree is empty, we're done searching,
        // and are positioned to the left of the insertion point.
        result = LEFT;
        break;
      }
    }
    else if (this->lessthan (k, current->key ()))
    {
      // Else if the search key is less than the current node's key.
      if (current->left ())
        // If the left subtree is not empty, search to the left.
        current = current->left ();
      else
      {
        // If the left subtree is empty, we're done searching,
        // and are positioned to the right of the insertion point.
        result = RIGHT;
        break;
      }
    }
    else
    {
      // If the keys match exactly, we're done as well.
      result = EXACT;
      break;
    }
  }

  return current;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>  void
Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_rotate_right (TreeNode<EXT_ID, INT_ID> *x)
{
  if (!x)
  {
    //ACE_ERROR ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nerror: x is a null pointer in ") ACE_LIB_TEXT ("Tree<EXT_ID, INT_ID>::RB_rotate_right\n")));
  }
  else if (! (x->left()))
  {
    //ACE_ERROR ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nerror: x->left () is a null pointer in ") ACE_LIB_TEXT ("Tree<EXT_ID, INT_ID>::RB_rotate_right\n")));
  }
  else
  {
    TreeNode<EXT_ID, INT_ID> * y;
    y = x->left ();
    x->left (y->right ());
    if (y->right ())
      y->right ()->parent (x);
    y->parent (x->parent ());
    if (x->parent ())
    {
      if (x == x->parent ()->right ())
        x->parent ()->right (y);
      else
        x->parent ()->left (y);
    }
    else
      root_ = y;
    y->right (x);
    x->parent (y);
  }
}

// Method for left rotation of the tree about a given node.

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
void Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_rotate_left (TreeNode<EXT_ID, INT_ID> * x)
{
  if (! x)
  {
    //ACE_ERROR ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nerror: x is a null pointer in ") ACE_LIB_TEXT ("Tree<EXT_ID, INT_ID>::RB_rotate_left\n")));
  }
  else if (! (x->right()))
  {
    //ACE_ERROR ((LM_ERROR, ACE_LIB_TEXT ("%p\n"), ACE_LIB_TEXT ("\nerror: x->right () is a null pointer ") ACE_LIB_TEXT ("in Tree<EXT_ID, INT_ID>::RB_rotate_left\n")));
  }
  else
  {
    TreeNode<EXT_ID, INT_ID> * y;
    y = x->right ();
    x->right (y->left ());
    if (y->left ())
      y->left ()->parent (x);
    y->parent (x->parent ());
    if (x->parent ())
    {
      if (x == x->parent ()->left ())
        x->parent ()->left (y);
      else
        x->parent ()->right (y);
    }
    else
      root_ = y;
    y->left (x);
    x->parent (y);
  }
}

#define ENOENT          2

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::remove_i (const EXT_ID &k)
{
  // Find a matching node, if there is one.
  TreeNode<EXT_ID, INT_ID> *z;
  RB_SearchResult result = LEFT;
  z = find_node (k, result);

  // If there is a matching node: remove and destroy it.
  if (z && result == EXACT)
  {
    // Return the internal id stored in the deleted node.
    (void) z->item ();
    return -1 == this->remove_i (z) ? -1 : 1;
  }
  else
  {
    // No matching node was found: return 0.
    return 0;
  }
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::remove_i (TreeNode<EXT_ID, INT_ID> *z)
{
  // Delete the node and reorganize the tree to satisfy the Red-Black
  // properties.

  TreeNode<EXT_ID, INT_ID> *x;
  TreeNode<EXT_ID, INT_ID> *y;
  TreeNode<EXT_ID, INT_ID> *parent;

  if (z->left () && z->right ())
    y = RB_tree_successor (z);
  else
    y = z;

  if (y->left ())
    x = y->left ();
  else
    x = y->right ();

  parent = y->parent ();
  if (x)
  {
    x->parent (parent);
  }

  if (parent)
  {
    if (y == parent->left ())
      parent->left (x);
    else
      parent->right (x);
  }
  else
    this->root_ = x;

  if (y != z)
  {
    // Copy the elements of y into z.
    z->key () = y->key ();
    z->item () = y->item ();
  }

  // CLR pp. 263 says that nil nodes are implicitly colored BLACK
  if (!y || y->color () == TreeNodeBase::BLACK)
    RB_delete_fixup (x, parent);

  y->parent (0);
  y->right (0);
  y->left (0);
  delete y;
  --current_size_;

  return 0;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
void Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_delete_fixup (TreeNode<EXT_ID, INT_ID> *x, TreeNode<EXT_ID, INT_ID> *parent)
{
  while (x != this->root_
    && (!x
    || x->color () == TreeNodeBase::BLACK))
  {
    if (x == parent->left ())
    {
      TreeNode<EXT_ID, INT_ID> *w = parent->right ();
      if (w && w->color () == TreeNodeBase::RED)
      {
        w->color (TreeNodeBase::BLACK);
        parent->color (TreeNodeBase::RED);
        RB_rotate_left (parent);
        w = parent->right ();
      }
      // CLR pp. 263 says that nil nodes are implicitly colored BLACK
      if (w
        && (!w->left ()
        || w->left ()->color () == TreeNodeBase::BLACK)
        && (!w->right ()
        || w->right ()->color () == TreeNodeBase::BLACK))
      {
        w->color (TreeNodeBase::RED);
        x = parent;
        parent = x->parent ();
      }
      else
      {
        // CLR pp. 263 says that nil nodes are implicitly colored BLACK
        if (w
          && (!w->right ()
          || w->right ()->color () == TreeNodeBase::BLACK))
        {
          if (w->left ())
            w->left ()->color (TreeNodeBase::BLACK);
          w->color (TreeNodeBase::RED);
          RB_rotate_right (w);
          w = parent->right ();
        }
        if (w)
        {
          w->color (parent->color ());
          if (w->right ())
            w->right ()->color (TreeNodeBase::BLACK);
        }
        parent->color (TreeNodeBase::BLACK);
        RB_rotate_left (parent);
        x = root_;
      }
    }
    else
    {
      TreeNode<EXT_ID, INT_ID> *w = parent->left ();
      if (w && w->color () == TreeNodeBase::RED)
      {
        w->color (TreeNodeBase::BLACK);
        parent->color (TreeNodeBase::RED);
        RB_rotate_right (parent);
        w = parent->left ();
      }
      // CLR pp. 263 says that nil nodes are implicitly colored BLACK
      if (w
        && (!w->left ()
        || w->left ()->color () == TreeNodeBase::BLACK)
        && (!w->right ()
        || w->right ()->color () == TreeNodeBase::BLACK))
      {
        w->color (TreeNodeBase::RED);
        x = parent;
        parent = x->parent ();
      }
      else
      {
        // CLR pp. 263 says that nil nodes are implicitly colored BLACK
        if (w
          && (!w->left ()
          || w->left ()->color () == TreeNodeBase::BLACK))
        {
          w->color (TreeNodeBase::RED);
          if (w->right ())
            w->right ()->color (TreeNodeBase::BLACK);
          RB_rotate_left (w);
          w = parent->left ();
        }
        if (w)
        {
          w->color (parent->color ());
          if (w->left ())
            w->left ()->color (TreeNodeBase::BLACK);
        }
        parent->color (TreeNodeBase::BLACK);
        RB_rotate_right (parent);
        x = root_;
      }
    }
  }

  if (x)
    x->color (TreeNodeBase::BLACK);
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_tree_successor (TreeNode<EXT_ID, INT_ID> *x) const
{
  if (x == 0)
    return 0;

  if (x->right ())
    return RB_tree_minimum (x->right ());

  TreeNode<EXT_ID, INT_ID> *y = x->parent ();
  while ((y) && (x == y->right ()))
  {
    x = y;
    y = y->parent ();
  }

  return y;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_tree_maximum (TreeNode<EXT_ID, INT_ID> *x) const
{
  while ((x) && (x->right ()))
    x = x->right ();

  return x;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::RB_tree_minimum (TreeNode<EXT_ID, INT_ID> *x) const
{
  while ((x) && (x->left ()))
    x = x->left ();

  return x;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::find_i (const EXT_ID &k, TreeNode<EXT_ID, INT_ID>* &entry, int find_exact)
{
  // Try to find a match.
  RB_SearchResult result = LEFT;
  TreeNode<EXT_ID, INT_ID> *current = find_node (k, result);

  if (current)
  {
    // Found a match
    if (!find_exact || result == EXACT)
      entry = current;  // Assign the entry for any match.
    return (result == EXACT ? 0 : -1);
  }
  else
    // The node is not there.
    return -1;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::find_i (const EXT_ID &k, const TreeNode<EXT_ID, INT_ID>* &entry, int find_exact) const
{
  // Try to find a match.
  RB_SearchResult result = LEFT;
  const TreeNode<EXT_ID, INT_ID> *current = find_node (k, result);

  if (current)
  {
    // Found a match
    if (!find_exact || result == EXACT)
      entry = (const TreeNode<EXT_ID, INT_ID>*) current;  // Assign the entry for any match.
    return (result == EXACT ? 0 : -1);
  }
  else
    // The node is not there.
    return -1;
}

// -------------------------------------------------------

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Set(const EXT_ID &ext_id, const INT_ID &int_id)
{
  TreeNode<EXT_ID, INT_ID> *entry = 0;
  int result = this->insert_i (ext_id, int_id, entry);

  switch (result)
  {
  case 1:
    // If the key was already there, return success.
    return 1;
  case 0:
    // On successfuk insert, return success.
    return 1;
  case -1:
    // If an error happened, just break.
    break;
  }

  return 0;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Find(const EXT_ID &ext_id)
{
  TreeNode<EXT_ID, INT_ID> *entry = 0;

  (void) this->find_i (ext_id, entry);

  return entry;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
const TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Find(const EXT_ID &ext_id) const
{
  const TreeNode<EXT_ID, INT_ID> *entry = 0;

  (void) this->find_i (ext_id, entry);

  return (const TreeNode<EXT_ID, INT_ID> *) entry;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::IsSet(const EXT_ID &ext_id) const
{
  const TreeNode<EXT_ID, INT_ID> *entry = 0;

  (void) this->find_i (ext_id, entry);

  return entry ? 1 : 0;
}


template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Get(const EXT_ID &ext_id, INT_ID &int_id)
{
  TreeNode<EXT_ID, INT_ID> *entry = 0;

  (void) this->find_i (ext_id, entry);

  if (entry != 0) {
    int_id = entry->item();
    return 1;
  }

  return 0;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Unset(const EXT_ID &ext_id)
{
  int result = this->remove_i (ext_id);

  switch (result)
  {
  case 1:
    // If the node was found and deleted, return success.
    return 1;
  case 0:
    // If nothing was found, set errno and break.
    errno = ENOENT;
    break;
  case -1:
    // If an error happened, just break.
    break;
  }

  // Return an error if we didn't already return success.
  return 0;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Next (TreeNode<EXT_ID, INT_ID> * entry)
{
  TreeNode<EXT_ID, INT_ID> *current = entry;

  if (current == 0)
  {
    current = this->RB_tree_minimum (root_);
  }
  else
  {
    current = this->RB_tree_successor (current);
  }

  return current;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
const TreeNode<EXT_ID, INT_ID>* 
Tree<EXT_ID, INT_ID, COMPARE_KEYS>::Next(const TreeNode<EXT_ID, INT_ID>* entry) const
{
  TreeNode<EXT_ID, INT_ID> *current = (TreeNode<EXT_ID, INT_ID> *)entry;

  if (current == 0)
  {
    current = this->RB_tree_minimum (root_);
  }
  else
  {
    current = this->RB_tree_successor (current);
  }

  return (const TreeNode<EXT_ID, INT_ID> *)current;
}

// -------------------------------------------------------

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>::TreeIterator (const Tree<EXT_ID, INT_ID, COMPARE_KEYS> &tree, int set_first)
: tree_ (&tree), node_ (0)
{
  // Position the iterator at the first (or last) node in the tree.
  if (set_first)
    node_ = tree_->RB_tree_minimum (tree_->root_);
  else
    node_ = tree_->RB_tree_maximum (tree_->root_);
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>::~TreeIterator ()
{
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
TreeNode<EXT_ID, INT_ID> * TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>::Next (void)
{
  TreeNode<EXT_ID, INT_ID> * entry = 0;

  if (!this->done()) {
    entry = this->node_;

    this->forward_i ();
  }

  return entry;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>::done (void) const
{
  return node_ ? 0 : 1;
}

template <class EXT_ID, class INT_ID, class COMPARE_KEYS>
int TreeIterator<EXT_ID, INT_ID, COMPARE_KEYS>::forward_i (void)
{
  if (node_)
  {
    node_ = tree_->RB_tree_successor (node_);
  }

  return node_ ? 1 : 0;
}

#endif // !defined(Tree_h_INCLUDED)
