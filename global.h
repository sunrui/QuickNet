#if !defined(DEFINE_H__INCLUDED_)
#define DEFINE_H__INCLUDED_

#include <windows.h>
#include <commctrl.h>
#include <snmp.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "snmpapi.lib")
#pragma comment(lib, "comctl32.lib")

using namespace std;

#define BUFFER_SIZE 10240

typedef struct _tagEthernet
{
	string strMACAddress;     // 网卡地址
	string strAdapters;       // 接口号
} ETHERNET;

typedef struct _tagNetcard
{
	ETHERNET Ethernet;

	string strDescription;    // 网卡描述
	string strServiceName;    // CLSID
	string strIPAddress;      // IP 地址
	string strSubnetMask;     // 子网掩码
	string strDefaultGateway; // 默认网关
	string strNameServer;     // DNS 服务器
} NETCARD;

#endif // !defined(DEFINE_H__INCLUDED_)