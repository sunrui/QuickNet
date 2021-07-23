// QuickNet.cpp: implementation of the CQuickNet class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "QuickNet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuickNet::CQuickNet()
{
	InitLocalIni();
	InitIniCheck();
}

CQuickNet::~CQuickNet()
{

}

BOOL CQuickNet::HasExistFile(LPCTSTR lpFileName)
{
	WIN32_FIND_DATA findData;
	HANDLE hFindFile;
	BOOL bResult = TRUE;

	hFindFile = FindFirstFile(lpFileName, &findData);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		bResult = FALSE;
	}
	FindClose(hFindFile);

	return bResult;
}

void CQuickNet::InitLocalIni()
{
	TCHAR szLocalDir[_MAX_PATH] = "\0";
	TCHAR szLocalApp[_MAX_PATH] = "\0";

	GetModuleFileName(NULL, szLocalApp, _MAX_PATH);
	for (int i = lstrlen(szLocalApp); i > 0; i--)
	{
		if (szLocalApp[i] == '\\')
		{
			for (int j = 0; j < i; j++)
			{
				szLocalDir[j] = szLocalApp[j];
			}

			break;
		}
	}

	wsprintf(m_szLocalIni, "%s\\Interface.ini", szLocalDir);
}

void CQuickNet::InitIniCheck()
{
	if (!HasExistFile(GetLocalIni()))
	{
		TCHAR szTitle[256];
		SYSTEMTIME st;

		GetLocalTime(&st);
		wsprintf(szTitle, "%d-%d-%d %d:%d:%d",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond);

		WritePrivateProfileString(szTitle, "MACAddress", "", GetLocalIni());
		WritePrivateProfileString(szTitle, "ComputerName", "", GetLocalIni());
		WritePrivateProfileString(szTitle, "IPAddress", "", GetLocalIni());
		WritePrivateProfileString(szTitle, "SubnetMask", "", GetLocalIni());
		WritePrivateProfileString(szTitle, "DefaultGateway", "", GetLocalIni());
		WritePrivateProfileString(szTitle, "NameServer", "", GetLocalIni());
	}
}

BOOL CQuickNet::AppValueGet()
{
	TCHAR szBuffer[BUFFER_SIZE] = "\0";
	LPCTSTR lpAppName;

	GetPrivateProfileSectionNames(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), GetLocalIni());
	for (lpAppName = szBuffer; *lpAppName != NULL; lpAppName += lstrlen(lpAppName) + 1)
	{
		LPCTSTR lpMACAddress = GetProfileString(lpAppName, "MACAddress");

		if (lstrlen(lpMACAddress) != 0)
		{
			vector<NETCARD>::iterator iter;
			for (iter = m_vecNetcard.begin(); iter != m_vecNetcard.end(); iter++)
			{
				NETCARD Netcard = *iter;

				if (_stricmp(Netcard.Ethernet.strMACAddress.c_str(), lpMACAddress) == 0)
				{
					wsprintf(m_szComputerName, MakeValidComputerName(GetProfileString(lpAppName, "ComputerName")));
					m_cfgNetcard.strIPAddress = MakeValidIPAddress(GetProfileString(lpAppName, "IPAddress"));
					m_cfgNetcard.strSubnetMask = MakeValidIPAddress(GetProfileString(lpAppName, "SubnetMask"));
					m_cfgNetcard.strDefaultGateway = MakeValidIPAddress(GetProfileString(lpAppName, "DefaultGateway"));
					m_cfgNetcard.strNameServer = MakeValidIPAddress(GetProfileString(lpAppName, "NameServer"));

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CQuickNet::HasSetConfig()
{
	TCHAR szRunName[_MAX_PATH];
	DWORD cbData = _MAX_PATH;
	BOOL  bRetVal = FALSE;
	HKEY  hKey;
    LONG  lRet;

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_ALL_ACCESS,
		&hKey);

	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegQueryValueEx(hKey, "QuickNet", NULL, NULL, (LPBYTE)&szRunName, &cbData);

		if (lRet == ERROR_SUCCESS)
		{
			TCHAR szLocalApp[_MAX_PATH];

			GetModuleFileName(NULL, szLocalApp, _MAX_PATH);
			if (lstrcmpi(szLocalApp, szRunName) == 0)
			{
				RegDeleteValue(hKey, "QuickNet");
				bRetVal = TRUE;
			}
		}
		RegCloseKey(hKey);
	}

	return bRetVal;
}

LPCTSTR CQuickNet::GetLocalIni()
{
	return m_szLocalIni;
}

LPCTSTR CQuickNet::MakeValidComputerName(LPCTSTR lpComputerName)
{
	static TCHAR szComputerName[15] = "\0";

	if (lstrlen(lpComputerName) == 0)
	{
		return "QuickNet";
	}

	for (int i = 0; i < 15; i++)
	{
		szComputerName[i] = lpComputerName[i];
	}

	return szComputerName;
}

LPCTSTR CQuickNet::MakeValidIPAddress(LPCTSTR lpIPAddress)
{
	static TCHAR szNewIPAddr[16] = "\0";
	DWORD dwIPAddress;
	in_addr addr;

	if (lstrlen(lpIPAddress) == 0)
	{
		return "0.0.0.0";
	}

	dwIPAddress = inet_addr(lpIPAddress);
	addr.S_un.S_addr = (dwIPAddress);
	wsprintf(szNewIPAddr, inet_ntoa(addr));

	return szNewIPAddr;
}

LPCTSTR CQuickNet::GetProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName)
{
	static TCHAR szKeyValue[_MAX_PATH] = "\0";
	DWORD dwKeyRet;

	dwKeyRet = GetPrivateProfileString(lpAppName, lpKeyName, NULL,
		szKeyValue, _MAX_PATH, GetLocalIni());
	if (dwKeyRet == 0)
	{
		return "";
	}

	return szKeyValue;
}