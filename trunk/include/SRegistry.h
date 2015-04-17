// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SRegistry_h_INCLUDED)
#define SRegistry_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(WIN32)

#include "SSystem.h"

class SExport SRegistry
{
public:
  static int SetString(HKEY hMainKey, const String& sPath, const String& sName, const String& sValue);
  static String GetString(HKEY hMainKey, const String& sPath, const String& sName, const String& sDefault);

  static int SetInt(HKEY hMainKey, const String& sPath, const String& sName, int nValue);
  static int GetInt(HKEY hMainKey, const String& sPath, const String& sName, int nDefault);
  static bool HasKey(HKEY hMainKey, const String& sPath);

  static int Delete(HKEY hMainKey, const String& sPath, const String& sName = "");
  static int Create(HKEY hMainKey, const String& sPath);

  static int Test();
};

#endif // defined(WIN32) 

#endif // !defined(SRegistry_h_INCLUDED)
