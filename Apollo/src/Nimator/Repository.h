// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Repository_H_INCLUDED)
#define Repository_H_INCLUDED

#include "STree.h"
#include "Image.h"

class Frame: public Elem
{
public:
  Frame()
    :nDurationMSec_(0)
  {}

  int nDurationMSec_;
  Apollo::Image img_;
};

// ------------------------------------------------------------

class AnimationFile: public ListT<Frame, Elem>
{
public:
  AnimationFile()
    :bLoaded_(0)
    ,nDurationMSec_(0)
    ,nFramesCount_(0)
    ,nCountUser_(0)
  {}

  void Load();
  void AppendFrame(Frame* pFrame);
  void Unload();

  void AddRef();
  void DeleteRef();

  String sSrc_;
  Buffer sbData_;
  int bLoaded_;
  int nDurationMSec_;
  int nFramesCount_;
  int nCountUser_;
};

// ------------------------------------------------------------

typedef StringPointerTree<AnimationFile*> AnimationFileList;
typedef StringPointerTreeNode<AnimationFile*> AnimationFileNode;
typedef StringPointerTreeIterator<AnimationFile*> AnimationFileIterator;

class Repository
{
public:
  Repository();
  virtual ~Repository();

  AnimationFile* Find(const String& sUrl);
  AnimationFile* Load(const String& sUrl);

protected:
  AnimationFileList files_;
};

#endif // Repository_H_INCLUDED
