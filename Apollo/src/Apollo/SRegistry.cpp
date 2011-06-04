// ============================================================================

#if defined(WIN32)

#include "SRegistry.h"

int SRegistry::Create(HKEY hMainKey, const String& sPath)
{
  int ok = 1;

  String sPieces = sPath;

  // Now create the damn path from top to bottom
  HKEY hParent = NULL;
  if (::RegOpenKeyEx(hMainKey, _T(""), 0, KEY_ALL_ACCESS, &hParent) == ERROR_SUCCESS) {
    String sPiece;
    while (sPieces.nextToken("\\/", sPiece)) {
      if (sPiece != "") {
        HKEY hPiece = NULL;
        if (::RegOpenKeyEx(hParent, sPiece, 0, KEY_ALL_ACCESS, &hPiece) != ERROR_SUCCESS) {
          DWORD dwDisposition;
          ::RegCreateKeyEx(hParent, sPiece, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hPiece, &dwDisposition);
        }
        if (hPiece != NULL) {
          if (hParent != hMainKey) {
            ::RegCloseKey(hParent);
          }
          hParent = hPiece;
        }
      }
    }
  }
  if (hParent != hMainKey) {
    ::RegCloseKey(hParent);
  }

  return ok;
}

int SRegistry::SetString(HKEY hMainKey, const String& sPath, const String& sName, const String& sValue)
{
  int ok = 0;

  HKEY hKey = NULL;
  if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
    hKey = NULL;

    (void) Create(hMainKey, sPath);

    // next try
    if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
      hKey = NULL;
    }
  }

  if (hKey != NULL) {
    if (::RegSetValueEx(hKey, sName, 0, REG_SZ, (UCHAR*) (PTSTR) sValue, sizeof(TCHAR) * (sValue.chars() + 1)) == ERROR_SUCCESS) {
      ok = 1;
    }
    ::RegCloseKey(hKey);
  }

  return ok;
}

int SRegistry::SetInt(HKEY hMainKey, const String& sPath, const String& sName, int nValue)
{
  int ok = 0;

  HKEY hKey = NULL;
  if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
    hKey = NULL;

    (void) Create(hMainKey, sPath);

    // next try
    if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
      hKey = NULL;
    }
  }

  if (hKey != NULL) {
    if (::RegSetValueEx(hKey, sName, 0, REG_DWORD, (UCHAR*) &nValue, sizeof(nValue)) == ERROR_SUCCESS) {
      ok = 1;
    }
    ::RegCloseKey(hKey);
  }

  return ok;
}

String SRegistry::GetString(HKEY hMainKey, const String& sPath, const String& sName, const String& sDefault)
{
  String sValue = sDefault;

  HKEY hKey = NULL;
  if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD dwDummy;
    TCHAR buf[8192]; buf[0]= _T('\0');
    DWORD dwSize = sizeof(buf);
    if (::RegQueryValueEx(hKey, sName, 0, &dwDummy, (UCHAR*) buf, &dwSize) == ERROR_SUCCESS ) {
      sValue = buf;
    }
    ::RegCloseKey(hKey);
  }

  return sValue;
}

int SRegistry::GetInt(HKEY hMainKey, const String& sPath, const String& sName, int nDefault)
{
  int nValue = nDefault;

  HKEY hKey = NULL;
  if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD dwType;
    char buf[1000]; buf[0]= '\0';
    DWORD dwSize = sizeof(buf);
    if (::RegQueryValueEx(hKey, sName, 0, &dwType, (LPBYTE) buf, &dwSize) == ERROR_SUCCESS ) {
      if (dwType == REG_DWORD) {
        nValue = *((int*) buf);
      } else if (dwType == REG_SZ) {
        nValue = atoi(buf);
      } else {
        nValue = 0;
      }
    }
    ::RegCloseKey(hKey);
  }

  return nValue;
}

int SRegistry::Delete(HKEY hMainKey, const String& sPath, const String& sName)
{
  int ok = 0;

  HKEY hKey = NULL;
  if (::RegOpenKeyEx(hMainKey, sPath, 0, KEY_ALL_ACCESS | DELETE, &hKey) == ERROR_SUCCESS) {
    if (sName.empty()) {

      DWORD dwIndex= 0;
      LONG lRetCode = ERROR_SUCCESS;
      while (lRetCode == ERROR_SUCCESS) {
        TCHAR pszName[MAX_PATH];
        DWORD cName = sizeof(pszName);
        FILETIME ftLastWriteTime;
        lRetCode= RegEnumKeyEx(
          hKey, // handle to key to enumerate
          dwIndex, // subkey index
          (PTSTR) &pszName, // subkey name
          &cName, // size of subkey buffer
          NULL,
          NULL, // class string buffer
          NULL, // size of class string buffer
          &ftLastWriteTime // last write time
        );

        if (lRetCode == ERROR_SUCCESS) {
          ok = SRegistry::Delete(hKey, pszName);
        }
      }

      lRetCode = ::RegDeleteKey(hMainKey, sPath);
		  if (lRetCode == ERROR_SUCCESS) {
		    ok = 1;
      }
    } else {
      LONG lRetCode = ::RegDeleteValue(hKey, sName);
		  if (lRetCode == ERROR_SUCCESS) { 
		    ok = 1;
      }
    }
    ::RegCloseKey(hKey);
  }

  return ok;
}

int SRegistry::Test()
{
  SRegistry::SetString(                HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Name", "42");
  String sValue = SRegistry::GetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Name", "41");

  SRegistry::SetInt(             HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Int", 42);
  int nValue = SRegistry::GetInt(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Int", 41);

  SRegistry::SetString(                       HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "", "43");
  String sDefaultValue = SRegistry::GetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "", "41");

  SRegistry::Delete(                          HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Name");
  String sDeletedValue = SRegistry::GetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Name", "41");

  SRegistry::SetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Name1", "Value1");
  SRegistry::SetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\yy", "Name2", "Value2");
  SRegistry::SetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx\\zz", "Name4", "Value4");
  SRegistry::SetString(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx", "Name3", "Value3");
  SRegistry::Delete(HKEY_CURRENT_USER, "Software\\Test\\Section\\SubSection\\xx");
  SRegistry::Delete(HKEY_CURRENT_USER, "Software\\Test");

  return sValue == "42" && sDefaultValue == "43" && sDeletedValue == "41" && nValue == 42;
}

#endif // defined(WIN32) 
