// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "Registry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{

}

CRegistry::~CRegistry()
{

}

BOOL CRegistry::SetRegistryValue(HKEY hKeyName, LPCTSTR lpSubKey, DWORD dwType, LPCTSTR lpValueName, LPCTSTR lpValue)
{
    DWORD cbData = lstrlen(lpValue);
	BOOL  bRetVal = FALSE;
	HKEY  hKey;
    LONG  lRet;

	lRet = RegOpenKeyEx(hKeyName, lpSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet != ERROR_SUCCESS)
	{
		return bRetVal;
	}

	lRet = RegSetValueEx(hKey, lpValueName, 0, dwType, (LPBYTE)lpValue, cbData);
	if (lRet == ERROR_SUCCESS)
	{
		bRetVal = TRUE;
	}
	RegCloseKey(hKey);

	return bRetVal;
}

BOOL CRegistry::GetQueryValue(HKEY hKeyName, LPCTSTR lpSubKey, DWORD dwType, LPCTSTR lpValueName, LPTSTR lpValue)
{
	TCHAR szData[_MAX_PATH] = "\0";
	DWORD cbData = _MAX_PATH;
	BOOL  bRetVal = TRUE;
	HKEY  hKey;
    LONG  lRet;

	lRet = RegOpenKeyEx(hKeyName, lpSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet != ERROR_SUCCESS)
	{
		return bRetVal;
	}

	lRet = RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)szData, &cbData);
	if (lRet == ERROR_SUCCESS)
	{
		bRetVal = TRUE;
	}

	if (bRetVal && lpValue != NULL)
	{
		lstrcpy(lpValue, szData);
	}
	RegCloseKey(hKey);

	return bRetVal;
}

BOOL CRegistry::SetRegistryValue(HKEY hKeyName, LPCTSTR lpSubKey, DWORD dwType, LPCTSTR lpValueName, DWORD dwValue)
{
	BOOL  bRetVal = TRUE;
    DWORD cbData = sizeof(DWORD);
	HKEY  hKey;
    LONG  lRet;

	lRet = RegOpenKeyEx(hKeyName, lpSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet != ERROR_SUCCESS)
	{
		bRetVal = FALSE;
	}

	lRet = RegSetValueEx(hKey, lpValueName, 0, dwType, (LPBYTE)&dwValue, cbData);
	if (lRet != ERROR_SUCCESS)
	{
		bRetVal = FALSE;
	}
	RegCloseKey(hKey);

	return bRetVal;
}
