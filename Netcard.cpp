// Netcard.cpp: implementation of the CNetcard class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "Netcard.h"

typedef BOOL(WINAPI* pSnmpExtensionInit)(IN DWORD dwTimeZeroReference,
										 OUT HANDLE* hPollForTrapEvent,
										 OUT AsnObjectIdentifier* supportedView);

typedef BOOL(WINAPI* pSnmpExtensionTrap)(OUT AsnObjectIdentifier* enterprise,
										 OUT AsnInteger* genericTrap,
										 OUT AsnInteger* specificTrap,
										 OUT AsnTimeticks* timeStamp,
										 OUT RFC1157VarBindList* variableBindings); 

typedef BOOL(WINAPI* pSnmpExtensionQuery)(IN BYTE requestType,
										  IN OUT RFC1157VarBindList* variableBindings,
										  OUT AsnInteger* errorStatus,
										  OUT AsnInteger* errorIndex);

typedef BOOL(WINAPI* pSnmpExtensionInitEx)(OUT AsnObjectIdentifier* supportedView);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetcard::CNetcard()
{

}

CNetcard::~CNetcard()
{

}

BOOL CNetcard::GetNetcardInfo()
{
	TCHAR szDescription[256] = "\0";
	TCHAR szServiceName[256] = "\0";

	TCHAR szIPAddress[16] = "\0";
	TCHAR szSubnetMask[16] = "\0";
	TCHAR szDefaultGateway[16] = "\0";
	TCHAR szNameServer[16] = "\0";

	TCHAR szSubKey[256] = "\0";
	BOOL  bRetVal = TRUE;

	if (!m_vecNetcard.empty())
	{
		m_vecNetcard.clear();
	}

	vector<ETHERNET>::iterator iEthernet;
	for (iEthernet = m_vecEthernet.begin(); iEthernet != m_vecEthernet.end(); iEthernet++)
	{
		ETHERNET Ethernet = *iEthernet;

		// Description
		wsprintf(szSubKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\%s", Ethernet.strAdapters.c_str());
		bRetVal = GetQueryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_SZ, "Description", szDescription);

		// CLSID
		bRetVal = GetQueryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_SZ, "ServiceName", szServiceName);
		if (!bRetVal || lstrlen(szServiceName) == 0)
		{
			continue;
		}

		wsprintf(szSubKey, "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s", szServiceName);
		bRetVal = GetQueryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_MULTI_SZ, "IPAddress", szIPAddress);
		bRetVal = GetQueryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_MULTI_SZ, "SubnetMask", szSubnetMask);
		bRetVal = GetQueryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_MULTI_SZ, "DefaultGateway", szDefaultGateway);
		bRetVal = GetQueryValue(HKEY_LOCAL_MACHINE, szSubKey, REG_SZ, "NameServer", szNameServer);

		NETCARD Netcard;
		Netcard.Ethernet = Ethernet;
		Netcard.strDescription = szDescription;
		Netcard.strServiceName = szServiceName;
		Netcard.strNameServer = szNameServer;
		Netcard.strIPAddress = szIPAddress;
		Netcard.strSubnetMask = szSubnetMask;
		Netcard.strDefaultGateway = szDefaultGateway;
		Netcard.strNameServer = szNameServer;

		m_vecNetcard.push_back(Netcard);
	}

	return !m_vecNetcard.empty();
}

BOOL CNetcard::GetEthernet()
{
	WSADATA wsaData;
	WORD wVersion;

	wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData) != 0)
	{
		return FALSE;
	}

	if (m_vecEthernet.size())
	{
		m_vecEthernet.clear();
	}

	HINSTANCE m_hInst;
	pSnmpExtensionInit m_Init;
	pSnmpExtensionInitEx m_InitEx;
	pSnmpExtensionQuery m_Query;
	pSnmpExtensionTrap m_Trap;
	HANDLE PollForTrapEvent;
	AsnObjectIdentifier SupportedView;

	UINT OID_ifEntryType[] = {
		1, 3, 6, 1, 2, 1, 2, 2, 1, 3 };
	UINT OID_ifEntryNum[] = {
		1, 3, 6, 1, 2, 1, 2, 1 };
	UINT OID_ipMACEntAddr[] = {
		1, 3, 6, 1, 2, 1, 2, 2, 1, 6 };

	AsnObjectIdentifier MIB_ifMACEntAddr = {
		sizeof(OID_ipMACEntAddr) /sizeof(UINT), OID_ipMACEntAddr };
	AsnObjectIdentifier MIB_ifEntryType = {
		sizeof(OID_ifEntryType) /sizeof(UINT), OID_ifEntryType };
	AsnObjectIdentifier MIB_ifEntryNum = {
		sizeof(OID_ifEntryNum) /sizeof(UINT), OID_ifEntryNum };

	RFC1157VarBindList varBindList;
	RFC1157VarBind varBind[2];
	AsnInteger errorStatus;
	AsnInteger errorIndex;
	AsnObjectIdentifier MIB_NULL = { 0, 0 };

	int ret;
	int dtmp;
	int i = 0, j = 0;
	BOOL found = FALSE;
	char TempEthernet[32];
	m_Init = NULL;
	m_InitEx = NULL;
	m_Query = NULL;
	m_Trap = NULL;

	/* Load the SNMP dll and get the addresses of the functions necessary */
	m_hInst = LoadLibrary("inetmib1.dll");
	if (m_hInst < (HINSTANCE) HINSTANCE_ERROR)
	{
		m_hInst = NULL;
		return FALSE;
	}

	m_Init = (pSnmpExtensionInit) GetProcAddress(m_hInst, "SnmpExtensionInit");
	m_InitEx = (pSnmpExtensionInitEx) GetProcAddress(m_hInst, "SnmpExtensionInitEx");
	m_Query = (pSnmpExtensionQuery) GetProcAddress(m_hInst, "SnmpExtensionQuery");
	m_Trap = (pSnmpExtensionTrap) GetProcAddress(m_hInst, "SnmpExtensionTrap");
	m_Init(GetTickCount(), &PollForTrapEvent, &SupportedView);

	/* Initialize the variable list to be retrieved by m_Query */
	varBindList.list = varBind;
	varBind[0].name = MIB_NULL;
	varBind[1].name = MIB_NULL;

	/* Copy in the OID to find the number of entries in the 
	Inteface table */
	varBindList.len = 1;/* Only retrieving one item */
	SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryNum);
	ret = m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex);

	varBindList.len = 2;

	/* Copy in the OID of ifType, the type of interface */
	SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryType);

	/* Copy in the OID of ifPhysAddress, the address */
	SNMP_oidcpy(&varBind[1].name, &MIB_ifMACEntAddr);

	do {
	
	/* Submit the query. Responses will be loaded into varBindList. 
	We can expect this call to succeed a # of times corresponding 
		to the # of adapters reported to be in the system */
		ret = m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus, &errorIndex);
		if (!ret) {
			ret = 1;
		}
		else {
			/* Confirm that the proper type has been returned */
			ret = SNMP_oidncmp(&varBind[0].name, &MIB_ifEntryType, MIB_ifEntryType.idLength);
		}

		if (!ret) {
			j++;

			dtmp = varBind[0].value.asnValue.number;

			/* Type 6 describes ethernet interfaces */
			if (dtmp == 6) {
				/* Confirm that we have an address here */
				ret = 
					SNMP_oidncmp(&varBind[1].name, &MIB_ifMACEntAddr, 
					MIB_ifMACEntAddr.idLength);

				if ((!ret) && (varBind[1].value.asnValue.address.stream != NULL))
				{
					if ( 
						(varBind[1].value.asnValue.address.stream[0] == 0x44) &&
						(varBind[1].value.asnValue.address.stream[1] == 0x45) &&
						(varBind[1].value.asnValue.address.stream[2] == 0x53) &&
						(varBind[1].value.asnValue.address.stream[3] == 0x54) &&
						(varBind[1].value.asnValue.address.stream[4] == 0x00))
					{
						/* Ignore all dial-up networking adapters */
						continue;
					}
					if ((varBind[1].value.asnValue.address.stream[0] == 0x00) &&
						(varBind[1].value.asnValue.address.stream[1] == 0x00) &&
						(varBind[1].value.asnValue.address.stream[2] == 0x00) &&
						(varBind[1].value.asnValue.address.stream[3] == 0x00) &&
						(varBind[1].value.asnValue.address.stream[4] == 0x00) &&
						(varBind[1].value.asnValue.address.stream[5] == 0x00))
					{
						/* Ignore NULL addresses returned by other network 
						interfaces */
						continue;
					}

					sprintf_s(TempEthernet, "%02x-%02x-%02x-%02x-%02x-%02x", 
						varBind[1].value.asnValue.address.stream[0], 
						varBind[1].value.asnValue.address.stream[1], 
						varBind[1].value.asnValue.address.stream[2], 
						varBind[1].value.asnValue.address.stream[3], 
						varBind[1].value.asnValue.address.stream[4], 
						varBind[1].value.asnValue.address.stream[5]);

					ETHERNET Ethernet;
					TCHAR szAdapter[2];

					_strupr_s(TempEthernet);
					sprintf_s(szAdapter, "%d", j);
					Ethernet.strMACAddress = TempEthernet;
					Ethernet.strAdapters = szAdapter;

					m_vecEthernet.push_back(Ethernet);
				}
			}
		}
		} while (!ret); /* Stop only on an error. An error will occur
						when we go exhaust the list of interfaces to 
						be examined */

	/* Free the bindings */
	SNMP_FreeVarBind(&varBind[0]);
	SNMP_FreeVarBind(&varBind[1]);
	WSACleanup();

	if (m_vecEthernet.empty())
	{
		return FALSE;
	}

	return TRUE;
}
