// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgAnimation.h"
#include "Local.h"
#include "Item.h"
#include "ximagif.h"
#include "Image.h"

void Animation::AppendFrame(Frame* pFrame)
{
  nDurationMSec_ += pFrame->nDurationMSec_;
  nFramesCount_++;

  AddLast(pFrame);
}

void Animation::Load()
{
  CxImage img(sbData_.Data(), sbData_.Length(), CXIMAGE_FORMAT_GIF);
  img.SetRetreiveAllFrames(true);
  int nFrames = img.GetNumFrames();
	img.SetFrame(nFrames - 1);

  if (!img.Decode(sbData_.Data(), sbData_.Length(), CXIMAGE_FORMAT_GIF)) {
    apLog_Warning((LOG_CHANNEL, "Animation::Load", "Decode failed"));
  } else {
    sbData_.Empty();

    for (int i = 0; i < nFrames; i++) {

      // Not allocated here. Just points into an internal sub-image list
      CxImage* pImgFrame = img.GetFrame(i);
      if (pImgFrame) {

        Frame* pFrame = new Frame();
        if (pFrame) {
          pFrame->img_.Allocate(pImgFrame->GetWidth(), pImgFrame->GetHeight());
          CxMemFile mfDest((BYTE*) pFrame->img_.Pixels(), pFrame->img_.Size());
          pImgFrame->Encode2RGBA(&mfDest);
          pFrame->nDurationMSec_ = (int) pImgFrame->GetFrameDelay() * 10; // in 1/100 s
          
          AppendFrame(pFrame);
          bLoaded_ = 1;
        }

      } // if (pImgFrame)
    } // for nFrames
  } // img.Decode
}

// ------------------------------------------------------------

void Sequence::AppendAnimation(Animation* pAnimation)
{
  nDurationMSec_ += pAnimation->nDurationMSec_;
  AddLast(pAnimation);
}

Frame* Sequence::GetFrameByTime(int nTimeMSec)
{
  Frame* pFrame = 0;

  int nAccumulatedTime = 0;
  for (Animation* pAnimation = 0; (pAnimation = Next(pAnimation)) != 0 && pFrame == 0; ) {
    for (Frame* p = 0; (p = pAnimation->Next(p)) != 0 && pFrame == 0; ) {
      nAccumulatedTime += p->nDurationMSec_;
      if (nTimeMSec < nAccumulatedTime) {
        pFrame = p;
      }
    }
  }

  return pFrame;
}

//Animation* Sequence::GetAnimationByTime(int nTimeMSec)
//{
//  Animation* pAnimation = 0;
//
//  for (Animation* p = 0; (p = Next(p)) != 0; ) {
//    nTime += p->nDurationMSec_;
//    if (nTimeMSec < nTime) {
//      pAnimation = p;
//      break;
//    }
//  }
//
//  return pAnimation;
//}

// ------------------------------------------------------------

void Group::AddSequence(Sequence* pSequence)
{
  nTotalProbability_ += pSequence->nProbability_;
  AddLast(pSequence);
}

Sequence* Group::GetRandomSequence(int nRnd)
{
  Sequence* pSequence = 0;

  int nProbability = 0;
  for (Sequence* p = 0; (p = Next(p)) != 0; ) {
    nProbability += p->nProbability_;
    if (nRnd < nProbability) {
      pSequence = p;
      break;
    }
  }

  return pSequence;
}

// ------------------------------------------------------------

Item::~Item()
{
  if (bStarted_) {
    Stop();
  }
}

int Item::Start()
{
  int ok = 1;
  
  ok = StartTimer();
  if (ok) {
    bStarted_ = 1;
  }

  return ok;
}

void Item::Stop()
{
  StopTimer();

  if (pCurrentSequence_) {
    Msg_Animation_SequenceEnd msg;
    msg.hItem = hAp_;
    msg.sName = pCurrentSequence_->getName();
    msg.Send();
  }

  bStarted_ = 0;
}

void Item::SetDelay(int nDelayMSec)
{
  nDelayMSec_ = nDelayMSec;
}

void Item::SetData(Buffer& sbData)
{
  Apollo::XMLProcessor xml;
  if (!xml.XmlText((const char*) sbData.Data(), sbData.Length(), 1)) { throw ApException("xml.Parse() failed, hItem=" ApHandleFormat " %d bytes: %s", ApHandleType(hAp_), sbData.Length(), StringType(xml.GetErrorString())); }

  Apollo::XMLNode* pRoot = xml.Root();
  if (pRoot == 0) { throw ApException("xml.Root() == 0, hItem=" ApHandleFormat " %d bytes", ApHandleType(hAp_), sbData.Length()); }

  String sNs = pRoot->getAttribute("xmlns").getValue();
  String sVersion = pRoot->getAttribute("version").getValue();
  if (sNs != "http://schema.bluehands.de/character-config" || sVersion != "1.0") { throw ApException("Wrong xmlns=%s or version=%s, hItem=" ApHandleFormat "", StringType(sNs), StringType(sVersion), ApHandleType(hAp_)); }

  ResetAnimations();

  for (Apollo::XMLNode* pChild = 0; (pChild = pRoot->nextChild(pChild)) != 0; ) {
    if (0) {
    } else if (pChild->getName() == "param") {
      ParseParamNode(pChild);
    } else if (pChild->getName() == "sequence") {
      ParseSequenceNode(pChild);
    }
  }
}

void Item::SetStatus(const String& sStatus)
{
  sStatus_ = sStatus;
}

void Item::SetCondition(const String& sCondition)
{
  sCondition_ = sCondition;
}

void Item::PlayEvent(const String& sEvent)
{
  sNextEvent_ = sEvent;
}

void Item::SetPosition(int nX)
{
  nX_ = nX;
}

void Item::MoveTo(int nX)
{
  nDestX_ = nX;
}

// ------------------------------------------------------------

void Item::ResetAnimations()
{
  sDefaultSequence_ = "";
  lGroups_.Empty();
}

void Item::ParseParamNode(Apollo::XMLNode* pNode)
{
  String sName = pNode->getAttribute("name").getValue();
  String sValue = pNode->getAttribute("value").getValue();
  if (0) {
  } else if (sName == "defaultsequence") {
    sDefaultSequence_ = sValue;
  }
}

Group* Item::GetOrCreateGroup(const String& sGroup)
{
  Group* pGroup = lGroups_.FindByName(sGroup);
  if (pGroup == 0) {
    pGroup = new Group(sGroup);
    if (pGroup) {
      lGroups_.Add(pGroup);
    }
  }
  return pGroup;
}

void Item::ParseSequenceNode(Apollo::XMLNode* pNode)
{
  String sGroup = pNode->getAttribute("group").getValue();
  String sName = pNode->getAttribute("name").getValue();
  String sType = pNode->getAttribute("type").getValue();
  String sCondition = pNode->getAttribute("condition").getValue();
  String sProbability = pNode->getAttribute("probability").getValue();
  String sIn = pNode->getAttribute("in").getValue();
  String sOut = pNode->getAttribute("out").getValue();
  String sDx = pNode->getAttribute("dx").getValue();
  String sDy = pNode->getAttribute("dy").getValue();

  if (sName) {
    Sequence* pSequence = new Sequence(sName);
    if (pSequence) {
      pSequence->sType_ = sType;
      pSequence->sCondition_ = sCondition;
      pSequence->nProbability_ = String::atoi(sProbability);
      if (pSequence->nProbability_ <= 0) {
        pSequence->nProbability_ = 100;
      }
      pSequence->sIn_ = sIn;
      pSequence->sOut_ = sOut;
      pSequence->nDx_ = String::atoi(sDx);
      pSequence->nDy_ = String::atoi(sDy);

      for (Apollo::XMLNode* pChild = 0; (pChild = pNode->nextChild(pChild)) != 0; ) {
        if (0) {
        } else if (pChild->getName() == "animation") {
          String sSrc = pChild->getAttribute("src").getValue();
          if (sSrc) {
            Animation* pAnimation = new Animation();
            if (pAnimation) {
              pAnimation->sSrc_ = sSrc;
              pSequence->AppendAnimation(pAnimation);
            }
          }
        }
      }

      Group* pGroup = GetOrCreateGroup(sGroup);
      if (pGroup) {
        pGroup->AddSequence(pSequence);
      }
    }
  }
}

int Item::StartTimer()
{
  int ok = 0;

  if (ApIsHandle(hTimer_)) {
    StopTimer();
  }

  int nDelayMSec = nDelayMSec_;
  if (nDelayMSec <= 0) {
    nDelayMSec = 100;
  }
  hTimer_ = Apollo::startInterval(0, nDelayMSec * 1000);
  if (ApIsHandle(hTimer_)) {
    ok = 1;
  }

  return ok;
}

void Item::StopTimer()
{
  Apollo::cancelInterval(hTimer_);
  hTimer_ = ApNoHandle;
}

// ------------------------------------------------------------

void Item::OnTimer()
{
  Apollo::TimeValue tvCurrent = Apollo::TimeValue::getTime();

  Step(tvCurrent);
}

void Item::Step(Apollo::TimeValue& tvCurrent)
{
  // Assumed there is a current sequence

  Apollo::TimeValue tvDelay = tvCurrent - tvLastTimer_;
  tvLastTimer_ = tvCurrent;

  int nInSequenceMSec = nSpentInCurrentSequenceMSec_ + tvDelay.MilliSec();
  if (nInSequenceMSec > 10000) {
    nInSequenceMSec = 1000;
  }

  int bSequenceChanged = 0;
  Sequence* pPreviousSequence = pCurrentSequence_;
  while (nInSequenceMSec > pCurrentSequence_->nDurationMSec_) {
    bSequenceChanged = 1;
    Sequence* pNextSequence = SelectNextSequence();
    int nRemainingInCurrentSequence = pCurrentSequence_->nDurationMSec_ - nSpentInCurrentSequenceMSec_;
    nInSequenceMSec -= nRemainingInCurrentSequence;
    pCurrentSequence_ = pNextSequence;
  }

  nSpentInCurrentSequenceMSec_ = nInSequenceMSec;

  if (bSequenceChanged) {
    if (pPreviousSequence) {
      Msg_Animation_SequenceEnd msg;
      msg.hItem = hAp_;
      msg.sName = pPreviousSequence->getName();
      msg.Send();
    }

    {
      Msg_Animation_SequenceBegin msg;
      msg.hItem = hAp_;
      msg.sName = pCurrentSequence_->getName();
      msg.Send();
    }
  }

  Frame* pFrame = pCurrentSequence_->GetFrameByTime(nSpentInCurrentSequenceMSec_);
  int bFrameChanged = 1;
  if (pPreviousFrame_ == pFrame) {
    bFrameChanged = 0;
  }

  if (bFrameChanged) {
    Msg_Animation_Frame msg;
    //
  }

  pPreviousFrame_ = pFrame;
}

Sequence* Item::SelectNextSequence()
{
  Sequence * pSequence = 0;

  Task* pTask = lTasks_.First();
  int bDispose = 0;
  pTask->GetSequence(*this, bDispose);
  if (bDispose) {
    lTasks_.Remove(pTask);
    delete pTask;
    pTask = 0;
  }

  if (pSequence == 0) {
    pSequence = GetSequenceByName(sDefaultSequence_);
  }

  return pSequence;
}

String Item::GetDefaultSequence()
{
  String s = sDefaultSequence_;

  Sequence* pSequence = GetSequenceByName(s);
  if (pSequence == 0) {
    pSequence = GetSequenceByGroup("idle");
  }
  if (pSequence == 0) {
    Group* pGroup = lGroups_.First();
    if (pGroup) {
      pSequence = pGroup->First();
    }
  }

  return s;
}

Sequence* Item::GetSequenceByName(const String& sSequence)
{
  Sequence* pSequence = 0;

  int bDone = 0;
  Group* pGroup = 0;
  while (!bDone) {
    pGroup = lGroups_.Next(pGroup);
    if (pGroup) {
      pSequence = pGroup->FindByName(sSequence);
      if (pSequence) {
        bDone = 1;
      }
    } else {
      bDone = 1;
    }
  }

  return pSequence;
}

Sequence* Item::GetSequenceByGroup(const String& sGroup)
{
  Sequence* pSequence = 0;

  Group* pGroup = lGroups_.FindByName(sGroup);
  if (pGroup) {
    int nRnd = Apollo::getRandom(pGroup->nTotalProbability_);
    pSequence = pGroup->GetRandomSequence(nRnd);
  }

  return pSequence;
}

// ------------------------------------------------------------

Sequence* StatusTask::GetSequence(Item& item, int& bDispose)
{
  Sequence* pSequence = 0;
  bDispose = 0;
  pSequence = item.GetSequenceByGroup(sStatus_);
  return pSequence;
}

Sequence* EventTask::GetSequence(Item& item, int& bDispose)
{
  Sequence* pSequence = 0;
  bDispose = 1;
  pSequence = item.GetSequenceByGroup(sEvent_);
  return pSequence;
}

//Sequence* MoveTask::GetSequence(Item& item, int& bDispose)
//{
//  Sequence* pSequence = 0;
//  bDispose = 1;
//  String sGroup;
//  if (nDestX_ > item.nX_) {
//    sGroup = "moveright";
//  } else if (nDestX_ < item.nX_) {
//    sGroup = "moveleft";
//  } else {
//    sGroup = "idle";
//  }
//  pSequence = item.GetSequenceByGroup(sGroup);
//  return pSequence;
//}

