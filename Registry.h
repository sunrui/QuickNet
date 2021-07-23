// Registry.h: interface for the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__F2BB9C8A_EF6B_48F2_81B8_BCD9A4B3EEB0__INCLUDED_)
#define AFX_REGISTRY_H__F2BB9C8A_EF6B_48F2_81B8_BCD9A4B3EEB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry  
{
public:
	CRegistry();
	virtual ~CRegistry();

public:
	BOOL SetRegistryValue(HKEY hKeyName, LPCTSTR lpSubKey, DWORD dwType, LPCTSTR lpValueName, DWORD dwValue);
	BOOL GetQueryValue(HKEY hKeyName, LPCTSTR lpSubKey, DWORD dwType, LPCTSTR lpValueName, LPTSTR lpValue);
	BOOL SetRegistryValue(HKEY hKeyName, LPCTSTR lpSubKey, DWORD dwType, LPCTSTR lpValueName, LPCTSTR lpValue);
};

#endif // !defined(AFX_REGISTRY_H__F2BB9C8A_EF6B_48F2_81B8_BCD9A4B3EEB0__INCLUDED_)
