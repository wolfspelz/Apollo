// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "babelfish.h"
#include "MsgTranslation.h"
#include "MsgCore.h"
#include "STree.h"
#include "SrpcGateHelper.h"

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

#define LOG_CHANNEL "Translation"
#define MODULE_NAME "Translation"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "Translation",
  "Translation Module",
  "1",
  "Translates text",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

BABELFISH_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class ModuleText
{
public:
  ModuleText() {}

public:
  StringTree<String> stText_;
};

class Language: public Elem
{
public:
  Language(const char* szLanguage) :Elem(szLanguage) {}
  virtual ~Language();

public:
  StringTree<ModuleText*> stModules_;
};

class TranslationPlane: public Elem
{
public:
  TranslationPlane(const char* szId)
    :Elem(szId)
  {}

public:
  ListT<Language, Elem> lLanguages_;
};

class BabelfishModule
{
public:
  BabelfishModule()
    :sCurrentPlane_("default")
  {}

  int init();
  int exit();

  void On_Translation_Get(Msg_Translation_Get* pMsg);
  void On_Translation_SetLanguage(Msg_Translation_SetLanguage* pMsg);
  void On_Translation_GetLanguage(Msg_Translation_GetLanguage* pMsg);
  void On_Translation_Clear(Msg_Translation_Clear* pMsg);
  void On_Translation_LoadModuleLanguageFile(Msg_Translation_LoadModuleLanguageFile* pMsg);
  void On_Translation_UnloadLanguage(Msg_Translation_UnloadLanguage* pMsg);
  void On_Translation_Plane(Msg_Translation_Plane* pMsg);
  void On_Core_ModuleLoaded(Msg_Core_ModuleLoaded* pMsg);

protected:
  int getText(String& sLang, String& sModule, String& sContext, String& sText, String& sTranslated);

protected:
  String sContextSeparator_;
  String sDefaultLanguage_;
  String sCurrentLanguage_;

  String sCurrentPlane_;
  ListT<TranslationPlane, Elem> lPlanes_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
};

typedef ApModuleSingleton<BabelfishModule> BabelfishModuleInstance;

//----------------------------------------------------------

Language::~Language()
{
  StringTreeNode<ModuleText*>* pNode = 0;
  while ((pNode = stModules_.Next(0)) != 0) {
    String sKey = pNode->Key();
    ModuleText* pText = pNode->Value();
    if (stModules_.Unset(sKey)) {
      delete pText;
      pText = 0;
    }
  }
}

//----------------------------------------------------------

void SrpcGate_Translation_Get(ApSRPCMessage* pMsg)
{
  Msg_Translation_Get msg;
  msg.sModule = pMsg->srpc.getString("sModule");
  msg.sContext = pMsg->srpc.getString("sContext");
  msg.sText = pMsg->srpc.getString("sText");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.setString("sTranslated", msg.sTranslated);
}

//----------------------------------------------------------

int BabelfishModule::init()
{
  sContextSeparator_ = Apollo::getModuleConfig(MODULE_NAME, "ContextSeparator", ".");
  sDefaultLanguage_ = Apollo::getModuleConfig(MODULE_NAME, "DefaultLanguage", "en");
  sCurrentLanguage_ = sDefaultLanguage_;

  srpcGateRegistry_.add("Translation_Get", SrpcGate_Translation_Get);

  return 1;
}

int BabelfishModule::exit()
{
  srpcGateRegistry_.finish();
  return 1;
}

int BabelfishModule::getText(String& sLang, String& sModule, String& sContext, String& sText, String& sTranslated)
{
  int ok = 0;

  TranslationPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  if (pPlane != 0) {
    Language* pLang = pPlane->lLanguages_.FindByName(sLang);
    if (pLang != 0) {
      StringTreeNode<ModuleText*>* pModuleTextNode = pLang->stModules_.Find(sModule);
      if (pModuleTextNode != 0) {
        ModuleText* pText = pModuleTextNode->Value();
        if (pText != 0) {
          String sKey;
          if (!sContext.empty()) {
            sKey = sContext + sContextSeparator_ + sText;
          } else {
            sKey = sText;
          }
          StringTreeNode<String>* pTextNode = pText->stText_.Find(sKey);
          if (pTextNode != 0) {
            sTranslated = pTextNode->Value();
            ok = 1;
          }
        }
      }
    }
  } // plane

  return ok;
}

//----------------------------------------------------------

void BabelfishModule::On_Translation_Get(Msg_Translation_Get* pMsg)
{
 int ok = 0;

  ok = getText(sCurrentLanguage_, pMsg->sModule, pMsg->sContext, pMsg->sText, pMsg->sTranslated);
  if (!ok && sCurrentLanguage_ != sDefaultLanguage_) {
    (void) getText(sDefaultLanguage_, pMsg->sModule, pMsg->sContext, pMsg->sText, pMsg->sTranslated);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void BabelfishModule::On_Translation_SetLanguage(Msg_Translation_SetLanguage* pMsg)
{
  int ok = 1;

  Msg_Core_GetLoadedModules modulesMsg;
  if (ok) {
    ok = modulesMsg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "BabelfishModule::On_Translation_SetLanguage", "Msg_Core_GetLoadedModules failed"));
    }
  }

  if (ok) {
    for (Apollo::ValueElem* e = 0; (e = modulesMsg.vlModules.nextElem(e)); ) {
      Msg_Translation_LoadModuleLanguageFile msg;
      msg.sModule = e->getString();
      msg.sLanguage = pMsg->sLanguage;
      (void) msg.Request();
    }
  }

  sCurrentLanguage_ = pMsg->sLanguage;
  Msg_Translation_CurrentLanguage msg; 
  msg.sLanguage = sCurrentLanguage_;
  msg.Send();
  apLog_Info((LOG_CHANNEL, "BabelfishModule::On_Translation_SetLanguage", "Setting Language to %s", StringType(sCurrentLanguage_)));

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void BabelfishModule::On_Translation_GetLanguage(Msg_Translation_GetLanguage* pMsg)
{
  pMsg->sLanguage = sCurrentLanguage_;
  pMsg->apStatus = ApMessage::Ok;
}

void BabelfishModule::On_Translation_Clear(Msg_Translation_Clear* pMsg)
{
  apLog_Verbose((LOG_CHANNEL, "BabelfishModule::On_Translation_Clear", ""));

  TranslationPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  if (pPlane != 0) {
    Language* pLang = 0;
    while ((pLang = pPlane->lLanguages_.First()) != 0) {
      pPlane->lLanguages_.Remove(pLang);
      delete pLang;
      pLang = 0;
    }
  } // plane

  pMsg->apStatus = ApMessage::Ok;
}

void BabelfishModule::On_Translation_LoadModuleLanguageFile(Msg_Translation_LoadModuleLanguageFile* pMsg)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "BabelfishModule::On_Translation_LoadModuleLanguageFile", "module=%s lang=%s", StringType(pMsg->sModule), StringType(pMsg->sLanguage)));

  String sSelector = "Language/";
  sSelector += pMsg->sLanguage;
  String sPath = Apollo::getModuleConfig(pMsg->sModule, sSelector, "");

  if (!sPath.empty()) {
    String sData;
    if (!Apollo::loadFile(sPath, sData)) {
      ok = 0;
      apLog_Warning((LOG_CHANNEL, "BabelfishModule::On_Translation_LoadModuleLanguageFile", "file.Load(%s) failed", StringType(sPath)));
    } else {
      List lData;
      KeyValueBlob2List(sData, lData, "\r\n", "=", "");

      TranslationPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
      if (pPlane == 0) {
        pPlane = new TranslationPlane(sCurrentPlane_);
        if (pPlane != 0) {
          lPlanes_.Add(pPlane);
        }
      }

      if (pPlane != 0) {
        Language* pLang = pPlane->lLanguages_.FindByName(pMsg->sLanguage);
        if (pLang == 0) {
          pLang = new Language(pMsg->sLanguage);
          if (pLang != 0) {
            pPlane->lLanguages_.AddFirst(pLang);
          }
        } // if (pLang == 0)

        if (pLang != 0) {
          // Delete old module text map
          StringTreeNode<ModuleText*>* pModuleTextNode = pLang->stModules_.Find(pMsg->sModule);
          if (pModuleTextNode != 0) {
            ModuleText* pText = pModuleTextNode->Value();
            if (pText != 0) {
              delete pText;
              pText = 0;
            }
            pLang->stModules_.Unset(pMsg->sModule);
          }

          // Create new module text map
          ModuleText* pText = new ModuleText();
          if (pText != 0) {
            pLang->stModules_.Set(pMsg->sModule, pText);
          }

          if (pText != 0) {
            for (Elem* e = 0; (e = lData.Next(e)); ) {
              String sText = e->getName();
              sText.trimWSP();
              if (!sText.startsWith("#")) {
                String sTranslated = e->getString();
                sTranslated.trimWSP();
                sTranslated.unescape(String::EscapeCRLF);
                if (!sTranslated.empty()) {
                  pText->stText_.Set(e->getName(), sTranslated);
                }
              }
            }
          } // if (pText != 0)

        } // if (pLang != 0)
      } // plane
      
    } // if (fData.Load()
  } // if (!sPath.empty())

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void BabelfishModule::On_Translation_UnloadLanguage(Msg_Translation_UnloadLanguage* pMsg)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "BabelfishModule::On_Translation_UnloadLanguage", "lang=%s", StringType(pMsg->sLanguage)));

  String sLanguage = pMsg->sLanguage;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void BabelfishModule::On_Translation_Plane(Msg_Translation_Plane* pMsg)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "BabelfishModule::On_Translation_Plane", "plane=%s", StringType(pMsg->sPlane)));

  sCurrentPlane_ = pMsg->sPlane;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void BabelfishModule::On_Core_ModuleLoaded(Msg_Core_ModuleLoaded* pMsg)
{
  int ok = 1;

  Msg_Translation_LoadModuleLanguageFile msg;
  msg.sModule = pMsg->sShortName;
  msg.sLanguage = sCurrentLanguage_;
  (void) msg.Request();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_Get)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_SetLanguage)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_GetLanguage)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_Clear)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_LoadModuleLanguageFile)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_UnloadLanguage)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Translation_Plane)
AP_REFINSTANCE_MSG_HANDLER(BabelfishModule, Core_ModuleLoaded)

//----------------------------------------------------------

BABELFISH_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  (void) BabelfishModuleInstance::Get()->init();

  { Msg_Translation_Get msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_Get), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Translation_SetLanguage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_SetLanguage), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Translation_GetLanguage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_GetLanguage), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Translation_Clear msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_Clear), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Translation_LoadModuleLanguageFile msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_LoadModuleLanguageFile), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Translation_UnloadLanguage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_UnloadLanguage), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Translation_Plane msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_Plane), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Core_ModuleLoaded msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Core_ModuleLoaded), BabelfishModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  {
    Msg_Translation_SetLanguage msg;
    msg.sLanguage = Apollo::getModuleConfig(MODULE_NAME, "Language", "en");
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "BabelfishModule Load", "Msg_Translation_SetLanguage failed"));
    }
  }

  return BabelfishModuleInstance::Get() != 0;
}

BABELFISH_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_Translation_Get msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_Get), BabelfishModuleInstance::Get()); }
  { Msg_Translation_SetLanguage msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_SetLanguage), BabelfishModuleInstance::Get()); }
  { Msg_Translation_GetLanguage msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_GetLanguage), BabelfishModuleInstance::Get()); }
  { Msg_Translation_Clear msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_Clear), BabelfishModuleInstance::Get()); }
  { Msg_Translation_LoadModuleLanguageFile msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_LoadModuleLanguageFile), BabelfishModuleInstance::Get()); }
  { Msg_Translation_UnloadLanguage msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_UnloadLanguage), BabelfishModuleInstance::Get()); }
  { Msg_Translation_Plane msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Translation_Plane), BabelfishModuleInstance::Get()); }
  { Msg_Core_ModuleLoaded msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(BabelfishModule, Core_ModuleLoaded), BabelfishModuleInstance::Get()); }

  (void) BabelfishModuleInstance::Get()->exit();

  BabelfishModuleInstance::Delete();

  return 1;
}
