/************************************************************************/
/*      Copyright (C) Stas Khirman 1998.  All rights reserved.          */
/*      Written by Stas Khirman (staskh@rocketmail.com).                */
/*            and                         */
/*                 Raz Galili (razgalili@hotmail.com)           */
/*                                                                      */
/*      Free software: no warranty; use anywhere is ok; spread the      */
/*      sources; note any modifications; share variations and           */
/*      derivatives (including sending to staskh@rocketmail.com).       */
/*                                                                      */
/************************************************************************/

#include <winsock.h>

#include "ApCompiler.h"
#include "MibAccess.h"

static AP_UNUSED_FUNCTION int IsWin9598Me()
{
  int isWin95 = 0;
  OSVERSIONINFOEX osvi;
  BOOL bOsVersionInfoEx;
  
  // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
  // If that fails, try using the OSVERSIONINFO structure.
  
  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  
  if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
  {
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
      return FALSE;
  }
  
  switch (osvi.dwPlatformId)
  {
    // Test for the Windows Me/98/95.
  case VER_PLATFORM_WIN32_WINDOWS:
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
      isWin95 = 1;
    } 
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
      isWin95 = 1;
    } 
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
      isWin95 = 1;
    } 
    break;
  default:
    break;
  }
  return isWin95;
}

MibExtLoad::MibExtLoad(LPCSTR MibDllName)
{
  m_Init = NULL;  
  m_InitEx = NULL;
  m_Query = NULL; 
  m_Trap = NULL;
  
  char path[MAX_PATH] = "";
  GetSystemDirectoryA(path, MAX_PATH-1);

  if (strlen(path) > 1) {
    strcat(path, "\\");
    strcat(path, MibDllName);
  } else {
    strcpy(path, MibDllName);
  }

  m_hInst = LoadLibraryA(path);
  if(m_hInst < (HINSTANCE) HINSTANCE_ERROR) {
    m_hInst = NULL;
    return;
  }   

  m_Init = (pSnmpExtensionInit) GetProcAddress(m_hInst, "SnmpExtensionInit");
  m_InitEx = (pSnmpExtensionInitEx) GetProcAddress(m_hInst, "SnmpExtensionInitEx");
  m_Query = (pSnmpExtensionQuery) GetProcAddress(m_hInst, "SnmpExtensionQuery");
  m_Trap = (pSnmpExtensionTrap) GetProcAddress(m_hInst, "SnmpExtensionTrap");
  m_Close = (pSnmpExtensionClose) GetProcAddress(m_hInst, "SnmpExtensionClose");
}

MibExtLoad::~MibExtLoad()
{
  if (m_hInst) {
    if (m_Close) m_Close();
    FreeLibrary(m_hInst);
  }

  m_hInst = NULL;
}

BOOL MibExtLoad::Init(DWORD dwTimeZeroReference,HANDLE *hPollForTrapEvent,AsnObjectIdentifier *supportedView)
{
  if(m_hInst && m_Init)
    return m_Init(dwTimeZeroReference,hPollForTrapEvent,supportedView);
  return FALSE;
}
BOOL MibExtLoad::InitEx(AsnObjectIdentifier *supportedView)
{
  if(m_hInst && m_InitEx)
    return m_InitEx(supportedView);
  
  return FALSE;
}

BOOL MibExtLoad::Query(BYTE requestType,OUT RFC1157VarBindList *variableBindings,
             AsnInteger *errorStatus,AsnInteger *errorIndex)
{
  if(m_hInst && m_Query)
    return m_Query(requestType,variableBindings,errorStatus,errorIndex);
  
  return FALSE;
}

BOOL MibExtLoad::Trap(AsnObjectIdentifier *enterprise, AsnInteger *genericTrap,
            AsnInteger *specificTrap, AsnTimeticks *timeStamp, 
            RFC1157VarBindList  *variableBindings)
{
  if(m_hInst && m_Trap)
    return m_Trap(enterprise, genericTrap,specificTrap, timeStamp, variableBindings);
  
  return FALSE;
}

MibII::MibII():MibExtLoad("Inetmib1.dll")
//MibII::MibII():MibExtLoad("Snmpapi.dll")
{
  WSADATA wsa;
  int   err;

  err = WSAStartup( 0x0101, &wsa );  
}

MibII::~MibII()
{
  WSACleanup();
}

int MibII::Init(void)
{
  HANDLE PollForTrapEvent;
  AsnObjectIdentifier SupportedView;

  return MibExtLoad::Init(GetTickCount(),&PollForTrapEvent,&SupportedView);

}


BOOL MibII::GetIPAddress(UINT IpArray[],UINT &IpArraySize)
{
  
  UINT OID_ipAdEntAddr[] = { 1, 3, 6, 1, 2, 1, 4 , 20, 1 ,1 };
  AsnObjectIdentifier MIB_ipAdEntAddr = { sizeof(OID_ipAdEntAddr)/sizeof(UINT), OID_ipAdEntAddr };
  RFC1157VarBindList  varBindList;
  RFC1157VarBind      varBind[1];
  AsnInteger          errorStatus;
  AsnInteger          errorIndex;
  AsnObjectIdentifier MIB_NULL = {0,0};
  BOOL                Exit;
  int                 ret;
  unsigned int        IpCount=0;
  DWORD               dtmp;
  
  varBindList.list = varBind;
  varBindList.len  = 1;
  varBind[0].name  = MIB_NULL;
  SNMP_oidcpy(&varBind[0].name,&MIB_ipAdEntAddr);
  Exit = FALSE;
  
  IpCount=0;
  while(!Exit){
    ret = Query(ASN_RFC1157_GETNEXTREQUEST,&varBindList,&errorStatus,&errorIndex);
    
    if(!ret)
      Exit=TRUE;
    else{
      ret = SNMP_oidncmp(&varBind[0].name,&MIB_ipAdEntAddr,MIB_ipAdEntAddr.idLength);
      if(ret!=0){
        Exit=TRUE;
      }
      else{
        //hw illegal access: "address" was invalid
        dtmp = *((DWORD *)varBind[0].value.asnValue.address.stream);
        IpArray[IpCount] = dtmp;
        IpCount++;
        if(IpCount>=IpArraySize)
          Exit = TRUE;
      }
    }
  }
  
  IpArraySize = IpCount;
  
  SNMP_FreeVarBind(&varBind[0]);

  return Exit;
}

BOOL MibII::GetIPMask(UINT IpArray[],UINT &IpArraySize)
{
  
  UINT OID_ipAdEntMask[] = { 1, 3, 6, 1, 2, 1, 4 , 20, 1 ,3 };
  AsnObjectIdentifier MIB_ipAdEntMask = { sizeof(OID_ipAdEntMask)/sizeof(UINT), OID_ipAdEntMask };
  RFC1157VarBindList  varBindList;
  RFC1157VarBind      varBind[1];
  AsnInteger          errorStatus;
  AsnInteger          errorIndex;
  AsnObjectIdentifier MIB_NULL = {0,0};
  BOOL                Exit;
  int                 ret;
  unsigned int        IpCount=0;
  DWORD               dtmp;
  
  varBindList.list = varBind;
  varBindList.len  = 1;
  varBind[0].name  = MIB_NULL;
  SNMP_oidcpy(&varBind[0].name,&MIB_ipAdEntMask);
  Exit = FALSE;
  
  IpCount=0;
  while(!Exit){
    ret = Query(ASN_RFC1157_GETNEXTREQUEST,&varBindList,&errorStatus,&errorIndex);
    
    if(!ret)
      Exit=TRUE;
    else{
      ret = SNMP_oidncmp(&varBind[0].name,&MIB_ipAdEntMask,MIB_ipAdEntMask.idLength);
      if(ret!=0){
        Exit=TRUE;
      }
      else{
        dtmp = *((DWORD *)varBind[0].value.asnValue.address.stream);
        IpArray[IpCount] = dtmp;
        IpCount++;
        if(IpCount>=IpArraySize)
          Exit = TRUE;
      }
    }
  }
  
  IpArraySize = IpCount;
  
  SNMP_FreeVarBind(&varBind[0]);

  return Exit;

}

