// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApCompiler.h"
#include "Apollo.h"
#include "ApCore.h"
#include "ApLog.h"
#include "MsgSystem.h"
#include "MsgCore.h"
#include "MsgConfig.h"

//#define DEBUG_CHECK_MEMORY 1

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

APCORE_API ApCore gApCore;

#define LOG_CHANNEL "ApCore"
#define APCORE_NAME "Apollo"

// -------------------------------------------------------------------

static int ApCore_GetInfoFlags()
{
  return
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ;
}

int ApCore::loadModuleConfig(const String& sModuleName, const String& sModuleFile, const String& sConfigFile)
{
  int ok = 0;

  String sData;
  ok = Apollo::loadFile(sConfigFile, sData);
  if (ok) {
    apLog_Verbose((LOG_CHANNEL, "ApCore::loadModuleConfig", "%s", StringType(sConfigFile)));

    List lParams;
    KeyValueLfBlob2List(sData, lParams);
    for (Elem* e = 0; (e = lParams.Next(e)) != 0; ) {
      Apollo::setModuleConfig(sModuleName, e->getName(), e->getString());
    }
  }

  return ok;
}

int ApCore::loadModule(const char* szModuleName, const char* szDllPath)
{
  int ok = 0;

  String sDllPath = szDllPath;
  String sBasePath = String::filenameBasePath(sDllPath);
  String sBaseName = sDllPath.subString(sBasePath.chars(), sDllPath.chars() - sBasePath.chars());
  String sFileName = Apollo::normalizeLibraryName(sBaseName);
  String sExtension = String::filenameExtension(sFileName);

  if (sBasePath.empty() || sBasePath == (String(".").append(String::filenamePathSeparator()))) {  
    sBasePath = Apollo::getModuleLibraryPath(sBaseName);
  }

  sDllPath = sBasePath + sFileName;

  apLog_Verbose((LOG_CHANNEL, "ApCore::loadModule", "%s", StringType(sDllPath)));

  int bDependenciesLoaded = 1;
  ListT<SupportDll, Elem> lSupportDlls;

  if (Apollo::isLoadedModule("Config")) {

    bHeapCheck_ = Apollo::getConfig("Core/Debug/HeapCheck", 0);

    {
      // Load module config and write to config module
      String sModuleConfigFile = Apollo::getModuleResourcePath(sBaseName);
      sModuleConfigFile += Apollo::getConfig("Core/ModuleConfig/File", "config.txt");
      (void) loadModuleConfig(szModuleName, sBaseName, sModuleConfigFile);
    }

    {
      // Load module config and write to config module
      String sModuleConfigFile = Apollo::getModuleResourcePath(sBaseName);
      sModuleConfigFile += Apollo::getConfig("Core/ModuleConfig/LocalFile", "config.local.txt");
      (void) loadModuleConfig(szModuleName, sBaseName, sModuleConfigFile);
    }

    // Check if DLLs must be loaded prior to module dll
    Apollo::ValueList vlUnsortedKeys;
    String sPath = "LoadDLL";    
    if (! Apollo::getModuleConfigKeys(szModuleName, sPath, vlUnsortedKeys)) {
      apLog_Verbose((LOG_CHANNEL, "ApCore::loadModule", "Apollo::getModuleConfigKeys failed: Module=%s Path=%s", szModuleName, StringType(sPath)));
    } else {

      List lKeys;
      {
        // Values are in Elem.String, Copy to new list with values in Elem.Name for SortbyName
        for (Apollo::ValueElem* e = 0; (e = vlUnsortedKeys.nextElem(e)) != 0; ) {
          lKeys.AddLast(e->getString());
        }
      }
      lKeys.SortByName();

      for (Elem* e = 0; (e = lKeys.Next(e)); ) {
        String sConfigPath = sPath;
        sConfigPath += "/";
        sConfigPath += e->getName();
        String sSupportDllConfig = Apollo::getModuleConfig(szModuleName, sConfigPath, "");
        if (!sSupportDllConfig.empty()) {
          String sSupportDllBasePath = String::filenameBasePath(sSupportDllConfig);
          String sSupportDllBaseName = sSupportDllConfig.subString(sSupportDllBasePath.chars(), sSupportDllConfig.chars() - sSupportDllBasePath.chars());
          String sSupportDll = sSupportDllBasePath;
          if (String::filenameExtension(sSupportDllBaseName).empty()) {
            // No ext. given, normalize for platform and bail out verbosely if load fails
            sSupportDll += Apollo::normalizeLibraryName(sSupportDllBaseName);
          } else {
            // This is a OS specific library because of included ext.
            // -> load only if present
            sSupportDll += sSupportDllBaseName;
            if (!Apollo::fileExists(sSupportDll)) { sSupportDll.clear(); }
          }
          if (!sSupportDll.empty()) {
            HMODULE hDll = (HMODULE) Apollo::libraryLoad(sSupportDll);
            if (hDll == NULL) {
              apLog_Error((LOG_CHANNEL, "ApCore::loadModule", "Apollo::libraryLoad(%s) failed", StringType(sSupportDll)));
              bDependenciesLoaded = 0;
            } else {
              SupportDll* pSupportDll = new SupportDll(sSupportDll, hDll);
              if (pSupportDll != 0) {
                lSupportDlls.AddFirst(pSupportDll);
              }
            }
          }
        }
      }
    }
  }

  // Load module dll
  if (!bDependenciesLoaded) {
    apLog_Error((LOG_CHANNEL, "ApCore::loadModule", "Missing Dependencies for: %s", StringType(sDllPath)));

  } else {
    HMODULE hDll = (HMODULE) Apollo::libraryLoad(sDllPath);
    if (hDll == NULL) {
      apLog_Error((LOG_CHANNEL, "ApCore::loadModule", "LoadLibrary failed: %s", StringType(sDllPath)));
    } else {
      String sModuleName;
      List lModuleServices;
      ApModule* pMod = 0;

      String sModInfoSym = "Info";
      String sModLoadSym = "Load";
      String sModUnloadSym = "UnLoad";

      ApModuleInfoF fnInfo = (ApModuleInfoF) Apollo::libraryGetProcAddress(hDll, sModInfoSym);
      ApModuleLoadF fnLoad = (ApModuleLoadF) Apollo::libraryGetProcAddress(hDll, sModLoadSym);
      ApModuleUnLoadF fnUnLoad = (ApModuleUnLoadF) Apollo::libraryGetProcAddress(hDll, sModUnloadSym);

      if (fnInfo == 0 || fnLoad == 0 || fnUnLoad == 0) {
        apLog_Error((LOG_CHANNEL, "ApCore::loadModule", "Missing module function %s, Info()=0x%08x, Load()=0x%08x, UnLoad()=0x%08x", StringType(sDllPath), (unsigned long) fnInfo, (unsigned long) fnLoad, (unsigned long) fnUnLoad));
      } else {
        AP_MODULE_CALL mcInfo;
        mcInfo.nSize = sizeof(mcInfo);
        mcInfo.nFlags = ApCore_GetInfoFlags();
        //mcInfo.fpGetAddressByName = ApCore_GetAddressByName;
        AP_MODULE_INFO* pmi = fnInfo(&mcInfo);
        if (pmi == 0) {
          apLog_Warning((LOG_CHANNEL, "ApCore::loadModule", "Info() returned 0, DLL: %s", StringType(sDllPath)));
        } else {

          sModuleName = pmi->szName;

          List lServices;
          {
            // Check implemented services and flags
            String sLines = pmi->szServices;
            List lLines;
            KeyValueLfBlob2List(sLines, lLines);
            for (Elem* eLine = 0; (eLine = lLines.Next(eLine)); ) {
              if (!eLine->getName().empty()) {
                List lParams;
                KeyValueBlob2List(eLine->getString(), lParams, ",; ", "=:", "");
                // eLine->getName() -> service
                List* pServiceFlags = new List(eLine->getName());
                if (pServiceFlags != 0) {
                  for (Elem* eParam = 0; (eParam = lParams.Next(eParam)); ) {
                    if (!eParam->getName().empty()) {
                      // eParam->getName() -> flag name
                      // eParam->getString() -> flag value
                      pServiceFlags->Add(eParam->getName(), eParam->getString());
                    }
                  }
                }
                if (pServiceFlags != 0) {
                  lServices.Add(pServiceFlags);
                }
              }
            }
          }

          if (isLoadedModule(sModuleName)) {
            apLog_Warning((LOG_CHANNEL, "ApCore::loadModule", "Module %s already loaded: %s", StringType(sModuleName), StringType(sDllPath)));
          } else {
            
            { Msg_Core_BeforeModuleLoaded msg; msg.sModuleName = sModuleName; msg.Send(); }

            int bLoadOk = fnLoad(&mcInfo);
            if (!bLoadOk) {
              apLog_Warning((LOG_CHANNEL, "ApCore::loadModule", "Load() returned 0, DLL: %s", StringType(sDllPath)));
            } else {
              pMod = new ApModule(sModuleName);
              if (pMod) {
                pMod->hDLL_ = hDll;
                pMod->sModulePackageName_ = sBaseName;
                pMod->sDLLPath_ = sDllPath;
                //pMod->sLangPath_;
                pMod->fnInfo_ = fnInfo;
                pMod->fnLoad_ = fnLoad;
                pMod->fnUnLoad_ = fnUnLoad;
                pMod->lServices_ = lServices;
                pMod->lSupportDlls_ = lSupportDlls;

                pMod->sLongName_ = pmi->szLongName;
                pMod->sVersion_ = pmi->szVersion;
                pMod->sDescription_ = pmi->szDescription;
                pMod->sAuthor_ = pmi->szAuthor;
                pMod->sEmail_ = pmi->szEmail;
                pMod->sCopyright_ = pmi->szCopyright;
                pMod->sHomepageUrl_ = pmi->szHomepageUrl;

              } else {
                fnUnLoad(&mcInfo);
              }
            }
          }
        }
      }

      if (pMod == 0) {
        Apollo::libraryFree(hDll);
        hDll = NULL;

        for (SupportDll* pSupportDll = 0; (pSupportDll = lSupportDlls.Next(pSupportDll)); ) {
          Apollo::libraryFree(pSupportDll->Handle());
        }

      } else {
        lModules_.AddLast(pMod);

        Msg_Core_ModuleLoaded msg;
        msg.sShortName = sModuleName;
        msg.sDLLPath = sDllPath;
        msg.Send();
        ok = 1;

        apLog_Info((LOG_CHANNEL, "ApCore::loadModule", "Module %s loaded from %s", StringType(sModuleName), StringType(sDllPath)));
      }
    }
  }

  return ok;
}

int ApCore::unloadModule(const char* szModuleName)
{
  int ok = 0;

  String sModuleName = szModuleName;
  ApModule* pMod = (ApModule*) lModules_.FindByName(szModuleName);
  if (pMod == 0) {
    apLog_Warning((LOG_CHANNEL, "ApCore::unloadModule", "Module %s not loaded", StringType(sModuleName)));
  } else {
    AP_MODULE_CALL mcInfo;
    mcInfo.nSize = sizeof(mcInfo);
    mcInfo.nFlags = ApCore_GetInfoFlags();
    //mcInfo.fpGetAddressByName = ApCore_GetAddressByName;
    
    { Msg_Core_BeforeModuleUnloaded msg; msg.sModuleName = szModuleName; msg.Send(); }

    int bUnLoadOk = 0;
    if (pMod->fnUnLoad_ != 0) {  bUnLoadOk = pMod->fnUnLoad_(&mcInfo); }
    if (!bUnLoadOk) {
      apLog_Warning((LOG_CHANNEL, "ApCore::loadModule", "UnLoad() returned 0: %s", StringType(pMod->getName())));
    } else {
      // do it anyway;
    }
    // Unload the module itself
    Apollo::libraryFree(pMod->hDLL_);
    // Free support libraries preloaded on module load
    for (SupportDll* pSupportDll = 0; (pSupportDll = pMod->lSupportDlls_.Next(pSupportDll)); ) {
      Apollo::libraryFree(pSupportDll->Handle());
    }

    lModules_.Remove(pMod);

    Msg_Core_ModuleUnloaded msg;
    msg.sShortName = sModuleName;
    msg.Send();

    apLog_Info((LOG_CHANNEL, "ApCore::unloadModule", "Module %s unloaded", StringType(sModuleName)));

    delete pMod;
    ok = 1;
  }

  return ok;
}

int ApCore::isLoadedModule(const char* szModuleName)
{
  return (lModules_.FindByName(szModuleName) != 0);
}

int ApCore::getModuleInfo(const char* szModuleName, Apollo::KeyValueList& kvInfo)
{
  int ok = 0;

  ApModule* pMod = (ApModule*) lModules_.FindByName(szModuleName);
  if (pMod) {
    kvInfo.add("LongName", pMod->sLongName_);
    kvInfo.add("Version", pMod->sVersion_);
    kvInfo.add("Description", pMod->sDescription_);
    kvInfo.add("Author", pMod->sAuthor_);
    kvInfo.add("Email", pMod->sEmail_);
    kvInfo.add("Copyright", pMod->sCopyright_);
    kvInfo.add("HomepageUrl", pMod->sHomepageUrl_);

    ok = 1;
  }

  return ok;
}

int ApCore::getLoadedModules(Apollo::ValueList& lModules)
{
  for (ApModule* pMod = 0; (pMod = (ApModule*) lModules_.Next(pMod)); ) {
    lModules.add(pMod->getName());
  }

  return 1;
}

// -------------------------------------------------------------------

int ApCore::hookMsg(const char* szMsgType, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition)
{
  ApCallQueue* pQueue = (ApCallQueue*) lMessageMap_.FindByName(szMsgType);

  if (pQueue == 0) {
    pQueue = new ApCallQueue(szMsgType);
    if (pQueue != 0) {
      lMessageMap_.Add(pQueue);
    }
  }

  int nRealPos = nPosition;
  if (nPosition == ApCallbackPosNormal) {
    nRealPos = Apollo::modulePos(0, szModuleName);
  }

  if (pQueue != 0) {
    ApCallQueueElem* eNew = new ApCallQueueElem(fnHandler, nRef, szModuleName, nRealPos);
    if (eNew != 0) {

      int bIsFirst = 1;
      ApCallQueueElem* ePrevious = 0;
      ApCallQueueElem* e = pQueue->First();
      while (e != 0) {
        if (e->Pos() > nRealPos) {
          break;
        }
        bIsFirst = 0;
        ePrevious = e;
        e = pQueue->Next(e);
      }

      if (bIsFirst) {
        pQueue->AddFirst(eNew);
      } else {
        pQueue->AddAfter(ePrevious, eNew);
      }

    }
  }

  return 1;
}

int ApCore::unhookMsg(const char* szMsgType, const char* szModuleName, ApCallback fnHandler, ApCallbackRef pRef)
{
  AP_UNUSED_ARG(szModuleName);
  int ok = 0;
  ApCallQueue* pQueue = (ApCallQueue*) lMessageMap_.FindByName(szMsgType);
  if (pQueue != 0) {
    ApCallQueueElem* qe = pQueue->FindByHandlerAndReference(fnHandler,pRef);
    if (qe != 0) {
      pQueue->Remove(qe);
      delete qe;
      ok = 1;
    } else {
      // not found
      apLog_Verbose((LOG_CHANNEL, "ApCore::unhookMsg", "Unhook failed szMsgType=%s, szModuleName=%s, pRef=&p"
                                                , szMsgType
                                                , szModuleName
                                                , pRef));
    }

    // Do not delete the queue while in message handlers
    // if (pQueue->length() == 0) {
    //   lMessageMap_.Remove(pQueue);
    //   delete pQueue;
    // }
  }

  return ok;
}

int ApCore::Call(ApMessage* pMsg)
{
  int bCalled = 0;

  int bDoCall = 0;
  if (Apollo::isMainThread()) {
    bDoCall = 1;
  } else {
    Msg_System_ThreadMessage msg;
    if (pMsg->getName() == msg.Type()) {
      bDoCall = 1;
    }
  }

  if (!bDoCall) {
    String sMessage;
    sMessage.appendf("Msg delivery denied outside of main thread for %s", StringType(pMsg->getName()));
    Apollo::sendLog(apLog_MaskWarning, LOG_CHANNEL, "ApCore::Call", sMessage);
    pMsg->apStatus = ApMessage::Error;
    AP_DEBUG_BREAK();
  } else {

    if (apLog_IsTrace) {
      if (0
        || pMsg->getName().startsWith("Log_")
        || pMsg->getName().startsWith("Core_")
        || pMsg->getName().startsWith("System_")
        || pMsg->getName().startsWith("MainLoop_")
        ) {
          // Do not trace
      } else {
        apLog_TraceI(LOG_CHANNEL, "ApCore::Call", "%s", StringType(pMsg->getName()));
      }
    }

    ApCallQueue* pQueue = (ApCallQueue*) lMessageMap_.FindByName(pMsg->Type());
    if (pQueue == 0) {
      pMsg->apStatus = ApMessage::Error;
      pMsg->sComment.appendf("No handler for message type=%s", StringType(pMsg->Type()));
    } else {

      bool bContinue = true;
      ApCallQueueIterator iter(*pQueue);
      while (iter.Next() && bContinue) {
        ApCallQueueElem* pCurrent = iter.Current();
        if (pCurrent == 0) {
          // TSNH, Current() can not be 0 after .Next()
          AP_DEBUG_BREAK();
        }
        ApCallback fnCallback = pCurrent->Handler();
        if (fnCallback != 0) {
          pMsg->Ref(pCurrent->Ref());

          #if defined(_DEBUG)
          if (bHeapCheck_) {
            #if defined(_WIN32) && defined(_MSC_VER)
              //_CrtCheckMemory();
              //unsigned char* p = (unsigned char *) malloc(12);
              //((int*)p)[2] = 0;
              //((int*)p)[3] = 0;
              //((int*)p)[4] = 0;
              _CrtCheckMemory();
            #endif
          }
          #endif

          try {

            fnCallback(pMsg);

          #if defined(_DEBUG)
          if (bHeapCheck_) {
            #if defined(_WIN32) && defined(_MSC_VER)
              _CrtCheckMemory();
            #endif
          }
          #endif

          } catch (ApException& ex) {
            apLog_Error((pCurrent->ModuleName(), pMsg->Type(), ex.getText()));
            if (pMsg->sComment.empty()) {
              pMsg->sComment = ex.getText();
            }
            pMsg->apStatus = ApMessage::Error;

          } catch (...) {
            #if defined(_DEBUG)
              AP_DEBUG_BREAK();
            #else
              if (pMsg->Type() != "Log_Line" && pCurrent == iter.Current()) {
                apLog_Fatal((LOG_CHANNEL, "ApCore::Call", "Exception in module:%s executing message:%s", StringType(pCurrent->ModuleName()), StringType(pMsg->Type())));
              }
            #endif
          } // catch

          if (pMsg->apStatus != ApMessage::Unknown && pMsg->Once()) {
            pMsg->Stop();
          }

          if (pMsg->apStatus == ApMessage::Error) {
            pMsg->Stop();
          }

          bContinue = pMsg->Forward();
          bCalled = 1;
        }
      }

    }
  }

  return bCalled == 1;
}

// -------------------------------------------------------------------

void ApCore::On_Core_Hook(Msg_Core_Hook* pMsg)
{
  int ok = hookMsg(pMsg->sCallName, pMsg->sModuleName, pMsg->fnHandler, pMsg->nRef, pMsg->nPosition);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "ApCore::On_Core_Hook", "hookMsg failed: %s %s", StringType(pMsg->sCallName), StringType(pMsg->sModuleName)));
    ok = 0;
  }

  pMsg->Stop();
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void ApCore::On_Core_Unhook(Msg_Core_Unhook* pMsg)
{
  int ok = unhookMsg(pMsg->sCallName, pMsg->sModuleName, pMsg->fnHandler, pMsg->nRef);
  if (!ok) {
    apLog_Verbose((LOG_CHANNEL, "ApCore::On_Core_Unhook", "unhookMsg failed: %s %s", StringType(pMsg->sCallName), StringType(pMsg->sModuleName)));
    ok = 0;
  }

  pMsg->Stop();
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void ApCore::On_Core_LoadModule(Msg_Core_LoadModule* pMsg)
{
  int ok = loadModule(pMsg->sModuleName, pMsg->sDllPath);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "ApCore::On_Core_LoadModule", "loadModule failed: path=%s", StringType(pMsg->sDllPath)));
    ok = 0;
  }

  pMsg->Stop();
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void ApCore::On_Core_UnloadModule(Msg_Core_UnloadModule* pMsg)
{
  int ok = unloadModule(pMsg->sModuleName);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "ApCore::On_Core_UnloadModule", "unloadModule failed: name=%s", StringType(pMsg->sModuleName)));
    ok = 0;
  }

  pMsg->Stop();
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void ApCore::On_Core_IsLoadedModule(Msg_Core_IsLoadedModule* pMsg)
{
  pMsg->bLoaded = isLoadedModule(pMsg->sModuleName);
  pMsg->Stop();
  pMsg->apStatus = ApMessage::Ok;
}

void ApCore::On_Core_GetModuleInfo(Msg_Core_GetModuleInfo* pMsg)
{
  int ok = getModuleInfo(pMsg->sModuleName, pMsg->kvInfo);
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void ApCore::On_Core_GetLoadedModules(Msg_Core_GetLoadedModules* pMsg)
{
  getLoadedModules(pMsg->vlModules);
  pMsg->apStatus = ApMessage::Ok;
}

void ApCore::On_Core_ModulePackageName(Msg_Core_ModulePackageName* pMsg)
{
  if (isLoadedModule(pMsg->sModuleName)) {
    ApModule* pMod = (ApModule*) lModules_.FindByName(pMsg->sModuleName);
    pMsg->sPackageName = pMod->sModulePackageName_;
    pMsg->apStatus = ApMessage::Ok;
  } else {
    pMsg->apStatus = ApMessage::Error;
  }
}

// -------------------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_Hook)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_Unhook)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_LoadModule)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_UnloadModule)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_IsLoadedModule)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_GetModuleInfo)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_GetLoadedModules)
AP_REFINSTANCE_MSG_HANDLER(ApCore, Core_ModulePackageName)

// -------------------------------------------------------------------

ApCore::ApCore()
:bExiting_(0)
,bHeapCheck_(1)
{
}

ApCore::~ApCore()
{
}

int ApCore::Init()
{
  int ok = 1;

  { Msg_Core_Hook msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_Hook), this, ApCallbackPosNormal); }
  { Msg_Core_Unhook msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_Unhook), this, ApCallbackPosNormal); }
  { Msg_Core_LoadModule msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_LoadModule), this, ApCallbackPosNormal); }
  { Msg_Core_UnloadModule msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_UnloadModule), this, ApCallbackPosNormal); }
  { Msg_Core_IsLoadedModule msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_IsLoadedModule), this, ApCallbackPosNormal); }
  { Msg_Core_GetModuleInfo msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_GetModuleInfo), this, ApCallbackPosNormal); }
  { Msg_Core_GetLoadedModules msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_GetLoadedModules), this, ApCallbackPosNormal); }
  { Msg_Core_ModulePackageName msg; hookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_ModulePackageName), this, ApCallbackPosNormal); }
  
  return ok;
}

int ApCore::Exit()
{
  { Msg_Core_ModulePackageName msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_ModulePackageName),this); }
  { Msg_Core_LoadModule msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_LoadModule),this); }
  { Msg_Core_UnloadModule msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_UnloadModule),this); }
  { Msg_Core_IsLoadedModule msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_IsLoadedModule),this); }
  { Msg_Core_GetModuleInfo msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_GetModuleInfo),this); }
  { Msg_Core_GetLoadedModules msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_GetLoadedModules),this); }
  { Msg_Core_Hook msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_Hook),this); }
  { Msg_Core_Unhook msg; unhookMsg(msg.Type(), APCORE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ApCore, Core_Unhook),this); }

  return 1;
}

// -------------------------------------------------------------------
// C-Interface

int Init()
{
  return gApCore.Init();
}

int Exit()
{
  return gApCore.Exit();
}

int Call(ApMessage* pMsg, int nFlags)
{
  int nResult = 0;

  if (nFlags & Apollo::CF_ASYNC) {
    Msg_System_ThreadMessage tm;
    tm.pMsg = pMsg;
    nResult = gApCore.Call(&tm);
    if (nResult) { nResult = (tm.apStatus == ApMessage::Ok ? nResult : 0); }
  } else {
    if (nFlags & Apollo::CF_UNSYNCHRONIZED) {
      nResult = gApCore.Call(pMsg);
    } else {
      if (gApCore.Exiting()) {
        if (gApCore.TryAcquire()) {
          nResult = gApCore.Call(pMsg);
          gApCore.Release();
        } else {
          // do not call
        }
      } else {
        gApCore.Acquire();
        nResult = gApCore.Call(pMsg);
        gApCore.Release();
      }
    }
  }

  return nResult;
}

// -------------------------------------------------------------------

/*
  lIgnoredDllPaths_.Add("Apollo.dll");
  lIgnoredDllPaths_.Add("config.dll");
  lIgnoredDllPaths_.Add("MFCArena.dll");
  lIgnoredDllPaths_.Add("sqlite3.dll");

#elif defined(LINUX)
  #if defined(_DEBUG)
    lIgnoredDllPaths_.Add("libApollod.so");
    lIgnoredDllPaths_.Add("libconfigd.so");
    lIgnoredDllPaths_.Add("libsqlite3d.so");
  #else
    lIgnoredDllPaths_.Add("libApollo.so");
    lIgnoredDllPaths_.Add("libconfig.so");
    lIgnoredDllPaths_.Add("libsqlite3.so");
  #endif
#elif defined(MAC)
  #if defined(_DEBUG)
    lIgnoredDllPaths_.Add("libApollod.dylib");
    lIgnoredDllPaths_.Add("libconfigd.dylib");
    lIgnoredDllPaths_.Add("libsqlite3d.dylib");
  #else
    lIgnoredDllPaths_.Add("libApollo.dylib");
    lIgnoredDllPaths_.Add("libconfig.dylib");
    lIgnoredDllPaths_.Add("libsqlite3.dylib");
  #endif
#else
#error Module loader not implemented for this OS
#endif
*/
