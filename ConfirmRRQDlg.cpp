// ConfirmRRQDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "ConfirmRRQDlg.h"
#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmRRQDlg dialog


CConfirmRRQDlg::CConfirmRRQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmRRQDlg::IDD, pParent)
{
	m_Daddy=NULL;
	//{{AFX_DATA_INIT(CConfirmRRQDlg)
	m_File = _T("");
	m_Host = _T("");
	//}}AFX_DATA_INIT
}


void CConfirmRRQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmRRQDlg)
	DDX_Text(pDX, IDC_FILE, m_File);
	DDX_Text(pDX, IDC_HOST, m_Host);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmRRQDlg, CDialog)
	//{{AFX_MSG_MAP(CConfirmRRQDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmRRQDlg message handlers

BOOL CConfirmRRQDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Daddy->m_Daddy->m_bnw.StartSound(m_Daddy->m_Daddy->m_bnwRequest);
	if(m_Daddy->m_Daddy->m_PromptTimeOut)
		SetTimer(1,m_Daddy->m_Daddy->m_PromptTimeOut*1000,NULL);
	SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfirmRRQDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
		EndDialog(IDCANCEL);
	CDialog::OnTimer(nIDEvent);
}
