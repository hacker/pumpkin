// PropsACL.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PumpKINDlg.h"
#include "ACLTargetCombo.h"
#include "PropsACL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropsACL property page

IMPLEMENT_DYNCREATE(CPropsACL, CPropertyPage)

CPropsACL::CPropsACL() : CPropertyPage(CPropsACL::IDD)
{
	//{{AFX_DATA_INIT(CPropsACL)
	//}}AFX_DATA_INIT
}

CPropsACL::~CPropsACL()
{
}

void CPropsACL::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropsACL)
	DDX_Control(pDX, IDC_ACL_REPLACE, m_ReplaceCtl);
	DDX_Control(pDX, IDC_ACL_NETMASK, m_NetmaskCtl);
	DDX_Control(pDX, IDC_ACL_XFER, m_XferCtl);
	DDX_Control(pDX, IDC_ACL_UP, m_UpCtl);
	DDX_Control(pDX, IDC_ACL_RULE, m_RuleCtl);
	DDX_Control(pDX, IDC_ACL_REMOVE, m_RemoveCtl);
	DDX_Control(pDX, IDC_ACL_LIST, m_ListCtl);
	DDX_Control(pDX, IDC_ACL_DOWN, m_DownCtl);
	DDX_Control(pDX, IDC_ACL_ADDR, m_AddrCtl);
	DDX_Control(pDX, IDC_ACL_ADD, m_AddCtl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropsACL, CPropertyPage)
	//{{AFX_MSG_MAP(CPropsACL)
	ON_CBN_SELCHANGE(IDC_ACL_XFER, OnSelchangeAclXfer)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ACL_LIST, OnItemchangedAclList)
	ON_BN_CLICKED(IDC_ACL_ADD, OnAclAdd)
	ON_BN_CLICKED(IDC_ACL_REPLACE, OnAclReplace)
	ON_BN_CLICKED(IDC_ACL_REMOVE, OnAclRemove)
	ON_BN_CLICKED(IDC_ACL_UP, OnAclUp)
	ON_BN_CLICKED(IDC_ACL_DOWN, OnAclDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropsACL message handlers

BOOL CPropsACL::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_FocusedRule=-1;

	m_Images.Create(16,16,TRUE,2,1);
	m_iRRQ = m_Images.Add(AfxGetApp()->LoadIcon(IDI_RRQ));
	m_iWRQ = m_Images.Add(AfxGetApp()->LoadIcon(IDI_WRQ));
	ASSERT(m_iRRQ>=0); ASSERT(m_iWRQ>=0);
	m_ListCtl.SetImageList(&m_Images,LVSIL_NORMAL);
	m_ListCtl.SetImageList(&m_Images,LVSIL_SMALL);
	m_ListCtl.SetImageList(&m_Images,LVSIL_STATE);
	
	CRect lrc; m_ListCtl.GetClientRect(&lrc);
	long lrcw3 = lrc.Width()/3;
	m_ListCtl.InsertColumn(0,"IP",LVCFMT_LEFT,lrcw3,subitemIP);
	m_ListCtl.InsertColumn(1,"netmask",LVCFMT_LEFT,lrcw3,subitemNM);
	m_ListCtl.InsertColumn(2,"action",LVCFMT_LEFT,lrc.Width()-lrcw3*2,subitemAction);

	m_UpCtl.SetIcon(AfxGetApp()->LoadIcon(IDI_UP));
	m_DownCtl.SetIcon(AfxGetApp()->LoadIcon(IDI_DOWN));
	m_RemoveCtl.SetIcon(AfxGetApp()->LoadIcon(IDI_REMOVE));

	m_XferCtl.SetItemData(0,tftp::opRRQ);
	m_XferCtl.SetItemData(1,tftp::opWRQ);

	m_AddrCtl.SetWindowText("192.168.0.0");
	m_NetmaskCtl.SetWindowText("255.255.255.0");

	for(int i=0;i<m_rulist.GetSize();++i) {
		m_ListCtl.InsertItem(i,0);
		SetListRule(i,m_rulist[i]);
	}

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropsACL::OnSelchangeAclXfer()  {
	int cs = m_XferCtl.GetCurSel();
	if(cs==CB_ERR) {
		m_RuleCtl.EnableWindow(FALSE);
	}else{
		int rq = m_XferCtl.GetItemData(cs);
		m_RuleCtl.SetOp(rq);
	}
}

void CPropsACL::OnItemchangedAclList(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(
		(pNMListView->uChanged&LVIF_STATE)
		&&
		(pNMListView->uNewState&LVIS_FOCUSED)!=(pNMListView->uOldState&LVIS_FOCUSED)
		&&
		pNMListView->iItem>=0 && pNMListView->iItem<m_ListCtl.GetItemCount()
	  ){
		if(pNMListView->uNewState&LVIS_FOCUSED)
			m_FocusedRule=pNMListView->iItem;
		else if(pNMListView->iItem==m_FocusedRule)
			m_FocusedRule=-1;
		UpdateControls();
	}
	
	*pResult = 0;
}

void CPropsACL::UpdateControls() {
	if(m_FocusedRule>=m_rulist.GetSize())
		m_FocusedRule=-1;
	if(m_FocusedRule>=0) {
		m_UpCtl.EnableWindow(m_FocusedRule>0);
		m_DownCtl.EnableWindow(m_FocusedRule<(m_ListCtl.GetItemCount()-1));
		acl_rule r;
		GetListRule(m_FocusedRule,r);
		SetRule(r);
		m_AddCtl.EnableWindow(TRUE);
		m_ReplaceCtl.EnableWindow(TRUE);
	}else{
		OnSelchangeAclXfer();
		m_AddCtl.EnableWindow(TRUE);
		m_ReplaceCtl.EnableWindow(FALSE);
	}
	m_RemoveCtl.EnableWindow(m_ListCtl.GetSelectedCount()!=0 || m_FocusedRule>=0);
}

void CPropsACL::OnAclAdd()  {
	acl_rule r;
	UINT err=GetRule(r);
	if(err) {
		AfxMessageBox(err,MB_OK);
	}else{
		int i=m_rulist.AppendRule(r);
		ASSERT(r.op==acl_rule::opRRQ || r.op==acl_rule::opWRQ);
		m_ListCtl.InsertItem(i,0);
		SetListRule(i,r);
	}
}

void CPropsACL::OnAclReplace() {
	acl_rule r;
	UINT err=GetRule(r);
	if(err) {
		AfxMessageBox(err,MB_OK);
	}else{
		ASSERT(m_FocusedRule>=0);
		m_rulist[m_FocusedRule]=r;
		SetListRule(m_FocusedRule,r);
	}
}

int CPropsACL::GetOp() {
	int cs=m_XferCtl.GetCurSel();
	if(cs==CB_ERR)
		return -1;
	else
		return m_XferCtl.GetItemData(cs);
}

void CPropsACL::SetOp(int op) {
	int os=m_XferCtl.GetCount();
	for(int i=0;i<os;++i) {
		if(m_XferCtl.GetItemData(i)==op) {
			m_XferCtl.SetCurSel(i);
			return;
		}
	}
	m_XferCtl.SetCurSel(-1);
}

void CPropsACL::SetListRule(int i,acl_rule& r) {
	m_ListCtl.SetItem(i,subitemIP,LVIF_TEXT|LVIF_IMAGE,r.str_addr(),(r.op==acl_rule::opRRQ)?m_iRRQ:m_iWRQ,0,0,0);
	m_ListCtl.SetItemText(i,subitemNM,r.str_mask());
	m_ListCtl.SetItemText(i,subitemAction,r.str_target());
}

void CPropsACL::SetRule(acl_rule& r) {
	SetOp(r.op);
	m_AddrCtl.SetWindowText(r.str_addr());
	m_NetmaskCtl.SetWindowText(r.str_mask());
	m_RuleCtl.SetTarget(r.target,r.op);
}

void CPropsACL::GetListRule(int i,acl_rule& r) {
	r = m_rulist[i];
}

UINT CPropsACL::GetRule(acl_rule& r)
{
	UINT rv=0;
	r.op=GetOp();
	if(r.op!=acl_rule::opRRQ && r.op!=acl_rule::opWRQ)
		rv=IDS_NO_XFER_OP;
	else{
		CString t;
		m_AddrCtl.GetWindowText(t);
		if(t.IsEmpty() || ( (r.addr=inet_addr((LPCSTR)t))==INADDR_NONE && t!="255.255.255.255") )
			rv=IDS_INVALID_IP;
		else{
			m_NetmaskCtl.GetWindowText(t);
			if(t.IsEmpty() || ( (r.mask=inet_addr((LPCSTR)t))==INADDR_NONE && t!="255.255.255.255") )
				rv=IDS_INVALID_NETMASK;
			else{
				r.target=m_RuleCtl.GetTarget();
				if(!r.IsValid())
					rv=IDS_INVALID_RULE;
			}
		}
	}
	return rv;
}

void CPropsACL::OnAclRemove()  {
	ASSERT(m_FocusedRule>=0);
	int fr=m_FocusedRule;
	if(fr<0 || fr>=m_rulist.GetSize()) return;
	m_rulist.DeleteRule(fr);
	m_ListCtl.DeleteItem(fr);
	ASSERT(m_rulist.GetSize()==m_ListCtl.GetItemCount());
	if(fr>=m_rulist.GetSize()) {
		if(fr>0) {
		 fr=m_rulist.GetSize()-1;
		}
	}else
		fr=-1;
	if(fr>0)
		SetListFocusSelection(fr);
	m_ListCtl.SetFocus();
}

void CPropsACL::OnAclUp()  {
	int s=m_FocusedRule;
	if(s<=0) return;
	int d=s-1;
	acl_rule r=m_rulist[s];
	m_rulist[s]=m_rulist[d];
	m_rulist[d]=r;
	SetListRule(d,m_rulist[d]);
	SetListRule(s,m_rulist[s]);
	SetListFocusSelection(d);
	m_ListCtl.SetFocus();
}

void CPropsACL::OnAclDown()  {
	int s=m_FocusedRule;
	int d=s+1;
	if(s<0 || d>=m_rulist.GetSize()) return;
	acl_rule r=m_rulist[s];
	m_rulist[s]=m_rulist[d];
	m_rulist[d]=r;
	SetListRule(d,m_rulist[d]);
	SetListRule(s,m_rulist[s]);
	SetListFocusSelection(d);
	m_ListCtl.SetFocus();
}

void CPropsACL::SetListFocusSelection(int i) {
	int s=m_ListCtl.GetItemCount();
	for(int t=0;t<s;++t)
		if(t!=i)
			m_ListCtl.SetItemState(t,0,LVIS_FOCUSED|LVIS_SELECTED);
	m_ListCtl.SetItemState(i,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
}

