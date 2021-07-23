// QuickNet.h: interface for the CQuickNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUICKNET_H__47752468_91FD_4BF1_AFC3_2EC77B92E6C1__INCLUDED_)
#define AFX_QUICKNET_H__47752468_91FD_4BF1_AFC3_2EC77B92E6C1__INCLUDED_

#include "Netcard.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQuickNet : public CNetcard
{
public:
	CQuickNet();
	virtual ~CQuickNet();

	LPCTSTR GetProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName);
	LPCTSTR MakeValidIPAddress(LPCTSTR lpIPAddress);
	LPCTSTR MakeValidComputerName(LPCTSTR lpComputerName);
	LPCTSTR GetLocalIni();
	BOOL HasSetConfig();
	BOOL AppValueGet();
	void InitIniCheck();
	BOOL HasExistFile(LPCTSTR lpFileName);

	NETCARD m_cfgNetcard;
	TCHAR m_szComputerName[15];

private:
	void InitLocalIni();
	TCHAR m_szLocalIni[_MAX_PATH];
};

#endif // !defined(AFX_QUICKNET_H__47752468_91FD_4BF1_AFC3_2EC77B92E6C1__INCLUDED_)
