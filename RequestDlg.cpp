// RequestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "RequestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRequestDlg dialog


CRequestDlg::CRequestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRequestDlg::IDD, pParent)
{
	m_Put=TRUE;
	m_Drop=FALSE;
	//{{AFX_DATA_INIT(CRequestDlg)
	m_Host = m_MRUHost;
	m_LocalFile = _T("");
	m_RemoteFile = _T("");
	m_Type = _T("octet");
	m_strBSize = _T("");
	//}}AFX_DATA_INIT
}


void CRequestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if(!pDX->m_bSaveAndValidate)
		m_strBSize.Format("%u",m_BSize);
	//{{AFX_DATA_MAP(CRequestDlg)
	DDX_Control(pDX, IDC_REMOTEFILE, m_RemoteFileCtl);
	DDX_Control(pDX, IDC_LOCALFILE, m_LocalFileCtl);
	DDX_Control(pDX, IDC_REFRESH, m_RefreshCtl);
	DDX_Control(pDX, IDOK, m_OKCtl);
	DDX_Control(pDX, IDC_TALKS, m_TalksCtl);
	DDX_Control(pDX, IDC_BROWSE, m_BrowseCtl);
	DDX_CBString(pDX, IDC_TALKS, m_Host);
	DDX_Text(pDX, IDC_LOCALFILE, m_LocalFile);
	DDX_Text(pDX, IDC_REMOTEFILE, m_RemoteFile);
	DDX_CBString(pDX, IDC_TYPE, m_Type);
	DDX_CBString(pDX, IDC_BSIZE, m_strBSize);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate)
		m_BSize=atoi(m_strBSize);
}

CString CRequestDlg::m_MRUHost;

BEGIN_MESSAGE_MAP(CRequestDlg, CDialog)
	//{{AFX_MSG_MAP(CRequestDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_LOCALFILE, OnChangeLocalfile)
	ON_EN_CHANGE(IDC_REMOTEFILE, OnChangeRemotefile)
	ON_CBN_EDITCHANGE(IDC_TALKS, OnEditchangeTalks)
	ON_CBN_SELCHANGE(IDC_TALKS, OnSelchangeTalks)
	ON_WM_CTLCOLOR()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRequestDlg message handlers

BOOL CRequestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_RefreshCtl.SendMessage(WM_SETFONT,(WPARAM)::GetStockObject(ANSI_FIXED_FONT),0);
	m_BrowseCtl.SetIcon(AfxGetApp()->LoadIcon(IDI_BROWSE));

CString title;
	title.LoadString(m_Put?IDS_TITLE_PUTREQUEST:IDS_TITLE_GETREQUEST);
	SetWindowText(title);
	UpdateOK();
	SyncNames();
	BringWindowToTop();
	SetForegroundWindow();
	OnRefresh();
	if(!m_MRUHost.IsEmpty())
		m_TalksCtl.SetWindowText(m_MRUHost);
	else{
		if(m_TalksCtl.GetCount()==1){
		CString tmp;
			m_TalksCtl.GetLBText(0,tmp);
			m_TalksCtl.SetWindowText(tmp);
		}
	}

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool(&m_BrowseCtl,IDC_BROWSE);
		m_tooltip.AddTool(&m_RefreshCtl,IDC_REFRESH);
	}

	if(m_Put)
		DragAcceptFiles();

	if(m_Drop || !m_Put){
		m_RemoteFileCtl.SetFocus();
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRequestDlg::OnRefresh() 
{
	m_TalksCtl.ResetContent();
CWnd *wnd = CWnd::FindWindow(NULL,NULL);
CString wtalkHead,wtalkAt;
	wtalkHead.LoadString(IDS_WTALKHEADING);wtalkAt.LoadString(IDS_WTALKAT);
CString otalxHead,otalxAt;
	otalxHead.LoadString(IDS_OTALXHEADING);otalxAt.LoadString(IDS_OTALXAT);
	while(wnd){
	CString text;
		wnd->GetWindowText(text);
	int tw = text.Find(wtalkHead);
		if(tw==0){
			text=text.Mid(wtalkHead.GetLength());
		int at = text.Find(wtalkAt);
			if(at>=0){
				if(text.GetLength()>(at+wtalkAt.GetLength())){
					text=text.Left(at)+'@'+text.Mid(at+wtalkAt.GetLength());
					text.TrimLeft();
					text.TrimRight();
					VERIFY(m_TalksCtl.AddString(text)>=0);
				}
			}
		}else{
			tw = text.Find(otalxHead);
			if(tw==0){
				text=text.Mid(otalxHead.GetLength());
			int at = text.Find(otalxAt);
				if(at>=0){
					if(text.GetLength()>(at+otalxAt.GetLength())){
						text=text.Left(at)+'@'+text.Mid(at+otalxAt.GetLength());
						text.TrimLeft();
						text.TrimRight();
						VERIFY(m_TalksCtl.AddString(text)>=0);
					}
				}
			}
		}
		wnd = wnd->GetNextWindow();
	}
	UpdateOK();
}

void CRequestDlg::OnBrowse() 
{
	UpdateData(TRUE);
CFileDialog cfd(m_Put,NULL,m_LocalFile.IsEmpty()?NULL:m_LocalFile,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|(m_Put?OFN_FILEMUSTEXIST:OFN_OVERWRITEPROMPT),NULL,this);
CString title;
	title.LoadString(IDS_TITLE_BROWSEFILE);
	cfd.m_ofn.lpstrTitle=(LPCTSTR)title;
	if(cfd.DoModal()==IDOK){
		m_LocalFile=cfd.GetPathName();
		UpdateData(FALSE);
		SyncNames();
		UpdateOK();
	}
}

void CRequestDlg::SyncNames()
{
	if(m_Put){
	CString tmp;
		m_LocalFileCtl.GetWindowText(tmp);
	int s=tmp.ReverseFind('\\');
		if(s>=0)
			tmp=tmp.Mid(s+1);
		else{
			s = tmp.ReverseFind('/');
			if(s>=0)
				tmp=tmp.Mid(s+1);
		}
		if(!tmp.IsEmpty())
			m_RemoteFileCtl.SetWindowText(tmp);
	}else{
	CString tmp;
		m_RemoteFileCtl.GetWindowText(tmp);
	int s=tmp.ReverseFind('\\');
		if(s>=0)
			tmp=tmp.Mid(s+1);
		else{
			s = tmp.ReverseFind('/');
			if(s>=0)
				tmp=tmp.Mid(s+1);
		}
		if(!tmp.IsEmpty())
			m_LocalFileCtl.SetWindowText(tmp);
	}
}

void CRequestDlg::OnChangeLocalfile() 
{
	if(m_Put)
		SyncNames();
	UpdateOK();
}

void CRequestDlg::OnChangeRemotefile() 
{
	if(!m_Put)
		SyncNames();
	UpdateOK();
}

void CRequestDlg::UpdateOK()
{
CString t;
	m_LocalFileCtl.GetWindowText(t);
	if(t.IsEmpty())
		goto	dAble;
	m_RemoteFileCtl.GetWindowText(t);
	if(t.IsEmpty())
		goto dAble;
	m_TalksCtl.GetWindowText(t);
	if(t.IsEmpty())
		goto dAble;
	m_OKCtl.EnableWindow(TRUE);
	return;
dAble:
	m_OKCtl.EnableWindow(FALSE);
}

void CRequestDlg::OnEditchangeTalks() 
{
	UpdateOK();	
}

void CRequestDlg::OnSelchangeTalks() 
{
	UpdateOK();
}

HBRUSH CRequestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);	
	UpdateOK();
	return hbr;
}

BOOL CRequestDlg::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		if(::IsWindow(m_tooltip.m_hWnd))
			m_tooltip.RelayEvent(pMsg);

		return CDialog::PreTranslateMessage(pMsg);
	}
}

int CRequestDlg::DoModal() 
{
int rv = CDialog::DoModal();
	if(rv==IDOK)
		m_MRUHost=m_Host;
	return rv;
}

void CRequestDlg::OnDropFiles(HDROP hDropInfo) 
{
UINT files = ::DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);
	if(files!=1){
	CString title,text;
		title.LoadString(IDS_DROPFILES_TITLE);
		text.LoadString(IDS_NOMULTIPLEDROP_TEXT);
		if(MessageBox(text,title,MB_ICONSTOP|MB_OKCANCEL)!=IDOK){
			::DragFinish(hDropInfo);
			return;
		}
	}
	ASSERT(files);
CString theFile;
UINT fileNameLength = ::DragQueryFile(hDropInfo,0,NULL,0);
	ASSERT(fileNameLength);
	VERIFY(::DragQueryFile(hDropInfo,0,theFile.GetBuffer(fileNameLength+5),fileNameLength+4)<=fileNameLength);
	theFile.ReleaseBuffer();
	m_LocalFileCtl.SetWindowText(theFile);
	SyncNames();
	::DragFinish(hDropInfo);
}
