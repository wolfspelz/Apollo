// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApCore_h_INCLUDED)
#define ApCore_h_INCLUDED

#if defined(WIN32)
  #ifdef APCORE_EXPORTS
    #define APCORE_API __declspec(dllexport)
  #else
    #define APCORE_API __declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) // both mac and linux gcc ver. above 4 support visibility for c and c++
  #if defined(APCORE_EXPORTS)
    // We also need compiler options "-fvisibility=hidden" and "-fvisibility-inlines-hidden"
    // This way all symbols will be "hidden" by default, except these defined with *_API on mac and linux
    // This will heavily reduce binaries' size, speedup loading and btw. avoid exporting of _all_ symbols
    #define APCORE_API __attribute__((visibility("default")))
  #else
    #define APCORE_API
  #endif
#else
  #define APCORE_API
#endif

class APCORE_API ApCallQueueElem: public Elem
{
public:
  ApCallQueueElem(ApCallback fnHandler, ApCallbackRef nRef, const char* szModuleName, int nPosition)
    :Elem(szModuleName)
  {
    setPtr((void*) fnHandler);
    setInt(nPosition);
    nRef_ = nRef;
  }

  ApCallback Handler() { return (ApCallback) getPtr(); }
  ApCallbackRef Ref() { return nRef_; }
  String& ModuleName() { return getName(); }
  ApCallbackPos Pos() { return getInt(); }

  ApCallbackRef nRef_;

protected:
  friend class ListT<ApCallQueueElem, Elem>;
  ApCallQueueElem() {}
  ApCallQueueElem(const String& sName) 
    : Elem(sName) 
  { setPtr((void*) NULL);
    setInt(0);
    nRef_ = 0; 
  }
};

typedef SafeListIteratorT<ApCallQueueElem> ApCallQueueIterator;

class APCORE_API ApCallQueue: public SafeListT<ApCallQueueElem>
{
public:
  ApCallQueue(const char* szName) :SafeListT<ApCallQueueElem>(szName) {}

  ApCallQueueElem* FindByHandler(ApCallback fnHandler)
  {
    return (ApCallQueueElem*) FindByPtr((void*) fnHandler);
  }

  ApCallQueueElem* FindByHandlerAndReference(ApCallback fnHandler, ApCallbackRef pRef)
  {
    ApCallQueueElem *qeStart, *qe;
    qe = qeStart = FindByHandler(fnHandler);
    if (qe != 0){
      do
        if (qe->Handler() == fnHandler && qe->Ref() == pRef){
          return qe;
        }
      while (( qe = Next(qe) ));
    }
    return NULL;
  }
};

class Msg_Core_Hook;
class Msg_Core_Unhook;
class Msg_Core_LoadModule;
class Msg_Core_UnloadModule;
class Msg_Core_IsLoadedModule;
class Msg_Core_GetModuleInfo;
class Msg_Core_GetLoadedModules;
class Msg_Core_ModulePackageName;

class SupportDll: protected Elem
{
public:
  SupportDll(const char* szPath, HMODULE h)
    :Elem(szPath, (long) h)
  {}

  String& Path() { return getName(); }
  HMODULE Handle() { return (HMODULE) getInt(); }

protected:
  friend class ListT<SupportDll, Elem>;
  SupportDll() {}
};

class APCORE_API ApModule: public Elem
{
public:
  ApModule(const char* szModuleName)
    :Elem(szModuleName)
    ,hDLL_(NULL)
    ,fnInfo_(0)
    ,fnLoad_(0)
    ,fnUnLoad_(0)
  {}
  ~ApModule() {}

  HMODULE hDLL_;
  String sDLLPath_;
  String sModulePackageName_;
  String sLangPath_;
  ApModuleInfoF fnInfo_;
  ApModuleLoadF fnLoad_;
  ApModuleUnLoadF fnUnLoad_;
  List lServices_;
  ListT<SupportDll, Elem> lSupportDlls_;

  String sLongName_;
  String sVersion_;
  String sDescription_;
  String sAuthor_;
  String sEmail_;
  String sCopyright_;
  String sHomepageUrl_;
};

class APCORE_API ApCore
{
public:
  ApCore();
  virtual ~ApCore();

  int Init();
  int Exit();

  int loadModule(const char* szModuleName, const char* szDllPath);
  int unloadModule(const char* szModuleName);
  int isLoadedModule(const char* szModuleName);
  int getModuleInfo(const char* szModuleName, Apollo::KeyValueList& kvInfo);
  int getLoadedModules(Apollo::ValueList& lModules);

  int Call(ApMessage* pMsg);

  int hookMsg(const char* szMsgType, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
  int unhookMsg(const char* szMsgType, const char* szModuleName, ApCallback fnHandler, ApCallbackRef pRef);

  void On_Core_Hook(Msg_Core_Hook* pMsg);
  void On_Core_Unhook(Msg_Core_Unhook* pMsg);
  void On_Core_LoadModule(Msg_Core_LoadModule* pMsg);
  void On_Core_UnloadModule(Msg_Core_UnloadModule* pMsg);
  void On_Core_IsLoadedModule(Msg_Core_IsLoadedModule* pMsg);
  void On_Core_GetModuleInfo(Msg_Core_GetModuleInfo* pMsg);
  void On_Core_GetLoadedModules(Msg_Core_GetLoadedModules* pMsg);
  void On_Core_ModulePackageName(Msg_Core_ModulePackageName* pMsg);

  int Exiting() { return bExiting_; }
  void Exiting(int bExiting) { bExiting_ = bExiting; }

  void Acquire() { oMutex_.Acquire(); }
  void Release() { oMutex_.Release(); }
  int TryAcquire() { return oMutex_.TryAcquire(); }

protected:
  int loadModuleConfig(const String& sModuleName, const String& sModuleFile, const String& sConfigFile);
  //int isIgnoredDLLPath(const char* szDllPath);

  List lMessageMap_;
  List lModules_;
  //List lIgnoredDllPaths_;
  int bExiting_;

  Apollo::Mutex oMutex_;
};

extern "C" {
  APCORE_API int Init();
  APCORE_API int Exit();
  APCORE_API int Call(ApMessage* pMsg, int nFlags);

} // extern "C"

#endif // !defined(ApCore_h_INCLUDED)
