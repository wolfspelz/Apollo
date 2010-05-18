// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "MsgUnitTest.h"

#if defined(AP_TEST_Core)
#include "MsgSample.h"

int Test_Core_ModuleLoad()
{
  int ok = 1;

  if (Apollo::isLoadedModule("Sample")) {
    Apollo::unloadModule("Sample");
  }

  if (Apollo::isLoadedModule("Sample")) {
    ok= 0;
  }

  Apollo::loadModule("Sample", "sample");
  
  if (!Apollo::isLoadedModule("Sample")) {
    ok= 0;
  }

  Msg_Sample_Get msg; msg.Request();
  if (msg.nValue != 42) {
    ok = 0;
  }

  if (Apollo::isLoadedModule("Sample")) {
    Apollo::unloadModule("Sample");
  }

  if (Apollo::isLoadedModule("Sample")) {
    ok= 0;
  }

  return ok;
}

//----------------------------------------------------------

#define MSG_Test "TEST/Test"
class Msg_TEST: public ApNotificationMessage
{
public:
  Msg_TEST() : ApNotificationMessage(MSG_Test) {}
};

static int g_nTest_Core_CallQueue_nCount1 = 0;
static int g_nTest_Core_CallQueue_nCount2 = 0;
static int g_nTest_Core_CallQueue_nCount3 = 0;
static int g_nTest_Core_CallQueue_nRefOk1 = 0;
static int g_nTest_Core_CallQueue_nRefOk2 = 0;
static int g_nTest_Core_CallQueue_nRefOk3 = 0;
static int g_nTest_Core_CallQueue_OnTest1(Msg_TEST* pMsg)
{
  g_nTest_Core_CallQueue_nCount1++;
  if ((int) pMsg->Ref() == 111) { g_nTest_Core_CallQueue_nRefOk1 = 1; }
  return 1;
}
static int g_nTest_Core_CallQueue_OnTest2(Msg_TEST* pMsg)
{
  g_nTest_Core_CallQueue_nCount2++;
  if ((int) pMsg->Ref() == 222) { g_nTest_Core_CallQueue_nRefOk2 = 1; }
  return 1;
}
static int g_nTest_Core_CallQueue_OnTest3(Msg_TEST* pMsg)
{
  g_nTest_Core_CallQueue_nCount3++;
  if ((int) pMsg->Ref() == 333) { g_nTest_Core_CallQueue_nRefOk3 = 1; }
  pMsg->Stop();
  return 1;
}


static int g_nTest_Core_CallQueue_nOrder = 0;
static int g_nTest_Core_CallQueue_OnTestOrder(Msg_TEST* pMsg)
{
  g_nTest_Core_CallQueue_nOrder = (int) pMsg->Ref();
  pMsg->Stop();
  return 1;
}

static int g_nTest_Core_CallQueue_nRefCount = 0;
static int g_nTest_Core_CallQueue_OnTestUnHook(Msg_TEST* pMsg)
{
  g_nTest_Core_CallQueue_nRefCount += (int) pMsg->Ref();
  //don't stop message here
  return 1;
}

String Test_Core_CallQueue()
{
  String sResult("");

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111, 0);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222, 1000);
    g_nTest_Core_CallQueue_nCount1 = 0;
    g_nTest_Core_CallQueue_nCount2 = 0;
    g_nTest_Core_CallQueue_nCount3 = 0;
    g_nTest_Core_CallQueue_nRefOk1 = 0;
    g_nTest_Core_CallQueue_nRefOk2 = 0;
    g_nTest_Core_CallQueue_nRefOk3 = 0;
    { Msg_TEST msg; msg.Send(); }
    if ( g_nTest_Core_CallQueue_nCount1 == 1 && g_nTest_Core_CallQueue_nCount2 == 1 && g_nTest_Core_CallQueue_nCount3 == 0
      && g_nTest_Core_CallQueue_nRefOk1 == 1 && g_nTest_Core_CallQueue_nRefOk2 == 1 && g_nTest_Core_CallQueue_nRefOk3 == 0) {
      //ok= 1;
    } else {
      sResult = "Fail_1";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111, 0);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222, 1000);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest3, (ApCallbackRef) 333, -1000);
    g_nTest_Core_CallQueue_nCount1 = 0;
    g_nTest_Core_CallQueue_nCount2 = 0;
    g_nTest_Core_CallQueue_nCount3 = 0;
    g_nTest_Core_CallQueue_nRefOk1 = 0;
    g_nTest_Core_CallQueue_nRefOk2 = 0;
    g_nTest_Core_CallQueue_nRefOk3 = 0;
    { Msg_TEST msg; msg.Send(); }
    if ( g_nTest_Core_CallQueue_nCount1 == 0 && g_nTest_Core_CallQueue_nCount2 == 0 && g_nTest_Core_CallQueue_nCount3 == 1
      && g_nTest_Core_CallQueue_nRefOk1 == 0 && g_nTest_Core_CallQueue_nRefOk2 == 0 && g_nTest_Core_CallQueue_nRefOk3 == 1) {
      //ok= 1;
    } else {
      sResult = "Fail_2";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest3, (ApCallbackRef) 333);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111, 0);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest3, (ApCallbackRef) 333, Apollo::modulePos(ApCallbackPosLate, MODULE_NAME));
    g_nTest_Core_CallQueue_nCount1 = 0;
    g_nTest_Core_CallQueue_nCount2 = 0;
    g_nTest_Core_CallQueue_nCount3 = 0;
    g_nTest_Core_CallQueue_nRefOk1 = 0;
    g_nTest_Core_CallQueue_nRefOk2 = 0;
    g_nTest_Core_CallQueue_nRefOk3 = 0;
    { Msg_TEST msg; msg.Send(); }
    if ( g_nTest_Core_CallQueue_nCount1 == 1 && g_nTest_Core_CallQueue_nCount2 == 1 && g_nTest_Core_CallQueue_nCount3 == 1
      && g_nTest_Core_CallQueue_nRefOk1 == 1 && g_nTest_Core_CallQueue_nRefOk2 == 1 && g_nTest_Core_CallQueue_nRefOk3 == 1) {
      //ok= 1;
    } else {
      sResult = "Fail_3";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest3, (ApCallbackRef) 333);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111, 0);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222, Apollo::modulePos(ApCallbackPosLate, "B"));
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest3, (ApCallbackRef) 333, Apollo::modulePos(ApCallbackPosLate, "A"));
    g_nTest_Core_CallQueue_nCount1 = 0;
    g_nTest_Core_CallQueue_nCount2 = 0;
    g_nTest_Core_CallQueue_nCount3 = 0;
    g_nTest_Core_CallQueue_nRefOk1 = 0;
    g_nTest_Core_CallQueue_nRefOk2 = 0;
    g_nTest_Core_CallQueue_nRefOk3 = 0;
    { Msg_TEST msg; msg.Send(); }
    if ( g_nTest_Core_CallQueue_nCount1 == 1 && g_nTest_Core_CallQueue_nCount2 == 0 && g_nTest_Core_CallQueue_nCount3 == 1
      && g_nTest_Core_CallQueue_nRefOk1 == 1 && g_nTest_Core_CallQueue_nRefOk2 == 0 && g_nTest_Core_CallQueue_nRefOk3 == 1) {
      //ok= 1;
    } else {
      sResult = "Fail_4";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest1, (ApCallbackRef) 111);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest2, (ApCallbackRef) 222);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTest3, (ApCallbackRef) 333);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1, 1);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10, 2);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100, 3);
    g_nTest_Core_CallQueue_nOrder = 0;
    { Msg_TEST msg; msg.Send(); }
    if(g_nTest_Core_CallQueue_nOrder == 1) {
    } else {
      sResult = "Fail_5";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1, -1);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10, -2);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100, -3);
    g_nTest_Core_CallQueue_nOrder = 0;
    { Msg_TEST msg; msg.Send(); }
    if (g_nTest_Core_CallQueue_nOrder == 100) {
    } else {
      sResult = "Fail_6";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1, 1);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10, 2);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100, -1);
    g_nTest_Core_CallQueue_nOrder = 0;
    { Msg_TEST msg; msg.Send(); }
    if(g_nTest_Core_CallQueue_nOrder == 100) {
    } else {
      sResult = "Fail_7";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100);
  }

  if (sResult.empty()) {
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1, 3);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10, 1);
    Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100, 2);
    g_nTest_Core_CallQueue_nOrder = 0;
    { Msg_TEST msg; msg.Send(); }
    if(g_nTest_Core_CallQueue_nOrder == 10) {
    } else {
      sResult = "Fail_8";
    }
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)   1);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef)  10);
    Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestOrder, (ApCallbackRef) 100);
  }
  
  if (sResult.empty()) {
    int nRef1 = 1;
    int nRef2 = 2;
    int nRef3 = 4;
    int nRef4 = 8;
    if (sResult.empty()){
      g_nTest_Core_CallQueue_nRefCount = 0;
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef1, 0);
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef2, 0);
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef3, 0);
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef4, 0);
      { Msg_TEST msg; msg.Send(); }
      if (g_nTest_Core_CallQueue_nRefCount != nRef1 + nRef2 + nRef3 + nRef4) {sResult = "Fail_9_1";}  
    }
    if (sResult.empty()){
      g_nTest_Core_CallQueue_nRefCount = 0;
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef1);
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef4);
      { Msg_TEST msg; msg.Send(); }
      if (g_nTest_Core_CallQueue_nRefCount != nRef2 + nRef3) {sResult = "Fail_9_2";}  
    }
    if (sResult.empty()){
      g_nTest_Core_CallQueue_nRefCount = 0;
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef2);
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef3);
      { Msg_TEST msg; msg.Send(); }
      if (g_nTest_Core_CallQueue_nRefCount != 0) {sResult = "Fail_9_3";}  
    }
    if (sResult.empty()){
      g_nTest_Core_CallQueue_nRefCount = 0;
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef2, 0);
      { Msg_TEST msg; msg.Send(); }
      if (g_nTest_Core_CallQueue_nRefCount != nRef2) {sResult = "Fail_9_4";}  
    }
    if (sResult.empty()){
      g_nTest_Core_CallQueue_nRefCount = 0;
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef1, 0);
      Apollo::hookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef4, 0);
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef1);
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef4);
      { Msg_TEST msg; msg.Send(); }
      if (g_nTest_Core_CallQueue_nRefCount != nRef2) {sResult = "Fail_9_5";} 
      Apollo::unhookMsg(MSG_Test, MODULE_NAME, (ApCallback) g_nTest_Core_CallQueue_OnTestUnHook, (ApCallbackRef) nRef2);
    }
  }

  return sResult;

  if (sResult.empty()) {
  

  }
}

//----------------------------------------------------------

int Test_Core_NewHandle()
{
  int ok = 1;

  ApHandle h1 = Apollo::newHandle();
  ApHandle h2 = Apollo::newHandle();
  ApHandle h3 = Apollo::newHandle();
  if (h1 == h2 || h2 == h3 || h3 == h1) {
    ok = 0;
  }

  return ok;
}

int Test_Core_Handle2String()
{
  ApHandle h1 = Apollo::newHandle();
  String s1 = Apollo::handle2String(h1);
  ApHandle h11 = Apollo::string2Handle(s1);
  ApHandle h2 = ApNoHandle;
  String s2 = Apollo::handle2String(h2);
  ApHandle h22 = Apollo::string2Handle(s2);
  return (h1 == h11 && h2 == h22);
}

//----------------------------------------------------------

String Test_Core_TimeValue_CheckResult(Apollo::TimeValue& tv1, const char* szOp, Apollo::TimeValue& tv2, Apollo::TimeValue& tvResult, Apollo::TimeValue& tvGood)
{
  String e;

  if (! (tvResult == tvGood) ) {
    e.appendf("failed to verify: %d.%d + %d.%d %s %d.%d != %d.%d", tv1.Sec(), tv1.MicroSec(), szOp, tv2.Sec(), tv2.MicroSec(), tvResult.Sec(), tvResult.MicroSec(), tvGood.Sec(), tvGood.MicroSec());
  }

  return e;
}

String Test_Core_TimeValue()
{
  String e;

  if (e.empty()) {
    Apollo::TimeValue tv = Apollo::TimeValue::getTime();
    
    time_t t = time(0);
    int nDiff = tv.Sec() - t;
    if (tv.Sec() == 0 || tv.MicroSec() == 0) { e.appendf(" sec=%d, usec=%d", tv.Sec(), tv.MicroSec()); }
    if (t == 0) { e.appendf(" time=%d", t); }
    if (nDiff != 0) { e.appendf(" TimeValue and time(0) differ: sec=%d, usec=%d <-> t=%d", tv.Sec(), tv.MicroSec(), t); }
  }
  
  // +
  if (e.empty()) { Apollo::TimeValue tv1(0, 0); Apollo::TimeValue tv2(0, 0); Apollo::TimeValue tv3(0, 0); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(0, 1); Apollo::TimeValue tv2(0, 1); Apollo::TimeValue tv3(0, 2); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(1, 0); Apollo::TimeValue tv3(2, 0); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 1); Apollo::TimeValue tv2(1, 1); Apollo::TimeValue tv3(2, 2); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 500000); Apollo::TimeValue tv2(1, 500000); Apollo::TimeValue tv3(3, 0); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 500000); Apollo::TimeValue tv2(1, 600000); Apollo::TimeValue tv3(3, 100000); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1000000000, 600000); Apollo::TimeValue tv2(1000000000, 600000); Apollo::TimeValue tv3(2000000001, 200000); Apollo::TimeValue tv = tv1 + tv2; e = Test_Core_TimeValue_CheckResult(tv1, "+", tv2, tv, tv3); }

  // -
  if (e.empty()) { Apollo::TimeValue tv1(0, 0); Apollo::TimeValue tv2(0, 0); Apollo::TimeValue tv3(0, 0); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(0, 1); Apollo::TimeValue tv2(0, 1); Apollo::TimeValue tv3(0, 0); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 1); Apollo::TimeValue tv2(1, 0); Apollo::TimeValue tv3(0, 1); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 1); Apollo::TimeValue tv2(0, 1); Apollo::TimeValue tv3(1, 0); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 500000); Apollo::TimeValue tv2(0, 500000); Apollo::TimeValue tv3(1, 0); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1, 500000); Apollo::TimeValue tv2(0, 600000); Apollo::TimeValue tv3(0, 900000); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }
  if (e.empty()) { Apollo::TimeValue tv1(1000000000, 600000); Apollo::TimeValue tv2(500000000, 700000); Apollo::TimeValue tv3(499999999, 900000); Apollo::TimeValue tv = tv1 - tv2; e = Test_Core_TimeValue_CheckResult(tv1, "-", tv2, tv, tv3); }

  // <
  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(2, 0); if (! (tv1 < tv2) ) { e.appendf("failed to verify: %d.%d < %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }

  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(2, 0); if (! (tv1 < tv2) ) { e.appendf("failed to verify: %d.%d < %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(1, 1); if (! (tv1 < tv2) ) { e.appendf("failed to verify: %d.%d < %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(0, 0); Apollo::TimeValue tv2(0, 1); if (! (tv1 < tv2) ) { e.appendf("failed to verify: %d.%d < %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(0, 0); Apollo::TimeValue tv2(1, 0); if (! (tv1 < tv2) ) { e.appendf("failed to verify: %d.%d < %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }

  // >
  if (e.empty()) { Apollo::TimeValue tv1(2, 0); Apollo::TimeValue tv2(1, 0); if (! (tv1 > tv2) ) { e.appendf("failed to verify: %d.%d > %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 1); Apollo::TimeValue tv2(1, 0); if (! (tv1 > tv2) ) { e.appendf("failed to verify: %d.%d > %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(0, 1); Apollo::TimeValue tv2(0, 0); if (! (tv1 > tv2) ) { e.appendf("failed to verify: %d.%d > %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(0, 1); if (! (tv1 > tv2) ) { e.appendf("failed to verify: %d.%d > %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }

  // >=
  if (e.empty()) { Apollo::TimeValue tv1(2, 0); Apollo::TimeValue tv2(1, 0); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 1); Apollo::TimeValue tv2(1, 0); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(0, 1); Apollo::TimeValue tv2(0, 0); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(0, 1); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(2, 0); Apollo::TimeValue tv2(2, 0); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 1); Apollo::TimeValue tv2(1, 1); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(0, 1); Apollo::TimeValue tv2(0, 1); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }
  if (e.empty()) { Apollo::TimeValue tv1(1, 0); Apollo::TimeValue tv2(1, 0); if (! (tv1 >= tv2) ) { e.appendf("failed to verify: %d.%d >= %d.%d", tv1.Sec(), tv1.MicroSec(), tv2.Sec(), tv2.MicroSec()); } }

  return e;
}

#endif // AP_TEST_Core

//----------------------------------------------------------

String Test_Core_AppPath()
{
  String s;

  String sPath;
  String sModuleBaseName = "sample";
  
  sPath = Apollo::getAppBasePath();
  if (s.empty()) {
    if (sPath.empty()) {
      s.appendf("getAppBasePath returned empty path: %s", StringType(sPath));
    }
  }
  if (s.empty()) {
    if (!String::filenameIsAbsolutePath(sPath)) {
      s.appendf("getAppBasePath returned no absolute path: %s", StringType(sPath));
    }
  }

  sPath = Apollo::getModuleResourcePath(sModuleBaseName);
  if (s.empty()) {
    if (sPath.empty()) {
      s.appendf("getAppResourcePath returned empty path: %s", StringType(sPath));
    }
  }
  if (s.empty()) {
    if (!String::filenameIsAbsolutePath(sPath)) {
      s.appendf("getAppResourcePath returned no absolute path: %s", StringType(sPath));
    }
  }

  sPath = Apollo::getAppModulePath();
  if (s.empty()) {
    if (sPath.empty()) {
      s.appendf("getAppModulePath returned empty path: %s", StringType(sPath));
    }
  }
  if (s.empty()) {
    if (!String::filenameIsAbsolutePath(sPath)) {
      s.appendf("getAppModulePath returned no absolute path: %s", StringType(sPath));
    }
  }

  sPath = Apollo::getAppResourcePath();
  if (s.empty()) {
    if (sPath.empty()) {
      s.appendf("getAppResourcePath returned empty path: %s", StringType(sPath));
    }
  }
  if (s.empty()) {
    if (!String::filenameIsAbsolutePath(sPath)) {
      s.appendf("getAppResourcePath returned no absolute path: %s", StringType(sPath));
    }
  }

  sPath = Apollo::getAppLibraryPath();
  if (s.empty()) {
    if (sPath.empty()) {
      s.appendf("getAppLibraryPath returned empty path: %s", StringType(sPath));
    }
  }
  if (s.empty()) {
    if (!String::filenameIsAbsolutePath(sPath)) {
      s.appendf("getAppLibraryPath returned no absolute path: %s", StringType(sPath));
    }
  }
  
  return s;
}

String Test_Core_File()
{
  String s;
  const unsigned char pInData1[] = "Test\0\1\2Data_1\255";
  const unsigned char pInData2[] = "Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255Test\0\1\2Data_2\255";

  const Buffer sbInData0(NULL,0);
  const Buffer sbInData1(pInData1,sizeof(pInData1) * sizeof(*pInData1));
  const Buffer sbInData2(pInData2,sizeof(pInData2) * sizeof(*pInData2));
  Buffer sbInData3;

  Buffer sbOutData1;
  Buffer sbOutData2;
  Buffer sbOutData3;
  Buffer sbOutData4;
  
  String sPath = Apollo::getModuleResourcePath(MODULE_NAME) + "testfile";
  String sPath2 = Apollo::getModuleResourcePath(MODULE_NAME) + "testfile2";

  Apollo::saveFile(sPath2,sbInData0);
  for (int i = 0; i < (17042) && s.empty(); i += sizeof(pInData2)){ //200,023 KB
    sbInData3.Append((unsigned char*)pInData2, sizeof(pInData2) * sizeof(*pInData2));
    if (!Apollo::appendFile(sPath2,sbInData2)){
      s = "append failed";
    }
  }

  if (s.empty()){ 
    if (!Apollo::saveFile(sPath, sbInData1)){
      s = "1 write failed";
    }
  }
  if (s.empty()){ 
    if (!Apollo::loadFile(sPath, sbOutData1)){
      s = "1 read failed";
    }
  }
  if (s.empty()){ 
    if (!Apollo::saveFile(sPath, sbInData2)){
      s = "2 write failed";
    }
  }
  if (s.empty()){ 
    if (!Apollo::loadFile(sPath, sbOutData2)){
      s = "2 read failed";
    }
  }
  if (s.empty()){ 
    if (!Apollo::saveFile(sPath, sbInData3)){
      s = "3 write failed";
    }
  }
  if (s.empty()){ 
    if (!Apollo::loadFile(sPath, sbOutData3)){
      s = "3 read failed";
    }
  }
  if (s.empty()){ 
    if (!Apollo::loadFile(sPath2, sbOutData4)){
      s = "append read failed";
    }
  }

  if (s.empty()){ s = (!::memcmp(sbInData1.Data(), sbOutData1.Data(), sbInData1.Length()))?"":"sbOutData1 wrong";; }
  if (s.empty()){ s = (!::memcmp(sbInData2.Data(), sbOutData2.Data(), sbInData2.Length()))?"":"sbOutData2 wrong";; }
  if (s.empty()){ s = (!::memcmp(sbInData3.Data(), sbOutData3.Data(), sbInData3.Length()))?"":"sbOutData3 wrong";; }
  if (s.empty()){ s = (!::memcmp(sbInData3.Data(), sbOutData4.Data(), sbInData3.Length()))?"":"append went wrong";; }


  return s;
}

String Test_Core_MachineId()
{
  String s;

  String sMachineId = Apollo::getMachineId();
  if (sMachineId.empty()) { s = "getMachineId() empty"; }

  String sMachineId2 = Apollo::getMachineId();
  if (sMachineId != sMachineId2) { s = "not equal"; }

  return s;
}

//----------------------------------------------------------

void Test_Core_Register()
{
#if defined(AP_TEST_Core)
  AP_UNITTEST_REGISTER(Test_Core_ModuleLoad);
  AP_UNITTEST_REGISTER(Test_Core_CallQueue);
  AP_UNITTEST_REGISTER(Test_Core_NewHandle);
  AP_UNITTEST_REGISTER(Test_Core_Handle2String);
  AP_UNITTEST_REGISTER(Test_Core_TimeValue);
  AP_UNITTEST_REGISTER(Test_Core_AppPath);
  AP_UNITTEST_REGISTER(Test_Core_File);
  AP_UNITTEST_REGISTER(Test_Core_MachineId);
#endif // AP_TEST_Core
}

void Test_Core_Execute()
{
#if defined(AP_TEST_Core)
  AP_UNITTEST_EXECUTE1(Test_Core_ModuleLoad);
  AP_UNITTEST_EXECUTE(Test_Core_CallQueue);
  AP_UNITTEST_EXECUTE1(Test_Core_NewHandle);
  AP_UNITTEST_EXECUTE1(Test_Core_Handle2String);
  AP_UNITTEST_EXECUTE(Test_Core_TimeValue);
  AP_UNITTEST_EXECUTE(Test_Core_AppPath);
  AP_UNITTEST_EXECUTE(Test_Core_File);
  AP_UNITTEST_EXECUTE(Test_Core_MachineId);
#endif // AP_TEST_Core
}
