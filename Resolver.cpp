// Resolver.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "Resolver.h"
#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResolver

CResolver::CResolver()
{
	ASSERT(NULL);
}

CResolver::~CResolver()
{
}


BEGIN_MESSAGE_MAP(CResolver, CWnd)
	//{{AFX_MSG_MAP(CResolver)
	ON_WM_CREATE()
	ON_MESSAGE(WM_RESOLVED, OnResolved)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResolver message handlers

CResolver::CResolver(CXferSocket *socket)
	: socket(socket)
{
}

int CResolver::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(socket);
CString hostName = socket->m_HostName;
int at = hostName.Find('@');
	if(at>=0)
		hostName=hostName.Mid(at+1);
	m_hAsync=WSAAsyncGetHostByName(m_hWnd,WM_RESOLVED,(LPCTSTR)hostName,(char*)socket->m_ResolveBuff,sizeof(socket->m_ResolveBuff));
	ASSERT(m_hAsync);
	return 0;
}

LRESULT CResolver::OnResolved(WPARAM wP,LPARAM lP)
{
	if(WSAGETASYNCERROR(lP))
		socket->OnFailedToResolve();
	else
		socket->OnResolved();
	return 0;
}

BOOL CResolver::Resolve()
{
	return Create(NULL,"PumpKIN-Resolver",WS_CHILD,CRect(0,0,0,0),socket->m_Daddy,0);
}
