// Retrier.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "Retrier.h"

#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRetrier

CRetrier::CRetrier(CPumpKINDlg *daddy)
	: m_Daddy(daddy)
{
}

CRetrier::~CRetrier()
{
}


BEGIN_MESSAGE_MAP(CRetrier, CWnd)
	//{{AFX_MSG_MAP(CRetrier)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRetrier message handlers

void CRetrier::OnTimer(UINT nIDEvent) 
{
	ASSERT(m_Daddy);
CXferSocket *socket;
	if(m_Daddy->m_Xfers.Lookup(nIDEvent,socket))
		socket->OnRetry();
	CWnd::OnTimer(nIDEvent);
}
