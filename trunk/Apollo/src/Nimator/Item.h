// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Item_H_INCLUDED)
#define Item_H_INCLUDED

#include "Image.h"
#include "XMLProcessor.h"

class Frame: public Elem
{
public:
  Frame()
    :nDurationMSec_(0)
  {}

  int nDurationMSec_;
  Apollo::Image img_;
};

class Animation: public ListT<Frame, Elem>
{
public:
  Animation()
    :bLoaded_(0)
    ,nTotalDurationMSec_(0)
    ,nFramesCount_(0)
  {}

  void Load();
  void AppendFrame(Frame* pFrame);

  String sSrc_;
  Buffer sbData_;
  int bLoaded_;
  int nTotalDurationMSec_;
  int nFramesCount_;
};

class Sequence: public ListT<Animation, Elem>
{
public:
  Sequence(const String& sName)
    :ListT<Animation, Elem>(sName)
    ,nProbability_(100)
    ,nDx_(0)
    ,nDy_(0)
  {}

  void AppendAnimation(Animation* pAnimation);

  String sType_;
  String sCondition_;
  int nProbability_;
  String sIn_;
  String sOut_;
  int nDx_;
  int nDy_;
};

class Group: public ListT<Sequence, Elem>
{
public:
  Group(const String& sName)
    :ListT<Sequence, Elem>(sName)
    ,nTotalProbability_(0)
  {}

  void AddSequence(Sequence* pSequence);

  int nTotalProbability_;
  String sType_;
};

class Item
{
public:
  Item(ApHandle hItem);
  virtual ~Item();

  void Start();
  void Stop();
  void SetDelay(int nDelayMSec);
  void SetData(Buffer& sbData);
  void SetStatus(const String& sStatus);
  void PlayEvent(const String& sEvent);

protected:
  void ResetAnimations();
  void ParseParamNode(Apollo::XMLNode* pNode);
  Group* GetOrCreateGroup(const String& sGroup);
  void ParseSequenceNode(Apollo::XMLNode* pNode);

protected:
  friend class NimatorModule;
  ApHandle hAp_;
  int bStarted_;
  int nDelayMSec_; // msec
  String sData_;
  String sDefaultGroup_;
  ListT<Group, Elem> lGroups_;
};

#endif // Item_H_INCLUDED
