// PropsSounds.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PropsSounds.h"
#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropsSounds property page

IMPLEMENT_DYNCREATE(CPropsSounds, CPropertyPage)

CPropsSounds::CPropsSounds() : CPropertyPage(CPropsSounds::IDD)
{
	//{{AFX_DATA_INIT(CPropsSounds)
	m_Abort = _T("");
	m_Success = _T("");
	m_Request = _T("");
	//}}AFX_DATA_INIT
}

CPropsSounds::~CPropsSounds()
{
}

void CPropsSounds::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropsSounds)
	DDX_Control(pDX, IDC_RING_PLAY, m_RequestPlayCtl);
	DDX_Control(pDX, IDC_RING_BROWSE, m_RequestBrowseCtl);
	DDX_Control(pDX, IDC_RING, m_RequestCtl);
	DDX_Control(pDX, IDC_FINISHED_PLAY, m_SuccessPlayCtl);
	DDX_Control(pDX, IDC_FINISHED_BROWSE, m_SuccessBrowseCtl);
	DDX_Control(pDX, IDC_FINISHED, m_SuccessCtl);
	DDX_Control(pDX, IDC_ABORTED_PLAY, m_AbortPlayCtl);
	DDX_Control(pDX, IDC_ABORTED_BROWSE, m_AbortBrowseCtl);
	DDX_Control(pDX, IDC_ABORTED, m_AbortCtl);
	DDX_CBString(pDX, IDC_ABORTED, m_Abort);
	DDX_CBString(pDX, IDC_FINISHED, m_Success);
	DDX_CBString(pDX, IDC_RING, m_Request);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropsSounds, CPropertyPage)
	//{{AFX_MSG_MAP(CPropsSounds)
	ON_BN_CLICKED(IDC_ABORTED_BROWSE, OnAbortedBrowse)
	ON_BN_CLICKED(IDC_FINISHED_BROWSE, OnFinishedBrowse)
	ON_BN_CLICKED(IDC_RING_BROWSE, OnRingBrowse)
	ON_BN_CLICKED(IDC_ABORTED_PLAY, OnAbortedPlay)
	ON_BN_CLICKED(IDC_FINISHED_PLAY, OnFinishedPlay)
	ON_BN_CLICKED(IDC_RING_PLAY, OnRingPlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropsSounds message handlers

BOOL CPropsSounds::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
HICON hP = AfxGetApp()->LoadIcon(IDI_PLAY);
HICON hB = AfxGetApp()->LoadIcon(IDI_BROWSE);
	m_RequestPlayCtl.SetIcon(hP);
	m_SuccessPlayCtl.SetIcon(hP);
	m_AbortPlayCtl.SetIcon(hP);
	m_RequestBrowseCtl.SetIcon(hB);
	m_SuccessBrowseCtl.SetIcon(hB);
	m_AbortBrowseCtl.SetIcon(hB);
	
CPumpKINDlg* pd = (CPumpKINDlg*)AfxGetMainWnd();
//	ASSERT_KINDOF(CPumpKINDlg,pd);
	m_bnw=&pd->m_bnw;

	m_bnw->FillInCombo(&m_RequestCtl);
	m_bnw->FillInCombo(&m_SuccessCtl);
	m_bnw->FillInCombo(&m_AbortCtl);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropsSounds::OnAbortedBrowse() 
{
	Browse(m_AbortCtl);
}
void CPropsSounds::OnFinishedBrowse() 
{
	Browse(m_SuccessCtl);
}
void CPropsSounds::OnRingBrowse() 
{
	Browse(m_RequestCtl);
}

void CPropsSounds::OnAbortedPlay() 
{
	Play(m_AbortCtl);
}

void CPropsSounds::OnFinishedPlay() 
{
	Play(m_SuccessCtl);
}

void CPropsSounds::OnRingPlay() 
{
	Play(m_RequestCtl);
}

void CPropsSounds::Browse(CComboBox& ctl)
{
CString f;
	ctl.GetWindowText(f);
CString filter;
	filter.LoadString(IDS_FILTER_WAV);
CFileDialog fd(TRUE,NULL,(LPCTSTR)f,
			   OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY
			   |OFN_LONGNAMES|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST,
			   filter,this);
CString title;
	title.LoadString(IDS_TITLE_WAV);
	fd.m_ofn.lpstrTitle=(LPCTSTR)title;
	if(fd.DoModal()==IDOK)
		ctl.SetWindowText(fd.GetPathName());
}

void CPropsSounds::Play(CComboBox& ctl)
{
CString s;
	ctl.GetWindowText(s);
CBellsNWhistles::Whistling w = m_bnw->StartSound(s);
	if(w){
		Sleep(5000);
		m_bnw->StopSound(w);
	}
}
