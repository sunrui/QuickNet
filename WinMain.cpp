/**
 * @biref QuickNet Application
 * @auther Sunrui <Smallrui@126.com>
 * @date 2008-8-2
 */

static char sszAppInfo[] = "QuickNet Application by Sunrui <Smallrui@126.com>.";

#include "global.h"
#include "resource.h"
#include "QuickNet.h"

void ReportNoFindEthernet(HWND hWnd);
void ReportNoConfigDb(HWND hWnd);
DWORD WINAPI ConfigProc(LPVOID lpParameter);

BOOL CALLBACK QuickNetDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

CQuickNet theApp;

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nShowCmd)
{
	if (theApp.HasSetConfig())
	{
		DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), NULL, ConfigDialogProc, NULL);
	}
	else
	{
		if (!theApp.GetEthernet())
		{
			ReportNoFindEthernet(NULL);
			return -1;
		}

		DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_QUICKNET_DIALOG), NULL, QuickNetDialogProc, NULL);
	}

	return 0;
}

void ReportNoFindEthernet(HWND hWnd)
{
	MessageBox(hWnd, "当前系统中没有找到活动的网卡! ", "QuickNet", MB_ICONERROR);
}

void ReportNoConfigDb(HWND hWnd)
{
	MessageBox(hWnd,
		"没有找到符合本机的预配置信息在本地数据库中!\n\n"
		"  在 Interface.ini 中无法加载预配置信息!",
		"QuickNet",
		MB_OK);
}

BOOL CALLBACK QuickNetDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitCommonControls();
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_QUICKNET_ICON));
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1] = "\0";
			DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
			GetComputerName(szComputerName, &dwSize);
			SetDlgItemText(hDlg, IDC_EDIT_COMPUTERNAME, szComputerName);

			theApp.GetNetcardInfo();
			vector<NETCARD>::iterator iter;
			for (iter = theApp.m_vecNetcard.begin(); iter != theApp.m_vecNetcard.end(); iter++)
			{
				NETCARD Netcard = *iter;

				SetDlgItemText(hDlg, IDC_EDIT_MACADDRESS, Netcard.Ethernet.strMACAddress.c_str());
				SetDlgItemText(hDlg, IDC_EDIT_DESCRIPTION, Netcard.strDescription.c_str());
				SetDlgItemText(hDlg, IDC_EDIT_CLSID, Netcard.strServiceName.c_str());

				SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_IP), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strIPAddress.c_str())));
				SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_SUBNETMASK), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strSubnetMask.c_str())));
				SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_DEFAULTGATEWAY), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strDefaultGateway.c_str())));
				SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_NAMESERVER), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strNameServer.c_str())));

				break;
			}

			if (theApp.m_vecNetcard.size() > 1)
			{
				SetTimer(hDlg, 0x00L, 250, NULL);
			}

			return TRUE;
		}
	case WM_TIMER:
		{
			switch (wParam)
			{
			case 0x00L:
				{
					static BOOL bState = FALSE;
					static int nFlash = 0;

					ShowWindow(GetDlgItem(hDlg, IDC_STATIC_ICON), bState ? SW_SHOW : SW_HIDE);
					if (nFlash == 6)
					{
						ShowWindow(GetDlgItem(hDlg, IDC_STATIC_ICON), SW_SHOW);
						KillTimer(hDlg, 0x00L);
						nFlash = 0;
					}

					bState =! bState;
					nFlash++;

					break;
				}
			}

			return TRUE;
		}
	case WM_COMMAND:
		{
			switch (wParam)
			{
			case IDC_STATIC_ICON:
				{
					if (theApp.m_vecNetcard.size() <= 1)
					{
						return TRUE;
					}

					static int nCurNetcard = 0;
					if (nCurNetcard == theApp.m_vecNetcard.size())
					{
						nCurNetcard = 0;
					}

					NETCARD Netcard= theApp.m_vecNetcard[nCurNetcard];

					SetDlgItemText(hDlg, IDC_EDIT_MACADDRESS, Netcard.Ethernet.strMACAddress.c_str());
					SetDlgItemText(hDlg, IDC_EDIT_DESCRIPTION, Netcard.strDescription.c_str());
					SetDlgItemText(hDlg, IDC_EDIT_CLSID, Netcard.strServiceName.c_str());

					SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_IP), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strIPAddress.c_str())));
					SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_SUBNETMASK), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strSubnetMask.c_str())));
					SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_DEFAULTGATEWAY), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strDefaultGateway.c_str())));
					SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_NAMESERVER), IPM_SETADDRESS, 0, (LPARAM) htonl(inet_addr(Netcard.strNameServer.c_str())));

					nCurNetcard++;

					return TRUE;
				}
			case IDC_CHECK_CONFIG:
				{
					if (!theApp.AppValueGet())
					{
						ReportNoConfigDb(hDlg);

						if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG) == BST_CHECKED)
						{
							CheckDlgButton(hDlg, IDC_CHECK_CONFIG, BST_UNCHECKED);
						}

						return TRUE;
					}

					int nRet = MessageBox(hDlg,
						"QuickNet 将在下次启动时设置以下信息: \n\n"
						"  - 取消获取自动指派的 IP 设置\n"
						"  - 所有本地网卡的网络信息\n"
						"  - 完整的计算机名称",
						"QuickNet",
						MB_OKCANCEL);

					if (nRet == IDOK)
					{
						TCHAR szLocalApp[_MAX_PATH];
						BOOL bRetVal;

						GetModuleFileName(NULL, szLocalApp, _MAX_PATH);
						bRetVal = theApp.SetRegistryValue(HKEY_LOCAL_MACHINE,
							"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
							REG_SZ, "QuickNet", szLocalApp);

						if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG) == BST_UNCHECKED)
						{
							CheckDlgButton(hDlg, IDC_CHECK_CONFIG, BST_CHECKED);
						}
					}
					else
					{
						HKEY hKey;
						LONG lRet;

						lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
							"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
							0, KEY_ALL_ACCESS, &hKey);
						if (lRet == ERROR_SUCCESS)
						{
							RegDeleteValue(hKey, "QuickNet");
							RegCloseKey(hKey);
						}

						if (IsDlgButtonChecked(hDlg, IDC_CHECK_CONFIG) == BST_CHECKED)
						{
							CheckDlgButton(hDlg, IDC_CHECK_CONFIG, BST_UNCHECKED);
						}
					}

					return TRUE;
				}
			case IDOK:
				{
					if (theApp.AppValueGet())
					{
						SetDlgItemText(hDlg, IDC_EDIT_COMPUTERNAME, theApp.m_szComputerName);
						SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_IP), IPM_SETADDRESS, 0,
							(LPARAM) htonl(inet_addr(theApp.m_cfgNetcard.strIPAddress.c_str())));
						SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_SUBNETMASK), IPM_SETADDRESS, 0,
							(LPARAM) htonl(inet_addr(theApp.m_cfgNetcard.strSubnetMask.c_str())));
						SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_DEFAULTGATEWAY), IPM_SETADDRESS, 0,
							(LPARAM) htonl(inet_addr(theApp.m_cfgNetcard.strDefaultGateway.c_str())));
						SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_NAMESERVER), IPM_SETADDRESS, 0,
							(LPARAM) htonl(inet_addr(theApp.m_cfgNetcard.strNameServer.c_str())));
					}

					return TRUE;
				}
			case IDCANCEL:
				{
					EndDialog(hDlg, 0);
					return TRUE;
				}
			}
			return TRUE;
		}
	case WM_CLOSE:
		{
			EndDialog(hDlg, 0);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CALLBACK ConfigDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_QUICKNET_ICON));
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			SendMessage(GetDlgItem(hDlg, IDC_PROGRESS_CONFIG), PBM_SETRANGE, 0, MAKELPARAM(0, 7));
			SendMessage(GetDlgItem(hDlg, IDC_PROGRESS_CONFIG), PBM_SETPOS, 0, 0L);

			HANDLE hConfig = CreateThread(NULL, 0, ConfigProc, (LPVOID) hDlg, 0, NULL);
			CloseHandle(hConfig);

			return TRUE;
		}
	case WM_CLOSE:
		{
			EndDialog(hDlg, 0);
			return TRUE;
		}
	}

	return FALSE;
}

DWORD WINAPI ConfigProc(LPVOID lpParameter)
{
	TCHAR szCfgName[][16] = { "初始化...", "取消 DHCP...", "计算机名称...", "IP 地址...",
		"子网掩码...", "默认网关...", "DNS 服务器...", "设置完成..." };

	HWND hDlg = (HWND)lpParameter;
	TCHAR szSubKey[_MAX_PATH] = "\0";

	for (int i = 0; i < sizeof(szCfgName) / 16; i++)
	{
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_STATE), szCfgName[i]);
		SendMessage(GetDlgItem(hDlg, IDC_PROGRESS_CONFIG), PBM_SETPOS, i, 0L);

		switch (i)
		{
		case 0: // 初始化
			{
				Sleep(500);
				if (!theApp.GetEthernet())
				{
					ReportNoFindEthernet(hDlg);
					EndDialog(hDlg, 0);
					return -1;
				}

				theApp.GetNetcardInfo();
				if (!theApp.AppValueGet())
				{
					ReportNoConfigDb(hDlg);
					EndDialog(hDlg, 0);
				}
				Sleep(100);
				break;
			}
		case 1: // 取消 DHCP
			{
				vector<NETCARD>::iterator iter;
				for (iter = theApp.m_vecNetcard.begin(); iter != theApp.m_vecNetcard.end(); iter++)
				{
					NETCARD Netcard = *iter;

					wsprintf(szSubKey, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s",
						Netcard.strServiceName.c_str());

					theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_DWORD, "EnableDHCP", (DWORD)0);
				}
				Sleep(150);
				break;
			}
		case 2: // 计算机名称
			{
				theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName", REG_SZ, "ComputerName", theApp.m_szComputerName);
				theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", REG_SZ, "Hostname", theApp.m_szComputerName);
				theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", REG_SZ, "NV Hostname", theApp.m_szComputerName);
				Sleep(150);
				break;
			}
		case 3: // IP 地址
			{
				vector<NETCARD>::iterator iter;
				for (iter = theApp.m_vecNetcard.begin(); iter != theApp.m_vecNetcard.end(); iter++)
				{
					NETCARD Netcard = *iter;

					wsprintf(szSubKey, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s",
						Netcard.strServiceName.c_str());

					theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_MULTI_SZ, "IPAddress", theApp.m_cfgNetcard.strIPAddress.c_str());
				}
				Sleep(150);
				break;
			}
		case 4: // 子网掩码
			{
				vector<NETCARD>::iterator iter;
				for (iter = theApp.m_vecNetcard.begin(); iter != theApp.m_vecNetcard.end(); iter++)
				{
					NETCARD Netcard = *iter;

					wsprintf(szSubKey, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s",
						Netcard.strServiceName.c_str());

					theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_MULTI_SZ, "SubnetMask", theApp.m_cfgNetcard.strSubnetMask.c_str());
				}
				Sleep(150);
				break;
			}
		case 5: // 默认网关
			{
				vector<NETCARD>::iterator iter;
				for (iter = theApp.m_vecNetcard.begin(); iter != theApp.m_vecNetcard.end(); iter++)
				{
					NETCARD Netcard = *iter;

					wsprintf(szSubKey, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s",
						Netcard.strServiceName.c_str());

					theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_MULTI_SZ, "DefaultGateway", theApp.m_cfgNetcard.strDefaultGateway.c_str());
				}
				Sleep(150);					
				break;
			}
		case 6: // DNS 服务器
			{
				vector<NETCARD>::iterator iter;
				for (iter = theApp.m_vecNetcard.begin(); iter != theApp.m_vecNetcard.end(); iter++)
				{
					NETCARD Netcard = *iter;

					wsprintf(szSubKey, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s",
						Netcard.strServiceName.c_str());

					theApp.SetRegistryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_SZ, "NameServer", theApp.m_cfgNetcard.strNameServer.c_str());
				}
				Sleep(150);
				break;
			}
		case 7: // 设置完成
			{
				theApp.m_vecNetcard.clear();
				Sleep(500);
				break;
			}
		}
	}
	EndDialog(hDlg, 0);

	return 0;
}