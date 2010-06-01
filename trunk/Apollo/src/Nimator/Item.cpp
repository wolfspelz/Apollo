// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Item.h"
#include "ximagif.h"
#include "Image.h"

void Animation::AppendFrame(Frame* pFrame)
{
  nTotalDurationMSec_ += pFrame->nDurationMSec_;
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
  AddLast(pAnimation);
}

// ------------------------------------------------------------

void Group::AddSequence(Sequence* pSequence)
{
  nTotalProbability_ += pSequence->nProbability_;
  AddLast(pSequence);
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
  sDefaultGroup_ = "";
  lGroups_.Empty();
}

void Item::ParseParamNode(Apollo::XMLNode* pNode)
{
  String sName = pNode->getAttribute("name").getValue();
  String sValue = pNode->getAttribute("value").getValue();
  if (0) {
  } else if (sName == "defaultsequence") {
    sDefaultGroup_ = sValue;
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
}

void Item::SelectSequence()
{
}

