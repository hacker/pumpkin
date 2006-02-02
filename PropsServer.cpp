// PropsServer.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PropsServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropsServer property page

IMPLEMENT_DYNCREATE(CPropsServer, CPropertyPage)

CPropsServer::CPropsServer() : CPropertyPage(CPropsServer::IDD)
{
	//{{AFX_DATA_INIT(CPropsServer)
	m_RRQMode = -1;
	m_TFTPRoot = _T("");
	m_TFTPSubdirs = FALSE;
	m_WRQMode = -1;
	m_LogFile = _T("");
	//}}AFX_DATA_INIT
}

CPropsServer::~CPropsServer()
{
}

void CPropsServer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropsServer)
	DDX_Control(pDX, IDC_LOGFILE_BROWSE, m_LogBrowseCtl);
	DDX_Control(pDX, IDC_BROWSE, m_BrowseCtl);
	DDX_Control(pDX, IDC_PROMPTTIMEOUT, m_PromptTimeoutCtl);
	DDX_Radio(pDX, IDC_RRQ_GIVEALL, m_RRQMode);
	DDX_Text(pDX, IDC_TFTPROOT, m_TFTPRoot);
	DDX_Check(pDX, IDC_TFTPSUBDIRS, m_TFTPSubdirs);
	DDX_Radio(pDX, IDC_WRQ_TAKEALL, m_WRQMode);
	DDX_Text(pDX, IDC_LOGFILE, m_LogFile);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate)
		m_PromptTimeOut=m_PromptTimeoutCtl.GetPos();
	else
		m_PromptTimeoutCtl.SetPos(m_PromptTimeOut);
}


BEGIN_MESSAGE_MAP(CPropsServer, CPropertyPage)
	//{{AFX_MSG_MAP(CPropsServer)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_LOGFILE_BROWSE, OnLogfileBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropsServer message handlers

BOOL CPropsServer::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_PromptTimeoutCtl.SetRange(5,60);
	m_BrowseCtl.SetIcon(AfxGetApp()->LoadIcon(IDI_BROWSE));
	m_LogBrowseCtl.SetIcon(AfxGetApp()->LoadIcon(IDI_BROWSE));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropsServer::OnBrowse() 
{
CString nr = m_TFTPRoot;
	if(Klever::BrowseForFolder(nr,IDS_SELECT_TFTPROOT,this)){
		UpdateData(TRUE);
		m_TFTPRoot=nr;
		UpdateData(FALSE);
	}
}

void CPropsServer::OnLogfileBrowse() 
{
	UpdateData(TRUE);
	CFileDialog cfd(
		FALSE, ".log", (LPCSTR)m_LogFile,
		OFN_EXPLORER|OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,
		"Log files (*.log)|*.log|All Files (*.*)|*.*||",
		this);
	if(cfd.DoModal()==IDOK) {
		m_LogFile = cfd.GetPathName();
		UpdateData(FALSE);
	}	
}
