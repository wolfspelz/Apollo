// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgAnimation.h"
#include "MsgDB.h"
#include "Local.h"
#include "Item.h"
#include "ximagif.h"
#include "Image.h"
#include "GalileoModule.h"

void Animation::SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType)
{
  if (!IsLoaded()) {
    if (sUrl == sSrc_) {
      sbData_ = sbData;
      sMimeType_ = sMimeType;
      bDataBroken_ = 0;
    }
  }
}

void Animation::Load()
{
  if (IsLoaded()) {
    // do nothing
  } else {
    if (HasData()) {
      if (IsBroken()) {
        // do nothing
      } else {
        LoadData();
      }
    } else {
      if (HasDataInCache()) {
        GetDataFromCache();
        if (HasData()) {
          LoadData();
        } else {
          RequestData();
        }
      } else {
        RequestData();
      }
    }
  }
}

int Animation::RequestSuspended()
{
  if (tRequestSuspendedTime_.isNull()) { return 0; }

  Apollo::TimeValue tSuspended = Apollo::getNow() - tRequestSuspendedTime_;
  if (tSuspended.Sec() > nRequestSuspendDelaySec_) {
    tRequestSuspendedTime_ = 0;
    return 0;
  }

  return 1;
}

void Animation::SuspendRequest()
{
  tRequestSuspendedTime_ = Apollo::getNow();
  if (nRequestSuspendDelaySec_ == 0) {
    nRequestSuspendDelaySec_ = Apollo::getModuleConfig(MODULE_NAME, "MinSuspendRequestSec", 20);
  } else {
    nRequestSuspendDelaySec_ *= 2;
    int nMaxSuspendRequestSec = Apollo::getModuleConfig(MODULE_NAME, "MaxSuspendRequestSec", 1800);
    if (nRequestSuspendDelaySec_ > nMaxSuspendRequestSec) {
      nRequestSuspendDelaySec_ = nMaxSuspendRequestSec;
    }
  }

  apLog_Verbose((LOG_CHANNEL, "Animation::SuspendRequest", "for %d sec (%s)", nRequestSuspendDelaySec_, StringType(sSrc_)));
}

int Animation::RequestData()
{
  int ok = 0;

  if (!RequestSuspended()) {
    SuspendRequest();

    Msg_Galileo_RequestAnimation msg;
    msg.hRequest = Apollo::newHandle();
    msg.sUrl = sSrc_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Animation::RequestData", "Msg_Galileo_RequestAnimation failed: url=%s", StringType(sSrc_)));
    }
  }

  return ok;
}

int Animation::HasDataInCache()
{
  int bAvailable = 0;

  if (sSrc_) {
    Msg_Galileo_IsAnimationDataInStorage msg;
    msg.sUrl = sSrc_;
    int ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Animation::HasDataInCache", "Msg_Galileo_IsAnimationDataInStorage failed: url=%s", StringType(sSrc_)));
    } else {
      bAvailable = msg.bAvailable;
    }
  }

  return bAvailable;
}

int Animation::GetDataFromCache()
{
  int ok = 0;

  if (sSrc_) {
    Msg_Galileo_LoadAnimationDataFromStorage msg;
    msg.sUrl = sSrc_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Animation::GetDataFromCache", "Msg_Galileo_LoadAnimationDataFromStorage failed: url=%s", StringType(sSrc_)));
    } else {
      sbData_ = msg.sbData;
      sMimeType_ = msg.sMimeType;
    }
  }

  return ok;
}

int Animation::SaveDataToCache()
{
  int ok = 0;

  if (sSrc_) {
    Msg_Galileo_SaveAnimationDataToStorage msg;
    msg.sUrl = sSrc_;
    msg.sbData = sbData_;
    msg.sMimeType = sMimeType_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "Animation::SaveDataToCache", "Msg_Galileo_SaveAnimationDataToStorage failed: url=%s", StringType(sSrc_)));
    }
  }

  return ok;
}

int Animation::LoadData()
{
  int ok = 0;

  CxImage img(sbData_.Data(), sbData_.Length(), CXIMAGE_FORMAT_GIF);
  img.SetRetreiveAllFrames(true);
  int nFrames = img.GetNumFrames();
	img.SetFrame(nFrames - 1);

  if (!img.Decode(sbData_.Data(), sbData_.Length(), CXIMAGE_FORMAT_GIF)) {
    apLog_Warning((LOG_CHANNEL, "Animation::LoadData", "Decode failed"));
  } else {
    apLog_Verbose((LOG_CHANNEL, "Animation::LoadData", "Decode successful src=%s #frames=%d", StringType(sSrc_), nFrames));
    bLoaded_ = 0;

    for (int i = 0; i < nFrames; i++) {

      // Not allocated here. Just points into an internal sub-image list
      CxImage* pImgFrame = img.GetFrame(i);
      if (pImgFrame) {

        Frame* pFrame = new Frame();
        if (pFrame) {
          pFrame->img_.Allocate(pImgFrame->GetWidth(), pImgFrame->GetHeight());
          CxMemFile mfDest((BYTE*) pFrame->img_.Pixels(), pFrame->img_.Size());
          pImgFrame->AlphaFromTransparency();
          pImgFrame->Encode2RGBA(&mfDest, true);

          int nDuration = (int) pImgFrame->GetFrameDelay() * 10; // in 1/100 s
          if (nDuration == 0) {
            nDuration = Apollo::getModuleConfig(MODULE_NAME, "DefaultFrameDuration", 1000);
          }
          pFrame->nDurationMSec_ = nDuration;

          AppendFrame(pFrame);
          bLoaded_ = 1;
        }

      } // if (pImgFrame)
    } // for nFrames

    if (bLoaded_) {
      ok = 1;

      if (!HasDataInCache()) {
        SaveDataToCache();
      }

      sbData_.Empty();
      bDataBroken_ = 0;
      if (nDurationMSec_ == 0) {
        nDurationMSec_ = Apollo::getModuleConfig(MODULE_NAME, "DefaultAnimationDuration", 1000);
      }

    } else {
      bDataBroken_ = 1;
    }
  } // img.Decode

  return ok;
}

void Animation::AppendFrame(Frame* pFrame)
{
  nDurationMSec_ += pFrame->nDurationMSec_;
  nFramesCount_++;

  AddLast(pFrame);
}

// ------------------------------------------------------------

void Sequence::AppendAnimation(Animation* pAnimation)
{
  //nDurationMSec_ += pAnimation->Duration();
  AddLast(pAnimation);
}

Frame* Sequence::GetFrameByTime(int nTimeMSec)
{
  Frame* pFrame = 0;

  int nAccumulatedTime = 0;
  for (Animation* pAnimation = 0; (pAnimation = Next(pAnimation)) != 0 && pFrame == 0; ) {
    for (Frame* p = 0; (p = pAnimation->Next(p)) != 0 && pFrame == 0; ) {
      nAccumulatedTime += p->nDurationMSec_;
      if (nTimeMSec <= nAccumulatedTime) {
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

void Sequence::SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType)
{
  for (Animation* pAnimation = 0; (pAnimation = Next(pAnimation)) != 0; ) {
    pAnimation->SetAnimationData(sUrl, sbData, sMimeType);
  }
}

void Sequence::Load()
{
  if (!IsLoaded()) {
    int bLoaded = 1;
    int nDuration = 0;
    for (Animation* pAnimation = 0; (pAnimation = Next(pAnimation)) != 0; ) {
      pAnimation->Load();
      bLoaded = bLoaded && pAnimation->IsLoaded();
      nDuration += pAnimation->Duration();
    }
    if (this->length() > 0) {
      bLoaded_ = bLoaded;
      nDurationMSec_ = nDuration;
    }
  }
}

// ------------------------------------------------------------

void Group::AddSequence(Sequence* pSequence)
{
  nTotalProbability_ += pSequence->Probability();
  AddLast(pSequence);
}

Sequence* Group::GetRandomSequence(int nRnd)
{
  Sequence* pSequence = 0;

  int nProbability = 0;
  for (Sequence* p = 0; (p = Next(p)) != 0; ) {
    nProbability += p->Probability();
    if (nRnd < nProbability) {
      pSequence = p;
      break;
    }
  }

  return pSequence;
}

void Group::SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType)
{
  for (Sequence* pSequence = 0; (pSequence = Next(pSequence)) != 0; ) {
    pSequence->SetAnimationData(sUrl, sbData, sMimeType);
  }
}

// ------------------------------------------------------------

#define Item_Activity_Move "move"
#define Item_Group_Standard "idle"
#define Item_Group_MoveRight "moveright"
#define Item_Group_MoveLeft "moveleft"

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
  if (bStarted_) {
    StopTimer();

    Sequence* pSequence_ = pCurrentSequence_;
    pCurrentSequence_ = 0;

    if (pSequence_) {
      Msg_Animation_SequenceEnd msg;
      msg.hItem = hAp_;
      msg.sName = pSequence_->getName();
      msg.Send();
    }

    bStarted_ = 0;
  }
}

void Item::SetDelay(int nDelayMSec)
{
  nDelayMSec_ = nDelayMSec;
}

void Item::SetData(Buffer& sbData, const String& sUrl)
{
  Apollo::XMLProcessor xml;
  if (!xml.XmlText((const char*) sbData.Data(), sbData.Length(), 1)) { throw ApException("xml.Parse() failed, hItem=" ApHandleFormat " %d bytes: %s", ApHandleType(hAp_), sbData.Length(), StringType(xml.GetErrorString())); }

  Apollo::XMLNode* pRoot = xml.Root();
  if (pRoot == 0) { throw ApException("xml.Root() == 0, hItem=" ApHandleFormat " %d bytes", ApHandleType(hAp_), sbData.Length()); }

  String sNs = pRoot->getAttribute("xmlns").getValue();
  String sVersion = pRoot->getAttribute("version").getValue();
  if (sNs != "http://schema.bluehands.de/character-config" || sVersion != "1.0") { throw ApException("Wrong xmlns=%s or version=%s, hItem=" ApHandleFormat "", StringType(sNs), StringType(sVersion), ApHandleType(hAp_)); }

  ResetAnimations();

  sBaseUrl_ = String::filenameBasePath(sUrl);

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
  sEvent_ = sEvent;
}

//void Item::SetPosition(int nX)
//{
//  nX_ = nX;
//}
//
//void Item::MoveTo(int nX)
//{
//  nDestX_ = nX;
//  sActivity_ = Item_Activity_Move;
//}

void Item::SetAnimationData(const String& sUrl, Buffer& sbData, const String& sMimeType)
{
  for (Group* pGroup = 0; (pGroup =lGroups_.Next(pGroup)) != 0; ) {
    pGroup->SetAnimationData(sUrl, sbData, sMimeType);
  }
}

// ------------------------------------------------------------

void Item::ResetAnimations()
{
  sDefaultSequence_ = "still";
  sDefaultStatus_ = Item_Group_Standard;
  lGroups_.Empty();
}

void Item::ParseParamNode(Apollo::XMLNode* pNode)
{
  String sName = pNode->getAttribute("name").getValue();
  String sValue = pNode->getAttribute("value").getValue();
  if (0) {
  } else if (sName == "defaultsequence") {
    sDefaultSequence_ = sValue;
  } else if (sName == "defaultstatus") {
    sDefaultStatus_ = sValue;
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
  int nProbability = String::atoi(sProbability);
  if (nProbability <= 0) { nProbability = 100; }

  if (sName) {
    Sequence* pSequence = new Sequence(sName, pModule_, sGroup, sType, sCondition, nProbability, sIn, sOut, String::atoi(sDx), String::atoi(sDy));
    if (pSequence) {
      for (Apollo::XMLNode* pChild = 0; (pChild = pNode->nextChild(pChild)) != 0; ) {
        if (0) {
        } else if (pChild->getName() == "animation") {
          String sSrc = pChild->getAttribute("src").getValue();
          if (sSrc) {

            if (sSrc.startsWith("http:") || sSrc.startsWith("https:")) {
              // Is absolute URL
            } else {
              sSrc = sBaseUrl_ + sSrc;
            }

            Animation* pAnimation = new Animation(pModule_);
            if (pAnimation) {
              pAnimation->Src(sSrc);
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

  tvLastTimer_ = Apollo::TimeValue::getTime();

  hTimer_ = Apollo::startInterval(0, nDelayMSec * 1000);
  if (ApIsHandle(hTimer_)) {
    ok = 1;
  }

  return ok;
}

void Item::StopTimer()
{
  if (ApIsHandle(hTimer_)) {
    Apollo::cancelInterval(hTimer_);
    hTimer_ = ApNoHandle;
  }
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

  Sequence* pPreviousSequence = 0;
  Sequence* pNextSequence = 0;

  if (pCurrentSequence_ == 0) {

      pNextSequence = SelectNextSequence();
      nSpentInCurrentSequenceMSec_ = 0;

  } else {

    Apollo::TimeValue tvDelay = tvCurrent - tvLastTimer_;
    int nInSequenceMSec = nSpentInCurrentSequenceMSec_ + tvDelay.MilliSec();

    if (nInSequenceMSec > pCurrentSequence_->Duration()) {
      pPreviousSequence = pCurrentSequence_;

      pNextSequence = SelectNextSequence();
      if (pNextSequence) {
        nInSequenceMSec -= pCurrentSequence_->Duration();
        if (nInSequenceMSec > pNextSequence->Duration()) {
          pNextSequence = SelectNextSequence();
          nInSequenceMSec = 0;
        }
      }
    }

    nSpentInCurrentSequenceMSec_ = nInSequenceMSec;
  }

  tvLastTimer_ = tvCurrent;

  if (pPreviousSequence) {
    // Async, because Msg_Animation_SequenceEnd may delete this
    ApAsyncMessage<Msg_Animation_SequenceEnd> msg;
    msg->hItem = hAp_;
    msg->sName = pPreviousSequence->getName();
    msg->sGroup = pPreviousSequence->Group();
    msg.Post();
  }

  if (pNextSequence) {
    // Async, because Msg_Animation_SequenceBegin may delete this
    // and Msg_Animation_SequenceBegin should be after Msg_Animation_SequenceEnd
    ApAsyncMessage<Msg_Animation_SequenceBegin> msg;
    msg->hItem = hAp_;
    msg->sName = pNextSequence->getName();
    msg->sGroup = pNextSequence->Group();
    msg.Post();

    pCurrentSequence_ = pNextSequence;
  }

  if (pCurrentSequence_ && pCurrentSequence_->IsLoaded()) {
    Frame* pFrame = pCurrentSequence_->GetFrameByTime(nSpentInCurrentSequenceMSec_);
    if (pFrame) {
      int bFrameChanged = 1;
      if (pPreviousFrame_ == pFrame) {
        bFrameChanged = 0;
      }

      if (bFrameChanged) {
        Msg_Animation_Frame msg;
        msg.hItem = hAp_;
        msg.iFrame.CopyReference(pFrame->img_);
        msg.Send();
      }

      pPreviousFrame_ = pFrame;
    }

  } // if (pNextSequence)

}

Sequence* Item::SelectNextSequence()
{
  Sequence * pSequence = 0;
  String sSequence;
  int bFinal = 0;

  if (!sSequence) {
    if (sNextSequence_) {
      sSequence = sNextSequence_;
      sNextSequence_ = "";
      bFinal = 1;
      apLog_Verbose((LOG_CHANNEL, "Item::SelectNextSequence", "item=" ApHandleFormat " selecting next definitive seq=%s", ApHandleType(hAp_), StringType(sSequence)));
    }
  }

  //if (!sSequence) {
  //  if (sActivity_) {
  //    if (sActivity_ == Item_Activity_Move) {
  //      if (nX_ == nDestX_) {
  //        // back to status or event
  //      } else if (nX_ > nDestX_) {
  //        sSequence = Item_Group_MoveLeft;
  //      } else if (nX_ < nDestX_) {
  //        sSequence = Item_Group_MoveRight;
  //      }
  //    }
  //  }
  //}

  if (!sSequence) {
    if (sEvent_) {
      sSequence = sEvent_;
      sEvent_ = "";
      apLog_Verbose((LOG_CHANNEL, "Item::SelectNextSequence", "item=" ApHandleFormat " selecting event seq=%s", ApHandleType(hAp_), StringType(sSequence)));
    }
  }

  if (!sSequence) {
    if (sStatus_) {
      sSequence = sStatus_;
      apLog_VeryVerbose((LOG_CHANNEL, "Item::SelectNextSequence", "item=" ApHandleFormat " selecting status seq=%s", ApHandleType(hAp_), StringType(sSequence)));
    }
  }

  if (!bFinal) {
    if (pCurrentSequence_) {
      if (pCurrentSequence_->Group() != sSequence) {
        String sTransitionSequence = pCurrentSequence_->Group() + "2" + sSequence;
        Sequence* pTransitionSequence = GetSequenceByGroupOrName(sTransitionSequence);
        if (pTransitionSequence) {
          sNextSequence_ = sSequence;
          sSequence = sTransitionSequence;
          apLog_Verbose((LOG_CHANNEL, "Item::SelectNextSequence", "item=" ApHandleFormat " selecting transition seq=%s", ApHandleType(hAp_), StringType(sSequence)));
        }
      }
    }
  }

  if (!bFinal) {
    String sModified;

    if (!sModified) {
      if (sCondition_ && sStatus_) {
        sModified = sSequence + "@" + sStatus_ + "#" + sCondition_;
        if (GetSequenceByGroupOrName(sModified) == 0) {
          sModified = "";
        }
      }
    }

    if (!sModified) {
      if (sStatus_) {
        sModified = sSequence + "@" + sStatus_;
        if (GetSequenceByGroupOrName(sModified) == 0) {
          sModified = "";
        }
      }
    }

    if (!sModified) {
      if (sCondition_) {
        sModified = sSequence + "#" + sCondition_;
        if (GetSequenceByGroupOrName(sModified) == 0) {
          sModified = "";
        }
      }
    }

    if (sModified) {
      sSequence = sModified;
      apLog_VeryVerbose((LOG_CHANNEL, "Item::SelectNextSequence", "item=" ApHandleFormat " selecting modified seq=%s", ApHandleType(hAp_), StringType(sSequence)));
    }
  }

  if (!sSequence) {
    sSequence = GetDefaultSequence();
    apLog_VeryVerbose((LOG_CHANNEL, "Item::SelectNextSequence", "item=" ApHandleFormat " selecting default seq=%s", ApHandleType(hAp_), StringType(sSequence)));
  }

  pSequence = GetSequenceByGroupOrName(sSequence);
  if (pSequence == 0) {
    pSequence = GetSequenceByName(sDefaultSequence_);
  }

  if (pSequence) {
    if (!pSequence->IsLoaded()) {
      pSequence->Load();
    }
  }

  return pSequence;
}

String Item::GetDefaultSequence()
{
  String s = sDefaultSequence_;

  Sequence* pSequence = GetSequenceByName(s);

  if (pSequence == 0) {
    pSequence = GetSequenceByGroup(s);
  }

  if (pSequence == 0) {
    pSequence = GetSequenceByGroup(Item_Group_Standard);
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
    int nSum = pGroup->GetProbabilitySum();
    int nRnd = Apollo::getRandom(nSum);
    pSequence = pGroup->GetRandomSequence(nRnd);
    if (pSequence) {
      apLog_VeryVerbose((LOG_CHANNEL, "Item::GetSequenceByGroup", "item=" ApHandleFormat " sum=%d rnd=%d %s -> %s", ApHandleType(hAp_), nSum, nRnd, StringType(sGroup), StringType(pSequence->getName())));
    }
  }

  return pSequence;
}

Sequence* Item::GetSequenceByGroupOrName(const String& sDesignation)
{
  Sequence* pSequence = GetSequenceByGroup(sDesignation);
  if (pSequence == 0) {
    pSequence = GetSequenceByName(sDesignation);
  }

  return pSequence;
}
