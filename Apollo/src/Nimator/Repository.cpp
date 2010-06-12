// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Repository.h"

#include "ximagif.h"
#include "Image.h"

void AnimationData::AppendFrame(Frame* pFrame)
{
  nDurationMSec_ += pFrame->nDurationMSec_;
  nFramesCount_++;

  AddLast(pFrame);
}

void AnimationData::Load()
{
  CxImage img(sbData_.Data(), sbData_.Length(), CXIMAGE_FORMAT_GIF);
  img.SetRetreiveAllFrames(true);
  int nFrames = img.GetNumFrames();
	img.SetFrame(nFrames - 1);

  if (!img.Decode(sbData_.Data(), sbData_.Length(), CXIMAGE_FORMAT_GIF)) {
    apLog_Warning((LOG_CHANNEL, "AnimationData::Load", "Decode failed"));
  } else {

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

void AnimationData::Unload()
{
}

void AnimationData::AddRef()
{
  nCountUser_++;
}

void AnimationData::DeleteRef()
{
  nCountUser_--;
  if (nCountUser_ == 0) {
    Unload();
  }
}

// ------------------------------------------------------------

Repository::Repository()
{
}

Repository::~Repository()
{
}

AnimationData* Repository::Find(const String& sUrl)
{
  AnimationData* p = 0;
  return p;
}

AnimationData* Repository::Load(const String& sUrl)
{
  AnimationData* p = 0;
  return p;
}
