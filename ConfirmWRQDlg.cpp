// ConfirmWRQDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "ConfirmWRQDlg.h"

#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmWRQDlg dialog


CConfirmWRQDlg::CConfirmWRQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmWRQDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmWRQDlg)
	m_File = _T("");
	m_Host = _T("");
	//}}AFX_DATA_INIT
}


void CConfirmWRQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmWRQDlg)
	DDX_Control(pDX, IDC_RESUME, m_ResumeCtl);
	DDX_Control(pDX, IDOK, m_OkCtl);
	DDX_Control(pDX, IDC_RENAME, m_RenameCtl);
	DDX_Text(pDX, IDC_FILE, m_File);
	DDX_Text(pDX, IDC_HOST, m_Host);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmWRQDlg, CDialog)
	//{{AFX_MSG_MAP(CConfirmWRQDlg)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESUME, OnResume)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmWRQDlg message handlers

void CConfirmWRQDlg::OnRename() 
{
	EndDialog(IDC_RENAME);
}

BOOL CConfirmWRQDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Daddy->m_Daddy->m_bnw.StartSound(m_Daddy->m_Daddy->m_bnwRequest);
	if(m_Daddy->m_Daddy->m_PromptTimeOut)
		SetTimer(1,m_Daddy->m_Daddy->m_PromptTimeOut*1000,NULL);
	SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
	m_ResumeCtl.EnableWindow(m_Daddy->m_bResume);
	if(m_Daddy->m_Rename){
		m_OkCtl.SetButtonStyle(m_OkCtl.GetButtonStyle()&~BS_DEFPUSHBUTTON);
		m_RenameCtl.SetButtonStyle(m_RenameCtl.GetButtonStyle()|BS_DEFPUSHBUTTON);
		m_RenameCtl.SetFocus();
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfirmWRQDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
		EndDialog(IDCANCEL);
	CDialog::OnTimer(nIDEvent);
}

void CConfirmWRQDlg::OnResume() 
{
	EndDialog(IDC_RESUME);
}
