// Netcard.h: interface for the CNetcard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETCARD_H__2259C1E2_2403_4DBB_BB10_CE7726E73568__INCLUDED_)
#define AFX_NETCARD_H__2259C1E2_2403_4DBB_BB10_CE7726E73568__INCLUDED_

#include "Registry.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetcard : public CRegistry
{
public:
	CNetcard();
	virtual ~CNetcard();

public:
	BOOL GetEthernet();
	BOOL GetNetcardInfo();

	vector<NETCARD> m_vecNetcard;

private:
	vector<ETHERNET> m_vecEthernet;
};

#endif // !defined(AFX_NETCARD_H__2259C1E2_2403_4DBB_BB10_CE7726E73568__INCLUDED_)
