// ConfirmWRQDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfirmWRQDlg dialog

class CWRQSocket;
class CConfirmWRQDlg : public CDialog
{
// Construction
public:
	CWRQSocket *m_Daddy;
	CConfirmWRQDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmWRQDlg)
	enum { IDD = IDD_CONFIRM_WRQ };
	CButton	m_ResumeCtl;
	CButton	m_OkCtl;
	CButton	m_RenameCtl;
	CString	m_File;
	CString	m_Host;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmWRQDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmWRQDlg)
	afx_msg void OnRename();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnResume();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
