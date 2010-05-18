// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgVpi_h_INCLUDED)
#define MsgVpi_h_INCLUDED

#include "ApMessage.h"

// app -> vpi
// Clear the VPI cache
class Msg_Vpi_Clear: public ApRequestMessage
{
public:
  Msg_Vpi_Clear() : ApRequestMessage("Vpi_Clear") {}
};

// Asynchronous request for location xml
// app -> vpi
class Msg_Vpi_LocationXmlRequest: public ApRequestMessage
{
public:
  Msg_Vpi_LocationXmlRequest() : ApRequestMessage("Vpi_LocationXmlRequest") {}
  ApIN ApHandle hRequest;
  ApIN String sDocumentUrl;
};

// Asynchronous response with location xml
// vpi -> app
class Msg_Vpi_LocationXmlResponse: public ApNotificationMessage
{
public:
  Msg_Vpi_LocationXmlResponse() : ApNotificationMessage("Vpi_LocationXmlResponse"), bSuccess(0) {}
  ApIN ApHandle hRequest;
  ApIN int bSuccess;
  ApIN String sDocumentUrl;
  ApIN String sLocationXml;
};

// Synchronous request for location xml
// app -> vpi -> app
class Msg_Vpi_GetLocationXml: public ApRequestMessage
{
public:
  Msg_Vpi_GetLocationXml() : ApRequestMessage("Vpi_GetLocationXml") {}
  ApIN String sDocumentUrl;
  ApOUT String sLocationXml;
};

// -------------------------------

// app -> vpi
class Msg_Vpi_GetStatus: public ApRequestMessage
{
public:
  Msg_Vpi_GetStatus() : ApRequestMessage("Vpi_GetStatus") {}
  ApOUT Apollo::KeyValueList kvStatus;
};

// app -> vpi
class Msg_Vpi_GetTraversedFiles: public ApRequestMessage
{
public:
  Msg_Vpi_GetTraversedFiles() : ApRequestMessage("Vpi_GetTraversedFiles") {}
  ApOUT Apollo::ValueList vlFiles;
};

// vpi -> vpi
class Msg_Vpi_RequestFile: public ApFilterMessage
{
public:
  Msg_Vpi_RequestFile() : ApFilterMessage("Vpi_RequestFile") {}
  ApIN String sUrl;
};

// vpi -> vpi
class Msg_Vpi_ReceiveFile: public ApNotificationMessage
{
public:
  Msg_Vpi_ReceiveFile() : ApNotificationMessage("Vpi_ReceiveFile") {}
  ApIN String sUrl;
  ApIN String sContentType;
  ApIN String sData;
};

// app -> vpi
class Msg_Vpi_GetLocationUrl: public ApRequestMessage
{
public:
  Msg_Vpi_GetLocationUrl() : ApRequestMessage("Vpi_GetLocationUrl") {}
  ApIN String sLocationXml;
  ApOUT String sLocationUrl;
};

// app -> vpi
class Msg_Vpi_GetDetailXml: public ApRequestMessage
{
public:
  Msg_Vpi_GetDetailXml() : ApRequestMessage("Vpi_GetDetailXml"), bInnerXml(1) {}
  ApIN String sLocationXml;
  ApIN String sPath;
  ApIN int bInnerXml;
  ApOUT String sXml;
};

// app -> vpi
class Msg_Vpi_SetSuffix: public ApRequestMessage
{
public:
  Msg_Vpi_SetSuffix() : ApRequestMessage("Vpi_SetSuffix") {}
  ApIN String sSuffix;
};

// app -> vpi
class Msg_Vpi_GetSuffix: public ApRequestMessage
{
public:
  Msg_Vpi_GetSuffix() : ApRequestMessage("Vpi_GetSuffix") {}
  ApOUT String sSuffix;
};

// app -> vpi
class Msg_Vpi_GetSelectOptions: public ApRequestMessage
{
public:
  Msg_Vpi_GetSelectOptions() : ApRequestMessage("Vpi_GetSelectOptions") {}
  ApIN String sLocationXml;
  ApOUT Apollo::ValueList vlOptions;
};

// app -> vpi
class Msg_Vpi_GetSelectOptionProperties: public ApRequestMessage
{
public:
  Msg_Vpi_GetSelectOptionProperties() : ApRequestMessage("Vpi_GetSelectOptionProperties") {}
  ApIN String sLocationXml;
  ApIN String sSuffix;
  ApOUT Apollo::KeyValueList kvProperties;
};

// --------------------------------

// vpi -> protocol modules
class Msg_Vpi_ComposeLocation: public ApRequestMessage
{
public:
  Msg_Vpi_ComposeLocation() : ApRequestMessage("Vpi_ComposeLocation") {}
  ApIN String sProtocol;
  ApIN String sName;
  ApIN String sService;
  ApOUT String sLocationUrl;
};

#endif // !defined(MsgVpi_h_INCLUDED)
