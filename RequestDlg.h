// RequestDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRequestDlg dialog

class CRequestDlg : public CDialog
{
// Construction
public:
	UINT m_BSize;
	static CString m_MRUHost;
	BOOL m_Drop;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateOK();
	void SyncNames();
	BOOL m_Put;
	CRequestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRequestDlg)
	enum { IDD = IDD_REQUEST };
	CEdit	m_RemoteFileCtl;
	CEdit	m_LocalFileCtl;
	CButton	m_RefreshCtl;
	CButton	m_OKCtl;
	CComboBox	m_TalksCtl;
	CButton	m_BrowseCtl;
	CString	m_Host;
	CString	m_LocalFile;
	CString	m_RemoteFile;
	CString	m_Type;
	CString	m_strBSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRequestDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CRequestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	afx_msg void OnBrowse();
	afx_msg void OnChangeLocalfile();
	afx_msg void OnChangeRemotefile();
	afx_msg void OnEditchangeTalks();
	afx_msg void OnSelchangeTalks();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
