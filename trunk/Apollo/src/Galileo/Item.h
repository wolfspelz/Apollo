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
  Animation(GalileoModule* pModule, const String& sSrc, int nDx, int nDy)
    :pModule_(pModule)
    ,sSrc_(sSrc)
    ,nDx_(nDx)
    ,nDy_(nDy)
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
  String Src() { return sSrc_;}
  int Dx() { return nDx_;}
  int Dy() { return nDy_;}
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);
  void GetAnimationData(Buffer& sbData, String& sMimeType);

  // public for Server_HttpRequest
  int HasDataInCache();
  int GetDataFromCache();
  void FlushData();

protected:
  int LoadData();
  int RequestData();
  int SaveDataToCache();

  int RequestSuspended();
  void SuspendRequest();

protected:
  friend class GalileoModuleTester;

  GalileoModule* pModule_;
  String sSrc_;
  int nDx_;
  int nDy_;
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
  Sequence(const String& sName, GalileoModule* pModule, const String& sGroup, const String& sType, const String& sCondition, int nProbability, const String& sIn, const String& sOut)
    :ListT<Animation, Elem>(sName)
    ,pModule_(pModule)
    ,sGroup_(sGroup)
    ,sType_(sType)
    ,sCondition_(sCondition)
    ,nProbability_(nProbability)
    ,sIn_(sIn)
    ,sOut_(sOut)
    ,nDurationMSec_(1000)
    ,bLoaded_(0)
  {}

  void AppendAnimation(Animation* pAnimation);
  Frame* GetFrameByTime(int nTimeMSec);
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);

  int Probability() { return nProbability_; }
  int Duration() { return nDurationMSec_; }
  String& Group() { return sGroup_; }
  String Src();
  int Dx();
  int Dy();
  void Load();
  int IsLoaded() { return bLoaded_; }
  void GetInfo(
                String& sGroup,
                String& sType,
                String& sCondition,
                String& sSrc,
                int& nProbability,
                String& sIn,
                String& sOut,
                int& nDx,
                int& nDy,
                int& nDuration
              );

protected:
  friend class GalileoModuleTester;

  GalileoModule* pModule_;
  String sGroup_;
  String sType_;
  String sCondition_;
  int nProbability_;
  String sIn_;
  String sOut_;
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
    ,bStatic_(0)
    ,bSwitchNow_(0)
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
  void PlayActivity(const String& sActivity);
  void PlayStatic(int bState);
  void SetPosition(int nX);
  void MoveTo(int nX);
  void SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType);
  void GetGroups(Apollo::ValueList& vlGroups);
  void GetGroupSequences(const String& sGroup, Apollo::ValueList& vlSequences);
  void GetSequenceInfo(const String& sSequence,
                              String& sGroup,
                              String& sType,
                              String& sCondition,
                              String& sSrc,
                              int& nProbability,
                              String& sIn,
                              String& sOut,
                              int& nDx,
                              int& nDy,
                              int& nDuration
                            );

  int HasTimer(const ApHandle& hTimer) { return ApIsHandle(hTimer) && hTimer_ == hTimer; }
  void OnTimer();

  // For Server_HttpRequest
  ListT<Group, Elem>& Groups() { return lGroups_; }
  inline ApHandle apHandle() { return hAp_; }
  String Src() { return sSrc_; }

protected:
  void ResetAnimations();
  void ParseParamNode(Apollo::XMLNode* pNode);
  Group* GetOrCreateGroup(const String& sGroup);
  void ParseSequenceNode(Apollo::XMLNode* pNode);

  void Step(Apollo::TimeValue& tvCurrent);

  Sequence* SelectNextSequence();
  Sequence* GetSequenceByGroup(const String& sGroup);
  Sequence* GetSequenceByName(const String& sSequence);
  Sequence* GetSequenceByGroupOrName(const String& sGroupOrName);
  Sequence* GetDefaultSequence();
  String GetDefaultSequenceName();

  int StartTimer();
  void StopTimer();

protected:
  friend class GalileoModuleTester;

  GalileoModule* pModule_;
  ApHandle hAp_;
  int bStarted_;
  int nDelayMSec_; // msec
  ApHandle hTimer_;
  String sSrc_;
  String sBaseUrl_;
  String sDefaultSequence_;
  String sDefaultStatus_;
  ListT<Group, Elem> lGroups_;
  String sStatus_;
  String sEvent_;
  String sActivity_;
  int bStatic_;
  int bSwitchNow_;
  //int nX_;
  //int nDestX_;
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
