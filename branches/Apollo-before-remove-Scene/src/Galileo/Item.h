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

// ------------------------------------------------------------

class GalileoModule;

class Animation: public ListT<Frame, Elem>
{
public:
  Animation(GalileoModule* pModule)
    :pModule_(pModule)
    ,bDataBroken_(0)
    ,nRequestSuspendDelaySec_(0)
    ,bLoaded_(0)
    ,nDurationMSec_(0)
    ,nFramesCount_(0)
  {}

  void Load();
  int IsLoaded() { return bLoaded_; }
  int IsBroken() { return bDataBroken_; }
  int HasData() { return sbData_.Length() > 0; }
  void AppendFrame(Frame* pFrame);
  int Duration() { return nDurationMSec_; }
  void Src(const String& sSrc) { sSrc_ = sSrc;}
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);

protected:
  int LoadData();
  int RequestData();
  int HasDataInCache();
  int GetDataFromCache();
  int SaveDataToCache();

  int RequestSuspended();
  void SuspendRequest();

protected:
  friend class GalileoModuleTester;

  GalileoModule* pModule_;
  String sSrc_;
  Buffer sbData_;
  String sMimeType_;
  int bDataBroken_;
  Apollo::TimeValue tRequestSuspendedTime_;
  int nRequestSuspendDelaySec_;
  int bLoaded_;
  int nDurationMSec_;
  int nFramesCount_;
};

// ------------------------------------------------------------

class Sequence: public ListT<Animation, Elem>
{
public:
  Sequence(const String& sName, GalileoModule* pModule, const String& sGroup, const String& sType, const String& sCondition, int nProbability, const String& sIn, const String& sOut, int nDx, int nDy)
    :ListT<Animation, Elem>(sName)
    ,pModule_(pModule)
    ,sGroup_(sGroup)
    ,sType_(sType)
    ,sCondition_(sCondition)
    ,nProbability_(nProbability)
    ,sIn_(sIn)
    ,sOut_(sOut)
    ,nDx_(nDx)
    ,nDy_(nDy)
    ,nDurationMSec_(1000)
    ,bLoaded_(0)
  {}

  void AppendAnimation(Animation* pAnimation);
  Frame* GetFrameByTime(int nTimeMSec);
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);

  int Probability() { return nProbability_; }
  int Duration() { return nDurationMSec_; }
  String& Group() { return sGroup_; }
  void Load();
  int IsLoaded() { return bLoaded_; }

protected:
  friend class GalileoModuleTester;

  GalileoModule* pModule_;
  String sGroup_;
  String sType_;
  String sCondition_;
  int nProbability_;
  String sIn_;
  String sOut_;
  int nDx_;
  int nDy_;
  int nDurationMSec_;
  int bLoaded_;
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
  int GetProbabilitySum() { return nTotalProbability_; }
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);

protected:
  int nTotalProbability_;
  String sType_;
};

// ------------------------------------------------------------

class Item
{
public:
  Item(const ApHandle& hItem, GalileoModule* pModule)
    :hAp_(hItem)
    ,pModule_(pModule)
    ,bStarted_(0)
    ,nDelayMSec_(100)
    //,nX_(0)
    //,nDestX_(0)
    ,pCurrentSequence_(0)
    ,nSpentInCurrentSequenceMSec_(0)
    ,pPreviousFrame_(0)
  {}

  virtual ~Item();

  int Start();
  void Stop();
  void SetDelay(int nDelayMSec);
  void SetData(Buffer& sbData, const String& sUrl);
  void SetStatus(const String& sStatus);
  void SetCondition(const String& sCondition);
  void PlayEvent(const String& sEvent);
  //void SetPosition(int nX);
  //void MoveTo(int nX);
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);

  int HasTimer(const ApHandle& hTimer) { return ApIsHandle(hTimer) && hTimer_ == hTimer; }
  void OnTimer();

protected:
  void ResetAnimations();
  void ParseParamNode(Apollo::XMLNode* pNode);
  Group* GetOrCreateGroup(const String& sGroup);
  void ParseSequenceNode(Apollo::XMLNode* pNode);

  void Step(Apollo::TimeValue& tvCurrent);

  Sequence* SelectNextSequence();
  Sequence* GetSequenceByName(const String& sSequence);
  Sequence* GetSequenceByGroup(const String& sGroup);
  Sequence* GetSequenceByGroupOrName(const String& sDesignation);
  String GetDefaultSequence();

  int StartTimer();
  void StopTimer();

protected:
  friend class GalileoModuleTester;

  GalileoModule* pModule_;
  ApHandle hAp_;
  int bStarted_;
  int nDelayMSec_; // msec
  ApHandle hTimer_;
  String sBaseUrl_;
  String sDefaultSequence_;
  String sDefaultStatus_;
  ListT<Group, Elem> lGroups_;
  String sStatus_;
  String sEvent_;
  String sActivity_;
  String sCondition_;
  String sNextSequence_; // enforce, e.g. after transition
  //int nX_;
  //int nDestX_;
  Sequence* pCurrentSequence_;
  int nSpentInCurrentSequenceMSec_;
  Apollo::TimeValue tvLastTimer_;
  Frame* pPreviousFrame_;
};

#endif // Item_H_INCLUDED