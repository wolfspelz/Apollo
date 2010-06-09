// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Item_H_INCLUDED)
#define Item_H_INCLUDED

#include "Repository.h"
#include "XMLProcessor.h"

class Animation: public Elem
{
public:
  Animation()
    :bLoaded_(0)
    ,nDurationMSec_(0)
    ,nFramesCount_(0)
  {}

  void Load();
  void AppendFrame(Frame* pFrame);

  String sSrc_;
  Buffer sbData_;
  int bLoaded_;
  int nDurationMSec_;
  int nFramesCount_;
};

// ------------------------------------------------------------

class Sequence: public ListT<Animation, Elem>
{
public:
  Sequence(const String& sName)
    :ListT<Animation, Elem>(sName)
    ,nProbability_(100)
    ,nDx_(0)
    ,nDy_(0)
    ,nDurationMSec_(0)
  {}

  void AppendAnimation(Animation* pAnimation);
  Frame* GetFrameByTime(int nTimeMSec);

  String sType_;
  String sCondition_;
  int nProbability_;
  String sIn_;
  String sOut_;
  int nDx_;
  int nDy_;
  int nDurationMSec_;
};

// ------------------------------------------------------------

class Group: public ListT<Sequence, Elem>
{
public:
  Group(const String& sName)
    :ListT<Sequence, Elem>(sName)
    ,nTotalProbability_(0)
  {}

  void AddSequence(Sequence* pSequence);
  Sequence* GetRandomSequence(int nRnd);

  int nTotalProbability_;
  String sType_;
};

// ------------------------------------------------------------

class Task;

class Item
{
public:
  Item(ApHandle hItem)
    :hAp_(hItem)
    ,bStarted_(0)
    ,nDelayMSec_(100)
    ,nX_(0)
    ,nDestX_(0)
    ,pCurrentSequence_(0)
    ,nSpentInCurrentSequenceMSec_(0)
    ,pPreviousFrame_(0)
  {}

  virtual ~Item();

  int Start();
  void Stop();
  void SetDelay(int nDelayMSec);
  void SetData(Buffer& sbData);
  void SetStatus(const String& sStatus);
  void SetCondition(const String& sCondition);
  void PlayEvent(const String& sEvent);
  void SetPosition(int nX);
  void MoveTo(int nX);

  int HasTimer(ApHandle hTimer) { return ApIsHandle(hTimer) && hTimer == hTimer_; }
  void OnTimer();

protected:
  void Reset();
  void ParseParamNode(Apollo::XMLNode* pNode);
  Group* GetOrCreateGroup(const String& sGroup);
  void ParseSequenceNode(Apollo::XMLNode* pNode);

  void Step(Apollo::TimeValue& tvCurrent);
  Sequence* SelectNextSequence();
  Sequence* GetSequenceByName(const String& sSequence);
  Sequence* GetSequenceByGroup(const String& sGroup);
  String GetDefaultSequence();
  int StartTimer();
  void StopTimer();

protected:
  friend class NimatorModule;
  friend class StatusTask;
  friend class EventTask;
  friend class MoveTask;

  ApHandle hAp_;
  int bStarted_;
  int nDelayMSec_; // msec
  ApHandle hTimer_;
  String sData_;
  String sDefaultSequence_;
  ListT<Group, Elem> lGroups_;
  String sStatus_;
  String sCondition_;
  String sNextEvent_;
  int nX_;
  int nDestX_;
  Sequence* pCurrentSequence_;
  int nSpentInCurrentSequenceMSec_;
  Apollo::TimeValue tvLastTimer_;
  Frame* pPreviousFrame_;
  ListT<Task, Elem> lTasks_;
};

// ------------------------------------------------------------

class Task: public Elem
{
public:
  Task(const String& sName) : Elem(sName) {}
  Sequence* GetSequence(Item& item, int& bDispose) { bDispose = 1; return 0; }
};

class StatusTask: public Task
{
public:
  StatusTask(const String& sName, const String& sStatus) : Task(sName), sStatus_(sStatus) {}
  Sequence* GetSequence(Item& item, int& bDispose);
  String sStatus_;
};

class EventTask: public Task
{
public:
  EventTask(const String& sName, const String& sEvent) : Task(sName), sEvent_(sEvent) {}
  Sequence* GetSequence(Item& item, int& bDispose);
  String sEvent_;
};

//class MoveTask: public Task
//{
//public:
//  MoveTask(const String& sName, int nDestX) : Task(sName), nDestX_(nDestX) {}
//  Sequence* GetSequence(Item& item, int& bDispose);
//  int nDestX_;
//};

#endif // Item_H_INCLUDED
