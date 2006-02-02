// ACLTargetCombo.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PumpKINDlg.h"
#include "ACLTargetCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CACLTargetCombo

CACLTargetCombo::CACLTargetCombo()
: m_op(-1)
{
}

CACLTargetCombo::~CACLTargetCombo()
{
}


BEGIN_MESSAGE_MAP(CACLTargetCombo, CComboBox)
	//{{AFX_MSG_MAP(CACLTargetCombo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACLTargetCombo message handlers

void CACLTargetCombo::SetOp(int op)
{
	m_op=op;
	ResetContent();
	switch(op) {
	case tftp::opRRQ:
		m_tmap.RemoveAll();
		SetItemData(m_tmap[acl_rule::rrqNone]=AddString("fallback to global"),acl_rule::rrqNone);
		SetItemData(m_tmap[acl_rule::rrqDeny]=AddString("deny access"),acl_rule::rrqDeny);
		SetItemData(m_tmap[acl_rule::rrqPrompt]=AddString("prompt"),acl_rule::rrqPrompt);
		SetItemData(m_tmap[acl_rule::rrqGrant]=AddString("grant access"),CPumpKINDlg::rrqGrant);
		SetCurSel(0);
		EnableWindow(TRUE);
		break;
	case tftp::opWRQ:
		m_tmap.RemoveAll();
		SetItemData(m_tmap[acl_rule::wrqNone]=AddString("fallback to global"),acl_rule::wrqNone);
		SetItemData(m_tmap[acl_rule::wrqDeny]=AddString("deny access"),acl_rule::wrqDeny);
		SetItemData(m_tmap[acl_rule::wrqPrompt]=AddString("prompt"),acl_rule::wrqPrompt);
		SetItemData(m_tmap[acl_rule::wrqPromptIfExists]=AddString("prompt if file exists"),acl_rule::wrqPromptIfExists);
		SetItemData(m_tmap[acl_rule::wrqGrant]=AddString("grant access"),acl_rule::wrqGrant);
		SetCurSel(0);
		EnableWindow(TRUE);
		break;
	default:
		EnableWindow(FALSE);
		break;
	}
}

int CACLTargetCombo::GetTarget()
{
	int cs=GetCurSel();
	if(cs==CB_ERR)
		return -1;
	return GetItemData(cs);
}

void CACLTargetCombo::SetTarget(int t,int op)
{
	if(op>=0)
		SetOp(op);
	ASSERT(m_op>=0);
	int i;
	if(m_tmap.Lookup(t,i))
		SetCurSel(i);
	else
		SetCurSel(0);
}
