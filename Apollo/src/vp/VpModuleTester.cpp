// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "VpModule.h"
#include "VpModuleTester.h"
#include "Local.h"

#if defined(AP_TEST)

String VpModuleTester::test_Identity2ParticipantMapping()
{
  String s;

  VpModule m;
  if (!s) { if (m.participant2Identity_.Count() != 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 0) { s = String::from(__LINE__); }}

  // Basic
  m.setIdentityParticipantMapping("url1", ApHandle(0, 1));
  if (!s) { if (m.participant2Identity_.Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url1")->Value().Count() != 1) { s = String::from(__LINE__); }}
  m.unsetIdentityParticipantMapping("url1", ApHandle(0, 1));
  if (!s) { if (m.participant2Identity_.Count() != 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 0) { s = String::from(__LINE__); }}

  // 1 url for 2 participant
  m.setIdentityParticipantMapping("url1", ApHandle(0, 1));
  m.setIdentityParticipantMapping("url2", ApHandle(0, 2));
  if (!s) { if (m.participant2Identity_.Count() != 2) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 2) { s = String::from(__LINE__); }}
  m.setIdentityParticipantMapping("url1", ApHandle(0, 3));
  if (!s) { if (m.participant2Identity_.Count() != 3) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 2) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url1")->Value().Count() != 2) { s = String::from(__LINE__); }}
  m.unsetIdentityParticipantMapping("url1", ApHandle(0, 1));
  m.unsetIdentityParticipantMapping("url1", ApHandle(0, 3));
  m.unsetIdentityParticipantMapping("url2", ApHandle(0, 2));
  if (!s) { if (m.participant2Identity_.Count() != 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 0) { s = String::from(__LINE__); }}

  // replace url for participant 1
  m.setIdentityParticipantMapping("url1", ApHandle(0, 1));
  m.setIdentityParticipantMapping("url2", ApHandle(0, 2));
  m.setIdentityParticipantMapping("url1", ApHandle(0, 3));
  m.setIdentityParticipantMapping("url3", ApHandle(0, 1));
  if (!s) { if (m.participant2Identity_.Count() != 3) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 3) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url1")->Value().Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url2")->Value().Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url3")->Value().Count() != 1) { s = String::from(__LINE__); }}
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 3));
  if (!s) { if (m.participant2Identity_.Count() != 2) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 2) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url2")->Value().Count() != 1) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Find("url3")->Value().Count() != 1) { s = String::from(__LINE__); }}
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 1));
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 2));
  if (!s) { if (m.participant2Identity_.Count() != 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 0) { s = String::from(__LINE__); }}

  // result list
  m.setIdentityParticipantMapping("url1", ApHandle(0, 1));
  m.setIdentityParticipantMapping("url2", ApHandle(0, 2));
  m.setIdentityParticipantMapping("url1", ApHandle(0, 3));
  m.setIdentityParticipantMapping("url3", ApHandle(0, 1));
  m.setIdentityParticipantMapping("url4", ApHandle(0, 4));
  m.setIdentityParticipantMapping("url4", ApHandle(0, 5));
  m.setIdentityParticipantMapping("url3", ApHandle(0, 2));
  m.setIdentityParticipantMapping("url3", ApHandle(0, 4));
  if (!s) {
    Apollo::ValueList vl;
    m.getParticipantHandlesByIdentity("url1", vl);
    Apollo::ValueList vlExpected; vlExpected.add(ApHandle(0, 3));
    s = Apollo::Test_CompareLists(String::from(__LINE__), vl, vlExpected, Apollo::Test_CompareLists_Flag_IgnoreOrder);
  }
  if (!s) {
    Apollo::ValueList vl;
    m.getParticipantHandlesByIdentity("url2", vl);
    if (vl.length() > 0) { s = String::from(__LINE__); }
  }
  if (!s) {
    Apollo::ValueList vl;
    m.getParticipantHandlesByIdentity("url3", vl);
    Apollo::ValueList vlExpected; vlExpected.add(ApHandle(0, 1)); vlExpected.add(ApHandle(0, 2)); vlExpected.add(ApHandle(0, 4));
    s = Apollo::Test_CompareLists(String::from(__LINE__), vl, vlExpected, Apollo::Test_CompareLists_Flag_IgnoreOrder);
  }
  if (!s) {
    Apollo::ValueList vl;
    m.getParticipantHandlesByIdentity("url4", vl);
    Apollo::ValueList vlExpected; vlExpected.add(ApHandle(0, 5));
    s = Apollo::Test_CompareLists(String::from(__LINE__), vl, vlExpected, Apollo::Test_CompareLists_Flag_IgnoreOrder);
  }
  // clean up
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 1));
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 2));
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 3));
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 4));
  m.removeParticipantFromIdentityParticipantMapping(ApHandle(0, 5));
  if (!s) { if (m.participant2Identity_.Count() != 0) { s = String::from(__LINE__); }}
  if (!s) { if (m.identity2Participant_.Count() != 0) { s = String::from(__LINE__); }}

  return s;
}

//----------------------------------------------------------

String VpModuleTester::test_DisplayProfile()
{
  String s;

  ApHandle hProfile = Apollo::newHandle();
  ApHandle hProfile2 = Apollo::newHandle();

  if (!s) {
    Msg_VpView_Profile_Create msg;
    msg.hProfile = hProfile;
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    if (!s) { if (profiles.Count() != 1) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_SetParticipantDetail msg;
    msg.hProfile = hProfile;
    msg.sKey = "key1";
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    DisplayProfile* pProfile = VpModuleInstance::Get()->findProfile(hProfile);
    if (!s) { DisplayProfileParticipantDetail* p = pProfile->lDetails_.FindByName("key1"); if (p == 0 || p->getMimeTypes().length() != 0) { s = String::from(__LINE__); } }
  }

  if (!s) {
    Msg_VpView_Profile_SetParticipantDetail msg;
    msg.hProfile = hProfile;
    msg.sKey = "key2";
    msg.vlMimeTypes.add("mimetype1.21");
    msg.vlMimeTypes.add("mimetype1.22");
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    DisplayProfile* pProfile = VpModuleInstance::Get()->findProfile(hProfile);
    if (!s) { DisplayProfileParticipantDetail* p = pProfile->lDetails_.FindByName("key2"); if (p == 0 || p->getMimeTypes().length() != 2) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_SetParticipantDetail msg;
    msg.hProfile = hProfile;
    msg.sKey = "key3";
    msg.vlMimeTypes.add("mimetype1.31");
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    DisplayProfile* pProfile = VpModuleInstance::Get()->findProfile(hProfile);
    if (!s) { DisplayProfileParticipantDetail* p = pProfile->lDetails_.FindByName("key3"); if (p == 0 || p->getMimeTypes().length() != 1) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_SetParticipantDetail msg;
    msg.hProfile = hProfile;
    msg.sKey = "key2";
    msg.vlMimeTypes.add("mimetype1.21b");
    msg.vlMimeTypes.add("mimetype1.22b");
    msg.vlMimeTypes.add("mimetype1.23b");
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    DisplayProfile* pProfile = VpModuleInstance::Get()->findProfile(hProfile);
    if (!s) { DisplayProfileParticipantDetail* p = pProfile->lDetails_.FindByName("key2"); if (p == 0 || p->getMimeTypes().length() != 3) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_Create msg;
    msg.hProfile = hProfile2;
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    if (!s) { if (profiles.Count() != 2) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_SetParticipantDetail msg;
    msg.hProfile = hProfile2;
    msg.sKey = "key1";
    msg.vlMimeTypes.add("mimetype2.1");
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    DisplayProfile* pProfile = VpModuleInstance::Get()->findProfile(hProfile2);
    if (!s) { DisplayProfileParticipantDetail* p = pProfile->lDetails_.FindByName("key1"); if (p == 0 || p->getMimeTypes().length() != 1) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_Delete msg;
    msg.hProfile = hProfile;
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    if (!s) { if (profiles.Count() != 1) { s = String::from(__LINE__); }}
  }

  if (!s) {
    Msg_VpView_Profile_Delete msg;
    msg.hProfile = hProfile2;
    if (!msg.Request()) {
      s = String::from(__LINE__);
    }
  }
  if (!s) {
    ApHandlePointerTree<DisplayProfile*>& profiles = VpModuleInstance::Get()->participantSubscriptionProfiles_;
    if (!s) { if (profiles.Count() > 0) { s = String::from(__LINE__); }}
  }

  return s;
}

//----------------------------------------------------------

String VpModuleTester::test_ItemDataExternUrl()
{
  String s;

  String sUrl = VpModuleInstance::Get()->getItemDataExternUrl("http://ydentiti.org/test/Planta/identity.xml", "avatar");

  String sHost = Apollo::getConfig("Server/HTTP/Address", "localhost");
  int nPort = Apollo::getConfig("Server/HTTP/Port", 23761);

  String sExpected;
  String sEscapedIdentityUrl = "http://ydentiti.org/test/Planta/identity.xml";
  sEscapedIdentityUrl.escape(String::EscapeURL);
  sExpected.appendf("http://%s:%d/Vp?%s|avatar|", StringType(sHost), nPort, StringType(sEscapedIdentityUrl));
  if (!sUrl.startsWith(sExpected)) { s.appendf("%s does not start with %s", StringType(sUrl), StringType(sExpected)); }

  return s;
}

#endif
