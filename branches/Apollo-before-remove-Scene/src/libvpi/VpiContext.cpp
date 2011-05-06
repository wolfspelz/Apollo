// ============================================================================
//
// libvpi
//
// ============================================================================

#include "libvpi.h"
#include "vpiLog_Component.h"
#include "VpiContext.h"
#include "URL.h"
#include "RegEx.h"
#include "MessageDigest.h"
#include "XMLProcessor.h"

//----------------------------------------------------------

static String Rel2AbsUrl(String& sBase, String& sFile, int nLevel)
{
  String sAbs = sBase;

  String sPart;

  // Make base URL
  sAbs.reverseToken("/", sPart);
  // if it was a directory URL, then the / is now removed (and sPart would be empty)
  // if there was a file part, then the file is removed and the / also (sPart has the file)

  // Remove more levels
  while (nLevel > 0 && !sAbs.empty()) {
    nLevel--;
    sAbs.reverseToken("/", sPart);
  }

  #define DIRPATH_SAME "./"
  #define DIRPATH_PARENT "../"

  int bContinue = 0;
  do {
    bContinue = 0;

    if (sFile.startsWith(DIRPATH_SAME)) {
      String sDirPath = DIRPATH_SAME;
      String sTmp = sFile.c_str() + sDirPath.bytes();
      sFile = sTmp;
      bContinue = 1;
    }

    if (sFile.startsWith(DIRPATH_PARENT)) {
      URL url(sAbs);
      String sPath = url.path();
      if (sPath != "/") {
        sAbs.reverseToken("/", sPart);
        String sDirPath = DIRPATH_PARENT;
        String sTmp = sFile.c_str() + sDirPath.bytes();
        sFile = sTmp;
        bContinue = 1;
      }
    }

  } while (bContinue);

  // Add the directory /
  sAbs += "/";

  // And the file
  sAbs += sFile;

  return sAbs;
}

//----------------------------------------------------------

VpiFile::VpiFile(const String& sUrl)
:Elem(sUrl)
,bXmlParsed_(0)
,tCreated_(0)
,nLifetime_(3600)
{
}

VpiFile::~VpiFile()
{
}

int VpiFile::SetData(const String& sXml, time_t tCreated)
{
  int ok = 1;

  int nLifetime = vpiLog_ReadInt("ErrorTimeToLive", 60);

  sXml_ = sXml;
  (void) ParseXml();

  if (xml_.Root() != 0) {
    int nDefaultTTL = vpiLog_ReadInt("DefaultTimeToLive", 3600);
    int nMinTTL = vpiLog_ReadInt("MinTimeToLive", 20);
    int nTTL = nDefaultTTL;

    XMLNode* pInfo = xml_.Root()->getChildByPath("Info");
    if (pInfo != 0) {
      String sVersion = pInfo->getAttribute("Version").getValue();
      String sDescription = pInfo->getAttribute("Description").getValue();
      String sAdminContact = pInfo->getAttribute("AdminContact").getValue();
      String sTechContact = pInfo->getAttribute("TechContact").getValue();
      String sTimeToLive = pInfo->getAttribute("TimeToLive").getValue();
      String sExpiryTime = pInfo->getAttribute("ExpiryTime").getValue();
      String sRefreshInterval = pInfo->getAttribute("RefreshInterval").getValue();
      String sTransferRetryTime = pInfo->getAttribute("TransferRetryTime").getValue();

      if (sTimeToLive.empty()) {
        nTTL = nDefaultTTL;
      } else if (sTimeToLive == "0") {
        nTTL = nMinTTL;
      } else {
        nTTL = ::atoi(sTimeToLive);
        if (nTTL <= 0) {
          nTTL = nDefaultTTL;
        }
      }

      if (nTTL < nMinTTL) {
        nTTL = nMinTTL;
      }
    }

    nLifetime = nTTL;
  }

  tCreated_ = tCreated;
  nLifetime_ = nLifetime;

  return ok;
}

int VpiFile::Expired(time_t tNow)
{
  return tCreated_ + nLifetime_ < tNow;
}

int VpiFile::ParseXml()
{
  int ok = 0;

  if (bXmlParsed_) {
    ok = 1;
  } else {
    bXmlParsed_ = 1;
    if (!sXml_.empty()) {
      if (!xml_.XmlText(sXml_)) {
        vpiLog_Verbose((VPI_LOGCHANNEL, "VpiFile::ParseXml", "xml.XmlText() failed err:%s, xml:%s", StringType(xml_.GetErrorString()), StringType(String::truncate(sXml_, 100))));
      } else {
        ok = 1;
      }
    }
  }

  return ok;
}

VpiSearchResult VpiFile::CheckMatch(VpiSearchState& state)
{
  VpiSearchResult nResult = ResultNoMatch;

  (void) ParseXml();

  if (bXmlParsed_ && xml_.Root() != 0) {

    int bDone = 0;
    for (XMLNode* pChild = 0; (pChild = xml_.Root()->nextChild(pChild)) && !bDone; ) {
      String sName = pChild->getName();

      if (sName == "location" || sName == "delegate") {
        String sMatch = pChild->getAttribute("match").getValue();
        if (sMatch.empty()) {
          sMatch = ".*";
        }

        int bMatch = 0;
        RegEx reMatch;
        reMatch.Compile(sMatch);
        if (reMatch.Match(state.sOriginalUrl_) > 0) {
          bMatch = 1;
        }

        if (bMatch) {
          if (sName == "location") {
            state.sLocationXml_ = reMatch.Replace(pChild->outerXml());
            nResult = ResultMatch;
            bDone = 1;
          } else if (sName == "delegate") {
            XMLNode* pUri = pChild->getChild("uri");
            if (pUri != 0) {
              String sDelegateUrl = pUri->getCData();
              sDelegateUrl = reMatch.Replace(sDelegateUrl);

              if (sDelegateUrl.startsWith("http:") || sDelegateUrl.startsWith("https:")) {
                // Is absolute URL
              } else {
                // Relative URL -> make absolute
                sDelegateUrl = ::Rel2AbsUrl(state.sVpiUrl_, sDelegateUrl, 0);
              }
              state.sDelegateUrl_ = sDelegateUrl;
              nResult = ResultDelegate;
              bDone = 1;
            } // delegate uri
          } // delegate
        } // if bMatch
      } // location || delegate

    } // for nextChild

  } // if bXmlParsed_ && xml_.Root()

  return nResult;
}

//----------------------------------------------------------

VpiContext::VpiContext()
:nError_(VPI_SUCCESS)
,fpRequestFileCallback_(0)
,nRequestFileCallbackRef_(0)
,fpComposeLocationCallback_(0)
,nComposeLocationCallbackRef_(0)
,tNow_(0)
,tLastExpireFiles_(0)
{
}

VpiContext::~VpiContext()
{
}

void VpiContext::SetError(const String& sWhere, int nError, const String& sError)
{
  switch (nError) {
  case VPI_ERROR_UNKNOWN:
  case VPI_SUCCESS:
  case VPI_ERROR_TEST_FAILED:
    break;
  case VPI_ERROR_NEED_DATA:
  case VPI_ERROR_REQUESTED_DATA:
    {
      String s = "need ";
      s += sError;
      vpiLog_VeryVerbose((VPI_LOGCHANNEL, sWhere, s));
    }
    break;
  default:
    vpiLog_Error((VPI_LOGCHANNEL, sWhere, sError));
  }

  nError_ = nError;
  sError_ = sError;
}

void VpiContext::SetRequestFileCallback(VpiRequestFileCallback fpCallback, long nRef)
{
  fpRequestFileCallback_ = fpCallback;
  nRequestFileCallbackRef_ = nRef;
}

void VpiContext::SetComposeLocationCallback(VpiComposeLocationCallback fpCallback, long nRef)
{
  fpComposeLocationCallback_ = fpCallback;
  nComposeLocationCallbackRef_ = nRef;
}

int VpiContext::Clear()
{
  int ok = 1;

  lFiles_.Empty();
  tLastExpireFiles_ = 0;
  lRequestedFiles_.Empty();
  lStatus_.Empty();
  sSuffix_ = "";

  return ok;
}

void VpiContext::IncrementStatusCounter(const String& sKey)
{
  int n = lStatus_[sKey].getInt();
  lStatus_[sKey].setInt(n + 1);
}

int VpiContext::GetStatus(List& lStatus)
{
  int ok = 1;

  for (Elem* e = 0; (e = lStatus_.Next(e)); ) {
    lStatus.AddLast(e->getName(), String::from(e->getInt()));
  }

  {
    lStatus.AddLast("Files", String::from(lFiles_.length()));
  }
  {
    lStatus.AddLast("CurrentlyRequestedFiles", String::from(lRequestedFiles_.length()));
  }
  lStatus.AddLast("Suffix", sSuffix_);

  lStatus.AddLast("LibMajorVersion", VPI_LIB_MAJORVERSION);
  lStatus.AddLast("LibMinorVersion", VPI_LIB_MINORVERSION);
  lStatus.AddLast("LibName", VPI_LIB_NAME);
  lStatus.AddLast("LibDescription", VPI_LIB_DESCRIPTION);

  return ok;
}

int VpiContext::GetTraversedFiles(List& lFiles)
{
  int ok = 1;

  lFiles = lTraversedFiles_;

  return ok;
}

int VpiContext::SetXmlFile(const String& sUrl, const String& sXml)
{
  int ok = 1;

  IncrementStatusCounter("FilesReceived");

  Elem* eRequest = lRequestedFiles_.FindByName(sUrl);
  if (eRequest != 0) {
    lRequestedFiles_.Remove(eRequest);
    delete eRequest;
    eRequest = 0;
  }

  VpiFile* eFile = lFiles_.FindByName(sUrl);
  if (eFile != 0) {
    lFiles_.Remove(eFile);
    delete eFile;
    eFile = 0;
  }

  eFile = new VpiFile(sUrl);
  if (eFile != 0) {
    eFile->SetData(sXml, tNow_);
    lFiles_.Add(eFile);
  }

  return ok;
}

int VpiContext::ExpireFiles()
{
  int ok = 1;

  if (tNow_ > tLastExpireFiles_) {

    for (VpiFile* eFile = 0; (eFile = lFiles_.Next(eFile)); ) {
      if (eFile->Expired(tNow_)) {
        IncrementStatusCounter("FilesExpired");
        lFiles_.Remove(eFile);
        delete eFile;
        eFile = 0; // restart
      }
    }

    tLastExpireFiles_ = tNow_;
  }

  return ok;
}

VpiSearchResult VpiContext::CheckMatch(VpiSearchState& state)
{
  VpiSearchResult nResult = NoResult;

  VpiFile* eFile = lFiles_.FindByName(state.sVpiUrl_);
  if (eFile == 0) {
    nResult = ResultMissingData;
  } else {
    lTraversedFiles_.AddLast(eFile->getName());
    nResult = eFile->CheckMatch(state);
  }

  return nResult;
}

int VpiContext::GetLocationXml(String& sUrl, String& sLocationXml)
{
  int ok = 0;
  SetError("", VPI_ERROR_UNKNOWN, "");
  vpiLog_VeryVerbose((VPI_LOGCHANNEL, "VpiContext::GetLocationXml", "for url=%s", StringType(sUrl)));

  IncrementStatusCounter("GetLocationXml");
  lTraversedFiles_.Empty();

  (void) ExpireFiles();

  int bDone = 0;

  URL uOriginal(sUrl);
  if (uOriginal.protocol().empty() || uOriginal.host().empty()) {
    String s;
    s.appendf("Invalid URL: %s", StringType(String::truncate(sUrl, 100)));
    SetError("VpiContext::GetLocationXml", VPI_ERROR_INVALID_URL, s);
  } else {

    VpiSearchPhase nPhase = PhaseLocal;

    VpiSearchState state;
    state.sOriginalUrl_ = sUrl;
    state.sVpiUrl_.appendf("%s://%s%s%s%s", StringType(uOriginal.protocol()), StringType(uOriginal.host()), StringType(uOriginal.port()), StringType(uOriginal.path()), StringType("_vpi.xml"));

    while (!bDone) {
      VpiSearchResult nResult = CheckMatch(state);

      switch (nResult) {
      case ResultMissingData:
        {
          // Need a vpi file
          // URL: sVpiUrl_

          if (lRequestedFiles_.FindByName(state.sVpiUrl_) != 0) {
            // Already requested. Pretend to request it, because it will come anyway

            SetError("VpiContext::GetLocationXml", VPI_ERROR_REQUESTED_DATA, state.sVpiUrl_);

          } else {
            // Not yet requested, then request now

            if (fpRequestFileCallback_ != 0) {
              lRequestedFiles_.AddLast(state.sVpiUrl_);
              if (fpRequestFileCallback_(nRequestFileCallbackRef_, state.sVpiUrl_)) {
                IncrementStatusCounter("FilesRequested");

                SetError("VpiContext::GetLocationXml", VPI_ERROR_REQUESTED_DATA, state.sVpiUrl_);
              } else {
                String s; s.appendf("RequestFileCallback returned error for url:%s", StringType(state.sVpiUrl_));
                SetError("VpiContext::GetLocationXml", VPI_ERROR_REQUEST_DATA_FAILED, s);
              }
            } else {
              SetError("VpiContext::GetLocationXml", VPI_ERROR_NEED_DATA, state.sVpiUrl_);
            }

          }

          bDone = 1;
        }
        break;
      case ResultNoMatch:
        {

          switch (nPhase) {
          case PhaseLocal:
            {

              // vpi file empty or invalid or no match
              // Local phase: ascend folder tree
              URL uVpiURL(state.sVpiUrl_);
              if (uVpiURL.path() == "/") {
                nPhase = PhaseGlobal;
                vpiLog_ConfigReadBuffer buf;
                state.sVpiUrl_ = vpiLog_Read("GlobalRoot", "http://lms.virtual-presence.org/root/root.xml", &buf);
              } else {
                vpiLog_ConfigReadBuffer buf;
                String sLogRead(vpiLog_Read("LocalVpiFileName", "_vpi.xml", &buf));
                state.sVpiUrl_ = ::Rel2AbsUrl(state.sVpiUrl_, sLogRead, 1);
              }

            }
            break;
          case PhaseGlobal:
            {

              // ResultNotApplicable in PhaseGlobal -> no way out, failed
              bDone = 1;
              String s; s.appendf("Mapping stopped due to invalid or no data during global phase in url:%s", StringType(state.sVpiUrl_));
              SetError("VpiContext::GetLocationXml", VPI_ERROR_INVALID_DATA, s);

            }
            break;
          default:
            bDone = 1;
            String s; s.appendf("Mapping stopped due to invalid phase:%d in url:%s", nPhase, StringType(state.sVpiUrl_));
            SetError("VpiContext::GetLocationXml", VPI_ERROR_INTERNAL_ERROR, s);
          }


        }
        break;
      case ResultDelegate:
        {
          // Discovered delegate
          // Break out of local mode and use the absolute delegate URL
          nPhase = PhaseGlobal;
          state.sVpiUrl_ = state.sDelegateUrl_;
        }
        break;
      case ResultMatch:
        {
          // Found matching location info
          // The matching tag XML in sLocationXml_
          bDone = 1;
          sLocationXml = state.sLocationXml_;
          SetError("", VPI_SUCCESS, "");
          ok = 1;
        }
        break;
      default:
        {
          bDone = 1;
          String s; s.appendf("VpiContext::GetLocationXml", "Invalid result:%d, phase:%d, url:%s", (int) nResult,  (int) nPhase, StringType(sUrl));
          SetError("VpiContext::GetLocationXml", VPI_ERROR_INTERNAL_ERROR, s);
        }
      } // case VpiSearchResult

    } // while (!bDone)

  } // Is a valid URL

  return ok;
}

int VpiContext::GetDetailXml(String& sXml, String& sPath, int nFlags, String& sDetailXml)
{
  int ok = 0;
  SetError("", VPI_ERROR_UNKNOWN, "");

  XMLProcessor xml;
  if (!xml.XmlText(sXml)) {
    String s;
    s.appendf("xml.XmlText() failed err:%s, xml: %s", StringType(xml.GetErrorString()), StringType(String::truncate(sXml, 100)));
    SetError("VpiContext::GetDetailXml", VPI_ERROR_XMLPARSER, s);
  } else {
    if (xml.Root() != 0) {

      XMLNode* pNode = xml.Root()->getChildByPath(sPath);
      if (pNode == 0) {
        SetError("", VPI_ERROR_NO_SUCH_NODE, "No node for this path");
      } else {
        if (nFlags & VPI_GDX_INNERXML) {
          sDetailXml = pNode->getCData();
        } else {
          sDetailXml = pNode->outerXml();
        }

        ok = 1;
        SetError("", VPI_SUCCESS, "");
      }
    }
  }

  return ok;
}

int VpiContext::GetLocationUrl(String& sXml, String& sLocationUrl)
{
  int ok = 0;
  SetError("", VPI_ERROR_UNKNOWN, "");

  {
    int bInner = 1;
    String sUrl("url");
    (void) GetDetailXml(sXml, sUrl, bInner, sLocationUrl);
  }
  if (!sLocationUrl.empty()) {
    ok = 1;

  } else {

    String sName;
    String sNameXml;
    {
      int bInner = 0;
      String sNameTmp("name");
      (void) GetDetailXml(sXml, sNameTmp, bInner, sNameXml);
    }
    XMLProcessor xml;
    if (!xml.XmlText(sNameXml)) {
      String s;
      s.appendf("xml.XmlText() failed err:%s, xml:%s", StringType(xml.GetErrorString()), StringType(String::truncate(sXml, 100)));
      SetError("VpiContext::GetLocationUrl", VPI_ERROR_XMLPARSER, s);
    } else {
      if (xml.Root() != 0) {
        sName = xml.Root()->getCData();

        String sHash = sHash.toLower(xml.Root()->getAttribute("hash").getValue());
        if (sHash == "true" || sHash == "sha1") {
          MessageDigest md((unsigned char *) sName.c_str(), sName.bytes());
          sName = md.getSHA1Hex();
        }

        String sPrefix = xml.Root()->getAttribute("prefix").getValue();
        if (!sPrefix.empty()) {
          sName = sPrefix + sName;
        }
      }
    }

    String sService;
    {
      String sServiceTmp("service");
      int bInner = 1;
      (void) GetDetailXml(sXml, sServiceTmp, bInner, sService);
    }
    String sProtocol;
    String sBareService = sService;
    sBareService.nextToken(":", sProtocol);

    if (sName.empty() || sService.empty() || sProtocol.empty()) {
      String s;
      s.appendf("Missing name=%s || service=%s || protocol=%s", StringType(sName), StringType(sService), StringType(sProtocol));
      SetError("VpiContext::GetLocationUrl", VPI_ERROR_ROOM_COMPONENT, s);
    } else {

      sName += sSuffix_;

      if (fpComposeLocationCallback_ != 0) {

        unsigned int nLength = 10000;
        Flexbuf<char> buf(nLength);
        if (!fpComposeLocationCallback_(nComposeLocationCallbackRef_, sProtocol, sName, sService, (char*) buf, &nLength)) {
          String s;
          s.appendf("ComposeLocationCallback returned error for name=%s || service=%s || protocol=%s", StringType(sName), StringType(sService), StringType(sProtocol));
          SetError("VpiContext::GetLocationUrl", VPI_ERROR_ASSEMBLELOCATION, s);
        } else {
          sLocationUrl = (char*) buf;
          ok = 1;
          SetError("", VPI_SUCCESS, "");
        }

      } else {
        if (sProtocol == "jabber" || sProtocol == "xmpp") {
          sLocationUrl.appendf("xmpp:%s@%s", StringType(sName), StringType(sBareService));
          ok = 1;
          SetError("", VPI_SUCCESS, "");
        } else {
          SetError("VpiContext::GetLocationUrl", VPI_SUCCESS, "No ComposeLocationCallback");
        }
      }

    }
  }

  return ok;
}

int VpiContext::GetSelectOptions(String& sXml, List& lOptions)
{
  int ok = 0;
  SetError("", VPI_ERROR_UNKNOWN, "");

  XMLProcessor xml;
  if (!xml.XmlText(sXml)) {
    String s;
    s.appendf("xml.XmlText() failed err:%s, xml: %s", StringType(xml.GetErrorString()), StringType(String::truncate(sXml, 100)));
    SetError("VpiContext::GetSelectOptions", VPI_ERROR_XMLPARSER, s);
  } else {
    if (xml.Root() != 0) {

      XMLNode* pSelect = xml.Root()->getChildByPath("select");
      if (pSelect != 0) {
        for (XMLNode* pOption = 0; (pOption = pSelect->nextChild(pOption, "option")); ) {
          String sSuffix = pOption->getAttribute("suffix").getValue();
          lOptions.AddLast(sSuffix);
        }

        ok = 1;
        SetError("", VPI_SUCCESS, "");
      }
    }
  }

  return ok;
}

int VpiContext::GetSelectOptionProperties(String& sXml, String& sSuffix, List& lProperties)
{
  int ok = 0;
  SetError("", VPI_ERROR_UNKNOWN, "");

  XMLProcessor xml;
  if (!xml.XmlText(sXml)) {
    String s;
    s.appendf("xml.XmlText() failed err:%s, xml: %s", StringType(xml.GetErrorString()), StringType(String::truncate(sXml, 100)));
    SetError("VpiContext::GetSelectOptionProperties", VPI_ERROR_XMLPARSER, s);
  } else {
    if (xml.Root() != 0) {

      XMLNode* pSelect = xml.Root()->getChildByPath("select");
      if (pSelect != 0) {
        ok = 1;
        SetError("", VPI_SUCCESS, "");

        for (XMLNode* pOption = 0; (pOption = pSelect->nextChild(pOption, "option")); ) {
          if (pOption->getAttribute("suffix").getValue() == sSuffix) {
            for (XMLAttr* pAttr = 0; (pAttr = pOption->getAttributes().nextAttribute(pAttr)); ) {
              lProperties.AddLast(pAttr->getName(), pAttr->getValue());
            }
            for (XMLNode* pTag = 0; (pTag = pOption->nextChild(pTag, "tag")); ) {
              lProperties.AddLast("tag", pTag->getCData());
            }
            break;
          }
        }
      }
    }
  }

  return ok;
}

int VpiContext::SetSuffix(String& sSuffix)
{
  int ok = 1;

  sSuffix_ = sSuffix;

  return ok;
}

int VpiContext::GetSuffix(String& sSuffix)
{
  int ok = 1;

  sSuffix = sSuffix_;

  return ok;
}
