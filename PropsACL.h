// PropsACL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropsACL dialog

class CPropsACL : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropsACL)

// Construction
public:
	void SetListFocusSelection(int i);
	UINT GetRule(acl_rule& r);
	void GetListRule(int i,acl_rule& r);
	void SetOp(int op);
	void SetRule(acl_rule& r);
	void SetListRule(int i,acl_rule& r);
	int m_iWRQ;
	int m_iRRQ;
	CImageList m_Images;
	int GetOp();
	acl_rules_t m_rulist;
	int m_FocusedRule;
	void UpdateControls();
	enum {
		subitemIP=0, subitemNM, subitemAction
	};

	CPropsACL();
	~CPropsACL();

// Dialog Data
	//{{AFX_DATA(CPropsACL)
	enum { IDD = IDD_PROPS_ACL };
	CButton	m_ReplaceCtl;
	CEdit	m_NetmaskCtl;
	CComboBox	m_XferCtl;
	CButton	m_UpCtl;
	CACLTargetCombo	m_RuleCtl;
	CButton	m_RemoveCtl;
	CListCtrl	m_ListCtl;
	CButton	m_DownCtl;
	CEdit	m_AddrCtl;
	CButton	m_AddCtl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropsACL)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropsACL)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAclXfer();
	afx_msg void OnItemchangedAclList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAclAdd();
	afx_msg void OnAclReplace();
	afx_msg void OnAclRemove();
	afx_msg void OnAclUp();
	afx_msg void OnAclDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
