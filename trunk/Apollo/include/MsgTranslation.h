// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgTranslation_h_INCLUDED)
#define MsgTranslation_h_INCLUDED

#include "ApMessage.h"

class Msg_Translation_Get: public ApRequestMessage
{
public:
  Msg_Translation_Get() : ApRequestMessage("Translation_Get") {}
  static String _(const char* szModule, const char* szContext, const char* szText)
  {
    Msg_Translation_Get msg;
    msg.sModule = szModule;
    msg.sContext = szContext;
    msg.sText = szText;
    (void) msg.Request();
    if (msg.sTranslated.empty()) {
      return szText;
    }
    return msg.sTranslated;
  }
public:
  ApIN String sModule;
  ApIN String sContext;
  ApIN String sText;
  ApOUT String sTranslated;
};

class Msg_Translation_SetLanguage: public ApRequestMessage
{
public:
  Msg_Translation_SetLanguage() : ApRequestMessage("Translation_SetLanguage") {}
  ApIN String sLanguage;
};

class Msg_Translation_GetLanguage: public ApRequestMessage
{
public:
  Msg_Translation_GetLanguage() : ApRequestMessage("Translation_GetLanguage") {}
  ApOUT String sLanguage;
};

class Msg_Translation_CurrentLanguage: public ApNotificationMessage
{
public:
  Msg_Translation_CurrentLanguage() : ApNotificationMessage("Translation_CurrentLanguage") {}
  ApIN String sLanguage;
};

class Msg_Translation_Clear: public ApRequestMessage
{
public:
  Msg_Translation_Clear() : ApRequestMessage("Translation_Clear") {}
};

class Msg_Translation_LoadModuleLanguageFile: public ApRequestMessage
{
public:
  Msg_Translation_LoadModuleLanguageFile() : ApRequestMessage("Translation_LoadModuleLanguageFile") {}
  ApIN String sModule;
  ApIN String sLanguage;
};

class Msg_Translation_UnloadLanguage: public ApRequestMessage
{
public:
  Msg_Translation_UnloadLanguage() : ApRequestMessage("Translation_UnloadLanguage") {}
  ApIN String sLanguage;
};

class Msg_Translation_Plane: public ApRequestMessage
{
public:
  Msg_Translation_Plane() : ApRequestMessage("Translation_Plane") {}
  ApIN String sPlane;
};

#endif // !defined(MsgTranslation_h_INCLUDED)
