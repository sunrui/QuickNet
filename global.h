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
	string strMACAddress;     // ������ַ
	string strAdapters;       // �ӿں�
} ETHERNET;

typedef struct _tagNetcard
{
	ETHERNET Ethernet;

	string strDescription;    // ��������
	string strServiceName;    // CLSID
	string strIPAddress;      // IP ��ַ
	string strSubnetMask;     // ��������
	string strDefaultGateway; // Ĭ������
	string strNameServer;     // DNS ������
} NETCARD;

#endif // !defined(DEFINE_H__INCLUDED_)