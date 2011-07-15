// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgDb.h"
#include "DunModule.h"

AP_MSG_HANDLER_METHOD(DunModule, Core_ModuleLoaded)
{
  // When Db is loaded after us
  // or if we are just loading and Db was before us, then do the Db related stuff
  if (pMsg->sShortName == "Db" || (pMsg->sShortName == MODULE_NAME && Apollo::isLoadedModule("Db"))) {
    String sDeleteDbFiles = Apollo::getModuleConfig(MODULE_NAME, "OnLoad/DeleteDbFiles", "");
    List lDeleteDbFiles;
    KeyValueBlob2List(sDeleteDbFiles, lDeleteDbFiles, ", ", "=", "");
    for (Elem* e = 0; (e = lDeleteDbFiles.Next(e)) != 0; ) {
      Msg_DB_DeleteFile msg;
      msg.sName = e->getName();
      if (!msg.Request()) {
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_DeleteFile failed name=%s", _sz(msg.sName)));
      }
    }
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(DunModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dun", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(DunModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dun", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(DunModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int DunModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DunModule, Core_ModuleLoaded, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DunModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DunModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DunModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

void DunModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
